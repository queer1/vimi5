import QtQuick 2.0
import QtMultimedia 5.0
import QtQuick.Controls 1.0

Item {
    id: gridItem
    width: gridView.cellWidth
    height: gridView.cellHeight

    Rectangle {
        //source: "images/bg.png"
        //fillMode: Image.Tile
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
            Behavior on color { ColorAnimation { duration: 500 } }
            anchors.top: rect.top
            anchors.left: rect.left
            anchors.right: rect.right
            anchors.margins: 5
        }


        Text {
            z: 1
            id: tagText
            visible: false
            text: modelData.tagList
            color: "white"
            styleColor: "black"
            style: Text.Outline
            renderType: Text.NativeRendering

            elide: Text.ElideRight
            width: parent.width
            Behavior on color { ColorAnimation { duration: 500 } }
            anchors.top: titleText.bottom
        }


        MediaPlayer {
            id: player
            autoPlay: true
            //onAvailabilityChanged: console.log(availability)
            /*            onSeekableChanged: {

                if (!playing()) {
                    pause()
                }
            }*/

            onStatusChanged: {
                if (status == MediaPlayer.Loaded || status == MediaPlayer.Buffered) {
                    video.visible = true
                    cover.visible = false

                    if (!seekable)
                        return

                    if (modelData.lastPosition > 0)
                        seek(modelData.lastPosition)
                    else
                        seek(duration / 2)
                } else {
                    video.visible = false
                    cover.visible = true
                }
            }
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
                    shown: true
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
                    target: toolbar
                    shown: false
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

        transitions:
            Transition {
            ParentAnimation {
                via: mainView
                SmoothedAnimation { properties: "x,y,width,height"; duration: 500 }
            }
        }

        VideoOutput {
            visible: false
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
        }
        MouseArea {
            id: videoMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                //player.autoLoad = true
                if (player.status == MediaPlayer.NoMedia)
                    player.source = encodeURIComponent(modelData.path + "/" + modelData.lastFile)

                player.play()
                parent.focus = true
                if (rect.state == "normal")
                    seekbar.opacity = 0.5
                tagText.visible = true
                tagList.visible = false
            }
            onExited: {
                if (rect.state == "normal") {
                    pauseTimer.restart()
                    seekbar.opacity = 0
                    tagText.visible = false
                    tagList.visible = true
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
                toolbar.opacity = (toolbar.height - mouse.y) / toolbar.height
            }

            Timer {
                id: pauseTimer
                interval: 100
                running: false
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
            //anchors.bottomMargin: 10
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

            MouseArea {
                id: seekbarMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    seekbar.opacity = 0.5
                }
                onExited: {
                    if (rect.state == "maximized" || !player.playing()) {
                        seekbar.opacity = 0
                    }
                }

                onClicked: {
                    player.seek(mouse.x * player.duration / seekbar.width)
                    modelData.setLastPosition(player.position)
                }
            }
            Behavior on opacity {
                NumberAnimation { duration: 1000 }
            }
        }

        Rectangle {
            id: toolbar
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
                onCurrentTextChanged: {
                    var newUrl = encodeURIComponent(modelData.path + "/" + currentText)
                    if (newUrl !== player.source && player.source != "") {
                        modelData.setLastFile(currentText)
                        player.source = newUrl
                    }
                }
            }

            Behavior on opacity {
                NumberAnimation { duration: 250 }
            }
        }


        Rectangle {
            id: tagList
            anchors.top: titleText.bottom
            height: Math.min(taglistlist.count * 8 + 20, parent.height - titleText.height - 10)
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.margins: 5

            color: "#80000000"
            radius: 10
            ListView {
                id: taglistlist
                anchors.fill: parent

                anchors.margins: 10
                model: modelData.tags
                delegate: Text {
                    text: modelData;
                    color: "white";
                    styleColor: "black"
                    font.bold: true
                    font.pointSize: 8
                    renderType: Text.NativeRendering
                    style: Text.Outline
                }

            }
        }
    }
}
