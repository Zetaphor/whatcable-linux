// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of PortSummary.swift — generates human-readable summary per device/port
#pragma once

#include <QString>
#include <QStringList>
#include "UsbDevice.h"
#include "TypeCPort.h"
#include "PowerDelivery.h"
#include "CableInfo.h"
#include "ChargingDiagnostic.h"
#include <optional>

namespace WhatCable {

struct DeviceSummary {
    enum Category { UsbDeviceCategory, TypeCPortCategory, HubCategory };
    enum Status { Empty, Connected, Charging, DataDevice, ThunderboltDevice, Unknown };

    Category category = UsbDeviceCategory;
    Status status = Empty;

    QString headline;
    QString subtitle;
    QStringList bullets;
    QString icon;

    // From USB device (always present for USB devices)
    std::optional<UsbDevice> usbDevice;

    // From Type-C (present only for USB-C ports)
    std::optional<TypeCPort> typecPort;
    std::optional<PowerDeliveryPort> powerDelivery;
    std::optional<CableInfo> cable;
    std::optional<ChargingDiagnostic> chargingDiag;

    static DeviceSummary fromUsbDevice(const UsbDevice &dev);
    static DeviceSummary fromTypeCPort(
        const TypeCPort &port,
        const std::optional<PowerDeliveryPort> &pd,
        const std::optional<CableInfo> &cable);
};

} // namespace WhatCable
