import QtQuick 2.0
import QtMultimedia 5.0

Rectangle {
    id: controlbar
    color: "#55000000"
    width: 150
    height: 250
    property var player
    property var bookmarks
    property int index
    property string file
    visible: false
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
            if (bks[i] > player.position) {
                player.seek(bks[i])
                return
            }
        }
    }
    function cover() {
        videoModel.createCover(player.source, player.position*1000)
    }

    Rectangle {
        id: skipButton
        anchors.bottom: coverButton.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "black"
        border.width: 1
        border.color: "white"
        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "create thumbs"
            font.bold: true
            font.pointSize: 12
            color: "white"
        }
        ClickableArea {
            onClicked: {
                takeScreenshots()
            }
        }
    }
    Rectangle {
        id: coverButton
        anchors.bottom: generateScreenshotsButton.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "black"
        border.width: 1
        border.color: "white"
        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "create cover"
            font.bold: true
            font.pointSize: 12
            color: "white"
        }
        ClickableArea {
            onClicked: {
                cover()
            }
        }
    }

    Rectangle {
        id: generateScreenshotsButton
        anchors.bottom: bookmarkButton.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "black"
        border.width: 1
        border.color: "white"
        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "next bookmark"
            font.bold: true
            font.pointSize: 12
            color: "white"
        }
        ClickableArea {
            onClicked: {
                next()
            }
        }
    }

    Rectangle {
        id: bookmarkButton
        anchors.bottom: pauseButton.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "black"
        border.width: 1
        border.color: "white"
        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "add bookmark"
            font.bold: true
            font.pointSize: 12
            color: "white"
        }
        ClickableArea {
            onClicked: {
                bookmark()
            }
        }
    }

    Rectangle {
        id: pauseButton
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "black"

        border.width: 1
        border.color: "white"
        Text {
            id: pauseButtonText
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: (mediaPlayer.playbackState === MediaPlayer.PlayingState) ? "pause" : "play"
            font.bold: true
            font.pointSize: 12
            color: "white"
        }
        ClickableArea {
            onClicked: {
                if (mediaPlayer.playbackState === MediaPlayer.PlayingState) {
                    mediaPlayer.pause()
                } else {
                    mediaPlayer.play()
                }
            }
        }
    }

    Behavior on opacity { NumberAnimation { duration: 300 } }
}
