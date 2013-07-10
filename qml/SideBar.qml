import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    id: tagView
    anchors.top: mainView.top
    anchors.bottom: mainView.bottom
    anchors.left: mainView.left
    width: 200
    color: "black"

    Rectangle {
        z:1
        id: textInputBox
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 30
        border.color: "white"
        border.width: 1
        color: "black"
        TextInput {
            id: searchInput
            color: "white"
            anchors.fill: parent; anchors.leftMargin: 10
            font.pixelSize: 24
            onTextChanged: videoModel.setFilter(text)

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "Name search..."
                visible: !(parent.cursorVisible || parent.text != "")
                color: "white"
            }
        }
    }

    Rectangle {
        z:1
        id: tagInputBox
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: textInputBox.bottom
        height: 30
        border.color: "white"
        color: "black"
        border.width: 1
        TextInput {
            id: tagInput
            color: "white"
            anchors.fill: parent; anchors.leftMargin: 10
            font.pixelSize: 24
            onTextChanged: videoModel.setTagFilter(text)

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "Tag search..."
                visible: !(parent.cursorVisible || parent.text != "")
                color: "white"
            }
        }
    }

    ListView {
        id: availableTagsView
        anchors.top: tagInputBox.bottom
        anchors.bottom: rescanButton.top
        anchors.right: scrollbar.left
        anchors.left: parent.left
        model: videoModel.allTags
        delegate: Text {
            text: modelData
            color: "white";
            font.pointSize: 8
            renderType: Text.NativeRendering
            property bool selected: false
            font.bold: selected
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
        view: availableTagsView
        anchors.top: availableTagsView.top
        anchors.bottom: availableTagsView.bottom
    }



    Rectangle {
        id: setPathButton
        height: 25
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: rescanButton.top
        border.color: "white"
        border.width: 1
        color: "black"
        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: "Set path"
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: folderDialog.visible = true
        }

        FileDialog {
            id: folderDialog
            title: "Select collection folder"
            selectFolder: true
            folder: config.collectionPath
            onAccepted: {
                config.collectionPath = fileUrls[0]
                videoModel.rescan()
            }
        }
    }

    Rectangle {
        id: rescanButton
        height: 25
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: fullscreenButton.top
        border.color: "white"
        border.width: 1
        color: "black"
        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: "Rescan"
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: videoModel.rescan();
        }
    }

    Rectangle {
        id: fullscreenButton
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
            text: "Fullscreen"
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (config.fullscreen) {
                    mainWindow.showMaximized()
                    config.fullscreen = false
                } else {
                    mainWindow.showFullScreen()
                    config.fullscreen = true
                }
            }
        }
    }


    Behavior on width {
        SmoothedAnimation { duration: 1000; }
    }


}
