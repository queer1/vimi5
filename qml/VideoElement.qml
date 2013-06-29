import QtQuick 2.0
import QtMultimedia 5.0
import QtQuick.Controls 1.0

Item {
    id: gridItem
    width: 300
    height: 200
    Image {
        source: "images/bg.png"
        fillMode: Image.Tile
        id: rect
        anchors.top: gridItem.top
        anchors.bottom: gridItem.bottom
        anchors.left: gridItem.left
        anchors.right: gridItem.right
        anchors.margins: 5

        Text {
            z: 1
            id: titleText
            text: modelData.name
            color: "white"
            styleColor: "black"
            style: Text.Outline
            font.bold: true
            smooth: true

            elide: Text.ElideRight
            width: parent.width
            Behavior on color { ColorAnimation { duration: 500 } }
            anchors.top: rect.top
        }


        Text {
            z: 1
            id: tagText
            text: modelData.tagList
            color: "white"
            styleColor: "black"
            style: Text.Outline

            elide: Text.ElideRight
            width: parent.width
            Behavior on color { ColorAnimation { duration: 500 } }
            anchors.top: titleText.bottom
        }


        MediaPlayer {
            id: player
            source: modelData.path + "/" + modelData.files[0]
            autoPlay: false
            autoLoad: false
            onSeekableChanged: {
                if (modelData.lastPosition > 0)
                    seek(modelData.lastPosition)
                else
                    seek(duration / 2)

                if (!playing()) {
                    pause()
                }
            }
            onStatusChanged: {
                if (status != MediaPlayer.Loaded && status != MediaPlayer.Buffered) {
                    video.visible = false
                    cover.visible = true
                } else {
                    video.visible = true
                    cover.visible = false
                }
            }
            onPositionChanged: {
                if (position > 0)
                    modelData.setLastPosition(position)
            }
        }

        Image {
            id: cover
            anchors.fill: parent
            visible: false
            source: modelData.coverPath
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
                    target: tagText
                    color: "transparent"
                    styleColor: "transparent"
                }
                PropertyChanges {
                    target: titleText
                    color: "transparent"
                    styleColor: "transparent"
                }
                PropertyChanges {
                    target: seekbar
                    opacity: 0
                }
                PropertyChanges {
                    target: toolbar
                    shown: true
                    visible: true
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
                    target: tagText
                    color: "white"
                    styleColor: "black"
                }
                PropertyChanges {
                    target: titleText
                    color: "white"
                    styleColor: "black"
                }
                PropertyChanges {
                    target: seekbar
                    opacity: 1
                }
                PropertyChanges {
                    target: toolbar
                    shown: false
                    visible: false
                }
            }
        ]

        transitions:
            Transition {
            ParentAnimation {
                via: mainView
                SmoothedAnimation { properties: "x,y,width,height"; duration: 500 }
            }
        }

        VideoOutput {
            id: video
            source: player
            anchors.fill: rect

            property bool isVisible : y + height <= gridView.contentBottom

            onIsVisibleChanged: player.autoLoad = true

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
        }
        MouseArea {
            id: videoMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                player.play()
                parent.focus = true
            }
            onExited: {
                if (rect.state == "normal") {
                    pauseTimer.restart()
                }
            }
            onClicked: {
                if (rect.state == "maximized") {
                    rect.state = "normal"
                } else {
                    rect.state = "maximized"
                }
            }

            onMouseYChanged: {
                //transparentStop.position = (parent.height - mouse.y) / parent.height
                toolbar.opacity = (toolbar.height - mouse.y) / toolbar.height
            }

            Timer {
                id: pauseTimer
                interval: 100
                running: true
                onTriggered: {
                    if (!videoMouseArea.containsMouse && !seekbarMouseArea.containsMouse) {
                        player.pause()
                        parent.focus = false
                    }
                }
            }
        }

        Rectangle {
            id: seekbar
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

        Rectangle {
            id: toolbar
            width: 100
            height: 100
            anchors.top: rect.top
            anchors.left: rect.left
            anchors.right: rect.right
            gradient: Gradient {
                GradientStop { id: transparentStop; position: 1.0; color: "transparent" }
                GradientStop { position: 0.0; color: "black" }
            }

            property bool shown: false
            onShownChanged: {
                if (shown)
                    opacity = 0
                else
                    opacity = 100
            }

            ComboBox {
                id: fileSelector
                model: modelData.files
                width: parent.width
                onCurrentIndexChanged: {
                    if (currentText.length > 0)
                        player.source = modelData.path + "/" + currentText
                }
            }

            Behavior on opacity {
                NumberAnimation { duration: 250 }
            }
        }


        /*Column {
        id: tagText
        anchors.bottom: parent.bottom
        property color col: "white";
        Repeater {
            model: modelData.tags
            delegate: Text { text: modelData; color: parent.col;}
        }
        Behavior on col { ColorAnimation { duration: 500 } }
    }*/
    }
}
