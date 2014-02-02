import QtQuick 2.0

Item {
    id: scrollBar

    property real position: view.visibleArea.yPosition
    property real pageSize: view.visibleArea.heightRatio
    property var view
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.top: parent.top
    width: 20
    onOpacityChanged: if (opacity == 0) { visible = false } else { visible = true }
    visible: view.visibleArea.heightRatio < 1


    Rectangle {
        id: thumb
        //color: "#77ffffff"
        color: "black"
        border.color: "white"
        y: scrollBar.position * (scrollBar.height-2) + 1
        width: parent.width
        height: Math.max(scrollBar.pageSize * (scrollBar.height-2), 30)
        Rectangle {
            id: hl
            anchors.centerIn: thumb
            height: 1
            width: parent.width
            color: "white"
        }
        MouseArea {
            hoverEnabled: true
            onEntered: { parent.color = "white"; hl.color = "black" }
            onExited: { parent.color = "black"; hl.color = "white" }
            anchors.fill: parent
            propagateComposedEvents: true
        }
    }


    MouseArea {
        anchors.fill: parent
        onMouseYChanged: {
            if (mouse.buttons & Qt.LeftButton) {
                if (mouse.y < 0) return;
                if (mouse.y > parent.height) return;

                var pos = mouse.y / height - parent.pageSize/2

                if (pos + parent.pageSize > 1) pos = 1 - parent.pageSize
                if (pos < 0) pos = 0
                view.contentY = pos * view.contentHeight
            }
        }
    }
}
