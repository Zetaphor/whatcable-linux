import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

ColumnLayout {
    id: card
    spacing: Kirigami.Units.smallSpacing

    property bool expanded: false

    Rectangle {
        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.smallSpacing
        radius: Kirigami.Units.cornerRadius
        color: {
            if (model.category === "typec")
                return Qt.rgba(Kirigami.Theme.highlightColor.r,
                              Kirigami.Theme.highlightColor.g,
                              Kirigami.Theme.highlightColor.b, 0.08)
            return Qt.rgba(Kirigami.Theme.textColor.r,
                          Kirigami.Theme.textColor.g,
                          Kirigami.Theme.textColor.b, 0.04)
        }
        implicitHeight: cardContent.implicitHeight + Kirigami.Units.smallSpacing * 2

        ColumnLayout {
            id: cardContent
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing
            spacing: Kirigami.Units.smallSpacing

            // Header row
            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: model.iconName || "drive-removable-media-usb"
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    PlasmaComponents.Label {
                        text: model.headline || ""
                        font.bold: true
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    PlasmaComponents.Label {
                        text: model.subtitle || ""
                        opacity: 0.7
                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        visible: text.length > 0
                    }
                }

                // Expand/collapse button
                PlasmaComponents.ToolButton {
                    icon.name: card.expanded ? "arrow-up" : "arrow-down"
                    onClicked: card.expanded = !card.expanded
                    implicitWidth: Kirigami.Units.iconSizes.small * 2
                    implicitHeight: Kirigami.Units.iconSizes.small * 2
                }
            }

            // Charging diagnostic banner
            Rectangle {
                Layout.fillWidth: true
                visible: model.hasChargingDiag || false
                radius: Kirigami.Units.cornerRadius
                color: (model.chargingIsWarning || false)
                    ? Qt.rgba(1.0, 0.8, 0.0, 0.15)
                    : Qt.rgba(0.0, 0.8, 0.0, 0.15)
                implicitHeight: chargingRow.implicitHeight + Kirigami.Units.smallSpacing * 2

                RowLayout {
                    id: chargingRow
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: (model.chargingIsWarning || false) ? "dialog-warning" : "dialog-positive"
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        PlasmaComponents.Label {
                            text: model.chargingSummary || ""
                            font.bold: true
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                        }

                        PlasmaComponents.Label {
                            text: model.chargingDetail || ""
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                            opacity: 0.7
                            visible: text.length > 0
                        }
                    }
                }
            }

            // Bullet details (shown when expanded)
            ColumnLayout {
                Layout.fillWidth: true
                visible: card.expanded
                spacing: 2

                Repeater {
                    model: card.parent ? bullets : []

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        PlasmaComponents.Label {
                            text: "•"
                            opacity: 0.5
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                        }

                        PlasmaComponents.Label {
                            text: modelData
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            // PDO list (shown when expanded, USB-C only)
            PowerSourceList {
                Layout.fillWidth: true
                visible: card.expanded && (model.hasPowerDelivery || false)
                pdos: model.sourcePdos || []
            }
        }
    }
}
