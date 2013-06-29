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
            text: modelData.name
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
            autoPlay: true

            onStatusChanged: {
                if (status == MediaPlayer.Loaded || status == MediaPlayer.Buffered) {
                    video.opacity = 1
                    cover.opacity = 0

                    if (!seekable)
                        return

                    if (modelData.lastPosition > 0)
                        seek(modelData.lastPosition)
                    else
                        seek(duration / 2)
                } else {
                    video.opacity = 0
                    cover.opacity = 1
                }
            }
            Component.onDestruction: modelData.setLastPosition(position)
            onPaused: modelData.setLastPosition(position)
        }

        Image {
            id: cover
            anchors.bottom: parent.bottom
            anchors.top: titleText.bottom
            anchors.right: parent.right
            anchors.left: parent.left

            source: encodeURIComponent(modelData.coverPath)
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
                    height: 20
                }
            }
        ]

        transitions: [
            Transition { to: "maximized"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        ScriptAction { script: player.play() }
                        SmoothedAnimation { properties: "x,y,width,height,opacity"; duration: 500 }
                    }
                }
            },
            Transition { to: "normal"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        SmoothedAnimation { properties: "x,y,width,height,opacity"; duration: 500 }
                        ScriptAction { script: player.pause() }
                    }
                }
            }
        ]

        VideoOutput {
            opacity: 0
            id: video
            source: player
            anchors.fill: rect

            Keys.onPressed: {
                var seekAmount = 0
                if (event.key == Qt.Key_Left) {
                    seekAmount = -3000
                } else if (event.key == Qt.Key_Right) {
                    seekAmount = 3000
                } else if (event.key == Qt.Key_Up) {
                    seekAmount = 30000
                } else if (event.key == Qt.Key_Down) {
                    seekAmount = -30000
                }

                player.seek(player.position + seekAmount)
            }
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
                    seekbar.opacity = 0.5
                else if(mouse.y < toolbar.maxHeight)
                    toolbar.state = "shown"
                else if (mouse.x < 200)
                    tagList.state = "maximized"
                else {
                    toolbar.state = "hidden"
                    seekbar.opacity = 0
                    tagList.state = "hidden"
                }
            }

            onClicked: {
                if (rect.state == "maximized") {
                    if (mouse.y > rect.height - seekbar.height) {

                        player.seek(mouse.x * player.duration / seekbar.width)
                        modelData.setLastPosition(player.position)
                    } else {
                        rect.state = "normal"
                        tagList.state = "normal"
                    }
                } else {
                    rect.state = "maximized"
                    tagList.state = "hidden"
                    if (player.status == MediaPlayer.NoMedia)
                        player.source = encodeURIComponent(modelData.path + "/" + modelData.lastFile)
                }
            }
        }

        Rectangle {
            id: seekbar
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            height: 30
            color: "black"
            radius: 10
            opacity: 0

            Rectangle {
                //radius: 10
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                height: parent.height/3
                color:"white"
                width: player.position * parent.width / player.duration

            }

            Behavior on opacity {
                NumberAnimation { duration: 1000 }
            }
        }

        Toolbar {
            id: toolbar
            model: modelData.files
            onVideoChanged: {
                var newUrl = encodeURIComponent(modelData.path + "/" + toolbar.video)
                if (newUrl !== player.source && player.source != "") {
                    setLastFile(toolbar.video)
                    player.source = newUrl
                }
            }
        }

        TagList {
            id: tagList
        }
    }
}
