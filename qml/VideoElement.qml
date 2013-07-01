import QtQuick 2.0
import QtMultimedia 5.0
import QtQuick.Controls 1.0

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
            }
            autoPlay: true
            onPlaying: video.focus = true
            onStatusChanged: {
                if (status == MediaPlayer.Loaded || status == MediaPlayer.Buffered) {
                    video.opacity = 1
                    cover.opacity = 0

                    if (!seekable)
                        return

                    seek(lastPosition)
                } else {
                    video.opacity = 0
                    cover.opacity = 1
                }
            }
            //Component.onDestruction: videoModel.setLastPosition(index, position)
            onPaused: videoModel.setLastPosition(index, position)
        }

        Image {
            id: cover
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
                    color: "transparent"
                    styleColor: "transparent"
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
                    color: "white"
                    styleColor: "black"
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
                        ScriptAction { script: gridView.visible = false }
                    }
                }
            },
            Transition { to: "normal"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        ScriptAction { script: gridView.visible = true}
                        SmoothedAnimation { properties: "x,y,width,height,opacity"; duration: 500 }
                        ScriptAction { script: player.pause() }
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
                if (mouse.y > rect.height - seekbar.height)
                    seekbar.opacity = 1
                else if(mouse.y < toolbar.maxHeight)
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

        Rectangle {
            id: seekbar
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            height: 30
            color: "#55000000"
            opacity: 0

            Rectangle {
                //radius: 10
                id: progressbar
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                height: 15
                color:"white"
                width: player.position * parent.width / player.duration
            }
            Text {
                text: Math.floor(player.position/(1000*60*60)) + ":" + Math.floor((player.position%(1000*60*60))/(1000*60)) + ":" + Math.floor(((player.position%(1000*60*60))%(1000*60))/1000) + "/" + Math.floor(player.duration/(1000*60*60)) + ":" + Math.floor((player.duration%(1000*60*60))/(1000*60)) + ":" + Math.floor(((player.duration%(1000*60*60))%(1000*60))/1000)
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter
                color: "#aaaaaa"
                style: Text.Outline
                renderType: Text.NativeRendering

                styleColor: "black"
            }


            Repeater {
                id: bookmarkTicks
                function removeBookmark(bookmark) {
                    videoModel.removeBookmark(index, player.file, bookmark)
                }

                model: bookmarks[player.file]
                Rectangle {
                    id: bookmarkTick
                    width: 1
                    color: "gray"
                    x: modelData * seekbar.width / player.duration
                    height: progressbar.height
                    y: progressbar.y
                    Text {
                        anchors.top: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "x"
                        color: "red"
                        opacity: 1
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: bookmarkTicks.removeBookmark(modelData)
                        }
                    }
                }
            }

            PropertyAnimation { id: seekbarPeek; target: seekbar; property: "opacity"; from: 1; to: 0; duration: 1000 }

            Behavior on opacity {
                NumberAnimation { duration: 1000 }
            }
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
