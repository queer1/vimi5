import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    id: actressPanel
    anchors.top: mainView.top
    anchors.bottom: mainView.bottom
    anchors.right: mainView.right
    width: 200
    color: "black"

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
                renderType: Text.NativeRendering
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

    Rectangle {
        id: setPathButton
        height: 25
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        border.color: "white"
        border.width: 1
        color: "black"

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: "Set picture path"
            color: "white"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: folderDialog.visible = true
        }

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
