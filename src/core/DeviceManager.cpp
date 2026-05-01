// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
#include "DeviceManager.h"
#include "CableInfo.h"
#include <QTimer>

namespace WhatCable {

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_monitor, &UDevMonitor::deviceChanged, this, &DeviceManager::onDeviceChanged);
}

void DeviceManager::startMonitoring()
{
    m_monitor.start();
    refresh();
}

void DeviceManager::onDeviceChanged()
{
    // Debounce: udev often fires multiple events for a single plug/unplug
    QTimer::singleShot(500, this, &DeviceManager::refresh);
}

void DeviceManager::refresh()
{
    m_devices.clear();

    // Enumerate all data sources
    m_usbDevices = UsbDevice::enumerate();
    m_typecPorts = TypeCPort::enumerate();
    m_pdPorts = PowerDeliveryPort::enumerate();

    // Build Type-C port summaries first
    for (const auto &tcPort : m_typecPorts) {
        std::optional<PowerDeliveryPort> pd;
        // Try to correlate PD port with Type-C port by port number
        for (const auto &pdPort : m_pdPorts) {
            // Heuristic: match by parent port number if available
            if (pdPort.parentPortNumber == tcPort.portNumber) {
                pd = pdPort;
                break;
            }
        }
        // If no match by number, and there's only one PD port and one TC port, pair them
        if (!pd && m_pdPorts.size() == 1 && m_typecPorts.size() == 1)
            pd = m_pdPorts[0];

        std::optional<CableInfo> cable;
        if (tcPort.cable)
            cable = CableInfo::fromTypeCCable(*tcPort.cable);

        m_devices.append(DeviceSummary::fromTypeCPort(tcPort, pd, cable));
    }

    // Build USB device summaries (skip root hubs)
    for (const auto &dev : m_usbDevices) {
        if (dev.isRootHub)
            continue;
        m_devices.append(DeviceSummary::fromUsbDevice(dev));
    }

    emit devicesChanged();
}

} // namespace WhatCable
