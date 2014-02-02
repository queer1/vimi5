import QtQuick 2.0
Rectangle {
    id: toolbar
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    color: "#f0000000"
    border.color: "white"
    property var model
    height: fileList.count * titleText.contentHeight + 2
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
        font.pointSize: 15
        font.bold: false
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
            font.pointSize: titleText.font.pointSize
            font.underline: true
            text: modelData
            elide: Text.ElideRight
            ClickableArea {
                hoverEnabled: toolbar.file !== modelData
                onClicked: {
                    toolbar.file = modelData
                }
            }
        }
    }
    Behavior on opacity { NumberAnimation { duration: 300 } }
}

