// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of VendorDB.swift + extended with common Linux USB vendors
#include "VendorDB.h"
#include <QMap>

namespace WhatCable {

static const QMap<uint16_t, QString> kVendors = {
    // Major USB-C / Thunderbolt cable and charger vendors (from original WhatCable)
    {0x05AC, QStringLiteral("Apple")},
    {0x0B95, QStringLiteral("ASIX Electronics")},
    {0x2109, QStringLiteral("VIA Labs")},
    {0x0BDA, QStringLiteral("Realtek")},
    {0x8087, QStringLiteral("Intel")},
    {0x04B4, QStringLiteral("Cypress")},
    {0x1D6B, QStringLiteral("Linux Foundation")},
    {0x1A40, QStringLiteral("Terminus Technology")},
    {0x2188, QStringLiteral("CalDigit")},
    {0x17EF, QStringLiteral("Lenovo")},
    {0x04E8, QStringLiteral("Samsung")},
    {0x0409, QStringLiteral("NEC")},
    {0x0835, QStringLiteral("Action Star Enterprise")},
    {0x291A, QStringLiteral("Anker")},
    {0x1BCF, QStringLiteral("Sunplus")},
    {0x2D01, QStringLiteral("Google")},
    {0x18D1, QStringLiteral("Google")},

    // Common USB peripheral vendors
    {0x046D, QStringLiteral("Logitech")},
    {0x1532, QStringLiteral("Razer")},
    {0x1B1C, QStringLiteral("Corsair")},
    {0x258A, QStringLiteral("Hailuck")},
    {0x0951, QStringLiteral("Kingston")},
    {0x090C, QStringLiteral("Silicon Motion")},
    {0x0781, QStringLiteral("SanDisk")},
    {0x0930, QStringLiteral("Toshiba")},
    {0x13FE, QStringLiteral("Phison")},
    {0x058F, QStringLiteral("Alcor Micro")},
    {0x05E3, QStringLiteral("Genesys Logic")},
    {0x0451, QStringLiteral("Texas Instruments")},
    {0x1058, QStringLiteral("Western Digital")},
    {0x0BC2, QStringLiteral("Seagate")},
    {0x174C, QStringLiteral("ASMedia")},
    {0x0438, QStringLiteral("AMD")},
    {0x1022, QStringLiteral("AMD")},
    {0x1B21, QStringLiteral("ASMedia")},
    {0x0955, QStringLiteral("NVIDIA")},
    {0x056A, QStringLiteral("Wacom")},
    {0x1050, QStringLiteral("Yubico")},
    {0x045E, QStringLiteral("Microsoft")},
    {0x03F0, QStringLiteral("HP")},
    {0x413C, QStringLiteral("Dell")},
    {0x04F2, QStringLiteral("Chicony")},
    {0x0C45, QStringLiteral("Microdia")},
    {0x0BDB, QStringLiteral("Ericsson")},
    {0x2C7C, QStringLiteral("Quectel")},
    {0x1199, QStringLiteral("Sierra Wireless")},
    {0x12D1, QStringLiteral("Huawei")},
    {0x22B8, QStringLiteral("Motorola")},
    {0x0E8D, QStringLiteral("MediaTek")},
    {0x0763, QStringLiteral("M-Audio")},
    {0x1235, QStringLiteral("Focusrite")},
    {0x1397, QStringLiteral("BEHRINGER")},
    {0x0582, QStringLiteral("Roland")},
    {0x1043, QStringLiteral("iCreate Technologies")},
    {0x262A, QStringLiteral("Savitech")},
    {0x37A8, QStringLiteral("Ultimate Gadget Laboratories")},
};

QString VendorDB::lookup(uint16_t vendorId)
{
    auto it = kVendors.find(vendorId);
    if (it != kVendors.end())
        return it.value();
    return QStringLiteral("0x%1").arg(vendorId, 4, 16, QChar('0'));
}

} // namespace WhatCable
