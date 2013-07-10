import QtQuick 2.0

Rectangle {
    id: seekbar
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.left: parent.left
    property string screenshot: ""
    height: 30
    color: "#55000000"
    opacity: 0


    Rectangle {
        //radius: 10
        id: progressbar
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: 15
        color:"white"
        width: player.position * parent.width / player.duration
    }
    Text {
        text: Math.floor(player.position/(1000*60*60)) + ":" + Math.floor((player.position%(1000*60*60))/(1000*60)) + ":" + Math.floor(((player.position%(1000*60*60))%(1000*60))/1000) + "/" + Math.floor(player.duration/(1000*60*60)) + ":" + Math.floor((player.duration%(1000*60*60))/(1000*60)) + ":" + Math.floor(((player.duration%(1000*60*60))%(1000*60))/1000)
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 5
        verticalAlignment: Text.AlignVCenter
        color: "#aaaaaa"
        style: Text.Outline
        renderType: Text.NativeRendering

        styleColor: "black"
    }


    Repeater {
        id: bookmarkTicks
        function removeBookmark(bookmark) {
            videoModel.removeBookmark(index, player.file, bookmark)
        }

        model: bookmarks[player.file]
        Rectangle {
            id: bookmarkTick
            width: 1
            color: "gray"
            x: modelData * seekbar.width / player.duration
            height: progressbar.height
            y: progressbar.y
            Text {
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                text: "x"
                color: "red"
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
        NumberAnimation { duration: 1000 }
    }

    Image {
        width: 100
        height: 100
        anchors.horizontalCenter: progressbar.right
        anchors.bottom: parent.top
        source: screenshot
    }
}

