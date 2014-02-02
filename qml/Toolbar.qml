import QtQuick 2.0
Rectangle {
    id: toolbar
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    color: "black"
    property var model
    height: fileList.count * 20 + 20
    property string videoName
    property string folderPath
    property string file
    opacity: 0

    Text {
        id: titleText
        anchors.right: parent.right
        anchors.top: parent.top
        color: "white"
        text: parent.videoName
        font.pixelSize: 15
        font.bold: true
        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                Qt.openUrlExternally("file://" + folderPath)
            }
            cursorShape: Qt.PointingHandCursor
        }
    }


    ListView {
        id: fileList
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        model: parent.model
        delegate: Text {
            id: fileEntry
            color: "white"
            font.bold: file === modelData
            font.pixelSize: 15
            text: " ● " + modelData
            elide: Text.ElideRight
            ClickableArea {
                hoverEnabled: file === modelData
                onClicked: {
                    file = modelData
                }
            }
        }
    }
    Behavior on opacity { NumberAnimation { duration: 300 } }
}

