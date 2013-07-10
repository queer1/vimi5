import QtQuick 2.0
import QtMultimedia 5.0

Rectangle {
    id: controlbar
    anchors.right: rect.right
    anchors.top: toolbar.bottom
    anchors.bottom: seekbar.top
    color: "black"
    property int maxWidth: 50


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
            text: "SS"
            font.bold: true
            font.pointSize: 20
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                videoModel.createScreenshots(player.source)
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
            text: "[C]"
            font.bold: true
            font.pointSize: 20
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                var bks = bookmarks[player.file]
                for (var i=0; i<bks.length; i++) {
                    if (bks[i] > player.position) {
                        player.seek(bks[i])
                        return
                    }
                }
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
            text: ">>"
            font.bold: true
            font.pointSize: 20
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                var bks = bookmarks[player.file]
                for (var i=0; i<bks.length; i++) {
                    if (bks[i] > player.position) {
                        player.seek(bks[i])
                        return
                    }
                }
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
            text: "*"
            font.bold: true
            font.pointSize: 20
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                videoModel.addBookmark(index, player.file, player.position)
                seekbarPeek.running = true
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
            text: "| |"
            font.bold: true
            font.pointSize: 20
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (player.playbackState == MediaPlayer.PlayingState) {
                    player.pause()
                    pauseButtonText.text = ">"
                } else {
                    player.play()
                    pauseButtonText.text = "| |"
                }
            }
        }
    }

    states: [
        State {
            name: "shown"
            PropertyChanges {
                target: controlbar
                width: maxWidth
                opacity: 0.75
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: controlbar
                width: 0
                opacity: 0
            }
        }
    ]
    state: "hidden"

    Behavior on opacity { NumberAnimation { duration: 1000 } }
    Behavior on width { SmoothedAnimation { duration: 1000 } }

}
