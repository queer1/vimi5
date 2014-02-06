import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    id: actressPanel
    width: config.starletsShow ? 200 : 0
    color: "black"
    border.color: "white"
    visible: width != 0
    onOpacityChanged: if (opacity == 0) { visible = false } else { visible = true }

    Behavior on width { SmoothedAnimation { duration: 200; } }

    ListView {
        id: actressList
        anchors.top: filter.bottom
        anchors.right: scrollbar.left
        anchors.left: parent.left
        anchors.bottom: setPathButton.top
        anchors.margins: 15
        model: videoModel.actresses
        delegate: Image {
            source: config.actressPath + "/" + modelData + ".jpg"
            width: parent.width
            fillMode: Image.PreserveAspectFit
            visible: modelData.indexOf(filter.text) !== -1
            Text {
                text: modelData
                color: "white";
                styleColor: "black"
                style: Text.Outline
                font.pointSize: 8
                font.bold: selected
            }
            property bool selected: videoModel.filterTagsContains(modelData)
            MouseArea {
                enabled: actressPanel.visible
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (parent.selected) {
                        videoModel.removeFilterTag(modelData)
                    } else {
                        videoModel.addFilterTag(modelData)
                    }
                    parent.selected = !parent.selected
                }
            }
        }
    }


    InputBox {
        id: filter
        helpText: "filter"
    }

    ScrollBar {
        id: scrollbar
        view: actressList
        anchors.top: actressList.top
        anchors.bottom: setPathButton.top
    }

    Button {
        id: setPathButton

        anchors.bottom: parent.bottom
        text: "set picture path..."
        onClicked: folderDialog.visible = true

        FileDialog {
            id: folderDialog
            title: "Select starlet folder"
            selectFolder: true
            folder: config.actressPath
            onAccepted: {
                config.actressPath = fileUrls[0]
            }
        }
    }
}
