// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
#include "UDevMonitor.h"
#include <QSocketNotifier>
#include <libudev.h>

namespace WhatCable {

UDevMonitor::UDevMonitor(QObject *parent)
    : QObject(parent)
{
}

UDevMonitor::~UDevMonitor()
{
    stop();
}

bool UDevMonitor::start()
{
    if (m_running)
        return true;

    m_udev = udev_new();
    if (!m_udev)
        return false;

    m_monitor = udev_monitor_new_from_netlink(m_udev, "udev");
    if (!m_monitor) {
        udev_unref(m_udev);
        m_udev = nullptr;
        return false;
    }

    udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "usb", nullptr);
    udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "typec", nullptr);
    udev_monitor_enable_receiving(m_monitor);

    m_fd = udev_monitor_get_fd(m_monitor);
    if (m_fd < 0) {
        udev_monitor_unref(m_monitor);
        udev_unref(m_udev);
        m_monitor = nullptr;
        m_udev = nullptr;
        return false;
    }

    auto *notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &UDevMonitor::onSocketActivated);

    m_running = true;
    return true;
}

void UDevMonitor::stop()
{
    if (!m_running)
        return;

    // QSocketNotifier destroyed with us via parent ownership
    if (m_monitor) {
        udev_monitor_unref(m_monitor);
        m_monitor = nullptr;
    }
    if (m_udev) {
        udev_unref(m_udev);
        m_udev = nullptr;
    }
    m_running = false;
    m_fd = -1;
}

void UDevMonitor::onSocketActivated()
{
    auto *dev = udev_monitor_receive_device(m_monitor);
    if (!dev)
        return;

    // We don't need to inspect the device details — just signal a refresh
    udev_device_unref(dev);
    emit deviceChanged();
}

} // namespace WhatCable
