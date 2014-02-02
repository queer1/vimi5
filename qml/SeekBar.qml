import QtQuick 2.0

Rectangle {
    id: seekbar
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.left: parent.left
    height: 30
    //color: "#55000000"
    color: "black"
    border.color: "white"
    opacity: 0
    property int position: 0
    property int duration: 0
    property int index
    property string file: ""
    property var bookmarks
    visible: opacity > 0

    Rectangle {
        //radius: 10
        id: progressbar
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: 15
        color:"white"
        width: position * parent.width / duration
        Behavior on width { SmoothedAnimation { duration: 100; } }
        border.color: "white"
    }
    Text {
        // HAHAHA
        text: (position < 10000*60*60 ? "0" : "") +
              Math.floor(position/ (1000*60*60)) +
              ":" +
              (position % (1000*60*60) < 600000 ? "0" : "") +
              Math.floor((position % (1000*60*60)) / (1000*60)) +
              ":" +
              ((position % (1000*60*60))%(1000*60) < 10000 ? "0" : "") +
              Math.floor(((position%(1000*60*60))%(1000*60))/1000) +
              "/" +
              (duration < 10000*60*60 ? "0" : "") +
              Math.floor(duration/ (1000*60*60)) +
              ":" +
              (duration%(1000*60*60) < 600000 ? "0" : "") +
              Math.floor((duration%(1000*60*60))/ (1000*60)) +
              ":" +
              ((duration%(1000*60*60))%(1000*60) < 10000 ? "0" : "") +
              Math.floor(((duration%(1000*60*60))%(1000*60))/1000)
        anchors.top: progressbar.top
        anchors.bottom: progressbar.bottom
        anchors.left: progressbar.left
        anchors.leftMargin: 5
        verticalAlignment: Text.AlignVCenter
        //color: "#aaaaaa"
        color: "black"
        style: Text.Outline

        styleColor: "white"
        renderType: Text.NativeRendering
    }

    Repeater {
        id: bookmarkTicks
        function removeBookmark(bookmark) {
            videoModel.removeBookmark(index, file, bookmark)
        }

        model: bookmarks[videoPlayer.file]
        Rectangle {
            id: bookmarkTick
            width: 1
            color: "gray"
            x: modelData * seekbar.width / mediaPlayer.duration
            height: progressbar.height
            y: progressbar.y
            Text {
                anchors.bottom: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                text: "x"
                color: "red"
                font.pointSize: 20
                font.bold: true
                opacity: 1
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: bookmarkTicks.removeBookmark(modelData)
                }
            }
        }
    }

    Behavior on opacity {
        NumberAnimation { duration: 300 }
    }


    MouseArea {
        anchors.fill: parent
        enabled: parent.visible
        onClicked: {
            mediaPlayer.seek(mouse.x * mediaPlayer.duration / seekbar.width)
            videoModel.setLastPosition(index, mediaPlayer.position)
        }
    }
}

