// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of PDVDO.swift — USB PD 3.x VDO bit-field decoding
#include "PDDecoder.h"

namespace WhatCable {

QString productTypeLabel(ProductType type)
{
    switch (type) {
    case ProductType::Hub:          return QStringLiteral("USB Hub");
    case ProductType::Peripheral:   return QStringLiteral("USB Peripheral");
    case ProductType::PassiveCable: return QStringLiteral("Passive Cable");
    case ProductType::ActiveCable:  return QStringLiteral("Active Cable");
    case ProductType::AMA:          return QStringLiteral("Alternate Mode Adapter");
    case ProductType::VPD:          return QStringLiteral("VCONN-Powered Device");
    case ProductType::Other:        return QStringLiteral("Other");
    default:                        return QStringLiteral("Unknown");
    }
}

QString cableSpeedLabel(CableSpeed speed)
{
    switch (speed) {
    case CableSpeed::USB20:     return QStringLiteral("USB 2.0");
    case CableSpeed::USB32Gen1: return QStringLiteral("USB 3.2 Gen 1 (5 Gbps)");
    case CableSpeed::USB32Gen2: return QStringLiteral("USB 3.2 Gen 2 (10 Gbps)");
    case CableSpeed::USB4Gen3:  return QStringLiteral("USB4 Gen 3 (20/40 Gbps)");
    case CableSpeed::USB4Gen4:  return QStringLiteral("USB4 Gen 4 (40/80 Gbps)");
    default:                    return QStringLiteral("Unknown");
    }
}

int cableSpeedMaxGbps(CableSpeed speed)
{
    switch (speed) {
    case CableSpeed::USB20:     return 0;
    case CableSpeed::USB32Gen1: return 5;
    case CableSpeed::USB32Gen2: return 10;
    case CableSpeed::USB4Gen3:  return 40;
    case CableSpeed::USB4Gen4:  return 80;
    default:                    return 0;
    }
}

QString cableCurrentLabel(CableCurrent current)
{
    switch (current) {
    case CableCurrent::USBDefault: return QStringLiteral("USB Default");
    case CableCurrent::ThreeAmp:   return QStringLiteral("3A");
    case CableCurrent::FiveAmp:    return QStringLiteral("5A");
    default:                       return QStringLiteral("Unknown");
    }
}

double cableCurrentMaxAmps(CableCurrent current)
{
    switch (current) {
    case CableCurrent::ThreeAmp: return 3.0;
    case CableCurrent::FiveAmp:  return 5.0;
    default:                     return 0.9;
    }
}

// USB PD 3.x ID Header VDO layout:
// Bits 31:    USB comm capable as host
// Bits 30:    USB comm capable as device
// Bits 29-27: Product type (UFP)
// Bits 26:    Modal operation
// Bits 25-23: Product type (DFP)
// Bits 15-0:  Vendor ID
IDHeaderVDO decodeIDHeader(uint32_t vdo)
{
    IDHeaderVDO hdr;
    hdr.usbCommCapableAsHost = (vdo >> 31) & 1;
    hdr.usbCommCapableAsDevice = (vdo >> 30) & 1;
    hdr.modalOperation = (vdo >> 26) & 1;
    hdr.vendorId = static_cast<uint16_t>(vdo & 0xFFFF);

    uint8_t ufpBits = (vdo >> 27) & 0x7;
    switch (ufpBits) {
    case 1: hdr.ufpProductType = ProductType::Hub; break;
    case 2: hdr.ufpProductType = ProductType::Peripheral; break;
    case 3: hdr.ufpProductType = ProductType::PassiveCable; break;
    case 4: hdr.ufpProductType = ProductType::ActiveCable; break;
    case 5: hdr.ufpProductType = ProductType::AMA; break;
    case 6: hdr.ufpProductType = ProductType::VPD; break;
    default: hdr.ufpProductType = ProductType::Undefined; break;
    }

    uint8_t dfpBits = (vdo >> 23) & 0x7;
    switch (dfpBits) {
    case 1: hdr.dfpProductType = ProductType::Hub; break;
    case 2: hdr.dfpProductType = ProductType::Peripheral; break;
    default: hdr.dfpProductType = ProductType::Undefined; break;
    }

    return hdr;
}

// USB PD 3.x Cable VDO layout:
// Bits 2-0:   Speed
// Bits 4:     VBUS through cable
// Bits 6-5:   Current capability
// Bits 10-9:  Max VBUS voltage
CableVDO decodeCableVDO(uint32_t vdo, bool isActive)
{
    CableVDO cable;
    cable.isActive = isActive;

    uint8_t speedBits = vdo & 0x7;
    switch (speedBits) {
    case 0: cable.speed = CableSpeed::USB20; break;
    case 1: cable.speed = CableSpeed::USB32Gen1; break;
    case 2: cable.speed = CableSpeed::USB32Gen2; break;
    case 3: cable.speed = CableSpeed::USB4Gen3; break;
    case 4: cable.speed = CableSpeed::USB4Gen4; break;
    default: cable.speed = CableSpeed::USB20; break;
    }

    cable.vbusThroughCable = (vdo >> 4) & 1;

    uint8_t currentBits = (vdo >> 5) & 0x3;
    switch (currentBits) {
    case 0: cable.currentRating = CableCurrent::USBDefault; break;
    case 1: cable.currentRating = CableCurrent::ThreeAmp; break;
    case 2: cable.currentRating = CableCurrent::FiveAmp; break;
    default: cable.currentRating = CableCurrent::USBDefault; break;
    }

    uint8_t voltageBits = (vdo >> 9) & 0x3;
    switch (voltageBits) {
    case 0: cable.maxVbusVolts = 20; break;
    case 1: cable.maxVbusVolts = 30; break;
    case 2: cable.maxVbusVolts = 40; break;
    case 3: cable.maxVbusVolts = 50; break;
    }

    double amps = cableCurrentMaxAmps(cable.currentRating);
    cable.maxWatts = static_cast<int>(cable.maxVbusVolts * amps);

    return cable;
}

} // namespace WhatCable
