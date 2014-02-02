import QtQuick 2.0
import QtMultimedia 5.0
import QtGraphicalEffects 1.0

Item {
    id: gridItem
    width: gridView.width; height: gridView.height
    Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: 5
        id: rect
        color: "black"
        radius: 10
        onOpacityChanged: if (opacity == 0) { visible = false } else { visible = true }

        TitleText {
            id: titleText
        }

        Image {
            id: cover
            sourceSize: parent.size
            anchors.bottom: parent.bottom
            anchors.top: titleText.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            //anchors.bottomMargin: 15

            source: coverPath == "" ? "/images/defaultcover.png" : "file:/" + encodeURIComponent(coverPath)
            cache: false
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
            onOpacityChanged: {
                if (opacity == 0) visible = false
                else visible = true
            }

            ClickableArea {
                onClicked: {
                    videoPlayer.show()
                }
            }
        }

    }
}
