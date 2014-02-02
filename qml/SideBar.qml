import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    id: tagView
    anchors.top: mainView.top
    anchors.bottom: mainView.bottom
    anchors.left: mainView.left
    width: 200
    color: "black"
    onOpacityChanged: if (opacity == 0) { visible = false } else { visible = true }

    InputBox {
        z: 1
        id: textInputBox
        onTextChanged: videoModel.setFilter(text)
        helpText: "Name filter"
        anchors.top: parent.top
    }

    InputBox {
        z: 1
        id: tagInputBox
        anchors.topMargin: 20
        anchors.top: textInputBox.bottom
        onTextChanged: videoModel.setTagFilter(text)
        helpText: "Tag search"
    }

    ListView {
        id: availableTagsView
        anchors.top: tagInputBox.bottom
        anchors.bottom: setPathButton.top
        anchors.right: scrollbar.left
        anchors.left: parent.left
        model: videoModel.allTags
        delegate: Text {
            text: modelData
            color: "white";
            font.pointSize: 8
            property bool selected: videoModel.filterTagsContains(modelData)
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

    Button {
        id: setPathButton
        anchors.bottom: rescanButton.top
        text: "Set path..."
        onClicked: folderDialog.visible = true

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

    Button {
        id: rescanButton
        text: "Rescan"
        onClicked: videoModel.rescan();
        anchors.bottom: fullscreenButton.top
    }


    Button {
        id: fullscreenButton
        text: "Fullscreen"
        anchors.bottom: parent.bottom
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


    Behavior on width {
        SmoothedAnimation { duration: 1000; }
    }


}
