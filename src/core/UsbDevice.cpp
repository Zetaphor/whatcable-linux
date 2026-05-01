// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux equivalent of USBDevice.swift + USBWatcher.swift
#include "UsbDevice.h"
#include "SysfsReader.h"
#include <QDir>
#include <QRegularExpression>

namespace WhatCable {

static const QString kUsbDevicesPath = QStringLiteral("/sys/bus/usb/devices");

QString UsbDevice::displayName() const
{
    if (!product.isEmpty())
        return product;
    return QStringLiteral("%1:%2").arg(vendorId, 4, 16, QChar('0')).arg(productId, 4, 16, QChar('0'));
}

QString UsbDevice::speedLabel() const
{
    if (speed >= 20000) return QStringLiteral("USB4 20 Gbps");
    if (speed >= 10000) return QStringLiteral("SuperSpeed+ 10 Gbps");
    if (speed >= 5000)  return QStringLiteral("SuperSpeed 5 Gbps");
    if (speed >= 480)   return QStringLiteral("High Speed 480 Mbps");
    if (speed >= 12)    return QStringLiteral("Full Speed 12 Mbps");
    if (speed >= 2)     return QStringLiteral("Low Speed 1.5 Mbps");
    return QStringLiteral("Unknown speed");
}

QString UsbDevice::powerLabel() const
{
    if (maxPowerMA <= 0)
        return {};
    if (maxPowerMA >= 1000)
        return QStringLiteral("%1 W").arg(maxPowerMA / 1000.0, 0, 'f', 1);
    return QStringLiteral("%1 mA").arg(maxPowerMA);
}

static int parseMaxPower(const QString &val)
{
    if (val.isEmpty()) return 0;
    QString numeric = val;
    numeric.remove(QRegularExpression(QStringLiteral("[^0-9]")));
    bool ok = false;
    return numeric.toInt(&ok);
}

std::optional<UsbDevice> UsbDevice::fromSysfs(const QString &path, const QString &name)
{
    // Skip interface entries like "1-2:1.0"
    if (name.contains(':'))
        return std::nullopt;

    UsbDevice dev;
    dev.sysfsPath = path;
    dev.busPort = name;

    auto vid = SysfsReader::readHexAttribute(path + QStringLiteral("/idVendor"));
    auto pid = SysfsReader::readHexAttribute(path + QStringLiteral("/idProduct"));
    if (!vid || !pid)
        return std::nullopt;

    dev.vendorId = static_cast<uint16_t>(*vid);
    dev.productId = static_cast<uint16_t>(*pid);
    dev.manufacturer = SysfsReader::readAttribute(path + QStringLiteral("/manufacturer"));
    dev.product = SysfsReader::readAttribute(path + QStringLiteral("/product"));
    dev.serial = SysfsReader::readAttribute(path + QStringLiteral("/serial"));
    dev.version = SysfsReader::readAttribute(path + QStringLiteral("/version")).trimmed();
    dev.removable = SysfsReader::readAttribute(path + QStringLiteral("/removable"));

    dev.speed = SysfsReader::readIntAttribute(path + QStringLiteral("/speed")).value_or(0);
    dev.maxPowerMA = parseMaxPower(SysfsReader::readAttribute(path + QStringLiteral("/bMaxPower")));
    dev.busNum = SysfsReader::readIntAttribute(path + QStringLiteral("/busnum")).value_or(0);
    dev.devNum = SysfsReader::readIntAttribute(path + QStringLiteral("/devnum")).value_or(0);
    dev.rxLanes = SysfsReader::readIntAttribute(path + QStringLiteral("/rx_lanes")).value_or(0);
    dev.txLanes = SysfsReader::readIntAttribute(path + QStringLiteral("/tx_lanes")).value_or(0);
    dev.numConfigurations = SysfsReader::readIntAttribute(path + QStringLiteral("/bNumConfigurations")).value_or(0);

    auto cls = SysfsReader::readHexAttribute(path + QStringLiteral("/bDeviceClass"));
    dev.deviceClass = cls ? static_cast<uint8_t>(*cls) : 0;
    auto sub = SysfsReader::readHexAttribute(path + QStringLiteral("/bDeviceSubClass"));
    dev.deviceSubClass = sub ? static_cast<uint8_t>(*sub) : 0;
    auto proto = SysfsReader::readHexAttribute(path + QStringLiteral("/bDeviceProtocol"));
    dev.deviceProtocol = proto ? static_cast<uint8_t>(*proto) : 0;

    dev.isHub = (dev.deviceClass == 0x09);
    dev.isRootHub = name.startsWith(QStringLiteral("usb"));

    // Parse interfaces
    QString numIf = SysfsReader::readAttribute(path + QStringLiteral("/bNumInterfaces"));
    dev.numInterfaces = numIf.trimmed().toInt();

    QDir devDir(path);
    const auto entries = devDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &entry : entries) {
        if (!entry.contains(':'))
            continue;
        QString ifPath = path + QStringLiteral("/") + entry;
        UsbInterface iface;
        auto ifClass = SysfsReader::readHexAttribute(ifPath + QStringLiteral("/bInterfaceClass"));
        if (!ifClass) continue;
        iface.classCode = static_cast<uint8_t>(*ifClass);
        auto ifSub = SysfsReader::readHexAttribute(ifPath + QStringLiteral("/bInterfaceSubClass"));
        iface.subClass = ifSub ? static_cast<uint8_t>(*ifSub) : 0;
        auto ifProto = SysfsReader::readHexAttribute(ifPath + QStringLiteral("/bInterfaceProtocol"));
        iface.protocol = ifProto ? static_cast<uint8_t>(*ifProto) : 0;

        // Read driver symlink
        QString driverLink = ifPath + QStringLiteral("/driver");
        QFileInfo driverInfo(driverLink);
        if (driverInfo.isSymLink())
            iface.driver = QFileInfo(driverInfo.symLinkTarget()).fileName();

        QString numStr = entry.section('.', -1);
        iface.number = numStr.toInt();
        dev.interfaces.append(iface);
    }

    dev.rawAttributes = SysfsReader::readAllAttributes(path);

    return dev;
}

void UsbDevice::buildTopology(QList<UsbDevice> &devices)
{
    QMap<QString, int> nameToIndex;
    for (int i = 0; i < devices.size(); ++i)
        nameToIndex.insert(devices[i].busPort, i);

    // Build parent-child: "1-2.3" is child of "1-2", "1-2" is child of "usb1"
    QList<int> topLevel;
    for (int i = 0; i < devices.size(); ++i) {
        const QString &bp = devices[i].busPort;
        if (devices[i].isRootHub) {
            topLevel.append(i);
            continue;
        }
        // Parent is everything before the last '.'
        int lastDot = bp.lastIndexOf('.');
        QString parentBp;
        if (lastDot > 0) {
            parentBp = bp.left(lastDot);
        } else {
            // "1-2" -> parent is "usb1" (bus number is before the '-')
            int dash = bp.indexOf('-');
            if (dash > 0)
                parentBp = QStringLiteral("usb") + bp.left(dash);
        }
        if (nameToIndex.contains(parentBp)) {
            devices[nameToIndex[parentBp]].children.append(devices[i]);
        }
    }
}

QList<UsbDevice> UsbDevice::enumerate()
{
    QList<UsbDevice> devices;
    QDir usbDir(kUsbDevicesPath);
    if (!usbDir.exists())
        return devices;

    const auto entries = usbDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &entry : entries) {
        auto dev = fromSysfs(usbDir.filePath(entry), entry);
        if (dev)
            devices.append(std::move(*dev));
    }

    buildTopology(devices);
    return devices;
}

} // namespace WhatCable
