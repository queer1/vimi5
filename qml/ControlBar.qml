import QtQuick 2.0
import QtMultimedia 5.0

Rectangle {
    id: controlbar
    color: "black"
    width: 200
    height: 500
    property var player
    property var bookmarks
    property int index
    property string file
    property string cover
    visible: false
    border.width: 1
    border.color: "white"
    onOpacityChanged: if (opacity < 0.1) { visible = false } else { visible = true }


    function bookmark() {
        videoModel.addBookmark(index, file, player.position)
    }
    function takeScreenshots() {
        videoModel.createScreenshots(player.source)
    }
    function next() {
        var bks = bookmarks[file]
        for (var i=0; i<bks.length; i++) {
            if (bks[i] > player.position + 5000) {
                player.seek(bks[i])
                return
            }
        }
    }
    function createCover() {
        videoModel.createCover(player.source, player.position*1000)
    }

    Image {
        anchors.bottom: skipButton.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: 5
        source: parent.cover
        anchors.top: parent.top
        fillMode: Image.PreserveAspectCrop
    }

    Button {
        id: skipButton
        anchors.bottom: coverButton.top
        border.color: "white"
            text: "create [s]creenshots"

            onClicked: {
                takeScreenshots()
            }
    }
    Button {
        id: coverButton
        anchors.bottom: generateScreenshotsButton.top
        text: "[c]reate cover"
        onClicked: {
            createCover()
        }

    }

    Button {
        id: generateScreenshotsButton
        anchors.bottom: bookmarkButton.top
        text: "[n]ext bookmark"
        onClicked: {
            next()
        }
    }

    Button {
        id: bookmarkButton
        anchors.bottom: pauseButton.top
        text: "add [b]ookmark"
        onClicked: {
            bookmark()
        }
    }

    Button {
        id: pauseButton
        anchors.bottom: parent.bottom
        text: (mediaPlayer.playbackState === MediaPlayer.PlayingState) ? "pause" : "play"
        onClicked: {
            if (mediaPlayer.playbackState === MediaPlayer.PlayingState) {
                mediaPlayer.pause()
            } else {
                mediaPlayer.play()
            }
        }
    }

    Behavior on opacity { NumberAnimation { duration: 300 } }
}
