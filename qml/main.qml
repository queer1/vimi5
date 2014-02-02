import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtMultimedia 5.0


Image {
    id: mainView
    width: 640
    height: 480
    source: "images/bg2.png"
    fillMode: Image.Tile

    Keys.enabled: true

    Keys.onPressed: {
        if (!videoPlayer.visible) {
            if (event.key === Qt.Key_Escape) {
                Qt.quit()
            } else if (event.key === Qt.Key_Return) {
                videoPlayer.show();
                event.accepted = true;
                return;
            } else {
                event.accepted = false
                return
            }
        }

        if (!videoPlayer.visible) {
            event.accepted = false
            return
        }

        var seekAmount = 0
        event.accepted = true;
        if (event.key === Qt.Key_Space) {
            if (mediaPlayer.playbackState === MediaPlayer.PausedState) {
                mediaPlayer.play()
            } else {
                mediaPlayer.pause()
            }
            return;
        } else if (event.key === Qt.Key_Escape) {
            videoPlayer.hide()
            return;
        } else if (event.key === Qt.Key_S) {
            videoPlayer.screenshots();
            return;
        } else if (event.key === Qt.Key_C) {
            videoPlayer.cover();
            return;
        } else if (event.key === Qt.Key_N) {
            videoPlayer.next();
            return;
        } else if (event.key === Qt.Key_B) {
            videoPlayer.bookmark();
            return;
        } else if (event.key === Qt.Key_B) {
            videoPlayer.bookmark();
            return;
        } else if (event.key === Qt.Key_T) {
            videoPlayer.tags()
        } else if (event.key === Qt.Key_Left) {
            seekAmount = -3000
        } else if (event.key === Qt.Key_Right) {
            seekAmount = 3000
        } else if (event.key === Qt.Key_Up) {
            seekAmount = 30000
        } else if (event.key === Qt.Key_Down) {
            seekAmount = -30000
        } else if (event.key === Qt.Key_PageUp) {
            seekAmount = 300000
        } else if (event.key === Qt.Key_PageDown) {
            seekAmount = -300000
        } else {
            event.accepted = false
            return
        }

        videoPlayer.seek(seekAmount)
    }

    SideBar {
        id: sideBar
        opacity: 1 - videoPlayer.opacity
    }

    GridView {
        id: gridView
        anchors.left: sideBar.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: scrollbar.left
        model: videoModel
        cellHeight: 315
        cellWidth: 215
        highlight: RectangularGlow { visible: !videoPlayer.visible; color:"white"; glowRadius: 15; spread:0.001}
        delegate: VideoElement {
            width: gridView.cellWidth;
            height: gridView.cellHeight;
            opacity: 1 - videoPlayer.opacity
            property var info: model
        }

        focus: true

        Keys.forwardTo: parent
        displaced: Transition { SmoothedAnimation { properties: "x,y"; duration: 100 } }
        add: Transition { SmoothedAnimation { properties: "x,y"; from: 100; duration: 100 } }
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            hoverEnabled: true
            onPositionChanged: {
                var x = mouse.x + gridView.contentX
                var y = mouse.y + gridView.contentY
                var index = gridView.indexAt(x, y)
                if (index !== -1) gridView.currentIndex = index
            }
        }
    }

    ScrollBar {
        id: scrollbar
        view: gridView
        anchors.right: actressPanel.left
        opacity: 1 - videoPlayer.opacity
    }

    ActressPanel {
        id: actressPanel
        opacity: 1 - videoPlayer.opacity
    }

    VideoPlayer {
        id: videoPlayer
    }

    Rectangle {
        id: busyWidget
        visible: videoModel.busy
        anchors.fill: parent
        opacity: 0.9
        color: "black"

        Rectangle {
            id: spinnerContainer
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            height: 100
            width: 100
            border.color: "white"
            color: "transparent"
            Rectangle {
                smooth: true
                height: 10
                width: 100
                anchors.centerIn: parent
                color: "white"
                RotationAnimation on rotation {
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 1500
                    running: busyWidget.visible
                }
            }
        }

        Text {
            opacity: 1
            text: videoModel.status
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            color: "white"
            font.pointSize: 20
            font.bold: true
        }
    }
}
