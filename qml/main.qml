import QtQuick 2.0
import QtGraphicalEffects 1.0

Image {
    id: mainView
    width: 640
    height: 480
    source: "images/bg2.png"
    fillMode: Image.Tile

    Keys.onEscapePressed: {
        Qt.quit()
    }

    Keys.onUpPressed: {
        if (gridView.contentY > 200)
            gridView.contentY -= 200
        else
            gridView.contentY = 0
    }
    Keys.onDownPressed: {
        if (gridView.contentY < gridView.contentHeight - 200 - gridView.height)
            gridView.contentY += 200
        else
            gridView.contentY = gridView.contentHeight - gridView.height
    }

    Keys.enabled: true
    focus: true

    SideBar {
        id: sideBar
    }

    GridView {
        id: gridView
        anchors.left: sideBar.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        model: videoModel
        cellHeight: 300
        cellWidth: 200
        delegate: VideoElement { }

        ScrollBar {
            id: scrollbar
            position: parent.visibleArea.yPosition
            pageSize: parent.visibleArea.heightRatio
        }
    }

    Rectangle {
        id: notification
        height: 100
        width: 200
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
        color: "black"
        radius: 25
        smooth: true
        opacity: 0

        Text {
            color: "white"
            anchors.fill: parent
            anchors.margins: 25
            font.pointSize: 12
            text: videoModel.status
            onTextChanged: {
                parent.opacity = 1
                notifyTimer.restart()
            }
            wrapMode: Text.WordWrap
            elide: Text.ElideRight

        }

        RectangularGlow {
            z:-1
            anchors.fill: notification
            glowRadius: 10
            spread: 0.2
            color: "white"
            cornerRadius: notification.radius + glowRadius
        }

        Timer {
            id: notifyTimer
            interval: 1000
            onTriggered: parent.opacity = 0
        }

        Behavior on opacity { NumberAnimation { duration: 1000; } }
    }


}
