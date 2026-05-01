import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.components as PlasmaComponents
import org.kde.whatcable 1.0

PlasmoidItem {
    id: root

    DeviceModel {
        id: deviceModel
        showHubs: Plasmoid.configuration.showHubs || false
        showEmptyPorts: Plasmoid.configuration.showEmptyPorts || false
    }

    Component.onCompleted: {
        deviceModel.startMonitoring()
    }

    switchWidth: Kirigami.Units.gridUnit * 20
    switchHeight: Kirigami.Units.gridUnit * 24

    toolTipMainText: "WhatCable"
    toolTipSubText: {
        var count = deviceModel.count
        if (count === 0) return "No USB devices"
        return count + " USB device" + (count !== 1 ? "s" : "")
    }

    compactRepresentation: CompactView {
        deviceCount: deviceModel.count
        onClicked: root.expanded = !root.expanded
    }

    fullRepresentation: ColumnLayout {
        Layout.preferredWidth: Kirigami.Units.gridUnit * 22
        Layout.preferredHeight: Kirigami.Units.gridUnit * 28
        Layout.minimumWidth: Kirigami.Units.gridUnit * 16
        Layout.minimumHeight: Kirigami.Units.gridUnit * 12
        spacing: 0

        // Header
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing

            Kirigami.Icon {
                source: "drive-removable-media-usb"
                Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
            }

            PlasmaComponents.Label {
                text: "WhatCable"
                font.bold: true
                Layout.fillWidth: true
            }

            PlasmaComponents.Label {
                text: deviceModel.count + " device" + (deviceModel.count !== 1 ? "s" : "")
                opacity: 0.6
                font.pointSize: Kirigami.Theme.smallFont.pointSize
            }

            PlasmaComponents.ToolButton {
                icon.name: "view-refresh"
                onClicked: deviceModel.refresh()
                PlasmaComponents.ToolTip { text: "Refresh" }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        // Device list
        PlasmaComponents.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: deviceListView
                model: deviceModel
                spacing: Kirigami.Units.smallSpacing

                delegate: DeviceCard {
                    width: deviceListView.width
                }

                PlasmaComponents.Label {
                    anchors.centerIn: parent
                    visible: deviceModel.count === 0
                    text: "No USB devices found"
                    opacity: 0.5
                }
            }
        }
    }
}
