# WhatCable-Linux

> **What can this USB cable actually do?**

A KDE Plasma 6 system tray widget and CLI tool that tells you, in plain English, what each USB device plugged into your Linux machine can actually do.

**WhatCable-Linux is a Linux port of [WhatCable](https://github.com/darrylmorley/whatcable), a macOS menu bar app by [Darryl Morley](https://github.com/darrylmorley).** This port expands the original USB-C focus to cover all USB devices, while preserving the rich USB-C Power Delivery diagnostics from the original.

## What it shows

### All USB devices
- **Device identity**: vendor, product name, serial number
- **Speed**: negotiated link speed (1.5 Mbps to 20 Gbps)
- **USB version**: 1.1, 2.0, 3.0, 3.1, 3.2
- **Power draw**: how much power the device is consuming
- **Device type**: HID, Audio, Mass Storage, Hub, etc.
- **Driver**: which kernel driver is handling the device
- **Topology**: hub hierarchy showing what's plugged into what

### USB-C ports (additional detail)
- **Port roles**: data role (host/device), power role (source/sink)
- **Cable e-marker info**: cable speed capability, current rating (3A/5A), active vs passive, cable vendor
- **Charger PDO list**: every voltage/current profile the charger advertises, with the active profile highlighted
- **Charging diagnostics**: identifies bottlenecks — cable limiting speed, charger undersized, etc.
- **Partner identity**: decoded from PD Discover Identity VDOs

## Install

### Build from source

```bash
# Install dependencies (Fedora)
sudo dnf install cmake extra-cmake-modules qt6-qtbase-devel qt6-qtdeclarative-devel \
    kf6-kirigami-devel kf6-ki18n-devel kf6-kcoreaddons-devel kf6-kpackage-devel \
    libplasma-devel plasma-workspace-devel systemd-devel

# Install dependencies (Arch/Manjaro)
sudo pacman -S cmake extra-cmake-modules qt6-base qt6-declarative \
    kirigami ki18n plasma-workspace systemd-libs kpackage

# Build
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build

# Or install just the plasmoid for your user
kpackagetool6 -t Plasma/Applet -i build/src/plasmoid
```

### CLI only

After building, the `whatcable-linux` binary is in `build/src/cli/`:

```bash
whatcable-linux              # human-readable summary of every USB device
whatcable-linux --json       # structured JSON output
whatcable-linux --watch      # stream updates as devices come and go
whatcable-linux --raw        # include raw sysfs attributes
whatcable-linux --version
whatcable-linux --help
```

## How it works

WhatCable-Linux reads three areas of the Linux sysfs virtual filesystem. No root access required for basic info:

| sysfs path | What it gives us |
|---|---|
| `/sys/bus/usb/devices/` | All USB devices: vendor, product, speed, power, class, interfaces, topology |
| `/sys/class/typec/` | USB-C port state: connection, roles, cable e-marker, partner identity |
| `/sys/class/usb_power_delivery/` | PD negotiation: PDO list from charger, active profile, PPS ranges |

Hotplug monitoring uses `libudev` to detect connect/disconnect events in real time.

Cable speed and power decoding follow the USB Power Delivery 3.x spec, ported from the original WhatCable's Swift implementation.

## Caveats

- **USB-C/PD data availability varies by hardware.** The Type-C connector class and USB PD sysfs interfaces depend on the kernel driver (UCSI, TCPM, platform-specific). Some systems expose full PD negotiation data; others expose only basic port info or nothing at all.
- **Cable e-marker info only appears for cables that carry one.** Same as the original — most USB-C cables under 60W are unmarked.
- **WhatCable trusts the e-marker.** Counterfeit or mis-flashed cables can lie about their capabilities.
- **Vendor name lookup is not exhaustive.** Common vendors are recognized; others show the hex VID.

## Credits

WhatCable-Linux is a port of [WhatCable](https://github.com/darrylmorley/whatcable) by [Darryl Morley](https://github.com/darrylmorley). The USB Power Delivery decoding logic, charging diagnostics, vendor database, and plain-English summary approach are derived from the original macOS app.

## License

[MIT](LICENSE)
