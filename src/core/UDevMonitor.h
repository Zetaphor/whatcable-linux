// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// udev-based hotplug monitoring for USB and Type-C subsystems
#pragma once

#include <QObject>
#include <memory>

struct udev;
struct udev_monitor;

namespace WhatCable {

class UDevMonitor : public QObject {
    Q_OBJECT
public:
    explicit UDevMonitor(QObject *parent = nullptr);
    ~UDevMonitor() override;

    bool start();
    void stop();
    bool isRunning() const { return m_running; }

signals:
    void deviceChanged();

private slots:
    void onSocketActivated();

private:
    udev *m_udev = nullptr;
    udev_monitor *m_monitor = nullptr;
    bool m_running = false;
    int m_fd = -1;
};

} // namespace WhatCable
