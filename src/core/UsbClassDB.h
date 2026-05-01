#pragma once

#include <QString>
#include <cstdint>

namespace WhatCable {

class UsbClassDB {
public:
    static QString className(uint8_t classCode);
    static QString interfaceClassName(uint8_t classCode, uint8_t subClass = 0);
};

} // namespace WhatCable
