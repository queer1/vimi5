import QtQuick 2.0

Image {
    id: mainView
    width: 640
    height: 480
    source: "images/bg2.png"
    fillMode: Image.Tile

    Keys.onEscapePressed: {
        Qt.quit()
    }

    Keys.onUpPressed: {
        if (gridView.contentY > 200)
            gridView.contentY -= 200
        else
            gridView.contentY = 0
    }
    Keys.onDownPressed: {
        if (gridView.contentY < gridView.contentHeight - 200 - gridView.height)
            gridView.contentY += 200
        else
            gridView.contentY = gridView.contentHeight - gridView.height
    }

    Keys.enabled: true
    focus: true

    Rectangle {
        id: tagView
        anchors.top: mainView.top
        anchors.bottom: mainView.bottom
        anchors.left: mainView.left
        width: 200
        color: "#99999999"
        Rectangle {
            id: textInputBox
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 30
            border.color: "steelblue"
            border.width: 5; radius: 10
            TextInput {
                id: searchInput
                anchors.fill: parent; anchors.leftMargin: 10
                font.pixelSize: 24
                onTextChanged: videoModel.setFilter(text)
            }
        }

        Rectangle {
            id: tagInputBox
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: textInputBox.bottom
            height: 30
            border.color: "steelblue"
            border.width: 5; radius: 10
            TextInput {
                id: tagInput
                anchors.fill: parent; anchors.leftMargin: 10
                font.pixelSize: 24
                onTextChanged: videoModel.setTagFilter(text)
            }
        }

        ListView {
            id: availableTagsView
            anchors.topMargin: 5
            anchors.top: tagInputBox.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            model: videoModel.allTags
            delegate: Text {
                text: modelData
                color: "black";
                styleColor: "white"
                font.pointSize: 8
                renderType: Text.NativeRendering
                style: Text.Outline
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

        /*ListModel {
            id: availableTags
        }
        ListModel {
            id: selectedTags
        }*/
    }



    GridView {
        id: gridView
        anchors.left: tagView.right
        anchors.bottom: mainView.bottom
        anchors.top: mainView.top
        anchors.right: scrollbar.left
        model: videoModel
        cellHeight: 300
        cellWidth: 200
        delegate: VideoElement { }

        Behavior on contentY {
            SmoothedAnimation { duration: 100 }
        }
    }
    ScrollBar {
        id: scrollbar
        position: gridView.visibleArea.yPosition
        pageSize: gridView.visibleArea.heightRatio

        MouseArea {
            anchors.fill: parent
            onMouseYChanged: {
                if (mouse.buttons & Qt.LeftButton) {
                    var pos = mouse.y / height - parent.pageSize/2
                    if (pos < 0) pos = 0
                    if (pos + parent.pageSize/2 > 1) pos = 1 - parent.pageSize/2
                    gridView.contentY = pos * gridView.contentHeight
                }
            }
        }
    }
}
