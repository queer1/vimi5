import QtQuick 2.0

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
        visible: opacity > 0
        border.width: 1
        border.color: "white"

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
            anchors.margins: 1

            source: coverPath == "" ? "/images/defaultcover.png" : "file:/" + encodeURIComponent(coverPath)
            cache: true
            //onSourceChanged: { cache = false; cache = true; }
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
                hoverEnabled: false
            }
        }

        Button {
            z: 1
            visible: coverPath === ""
            anchors.centerIn: parent
            anchors.right: undefined
            anchors.left: undefined
            width: parent.width - 10
            text: "generate cover"
            onClicked: videoModel.createCover("file:/" + encodeURIComponent(path + "/" + lastFile), -1)
        }
        Text {
            visible: resolution > 700
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: 2
            text: "HD"
            style: Text.Outline
            styleColor: "white"
            font.bold: true
            renderType: Text.NativeRendering
        }

        Text {
            visible: framerate > 50
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 2
            text: "60FPS"
            style: Text.Outline
            styleColor: "white"
            font.bold: true
            renderType: Text.NativeRendering
        }
    }
}
