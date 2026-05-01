#include "UsbClassDB.h"

namespace WhatCable {

QString UsbClassDB::className(uint8_t classCode)
{
    switch (classCode) {
    case 0x00: return QStringLiteral("Composite");
    case 0x01: return QStringLiteral("Audio");
    case 0x02: return QStringLiteral("Communications");
    case 0x03: return QStringLiteral("HID");
    case 0x05: return QStringLiteral("Physical");
    case 0x06: return QStringLiteral("Image");
    case 0x07: return QStringLiteral("Printer");
    case 0x08: return QStringLiteral("Mass Storage");
    case 0x09: return QStringLiteral("Hub");
    case 0x0A: return QStringLiteral("CDC Data");
    case 0x0B: return QStringLiteral("Smart Card");
    case 0x0D: return QStringLiteral("Content Security");
    case 0x0E: return QStringLiteral("Video");
    case 0x0F: return QStringLiteral("Personal Healthcare");
    case 0x10: return QStringLiteral("Audio/Video");
    case 0x11: return QStringLiteral("Billboard");
    case 0x12: return QStringLiteral("USB Type-C Bridge");
    case 0xDC: return QStringLiteral("Diagnostic");
    case 0xE0: return QStringLiteral("Wireless");
    case 0xEF: return QStringLiteral("Miscellaneous");
    case 0xFE: return QStringLiteral("Application Specific");
    case 0xFF: return QStringLiteral("Vendor Specific");
    default:   return QStringLiteral("0x%1").arg(classCode, 2, 16, QChar('0'));
    }
}

QString UsbClassDB::interfaceClassName(uint8_t classCode, uint8_t subClass)
{
    if (classCode == 0x01) {
        switch (subClass) {
        case 0x01: return QStringLiteral("Audio Control");
        case 0x02: return QStringLiteral("Audio Streaming");
        case 0x03: return QStringLiteral("MIDI Streaming");
        }
    }
    if (classCode == 0x03) {
        switch (subClass) {
        case 0x00: return QStringLiteral("HID (No Subclass)");
        case 0x01: return QStringLiteral("HID Boot Interface");
        }
    }
    if (classCode == 0x08) {
        switch (subClass) {
        case 0x01: return QStringLiteral("RBC");
        case 0x02: return QStringLiteral("MMC-5 (ATAPI)");
        case 0x04: return QStringLiteral("UFI");
        case 0x06: return QStringLiteral("SCSI");
        case 0x08: return QStringLiteral("UAS");
        }
    }
    if (classCode == 0x0E) {
        switch (subClass) {
        case 0x01: return QStringLiteral("Video Control");
        case 0x02: return QStringLiteral("Video Streaming");
        }
    }
    if (classCode == 0xE0) {
        switch (subClass) {
        case 0x01: return QStringLiteral("Bluetooth");
        case 0x02: return QStringLiteral("Wireless USB");
        }
    }
    return className(classCode);
}

} // namespace WhatCable
