import QtQuick 2.0
import QtMultimedia 5.0

MouseArea {
    hoverEnabled: parent.visible
    enabled: parent.visible
    anchors.fill: parent
    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
}
