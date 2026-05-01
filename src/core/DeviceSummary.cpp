// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of PortSummary.swift — plain-English summary logic
#include "DeviceSummary.h"
#include "VendorDB.h"
#include "UsbClassDB.h"
#include "PDDecoder.h"

namespace WhatCable {

DeviceSummary DeviceSummary::fromUsbDevice(const UsbDevice &dev)
{
    DeviceSummary s;
    s.category = dev.isHub ? HubCategory : UsbDeviceCategory;
    s.status = Connected;
    s.usbDevice = dev;

    QString vendorName = VendorDB::lookup(dev.vendorId);
    bool hasVendorName = !vendorName.startsWith(QStringLiteral("0x"));

    // Headline: product name or vendor:product
    s.headline = dev.displayName();

    // Subtitle: vendor + type
    QString deviceType;
    if (dev.deviceClass != 0 && dev.deviceClass != 0xFF) {
        deviceType = UsbClassDB::className(dev.deviceClass);
    } else if (!dev.interfaces.isEmpty()) {
        // Composite device — derive type from interfaces
        QStringList types;
        for (const auto &iface : dev.interfaces) {
            QString t = UsbClassDB::className(iface.classCode);
            if (!types.contains(t) && t != QStringLiteral("Composite") &&
                !t.startsWith(QStringLiteral("0x")))
                types.append(t);
        }
        deviceType = types.join(QStringLiteral(", "));
    }

    if (hasVendorName)
        s.subtitle = vendorName;
    if (!deviceType.isEmpty()) {
        if (!s.subtitle.isEmpty())
            s.subtitle += QStringLiteral(" · ");
        s.subtitle += deviceType;
    }

    // Bullets
    s.bullets.append(dev.speedLabel());

    if (dev.maxPowerMA > 0)
        s.bullets.append(QStringLiteral("Power: %1").arg(dev.powerLabel()));

    s.bullets.append(QStringLiteral("USB %1").arg(dev.version));

    if (!dev.serial.isEmpty())
        s.bullets.append(QStringLiteral("Serial: %1").arg(dev.serial));

    if (dev.removable == QStringLiteral("removable"))
        s.bullets.append(QStringLiteral("Removable"));
    else if (dev.removable == QStringLiteral("fixed"))
        s.bullets.append(QStringLiteral("Built-in"));

    // Driver info from interfaces
    QStringList drivers;
    for (const auto &iface : dev.interfaces) {
        if (!iface.driver.isEmpty() && !drivers.contains(iface.driver))
            drivers.append(iface.driver);
    }
    if (!drivers.isEmpty())
        s.bullets.append(QStringLiteral("Drivers: %1").arg(drivers.join(QStringLiteral(", "))));

    s.bullets.append(QStringLiteral("VID:PID %1:%2")
                         .arg(dev.vendorId, 4, 16, QChar('0'))
                         .arg(dev.productId, 4, 16, QChar('0')));

    // Icon
    if (dev.isHub) s.icon = QStringLiteral("network-wired");
    else if (deviceType.contains(QStringLiteral("Audio"))) s.icon = QStringLiteral("audio-card");
    else if (deviceType.contains(QStringLiteral("HID"))) s.icon = QStringLiteral("input-keyboard");
    else if (deviceType.contains(QStringLiteral("Mass Storage"))) s.icon = QStringLiteral("drive-removable-media");
    else if (deviceType.contains(QStringLiteral("Video"))) s.icon = QStringLiteral("camera-web");
    else if (deviceType.contains(QStringLiteral("Wireless"))) s.icon = QStringLiteral("network-wireless");
    else if (deviceType.contains(QStringLiteral("Printer"))) s.icon = QStringLiteral("printer");
    else s.icon = QStringLiteral("drive-removable-media-usb");

    return s;
}

DeviceSummary DeviceSummary::fromTypeCPort(
    const TypeCPort &port,
    const std::optional<PowerDeliveryPort> &pd,
    const std::optional<CableInfo> &cableOpt)
{
    DeviceSummary s;
    s.category = TypeCPortCategory;
    s.typecPort = port;
    s.powerDelivery = pd;
    s.cable = cableOpt;
    s.icon = QStringLiteral("plug");

    if (!port.isConnected()) {
        s.status = Empty;
        s.headline = QStringLiteral("USB-C Port %1").arg(port.portNumber);
        s.subtitle = QStringLiteral("Nothing connected");
        return s;
    }

    s.status = Connected;
    s.headline = QStringLiteral("USB-C Port %1").arg(port.portNumber);

    // Determine headline from what's connected
    if (port.hasPartner && port.partner) {
        if (port.partner->identity && !port.partner->identity->vdos.isEmpty()) {
            auto hdr = decodeIDHeader(port.partner->identity->vdos[0]);
            QString productLabel = productTypeLabel(hdr.ufpProductType);
            QString vendorLabel = VendorDB::lookup(hdr.vendorId);
            bool hasVendor = !vendorLabel.startsWith(QStringLiteral("0x"));
            s.subtitle = hasVendor
                ? QStringLiteral("%1 — %2").arg(vendorLabel, productLabel)
                : productLabel;
        } else {
            s.subtitle = QStringLiteral("Device connected");
        }
    }

    // Data and power roles
    QString dataStr = port.currentDataRole();
    QString powerStr = port.currentPowerRole();
    if (!dataStr.isEmpty() || !powerStr.isEmpty()) {
        QString roleInfo;
        if (!dataStr.isEmpty())
            roleInfo = QStringLiteral("Data: %1").arg(dataStr);
        if (!powerStr.isEmpty()) {
            if (!roleInfo.isEmpty()) roleInfo += QStringLiteral(", ");
            roleInfo += QStringLiteral("Power: %1").arg(powerStr);
        }
        s.bullets.append(roleInfo);
    }

    if (!port.powerOpMode.isEmpty())
        s.bullets.append(QStringLiteral("Power mode: %1").arg(port.powerOpMode));

    if (!port.pdRevision.isEmpty())
        s.bullets.append(QStringLiteral("PD revision: %1").arg(port.pdRevision));

    if (!port.orientation.isEmpty() && port.orientation != QStringLiteral("unknown"))
        s.bullets.append(QStringLiteral("Plug orientation: %1").arg(port.orientation));

    // Cable info
    if (cableOpt) {
        const auto &c = *cableOpt;
        if (c.speed)
            s.bullets.append(QStringLiteral("Cable speed: %1").arg(cableSpeedLabel(*c.speed)));
        if (c.currentRating)
            s.bullets.append(QStringLiteral("Cable current: %1").arg(cableCurrentLabel(*c.currentRating)));
        if (c.maxWatts > 0)
            s.bullets.append(QStringLiteral("Cable max power: %1W").arg(c.maxWatts));
        if (c.isActive)
            s.bullets.append(QStringLiteral("Active cable"));
        else if (c.isPassive)
            s.bullets.append(QStringLiteral("Passive cable"));
        if (!c.vendorName.isEmpty() && !c.vendorName.startsWith(QStringLiteral("0x")))
            s.bullets.append(QStringLiteral("Cable vendor: %1").arg(c.vendorName));
    }

    // Power Delivery
    if (pd && !pd->sourceCapabilities.isEmpty()) {
        int maxW = pd->maxSourcePowerMW / 1000;
        s.bullets.append(QStringLiteral("Charger max: %1W").arg(maxW));
        s.status = Charging;
    }

    // Charging diagnostic
    if (pd) {
        auto diag = ChargingDiagnostic::evaluate(*pd, cableOpt);
        if (diag)
            s.chargingDiag = std::move(diag);
    }

    return s;
}

} // namespace WhatCable
