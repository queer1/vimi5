import QtQuick 2.0

MouseArea {
    hoverEnabled: parent.visible
    enabled: parent.visible
    anchors.fill: parent
    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
}
