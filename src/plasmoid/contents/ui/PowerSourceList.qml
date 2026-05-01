import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

ColumnLayout {
    id: pdoList

    property var pdos: []

    spacing: 2

    PlasmaComponents.Label {
        text: "Charger Profiles"
        font.bold: true
        font.pointSize: Kirigami.Theme.smallFont.pointSize
        Layout.topMargin: Kirigami.Units.smallSpacing
    }

    Repeater {
        model: pdoList.pdos

        Rectangle {
            Layout.fillWidth: true
            radius: Kirigami.Units.cornerRadius / 2
            color: modelData.active
                ? Qt.rgba(Kirigami.Theme.highlightColor.r,
                          Kirigami.Theme.highlightColor.g,
                          Kirigami.Theme.highlightColor.b, 0.12)
                : "transparent"
            implicitHeight: pdoRow.implicitHeight + 4

            RowLayout {
                id: pdoRow
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.smallSpacing
                anchors.rightMargin: Kirigami.Units.smallSpacing
                spacing: Kirigami.Units.smallSpacing

                PlasmaComponents.Label {
                    text: modelData.active ? "▸" : " "
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    Layout.preferredWidth: Kirigami.Units.gridUnit
                }

                PlasmaComponents.Label {
                    text: modelData.voltage + " @ " + modelData.current
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    Layout.fillWidth: true
                }

                PlasmaComponents.Label {
                    text: modelData.power
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.bold: modelData.active
                    opacity: modelData.active ? 1.0 : 0.7
                }

                PlasmaComponents.Label {
                    text: modelData.type
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    opacity: 0.5
                }
            }
        }
    }
}
