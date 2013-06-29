import QtQuick 2.0


Rectangle {
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.left: parent.left
    height: 20
    color: "white"
    opacity: 1

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        color:"black"
        width: player.position * parent.width / player.duration

    }

    MouseArea {
        id: seekbarMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            seekbar.opacity = 1
        }
        onExited: {
            if (rect.state == "maximized") {
                seekbar.opacity = 0
            }
        }

        onClicked: player.seek(mouse.x * player.duration / seekbar.width)
    }
    Behavior on opacity {
        NumberAnimation { duration: 1000 }
    }
}
