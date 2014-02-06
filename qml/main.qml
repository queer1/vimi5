import QtQuick 2.0
import QtGraphicalEffects 1.0


Rectangle {
    id: mainView
    width: 640
    height: 480
    //source: "images/bg2.png"
    //fillMode: Image.Tile
    color: "black"

    Keys.enabled: true

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
            videoPlayer.togglePause()
            return;
        } else if (event.key === Qt.Key_Escape) {
            videoPlayer.hide()
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
        anchors.bottom: parent.bottom
        anchors.top: nameFilterInput.bottom
        anchors.left: sideBar.right
        anchors.right: scrollbar.left
        model: videoModel
        cellHeight: config.coverSize
        cellWidth: cellHeight * 2 / 3
        Behavior on cellHeight { SmoothedAnimation { duration: 200 } }
        boundsBehavior: Flickable.StopAtBounds
        flickDeceleration: 10000
        maximumFlickVelocity: 5000

        MouseArea {
            anchors.fill: parent
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
        highlight: RectangularGlow {
            visible: !videoPlayer.visible
            color:"white"
            glowRadius: 1
            spread:1
        }
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
        anchors.top: nameFilterInput.bottom
        anchors.bottom: parent.bottom
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
        helpText: "name search"
        anchors.top: parent.top
        anchors.left: sideBar.right
        anchors.right: actressPanel.left
        anchors.rightMargin: 2
        anchors.leftMargin: 2
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

    Rectangle {
        id: helpDialog
        anchors.centerIn: parent
        width: 800
        height: 500
        color: "black"
        border.color: "white"
        radius: 15
        opacity: 0
        visible: false
        onOpacityChanged: if (opacity < 0.1) visible = false; else visible = true

        Behavior on opacity { NumberAnimation { duration: 100; } }

        Text {
            id: helpTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 20
            text: "help"
            font.pointSize: 20
            font.bold: true
            color: "white"
        }

        Text {
            id: videoKeyboardTitle
            text: "keyboard shortcuts while playing video:"
            font.bold: true
            anchors.top: helpTitle.bottom
            anchors.left: parent.left
            anchors.topMargin: 10
            anchors.leftMargin: 20
            color: "white"
        }

        Text {
            id: keyboardHelp
            anchors.left: parent.left
            anchors.top: videoKeyboardTitle.bottom
            anchors.topMargin: 10
            anchors.leftMargin: 20
            color: "white"
            text: "\
escape:\tpause and go back to overview\n\
arrow right:\tskip forward 3 seconds\n\
arrow left:\tskip backward 3 seconds\n\
arrow up:\tskip forward 30 seconds\n\
arrow down:\tskip backward 30 seconds\n\
page up:\tskip forward 5 minutes\n\
page down:\tskip backward 5 minutes\n\
s:\tgenerate screenshot previews for seek bar\n\
t:\tedit tags\n\
b:\tadd bookmark at current position\n\
c:\tscreenshot video for cover at current position\n\
"
        }

        Text {
            id: helpTextTitle
            text: "general help:"
            font.bold: true
            anchors.top: helpTitle.bottom
            anchors.left: keyboardHelp.right
            anchors.topMargin: 10
            anchors.leftMargin: 20
            color: "white"
        }

        Text {
            anchors.left: keyboardHelp.right
            anchors.right: parent.right
            anchors.top: helpTextTitle.bottom
            wrapMode: Text.WordWrap
            anchors.topMargin: 10
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            color: "white"
            text: "\
Vimi assumes that each movie consists of a single directory, containing the \
individual video files, an optional cover image and an optional file named \
\"tags.txt\" containing tags for this move, one on each line.

When you add new tags in this application, these will also be written to the \"tags.txt\" file in \
the movie directory, as well as being written to the internal Vimi cache.

You can move around in the main view with the arrow keys, and start a video with enter. \
To quit the app, press escape while in the main view, or simply close the window.

You can also point Vimi to a folder with thumbs/headshot of your favourite starlets, then they \
will automatically be shown in the field to the right, and you can click on them to filter \
the videos by them (assuming the videos are tagged with their names).
"
        }
        MouseArea {
            anchors.fill: parent
            enabled: parent.visible
            onClicked: parent.opacity = 0
            cursorShape: "PointingHandCursor"
        }
    }

}
