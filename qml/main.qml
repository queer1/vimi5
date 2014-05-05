import QtQuick 2.0
import QtGraphicalEffects 1.0


Rectangle {
    id: mainView
    width: 640
    height: 480
    color: "black"

    Keys.enabled: true

    Keys.onReleased: {
        if (event.key === Qt.Key_Control) {
            videoPlayer.hideAllScreenshots()
            return
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Question || event.key === Qt.Key_H) {
            helpDialog.opacity = 1
            event.accepted = true
            return
        } else if (helpDialog.visible) {
            helpDialog.opacity = 0
            event.accepted = true
            return
        }

        if (event.key === Qt.Key_Q) {
            Qt.quit()
        }

        if (!videoPlayer.visible) {
            if (event.key === Qt.Key_Escape) {
                Qt.quit()
            } else if (event.key === Qt.Key_Return) {
                videoPlayer.show();
                event.accepted = true;
                return;
            } else if (event.text !== ""){
                nameFilterInput.addText(event.text)
                nameFilterInput.forceActiveFocus()
                return
            } else {
                event.accepted = false
                return
            }
        }

        if (!videoPlayer.visible) {
            event.accepted = false
            return
        }

        if (event.key === Qt.Key_Control) {
            videoPlayer.showAllScreenshots()
            event.accepted = true
            return
        }

        var seekAmount = 0
        event.accepted = true;
        if (event.key === Qt.Key_Space) {
            videoPlayer.togglePause()
            return;
        } else if (event.key === Qt.Key_Escape) {
            videoPlayer.escapePressed()
            return;
        } else if (event.key === Qt.Key_S) {
            videoPlayer.takeScreenshots();
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
        } else if (event.key === Qt.Key_1) {
            videoPlayer.setPlaybackSpeed(1)
        } else if (event.key === Qt.Key_2) {
            videoPlayer.setPlaybackSpeed(2)
        } else if (event.key === Qt.Key_3) {
            videoPlayer.setPlaybackSpeed(3)
        } else if (event.key === Qt.Key_4) {
            videoPlayer.setPlaybackSpeed(4)
        } else if (event.key === Qt.Key_5) {
            videoPlayer.setPlaybackSpeed(5)
        } else if (event.key === Qt.Key_Left) {
            seekAmount = -3000
        } else if (event.key === Qt.Key_Right) {
            seekAmount = 3000
        } else if (event.key === Qt.Key_Up) {
            seekAmount = 10000
        } else if (event.key === Qt.Key_Down) {
            seekAmount = -10000
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


    MouseArea {
        anchors.fill: gridView
        propagateComposedEvents: true
        hoverEnabled: true
        onPositionChanged: {
            var x = mouse.x + gridView.contentX
            var y = mouse.y + gridView.contentY
            var index = gridView.indexAt(x, y)
            if (index !== -1) {
                gridView.currentIndex = index
                cursorShape = Qt.PointingHandCursor
            } else cursorShape = Qt.ArrowCursor
        }
    }
    GridView {
        id: gridView
        anchors.bottom: parent.bottom
        anchors.top: nameFilterInput.bottom
        anchors.left: sideBar.right
        anchors.right: scrollbar.left
        model: videoModel.rescanning ? [] : videoModel
        cellHeight: config.coverSize * 1.618
        cellWidth: config.coverSize
        Behavior on cellHeight { SmoothedAnimation { duration: 200 } }
        boundsBehavior: Flickable.StopAtBounds
        flickDeceleration: 10000
        maximumFlickVelocity: 5000
        opacity: 1-videoPlayer.opacity
        onOpacityChanged: if (opacity == 0) { visible = false } else { visible = true }

        highlight: Rectangle {}
        /*highlight: RectangularGlow {
            visible: !videoPlayer.visible
            color:"white"
            glowRadius: 1
            spread:1
        }*/
        highlightMoveDuration: 0
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

    }


    ScrollBar {
        id: scrollbar
        view: gridView
        anchors.right: actressPanel.left
        anchors.top: gridView.top
        anchors.bottom: gridView.bottom
        opacity: 1 - videoPlayer.opacity
    }

    ActressPanel {
        id: actressPanel
        opacity: 1 - videoPlayer.opacity
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: hideShowStarletsButton.left
    }

    Button {
        id: hideShowStarletsButton
        text: config.starletsShow ?  "→\ns\nt\na\nr\nl\ne\nt\ns\n→" : "←\ns\nt\na\nr\nl\ne\nt\ns\n←"
        onClicked: config.starletsShow = !config.starletsShow
        anchors.top: actressPanel.top
        anchors.bottom: actressPanel.bottom
        anchors.left: undefined
        anchors.right: parent.right
        width: 15
    }


    InputBox {
        id: nameFilterInput
        onTextChanged: videoModel.setFilter(text)
        helpText: "filter names"
        anchors.top: parent.top
        anchors.left: sideBar.right
        anchors.right: randomize.left
        opacity: 1 - videoPlayer.opacity
    }

    Rectangle {
        id: randomize
        anchors.top: parent.top
        anchors.right: actressPanel.left
        anchors.bottom: nameFilterInput.bottom
        color: "black"
        border.color: "white"
        border.width: 1
        width: 100
        ClickableArea {
            onClicked: videoModel.setRandom(!videoModel.isRandom())
            onEntered: {
                randomizeLabel.color = "black"
                parent.color = "white"
            }
            onExited: {
                randomizeLabel.color = "white"
                parent.color = "black"
            }
        }
        Text {
            id: randomizeLabel
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: videoModel.random
            text: "random"
            color: "white"
            visible: (height > font.pointSize && width > 10) ? true : false
        }
    }

    VideoPlayer {
        id: videoPlayer
    }

    Item {
        anchors.fill: parent
        visible: videoModel.empty
        Rectangle {
            anchors.centerIn: parent
            width: 275
            height: 275
            radius: 25
            border.width: 10
            border.color: "#fff"
            color: "#222"

            RotationAnimation on rotation {
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 30000
            }
        }
        Text {
            color: "white"
            text: "coolwk's"
            anchors.bottom: logotext.top
            anchors.left: logotext.left
            font.pointSize: 13
        }

        Text {
            id: logotext
            anchors.centerIn: parent
            text: "vimi"
            font.pointSize: 30
            color: "white"
            font.bold: true
        }
        Text {
            anchors.top: logotext.bottom
            anchors.horizontalCenter: logotext.horizontalCenter
            anchors.topMargin: 20
            text: "no videos found,\nplease set path"
            color: "white"
            font.bold: true
        }
    }

    BusyWidget {
        id: busyWidget
        visible: videoModel.rescanning
        color: "#ee000000"
    }

    HelpDialog {
        id: helpDialog
    }

}
