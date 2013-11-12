import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    id: actressPanel
    anchors.top: mainView.top
    anchors.bottom: mainView.bottom
    anchors.right: mainView.right
    width: 200
    color: "black"

    Rectangle {
        z:1
        id: actressInputBox
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 30
        border.color: "white"
        color: "black"
        border.width: 1
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor
        }
        TextInput {
            id: tagInput
            color: "white"
            anchors.fill: parent; anchors.leftMargin: 10
            font.pixelSize: 24
            onAccepted: {
                config.addActress(text)
                text = ""
            }

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "Add actress..."
                visible: !(parent.cursorVisible || parent.text != "")
                color: "white"
            }
        }
    }


    ListView {
        id: actressList
        anchors.top: actressInputBox.bottom
        anchors.right: scrollbar.left
        anchors.left: parent.left
        anchors.bottom: setPathButton.top
        model: config.actresses
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

            property bool tried: false
            onStatusChanged: {
                if (status == Image.Error && !tried) {
                    tried = true
                    source = "/images/defaultactress.png"
                }
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
            Text {
                id: removeTagButton
                text: "x"
                color: "red"
                anchors.right: parent.right
                anchors.top: parent.top
                font.pointSize: 15
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: config.removeActress(modelData)
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
