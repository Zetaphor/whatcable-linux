// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Port of VendorDB.swift — USB VID to vendor name lookup
#pragma once

#include <QString>
#include <cstdint>

namespace WhatCable {

class VendorDB {
public:
    static QString lookup(uint16_t vendorId);
};

} // namespace WhatCable
