// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux equivalent of PowerSource.swift + PowerSourceWatcher.swift
#include "PowerDelivery.h"
#include "SysfsReader.h"
#include <QDir>

namespace WhatCable {

static const QString kPdPath = QStringLiteral("/sys/class/usb_power_delivery");

QString PowerDataObject::voltageLabel() const
{
    if (type == PPS && maxVoltageMV > 0)
        return QStringLiteral("%1-%2V").arg(voltageMV / 1000.0, 0, 'f', 1).arg(maxVoltageMV / 1000.0, 0, 'f', 1);
    return QStringLiteral("%1V").arg(voltageMV / 1000.0, 0, 'f', 1);
}

QString PowerDataObject::currentLabel() const
{
    return QStringLiteral("%1A").arg(currentMA / 1000.0, 0, 'f', 2);
}

QString PowerDataObject::powerLabel() const
{
    return QStringLiteral("%1W").arg(powerMW / 1000.0, 0, 'f', 0);
}

QString PowerDataObject::typeLabel() const
{
    switch (type) {
    case FixedSupply: return QStringLiteral("Fixed");
    case Battery: return QStringLiteral("Battery");
    case VariableSupply: return QStringLiteral("Variable");
    case PPS: return QStringLiteral("PPS");
    default: return QStringLiteral("Unknown");
    }
}

QList<PowerDataObject> PowerDeliveryPort::parsePDOs(const QString &capsPath)
{
    QList<PowerDataObject> pdos;
    if (!SysfsReader::pathExists(capsPath))
        return pdos;

    QDir capsDir(capsPath);
    const auto entries = capsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for (const QString &entry : entries) {
        QString pdoPath = capsDir.filePath(entry);
        PowerDataObject pdo;
        pdo.index = entry.mid(entry.lastIndexOf(':') + 1).toInt();

        QString typeStr = SysfsReader::readAttribute(pdoPath + QStringLiteral("/type"));
        if (typeStr == QStringLiteral("fixed_supply"))
            pdo.type = PowerDataObject::FixedSupply;
        else if (typeStr == QStringLiteral("battery"))
            pdo.type = PowerDataObject::Battery;
        else if (typeStr == QStringLiteral("variable_supply"))
            pdo.type = PowerDataObject::VariableSupply;
        else if (typeStr.contains(QStringLiteral("pps")))
            pdo.type = PowerDataObject::PPS;

        auto voltage = SysfsReader::readIntAttribute(pdoPath + QStringLiteral("/voltage"));
        if (voltage) pdo.voltageMV = *voltage;

        auto maxVoltage = SysfsReader::readIntAttribute(pdoPath + QStringLiteral("/maximum_voltage"));
        if (maxVoltage) pdo.maxVoltageMV = *maxVoltage;
        else if (!voltage) {
            auto minVoltage = SysfsReader::readIntAttribute(pdoPath + QStringLiteral("/minimum_voltage"));
            if (minVoltage) pdo.voltageMV = *minVoltage;
        }

        auto current = SysfsReader::readIntAttribute(pdoPath + QStringLiteral("/maximum_current"));
        if (!current)
            current = SysfsReader::readIntAttribute(pdoPath + QStringLiteral("/current"));
        if (current) pdo.currentMA = *current;

        auto power = SysfsReader::readIntAttribute(pdoPath + QStringLiteral("/maximum_power"));
        if (power)
            pdo.powerMW = *power;
        else if (pdo.voltageMV > 0 && pdo.currentMA > 0)
            pdo.powerMW = static_cast<int>(static_cast<int64_t>(pdo.voltageMV) * pdo.currentMA / 1000);

        pdos.append(pdo);
    }

    return pdos;
}

std::optional<PowerDeliveryPort> PowerDeliveryPort::fromSysfs(const QString &path, const QString &name)
{
    PowerDeliveryPort port;
    port.sysfsPath = path;
    port.name = name;

    port.sourceCapabilities = parsePDOs(path + QStringLiteral("/source-capabilities"));
    port.sinkCapabilities = parsePDOs(path + QStringLiteral("/sink-capabilities"));

    for (const auto &pdo : port.sourceCapabilities) {
        if (pdo.powerMW > port.maxSourcePowerMW)
            port.maxSourcePowerMW = pdo.powerMW;
    }

    port.rawAttributes = SysfsReader::readAllAttributes(path);

    if (port.sourceCapabilities.isEmpty() && port.sinkCapabilities.isEmpty())
        return std::nullopt;

    return port;
}

QList<PowerDeliveryPort> PowerDeliveryPort::enumerate()
{
    QList<PowerDeliveryPort> ports;
    if (!SysfsReader::pathExists(kPdPath))
        return ports;

    QDir pdDir(kPdPath);
    const auto entries = pdDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &entry : entries) {
        auto port = fromSysfs(pdDir.filePath(entry), entry);
        if (port)
            ports.append(std::move(*port));
    }

    return ports;
}

} // namespace WhatCable
