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
    border.width: 1
    border.color: "white"


    InputBox {
        z: 1
        id: tagInputBox
        anchors.top: parent.top
        onTextChanged: videoModel.setTagFilter(text)
        helpText: "filter tags"
    }

    ListView {
        id: availableTagsView
        anchors.top: tagInputBox.bottom
        anchors.bottom: helpButton.top
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
        id: helpButton
        text: "help"
        onClicked: helpDialog.opacity = 1
        anchors.bottom: setPathButton.top
        height: configShow ? 25 : 0
    }

    Button {
        id: setPathButton
        anchors.bottom: rescanButton.top
        text: "set path..."
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
        text: "rescan"
        onClicked: videoModel.rescan();
        anchors.bottom: fullscreenButton.top
        height: configShow ? 25 : 0
    }


    Button {
        id: fullscreenButton
        text: "fullscreen"
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
        height: configShow ? 25 : 0
        color: "black"
        border.color: "white"
        visible: height > 0 ? true : false

        Behavior on height { SmoothedAnimation { duration: 200; } }

        Rectangle {
            id: sizePosition
         //   color: "black"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            x: gridView.cellHeight / 2 - 100
            width: 1
            color: "white"
            opacity: 1
        }

        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onClicked: config.coverSize = (mouse.x + 100) * 2
            onPositionChanged: if (pressed) config.coverSize = (mouse.x + 100) * 2
            onEntered: {
                parent.color = "white"
                sizePosition.color = "black"
            }
            onExited: {
                parent.color = "black"
                sizePosition.color = "white"
            }
        }
        Text {
            id: sizeLabel
            anchors.fill: parent
            text: "cover size"
            style: Text.Outline
            styleColor: parent.color
            color: sizePosition.color
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            visible: height > font.pointSize ? true : false
        }
    }


    Button {
        id: hideShowButton
        anchors.bottom: parent.bottom
        text: configShow ? "↓ toggle config ↓" : "↑ toggle config ↑"
        height: 15
        onClicked: config.configShow = !config.configShow
    }


    Behavior on width {
        SmoothedAnimation { duration: 1000; }
    }
}
