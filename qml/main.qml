import QtQuick 2.0
import QtQuick.Controls 1.0


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

        /*ListView {
            id: selectedTagsView
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
        }*/

        ListView {
            id: availableTagsView
            anchors.fill: parent
            model: videoModel.allTags
            delegate: Text {
                text: modelData
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
