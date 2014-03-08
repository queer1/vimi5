import QtQuick 2.0
import QtMultimedia 5.0

Rectangle {
    id: controlbar
    color: "transparent"
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
                createCover()
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
