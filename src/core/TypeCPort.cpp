// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux equivalent of USBCPort.swift + USBCPortWatcher.swift
#include "TypeCPort.h"
#include "SysfsReader.h"
#include <QDir>
#include <QRegularExpression>

namespace WhatCable {

static const QString kTypecPath = QStringLiteral("/sys/class/typec");

bool TypeCPort::isConnected() const
{
    return hasPartner || hasCable;
}

QString TypeCPort::currentDataRole() const
{
    // sysfs shows current role in brackets like "[host] device"
    QRegularExpression re(QStringLiteral("\\[([^\\]]+)\\]"));
    auto match = re.match(dataRole);
    if (match.hasMatch())
        return match.captured(1);
    return dataRole;
}

QString TypeCPort::currentPowerRole() const
{
    QRegularExpression re(QStringLiteral("\\[([^\\]]+)\\]"));
    auto match = re.match(powerRole);
    if (match.hasMatch())
        return match.captured(1);
    return powerRole;
}

std::optional<TypeCIdentity> TypeCPort::readIdentity(const QString &path)
{
    QString idPath = path + QStringLiteral("/identity");
    if (!SysfsReader::pathExists(idPath))
        return std::nullopt;

    TypeCIdentity id;
    auto vid = SysfsReader::readHexAttribute(idPath + QStringLiteral("/id_header"));
    if (vid)
        id.vendorId = static_cast<uint16_t>(*vid & 0xFFFF);

    auto pid = SysfsReader::readHexAttribute(idPath + QStringLiteral("/product"));
    if (pid)
        id.productId = static_cast<uint16_t>(*pid & 0xFFFF);

    // Read VDO files (vdo1, vdo2, vdo3, etc. or numbered)
    QDir idDir(idPath);
    const auto entries = idDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &entry : entries) {
        if (!entry.startsWith(QStringLiteral("vdo")) &&
            entry != QStringLiteral("id_header") &&
            entry != QStringLiteral("cert_stat") &&
            entry != QStringLiteral("product") &&
            entry != QStringLiteral("product_type_vdo1") &&
            entry != QStringLiteral("product_type_vdo2") &&
            entry != QStringLiteral("product_type_vdo3"))
            continue;

        auto val = SysfsReader::readHexAttribute(idPath + QStringLiteral("/") + entry);
        if (val)
            id.vdos.append(*val);
    }

    if (id.vendorId == 0 && id.vdos.isEmpty())
        return std::nullopt;

    return id;
}

std::optional<TypeCPort> TypeCPort::fromSysfs(const QString &path, const QString &name)
{
    if (!name.startsWith(QStringLiteral("port")))
        return std::nullopt;

    TypeCPort port;
    port.sysfsPath = path;
    port.portName = name;

    QRegularExpression numRe(QStringLiteral("port(\\d+)"));
    auto match = numRe.match(name);
    if (match.hasMatch())
        port.portNumber = match.captured(1).toInt();

    port.dataRole = SysfsReader::readAttribute(path + QStringLiteral("/data_role"));
    port.powerRole = SysfsReader::readAttribute(path + QStringLiteral("/power_role"));
    port.portType = SysfsReader::readAttribute(path + QStringLiteral("/port_type"));
    port.powerOpMode = SysfsReader::readAttribute(path + QStringLiteral("/power_operation_mode"));
    port.orientation = SysfsReader::readAttribute(path + QStringLiteral("/orientation"));
    port.pdRevision = SysfsReader::readAttribute(path + QStringLiteral("/usb_power_delivery_revision"));
    port.usbTypeCRev = SysfsReader::readAttribute(path + QStringLiteral("/usb_typec_revision"));

    // Check for partner (portN-partner)
    QString partnerPath = path + QStringLiteral("-partner");
    if (SysfsReader::pathExists(partnerPath)) {
        port.hasPartner = true;
        TypeCPartner p;
        p.type = SysfsReader::readAttribute(partnerPath + QStringLiteral("/type"));
        p.identity = readIdentity(partnerPath);
        p.rawAttributes = SysfsReader::readAllAttributes(partnerPath);
        port.partner = std::move(p);
    }

    // Check for cable (portN-cable)
    QString cablePath = path + QStringLiteral("-cable");
    if (SysfsReader::pathExists(cablePath)) {
        port.hasCable = true;
        TypeCCable c;
        c.type = SysfsReader::readAttribute(cablePath + QStringLiteral("/type"));
        c.plugType = SysfsReader::readAttribute(cablePath + QStringLiteral("/plug_type"));
        c.identity = readIdentity(cablePath);
        c.rawAttributes = SysfsReader::readAllAttributes(cablePath);
        port.cable = std::move(c);
    }

    port.rawAttributes = SysfsReader::readAllAttributes(path);

    return port;
}

QList<TypeCPort> TypeCPort::enumerate()
{
    QList<TypeCPort> ports;
    if (!SysfsReader::pathExists(kTypecPath))
        return ports;

    QDir typecDir(kTypecPath);
    const auto entries = typecDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &entry : entries) {
        auto port = fromSysfs(typecDir.filePath(entry), entry);
        if (port)
            ports.append(std::move(*port));
    }

    return ports;
}

} // namespace WhatCable
