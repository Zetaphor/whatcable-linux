// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of ChargingDiagnostic.swift — identifies charging bottlenecks
#pragma once

#include <QString>
#include "PowerDelivery.h"
#include "CableInfo.h"
#include <optional>

namespace WhatCable {

struct ChargingDiagnostic {
    enum Bottleneck {
        NoCharger,
        ChargerLimit,
        CableLimit,
        DeviceLimit,
        Fine
    };

    Bottleneck bottleneck = NoCharger;
    QString summary;
    QString detail;
    bool isWarning = false;

    static std::optional<ChargingDiagnostic> evaluate(
        const PowerDeliveryPort &pdPort,
        const std::optional<CableInfo> &cable);
};

} // namespace WhatCable
