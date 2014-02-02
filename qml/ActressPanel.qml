import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    id: actressPanel
    anchors.top: mainView.top
    anchors.bottom: mainView.bottom
    anchors.right: mainView.right
    width: config.starletsShow ? 200 : 0
    color: "black"
    onOpacityChanged: if (opacity == 0) { visible = false } else { visible = true }

    Behavior on width { SmoothedAnimation { duration: 200; } }


    ListView {
        id: actressList
        anchors.top: parent.top
        anchors.right: scrollbar.left
        anchors.left: parent.left
        anchors.bottom: setPathButton.top
        model: videoModel.actresses
        delegate: Image {
            source: config.actressPath + "/" + modelData + ".jpg"
            width: parent.width
            fillMode: Image.PreserveAspectFit
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

    ScrollBar {
        id: scrollbar
        view: actressList
        anchors.top: actressList.top
        anchors.bottom: actressList.bottom
    }

    Button {
        id: setPathButton

        anchors.bottom: parent.bottom
        text: "Set picture path"
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
