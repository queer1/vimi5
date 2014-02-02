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
    property bool configShow: config.configShow


    InputBox {
        z: 1
        id: tagInputBox
        anchors.topMargin: 2
        anchors.top: parent.top
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
        height: configShow ? 25 : 0

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
        height: configShow ? 25 : 0
    }


    Button {
        id: fullscreenButton
        text: "Fullscreen"
        anchors.bottom: sizeSelector.top
        onClicked: {
            if (config.fullscreen) {
                mainWindow.showMaximized()
                config.fullscreen = false
            } else {
                mainWindow.showFullScreen()
                config.fullscreen = true
            }
        }
        height: configShow ? 25 : 0
    }

    Rectangle {
        id: sizeSelector
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: hideShowButton.top
        height: configShow ? 20 : 0
        color: "black"
        border.color: "white"
        visible: height > 0 ? true : false

        Behavior on height { SmoothedAnimation { duration: 200; } }

        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            x: gridView.cellHeight / 2 - 100
            width: 1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: config.coverSize = (mouse.x + 100) * 2
        }
        Text {
            anchors.fill: parent
            text: "Cover size"
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            visible: height > font.pointSize ? true : false
        }
    }


    Button {
        id: hideShowButton
        anchors.bottom: parent.bottom
        text: configShow ? "↓ config ↓" : "↑ config ↑"
        height: 15
        onClicked: config.configShow = !config.configShow
    }


    Behavior on width {
        SmoothedAnimation { duration: 1000; }
    }
}
