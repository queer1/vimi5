import QtQuick 2.0
import QtQuick.Controls 1.0


Image {
    id: mainView
    width: 640
    height: 480
    source: "images/bg2.png"
    fillMode: Image.Tile

    ScrollView {
        anchors.fill: parent
        GridView {
            id: gridView
            anchors.fill: parent
            model: videoModel
            cellHeight: 200
            cellWidth: 300
            delegate: VideoElement {}
            property real contentBottom: contentY + height
        }
    }
}
