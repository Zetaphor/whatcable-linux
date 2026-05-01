// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of ChargingDiagnostic.swift
#include "ChargingDiagnostic.h"

namespace WhatCable {

std::optional<ChargingDiagnostic> ChargingDiagnostic::evaluate(
    const PowerDeliveryPort &pdPort,
    const std::optional<CableInfo> &cable)
{
    if (pdPort.sourceCapabilities.isEmpty())
        return std::nullopt;

    int chargerMaxW = pdPort.maxSourcePowerMW / 1000;
    if (chargerMaxW <= 0)
        return std::nullopt;

    ChargingDiagnostic diag;

    // Find active PDO power
    int activeW = 0;
    for (const auto &pdo : pdPort.sourceCapabilities) {
        if (pdo.isActive) {
            activeW = pdo.powerMW / 1000;
            break;
        }
    }
    if (activeW <= 0 && !pdPort.sourceCapabilities.isEmpty()) {
        // Fallback: use the highest PDO
        activeW = chargerMaxW;
    }

    int cableMaxW = 0;
    if (cable && cable->maxWatts > 0)
        cableMaxW = cable->maxWatts;

    if (cableMaxW > 0 && cableMaxW < chargerMaxW) {
        diag.bottleneck = CableLimit;
        diag.summary = QStringLiteral("Cable is limiting charging speed");
        diag.detail = QStringLiteral("Cable rated for %1W, but charger can deliver %2W")
                          .arg(cableMaxW).arg(chargerMaxW);
        diag.isWarning = true;
    } else if (activeW > 0 && activeW < chargerMaxW * 0.8) {
        diag.bottleneck = DeviceLimit;
        diag.summary = QStringLiteral("Charging at %1W").arg(activeW);
        diag.detail = QStringLiteral("Charging at %1W (charger can do up to %2W)")
                          .arg(activeW).arg(chargerMaxW);
        diag.isWarning = false;
    } else {
        diag.bottleneck = Fine;
        diag.summary = QStringLiteral("Charging well at %1W").arg(activeW > 0 ? activeW : chargerMaxW);
        diag.detail.clear();
        diag.isWarning = false;
    }

    return diag;
}

} // namespace WhatCable
