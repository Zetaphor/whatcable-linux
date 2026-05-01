// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux equivalent of USBCPort.swift — reads /sys/class/typec/
#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <optional>
#include <cstdint>

namespace WhatCable {

struct TypeCIdentity {
    uint16_t vendorId = 0;
    uint16_t productId = 0;
    QList<uint32_t> vdos;
};

struct TypeCPartner {
    QString type;
    std::optional<TypeCIdentity> identity;
    QMap<QString, QString> rawAttributes;
};

struct TypeCCable {
    QString type;          // "active", "passive"
    QString plugType;
    std::optional<TypeCIdentity> identity;
    QMap<QString, QString> rawAttributes;
};

struct TypeCPort {
    QString sysfsPath;
    QString portName;      // "port0", "port1", ...
    int portNumber = -1;

    QString dataRole;      // "host", "device", "[host]", "[device]"
    QString powerRole;     // "source", "sink", "[source]", "[sink]"
    QString portType;      // "dual", "source", "sink"
    QString powerOpMode;   // "default", "1.5A", "3.0A", "usb_power_delivery"
    QString orientation;   // "normal", "reverse", "unknown"
    QString pdRevision;
    QString usbTypeCRev;

    bool hasPartner = false;
    std::optional<TypeCPartner> partner;
    bool hasCable = false;
    std::optional<TypeCCable> cable;

    QMap<QString, QString> rawAttributes;

    bool isConnected() const;
    QString currentDataRole() const;
    QString currentPowerRole() const;

    static QList<TypeCPort> enumerate();

private:
    static std::optional<TypeCPort> fromSysfs(const QString &path, const QString &name);
    static std::optional<TypeCIdentity> readIdentity(const QString &path);
};

} // namespace WhatCable
