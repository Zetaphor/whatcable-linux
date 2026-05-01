// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Aggregates all USB and Type-C data into DeviceSummary list
#pragma once

#include <QObject>
#include <QList>
#include "DeviceSummary.h"
#include "UDevMonitor.h"

namespace WhatCable {

class DeviceManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int deviceCount READ deviceCount NOTIFY devicesChanged)
public:
    explicit DeviceManager(QObject *parent = nullptr);

    void refresh();
    void startMonitoring();

    const QList<DeviceSummary> &devices() const { return m_devices; }
    int deviceCount() const { return m_devices.size(); }

    QList<UsbDevice> rawUsbDevices() const { return m_usbDevices; }
    QList<TypeCPort> rawTypeCPorts() const { return m_typecPorts; }

signals:
    void devicesChanged();

private:
    void onDeviceChanged();

    UDevMonitor m_monitor;
    QList<DeviceSummary> m_devices;
    QList<UsbDevice> m_usbDevices;
    QList<TypeCPort> m_typecPorts;
    QList<PowerDeliveryPort> m_pdPorts;
};

} // namespace WhatCable
