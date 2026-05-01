#include "DeviceModel.h"
#include "PDDecoder.h"
#include <QJsonArray>
#include <QJsonObject>

namespace WhatCable {

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&m_manager, &DeviceManager::devicesChanged, this, [this]() {
        rebuildFiltered();
    });
}

void DeviceModel::rebuildFiltered()
{
    beginResetModel();
    m_filtered.clear();
    for (const auto &dev : m_manager.devices()) {
        if (!m_showHubs && dev.category == DeviceSummary::HubCategory)
            continue;
        if (!m_showEmptyPorts && dev.status == DeviceSummary::Empty)
            continue;
        m_filtered.append(dev);
    }
    endResetModel();
    emit countChanged();
}

void DeviceModel::refresh()
{
    m_manager.refresh();
}

void DeviceModel::startMonitoring()
{
    m_manager.startMonitoring();
}

void DeviceModel::setShowHubs(bool v)
{
    if (m_showHubs == v) return;
    m_showHubs = v;
    rebuildFiltered();
    emit showHubsChanged();
}

void DeviceModel::setShowEmptyPorts(bool v)
{
    if (m_showEmptyPorts == v) return;
    m_showEmptyPorts = v;
    rebuildFiltered();
    emit showEmptyPortsChanged();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_filtered.size();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_filtered.size())
        return {};

    const auto &dev = m_filtered[index.row()];

    switch (role) {
    case HeadlineRole: return dev.headline;
    case SubtitleRole: return dev.subtitle;
    case IconRole: return dev.icon;
    case CategoryRole:
        return dev.category == DeviceSummary::TypeCPortCategory ? QStringLiteral("typec")
             : dev.category == DeviceSummary::HubCategory ? QStringLiteral("hub")
             : QStringLiteral("usb");
    case BulletsRole: return dev.bullets;
    case StatusRole: return static_cast<int>(dev.status);

    // USB device
    case VendorIdRole: return dev.usbDevice ? QStringLiteral("0x%1").arg(dev.usbDevice->vendorId, 4, 16, QChar('0')) : QString();
    case ProductIdRole: return dev.usbDevice ? QStringLiteral("0x%1").arg(dev.usbDevice->productId, 4, 16, QChar('0')) : QString();
    case SpeedRole: return dev.usbDevice ? dev.usbDevice->speed : 0;
    case SpeedLabelRole: return dev.usbDevice ? dev.usbDevice->speedLabel() : QString();
    case UsbVersionRole: return dev.usbDevice ? dev.usbDevice->version : QString();
    case MaxPowerRole: return dev.usbDevice ? dev.usbDevice->maxPowerMA : 0;
    case IsHubRole: return dev.usbDevice ? dev.usbDevice->isHub : false;
    case BusPortRole: return dev.usbDevice ? dev.usbDevice->busPort : QString();

    // Type-C
    case PortNumberRole: return dev.typecPort ? dev.typecPort->portNumber : -1;
    case DataRoleRole: return dev.typecPort ? dev.typecPort->currentDataRole() : QString();
    case PowerRoleRole: return dev.typecPort ? dev.typecPort->currentPowerRole() : QString();
    case IsConnectedRole: return dev.typecPort ? dev.typecPort->isConnected() : false;

    // Cable
    case CableSpeedRole: return (dev.cable && dev.cable->speed) ? cableSpeedLabel(*dev.cable->speed) : QString();
    case CableCurrentRole: return (dev.cable && dev.cable->currentRating) ? cableCurrentLabel(*dev.cable->currentRating) : QString();
    case CableMaxWattsRole: return dev.cable ? dev.cable->maxWatts : 0;
    case CableTypeRole: return dev.cable ? dev.cable->cableType : QString();
    case CableVendorRole: return dev.cable ? dev.cable->vendorName : QString();

    // Power delivery
    case HasPowerDeliveryRole: return dev.powerDelivery.has_value() && !dev.powerDelivery->sourceCapabilities.isEmpty();
    case SourcePdosRole: {
        if (!dev.powerDelivery) return QVariant();
        QJsonArray arr;
        for (const auto &pdo : dev.powerDelivery->sourceCapabilities) {
            QJsonObject obj;
            obj[QStringLiteral("voltage")] = pdo.voltageLabel();
            obj[QStringLiteral("current")] = pdo.currentLabel();
            obj[QStringLiteral("power")] = pdo.powerLabel();
            obj[QStringLiteral("type")] = pdo.typeLabel();
            obj[QStringLiteral("active")] = pdo.isActive;
            arr.append(obj);
        }
        return arr;
    }
    case MaxSourcePowerRole: return dev.powerDelivery ? dev.powerDelivery->maxSourcePowerMW / 1000 : 0;

    // Charging
    case HasChargingDiagRole: return dev.chargingDiag.has_value();
    case ChargingSummaryRole: return dev.chargingDiag ? dev.chargingDiag->summary : QString();
    case ChargingDetailRole: return dev.chargingDiag ? dev.chargingDiag->detail : QString();
    case ChargingIsWarningRole: return dev.chargingDiag ? dev.chargingDiag->isWarning : false;
    }

    return {};
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return {
        {HeadlineRole, "headline"},
        {SubtitleRole, "subtitle"},
        {IconRole, "iconName"},
        {CategoryRole, "category"},
        {BulletsRole, "bullets"},
        {StatusRole, "status"},
        {VendorIdRole, "vendorId"},
        {ProductIdRole, "productId"},
        {SpeedRole, "speed"},
        {SpeedLabelRole, "speedLabel"},
        {UsbVersionRole, "usbVersion"},
        {MaxPowerRole, "maxPower"},
        {IsHubRole, "isHub"},
        {BusPortRole, "busPort"},
        {PortNumberRole, "portNumber"},
        {DataRoleRole, "dataRole"},
        {PowerRoleRole, "powerRole"},
        {IsConnectedRole, "isConnected"},
        {CableSpeedRole, "cableSpeed"},
        {CableCurrentRole, "cableCurrent"},
        {CableMaxWattsRole, "cableMaxWatts"},
        {CableTypeRole, "cableType"},
        {CableVendorRole, "cableVendor"},
        {HasPowerDeliveryRole, "hasPowerDelivery"},
        {SourcePdosRole, "sourcePdos"},
        {MaxSourcePowerRole, "maxSourcePower"},
        {HasChargingDiagRole, "hasChargingDiag"},
        {ChargingSummaryRole, "chargingSummary"},
        {ChargingDetailRole, "chargingDetail"},
        {ChargingIsWarningRole, "chargingIsWarning"},
    };
}

} // namespace WhatCable
