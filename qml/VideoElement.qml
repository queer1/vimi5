import QtQuick 2.0
import QtMultimedia 5.0
import QtGraphicalEffects 1.0

Item {
    id: gridItem
    width: gridView.cellWidth; height: gridView.cellHeight
    Rectangle {
        //source: "images/bg.png"
        //fillMode: Image.Tile
        parent: gridView
        id: rect
        anchors.top: gridItem.top
        anchors.bottom: gridItem.bottom
        anchors.left: gridItem.left
        anchors.right: gridItem.right
        anchors.margins: 5
        color: "black"
        radius: 10

        Text {
            z: 1
            id: titleText
            text: name
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            styleColor: "black"
            style: Text.Outline
            font.bold: true
            renderType: Text.NativeRendering

            elide: Text.ElideRight
            width: parent.width
            Behavior on color { ColorAnimation { duration: 1000 } }
            anchors.top: rect.top
            anchors.left: rect.left
            anchors.right: rect.right
            anchors.margins: 5
        }

        MediaPlayer {
            id: player

            property string file: ""
            onFileChanged: {
                source = "file:" + encodeURIComponent(model.path + "/" + file)
                videoModel.setLastFile(index, file)
                screenshot.screenshots = model.screenshots
                screenshot.screenshots.sort()

            }
            autoPlay: true
            onPlaying: {
                video.opacity = 1
                cover.opacity = 0
                video.focus = true
            }
            onStatusChanged: {
                if (status == MediaPlayer.Loaded || status == MediaPlayer.Buffered) {
                    if (!seekable)
                        return

                    seek(lastPosition)
                } else {

                }
            }
            onPaused: {
                videoModel.setLastPosition(index, position)
                video.opacity = 0
                cover.opacity = 1
            }
            onPositionChanged: {

            }
        }

        Image {
            id: cover
            sourceSize: parent.size
            anchors.bottom: parent.bottom
            anchors.top: titleText.bottom
            anchors.right: parent.right
            anchors.left: parent.left

            source: "file:/" + encodeURIComponent(coverPath)

            asynchronous: true
            fillMode: Image.PreserveAspectCrop
            onOpacityChanged: {
                if (opacity == 0) visible = false
                else visible = true
            }
            Behavior on opacity { NumberAnimation { duration: 1000 } }
        }

        state: "normal"
        states: [
            State {
                name: "maximized"
                ParentChange {
                    target: rect
                    parent: mainView
                    width: mainView.width
                    height: mainView.height
                    x: mainView.x
                    y: mainView.y
                }
                AnchorChanges {
                    target: rect
                    anchors.top: undefined
                    anchors.bottom: undefined
                    anchors.left: undefined
                    anchors.right: undefined
                }
                PropertyChanges {
                    target: rect
                    radius: 0
                }
                PropertyChanges {
                    target: titleText
                    opacity: 0
                }
                PropertyChanges {
                    target: toolbar
                    visible: true
                }
                PropertyChanges {
                    target: seekbar
                    opacity: 0
                    height: 50
                }
            },
            State {
                name: "normal"
                ParentChange {
                    target: rect
                    parent: gridItem
                    width: gridItem.width
                    height: gridItem.height
                    x: gridItem.x
                    y: gridItem.y
                }
                PropertyChanges {
                    target: titleText
                    opacity: 1
                }
                PropertyChanges {
                    target: toolbar
                    visible: false
                }
                PropertyChanges {
                    target: rect
                    radius: 10
                }
                PropertyChanges {
                    target: seekbar
                    opacity: 0
                    height: 0
                }
            }
        ]

        transitions: [
            Transition { to: "maximized"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        ScriptAction { script: {
                                seekbarPeek.running = true
                                player.play()
                            }
                        }
                        SmoothedAnimation { properties: "x,y,width,height,opacity"; duration: 500 }
                        ScriptAction { script: { gridView.visible = false; sideBar.visible = false } }
                    }
                }
            },
            Transition { to: "normal"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        ScriptAction { script: gridView.visible = true}
                        SmoothedAnimation { properties: "x,y,width,height,opacity"; duration: 500 }
                        ScriptAction { script: { player.pause(); sideBar.visible = true } }
                    }
                }
            }
        ]

        Keys.onPressed: {
            event.accepted = true;
            var seekAmount = 0
            if (event.key == Qt.Key_Left) {
                seekAmount = -3000
            } else if (event.key == Qt.Key_Right) {
                seekAmount = 3000
            } else if (event.key == Qt.Key_Up) {
                seekAmount = 60000
            } else if (event.key == Qt.Key_Down) {
                seekAmount = -60000
            } else if (event.key == Qt.Key_PageUp) {
                seekAmount = 300000
            } else if (event.key == Qt.Key_PageDown) {
                seekAmount = -300000
            } else {
                event.accepted = false
                return
            }
            seekbarPeek.running = true
            player.seek(player.position + seekAmount)
        }

        VideoOutput {
            opacity: 0
            id: video
            source: player
            anchors.fill: rect

            Behavior on opacity { NumberAnimation { duration: 1000 } }
        }
        MouseArea {
            id: videoMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onMouseXChanged: {
                if (rect.state == "normal")
                    return
                if (mouse.y > rect.height - seekbar.height){
                    seekbar.opacity = 1
                    screenshot.x = mouse.x
                }else if(mouse.y < toolbar.maxHeight)
                    toolbar.state = "shown"
                else if (mouse.x < 200)
                    tagList.state = "maximized"
                else if (mouse.x > rect.width - controls.maxWidth)
                    controls.state = "shown"
                else {
                    toolbar.state = "hidden"
                    seekbar.opacity = 0
                    tagList.state = "hidden"
                    controls.state = "hidden"
                }
            }

            onClicked: {
                if (rect.state == "maximized") {
                    if (mouse.y > rect.height - seekbar.height) {

                        player.seek(mouse.x * player.duration / seekbar.width)
                        videoModel.setLastPosition(index, player.position)
                    } else {
                        rect.state = "normal"
                        tagList.state = "normal"
                    }
                } else {
                    rect.state = "maximized"
                    tagList.state = "hidden"
                    if (player.status == MediaPlayer.NoMedia)
                        player.file = model.lastFile
                }
            }
        }

        Image {
            id: screenshot
            height: 100
            width: 100
            anchors.bottom: seekbar.top
            opacity: seekbar.opacity
            property var screenshots
            onXChanged: {
                var file = player.file
                var position = x *player.duration / seekbar.width
                for (var i=screenshots.length-1; i>=0; --i) {
                    if (screenshots[i].indexOf(file) == -1)
                        continue

                    if (screenshots[i].split("_")[1] > position) {
                        screenshot.source = "file:" + encodeURIComponent(model.path + "/" + screenshots[i])
                    }
                }
            }
        }

        SeekBar {
            id: seekbar

            PropertyAnimation { id: seekbarPeek; target: seekbar; property: "opacity"; from: 1; to: 0; duration: 1000 }
        }

        ControlBar {
            id: controls
        }

        Toolbar {
            id: toolbar
            model: files
            onVideoChanged: {
                if (toolbar.video != player.file && player.file != "") {
                    player.file = toolbar.video
                }
            }
        }

        TagList {
            id: tagList
        }
    }
}
