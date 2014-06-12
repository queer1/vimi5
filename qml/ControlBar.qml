import QtQuick 2.0

Rectangle {
    id: controlbar
    color: "black"
    width: 225
    property var player
    property var bookmarks
    property int index
    property string file
    property string cover
    property string videoName
    property string folderPath
    property string description
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

    Text {
        id: titleText
        anchors.right: parent.right
        anchors.left: parent.left
        height: 150
        anchors.top: parent.top
        anchors.margins: 5
        color: "white"
        text: parent.videoName
        font.pointSize: 15
        font.bold: true
        wrapMode: Text.Wrap
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                Qt.openUrlExternally("file://" + folderPath)
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Image {
        id: cover
        anchors.bottom: skipButton.top
        anchors.top: titleText.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: 5
        source: parent.cover
        fillMode: Image.PreserveAspectCrop
        opacity: 0.3
    }

    Text {
        anchors.fill: cover
        color: "white"
        opacity: 1
        text: description
        wrapMode: Text.Wrap
        elide: Text.ElideRight
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
        anchors.bottom: speedButtons.top
        text: (mediaPlayer.playbackState === MediaPlayer.PlayingState) ? "pause" : "play"
        onClicked: {
            if (mediaPlayer.playbackState === MediaPlayer.PlayingState) {
                mediaPlayer.pause()
            } else {
                mediaPlayer.play()
            }
        }
    }

    Item {
        id: speedButtons
        height: 25
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Text {
            id: speedLabel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 100
            color: "white"
            text: "playback speed:"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            id: speed1
            text: mediaPlayer.playbackRate == 1 ? "(1x)" : "1x"
            width: 25
            anchors.right: undefined
            anchors.left: speedLabel.right
            onClicked: videoPlayer.setPlaybackSpeed(1)
        }
        Button {
            id: speed2
            text: mediaPlayer.playbackRate == 2 ? "(2x)" : "2x"
            width: 25
            anchors.right: undefined
            anchors.left: speed1.right
            onClicked: videoPlayer.setPlaybackSpeed(2)
        }
        Button {
            id: speed3
            text: mediaPlayer.playbackRate == 3 ? "(3x)" : "3x"
            width: 25
            anchors.right: undefined
            anchors.left: speed2.right
            onClicked: videoPlayer.setPlaybackSpeed(3)
        }
        Button {
            id: speed4
            text: mediaPlayer.playbackRate == 4 ? "(4x)" : "4x"
            width: 25
            anchors.right: undefined
            anchors.left: speed3.right
            onClicked: videoPlayer.setPlaybackSpeed(4)
        }
        Button {
            id: speed5
            text: mediaPlayer.playbackRate == 5 ? "(5x)" : "5x"
            width: 25
            anchors.right: undefined
            anchors.left: speed4.right
            onClicked: videoPlayer.setPlaybackSpeed(5)
        }
    }

    Behavior on opacity { NumberAnimation { duration: 300 } }
}
