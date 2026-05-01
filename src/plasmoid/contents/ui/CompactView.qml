import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore

MouseArea {
    id: compactRoot

    property int deviceCount: 0

    hoverEnabled: true

    Kirigami.Icon {
        anchors.fill: parent
        source: "drive-removable-media-usb"
        active: compactRoot.containsMouse
    }

    // Badge showing device count
    Rectangle {
        visible: deviceCount > 0
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: badgeText.implicitWidth + Kirigami.Units.smallSpacing * 2
        height: badgeText.implicitHeight + Kirigami.Units.smallSpacing
        radius: height / 2
        color: Kirigami.Theme.highlightColor

        Text {
            id: badgeText
            anchors.centerIn: parent
            text: compactRoot.deviceCount
            color: Kirigami.Theme.highlightedTextColor
            font.pointSize: Kirigami.Theme.smallFont.pointSize
            font.bold: true
        }
    }
}
