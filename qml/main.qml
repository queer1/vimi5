import QtQuick 2.0
import QtQuick.Controls 1.0


Image {
    id: mainView
    width: 640
    height: 480
    source: "images/bg2.png"
    fillMode: Image.Tile

    GridView {
        id: gridView
        anchors.left: mainView.left
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
