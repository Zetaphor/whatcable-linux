// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux equivalent of PowerSource.swift — reads /sys/class/usb_power_delivery/
#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <optional>
#include <cstdint>

namespace WhatCable {

struct PowerDataObject {
    enum Type { FixedSupply, Battery, VariableSupply, PPS, Unknown };

    Type type = Unknown;
    int voltageMV = 0;
    int maxVoltageMV = 0;   // for PPS/variable
    int currentMA = 0;
    int powerMW = 0;
    bool isActive = false;
    int index = 0;

    QString voltageLabel() const;
    QString currentLabel() const;
    QString powerLabel() const;
    QString typeLabel() const;
};

struct PowerDeliveryPort {
    QString sysfsPath;
    QString name;
    QString parentPortType;
    int parentPortNumber = -1;

    QList<PowerDataObject> sourceCapabilities;
    QList<PowerDataObject> sinkCapabilities;

    int maxSourcePowerMW = 0;
    std::optional<int> activeSourcePdoIndex;

    QMap<QString, QString> rawAttributes;

    static QList<PowerDeliveryPort> enumerate();

private:
    static std::optional<PowerDeliveryPort> fromSysfs(const QString &path, const QString &name);
    static QList<PowerDataObject> parsePDOs(const QString &capsPath);
};

} // namespace WhatCable
