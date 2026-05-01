// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux equivalent of USBDevice.swift — reads /sys/bus/usb/devices/
#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <optional>
#include <cstdint>

namespace WhatCable {

struct UsbInterface {
    int number = 0;
    uint8_t classCode = 0;
    uint8_t subClass = 0;
    uint8_t protocol = 0;
    QString driver;
};

struct UsbDevice {
    QString sysfsPath;
    QString busPort;

    uint16_t vendorId = 0;
    uint16_t productId = 0;
    QString manufacturer;
    QString product;
    QString serial;

    QString version;
    int speed = 0;        // Mbps
    int maxPowerMA = 0;

    uint8_t deviceClass = 0;
    uint8_t deviceSubClass = 0;
    uint8_t deviceProtocol = 0;

    int busNum = 0;
    int devNum = 0;
    int rxLanes = 0;
    int txLanes = 0;
    QString removable;    // "removable", "fixed", "unknown"

    int numInterfaces = 0;
    int numConfigurations = 0;

    QList<UsbInterface> interfaces;
    QList<UsbDevice> children;

    bool isHub = false;
    bool isRootHub = false;

    QMap<QString, QString> rawAttributes;

    QString displayName() const;
    QString speedLabel() const;
    QString powerLabel() const;

    static QList<UsbDevice> enumerate();

private:
    static std::optional<UsbDevice> fromSysfs(const QString &path, const QString &name);
    static void buildTopology(QList<UsbDevice> &devices);
};

} // namespace WhatCable
