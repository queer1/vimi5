import QtQuick 2.0
Rectangle {
    id: toolbar
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    color: "#f0000000"
    border.color: "white"
    property var model
    height: (fileList.count+1) * titleText.contentHeight + 5
    property string videoName
    property string folderPath
    property string file
    opacity: 0

    Text {
        id: titleText
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 5
        color: "white"
        text: parent.videoName + ":"
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
        anchors.top: titleText.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.left: parent.left
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

