import QtQuick 2.0
import QtMultimedia 5.0
import QtGraphicalEffects 1.0

Item {
    id: gridItem
    width: gridView.cellWidth; height: gridView.cellHeight
    Keys.onReturnPressed: rect.state = "maximized"
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
            verticalAlignment: Text.AlignVCenter
            color: "white"
            font.bold: true
            height: 40
            wrapMode: Text.WordWrap

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
                source = file == "" ? "" : "file:" + encodeURIComponent(model.path + "/" + file)
                videoModel.setLastFile(index, file)
            }
            autoPlay: true
            onPlaying: {
                video.opacity = 1
                cover.opacity = 0
                video.focus = true
                //screenshot.screenshots = model.screenshots
            }
            onStatusChanged: {
                if (status == MediaPlayer.EndOfMedia) {
                    seek(0)
                    play()
                }

            }
            onPaused: {
                videoModel.setLastPosition(index, position)
                gridView.focus = true
            }
        }

        Image {
            id: cover
            sourceSize: parent.size
            anchors.bottom: parent.bottom
            anchors.top: titleText.bottom
            anchors.right: parent.right
            anchors.left: parent.left

            source: coverPath == "" ? "../images/defaultcover.png": "file:/" + encodeURIComponent(coverPath)

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
                PropertyChanges { target: rect; radius: 0 }
                PropertyChanges { target: titleText; opacity: 0 }
                PropertyChanges { target: toolbar; visible: true }
                PropertyChanges { target: seekbar; opacity: 0; height: 50 }
                PropertyChanges { target: screenshotMask; visible: true }
                PropertyChanges { target: cover; opacity: 0 }
                PropertyChanges { target: video; opacity: 1 }
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
                PropertyChanges { target: titleText; opacity: 1 }
                PropertyChanges { target: toolbar; visible: false }
                PropertyChanges { target: rect; radius: 10 }
                PropertyChanges { target: seekbar; opacity: 0; height: 0 }
                PropertyChanges { target: screenshotMask; visible: false; opacity: 0 }
                PropertyChanges { target: cover; opacity: 1 }
                PropertyChanges { target: video; opacity: 0 }
            }
        ]

        transitions: [
            Transition { to: "maximized"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        ScriptAction { script: {
                                if (player.status == MediaPlayer.NoMedia) {
                                    player.file = model.lastFile
                                    player.seek(model.lastPosition)
                       //             screenshot.screenshots = model.screenshots
                                }
                                seekbarPeek.running = true
                                player.play()
                            }
                        }
                        SmoothedAnimation { properties: "x,y,width,height"; duration: 500 }
                        ScriptAction { script: { gridView.visible = false; sideBar.visible = false } }
                    }
                }
            },
            Transition { to: "normal"
                ParentAnimation { via: mainView
                    SequentialAnimation {
                        ScriptAction { script: {gridView.visible = true; sideBar.visible = true } }
                        SmoothedAnimation { properties: "x,y,width,height"; duration: 500 }
                        ScriptAction { script: { player.pause();  } }
                    }
                }
            }
        ]

        Keys.onPressed: {
            event.accepted = true;

            if (event.key == Qt.Key_Space) {
                if (player.playbackState == MediaPlayer.PausedState)
                    player.play()
                else
                    player.pause()

                return
            }

            var seekAmount = 0
            if (event.key == Qt.Key_Left) {
                seekAmount = -3000
            } else if (event.key == Qt.Key_Right) {
                seekAmount = 3000
            } else if (event.key == Qt.Key_Up) {
                seekAmount = 30000
            } else if (event.key == Qt.Key_Down) {
                seekAmount = -30000
            } else if (event.key == Qt.Key_PageUp) {
                seekAmount = 300000
            } else if (event.key == Qt.Key_PageDown) {
                seekAmount = -300000
            } else {
                event.accepted = false
                return
            }
            seekbarPeek.running = true

            var newPosition = player.position + seekAmount
            if (newPosition > player.duration) {
                newPosition %= player.duration
            }
            if (newPosition < 0) {
                newPosition = player.duration + newPosition
            }

            player.seek(newPosition)
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

            onEntered: {
                if (rect.state == "normal") tagList.opacity = 1
            }

            onExited: {
                if (rect.state == "normal") tagList.opacity = 0
            }

            onMouseXChanged: {
                if (rect.state == "normal")
                    return
                if (mouse.y > rect.height - seekbar.height){
                    seekbar.opacity = 1
                    screenshot.position = mouse.x
                    screenshot.y = mouse.y
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
                }
            }
        }

        Image {
            id: screenshot
            height: 200
            anchors.bottom: seekbar.top
            fillMode: Image.PreserveAspectFit
            property int position: 0
            x: position - (width * (position/parent.width))
            onXChanged: {
                var screenshots = model.screenshots
                if (screenshots === undefined) return;

                var file = player.file
                var position = screenshot.position * player.duration / parent.width
                var lastScreenshotPos = -1000000000
                var lastScreenshot =""
                for (var i=0; i<screenshots.length; i++) {
                    if (screenshots[i].indexOf(file) === -1)
                        continue

                    var screenshotPos = screenshots[i].split("_")[1]

                    if (screenshotPos > position) {
                        if (Math.abs(screenshotPos - position) < Math.abs(lastScreenshotPos - position)) {
                            screenshot.source = "file:" + encodeURIComponent(model.path + "/" + screenshots[i])
                        } else if (lastScreenshot !== ""){
                            screenshot.source = "file:" + encodeURIComponent(model.path + "/" + lastScreenshot)
                        }

                        screenshotFade.restart()
                        return
                    }
                    lastScreenshotPos = screenshotPos
                    lastScreenshot = screenshots[i]
                }
            }

            visible:false
            RadialGradient {
                id: mask
                anchors.fill: parent
                visible:false
                gradient: Gradient {
                    GradientStop { position: 0.3; color: "white" }
                    GradientStop { position: 0.5; color: "transparent" }
                }
            }
        }

        OpacityMask {
            id: screenshotMask
            source: screenshot
            maskSource: mask
            anchors.fill: screenshot
            opacity: seekbar.opacity
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
                    player.file = toolbar.video
            }
            videoName: name
        }

        TagList {
            id: tagList
            opacity: 0
        }
    }
}
