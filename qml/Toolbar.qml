import QtQuick 2.0
Rectangle {
    id: toolbar
    anchors.top: parent.top
    color: "#f0000000"
    border.color: "white"
    property var model
    property var screenshots
    height: Math.min((fileList.count+1) * 30 + 5, parent.height / 2)//titleText.contentHeight + 5
    property string videoName
    property string folderPath
    property string file
    opacity: 0
    Behavior on opacity { NumberAnimation { duration: 300 } }

    Text {
        id: titleText
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 5
        color: "white"
        text: parent.videoName + ":"
        font.pointSize: 15
        font.bold: false
        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                Qt.openUrlExternally("file://" + folderPath)
            }
            cursorShape: Qt.PointingHandCursor
        }
    }


    ListView {
        id: fileList
        anchors.top: titleText.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        model: parent.model
        clip: true
        interactive: false
        delegate: Item {
            width: parent.width
            height: 30
            property string videoFile: modelData
            property var shots: toolbar.screenshots
            onShotsChanged: filterScreenshots()
            onWidthChanged: filterScreenshots()
            function filterScreenshots() {
                var fileScreenshots = [];
                var screenshots = shots
                if (screenshots === undefined) return;

                for (var i=0; i<screenshots.length; i++) {
                    if (screenshots[i].indexOf(videoFile) === -1) continue;
                    fileScreenshots.push(screenshots[i]);
                }
                if (fileScreenshots.length == 0) return

                var shotCount = width/height - 1
                var skipAmount = fileScreenshots.length / shotCount
                screenshots = []
                for (var i=0,j=0; i<shotCount; i++) {
                    screenshots.push(fileScreenshots[Math.floor(j)])
                    j+=skipAmount
                }
                screenshotsList.model = screenshots
            }

            ListView {
                interactive: false
                anchors.fill: parent
                id: screenshotsList
                model: []
                orientation: ListView.Horizontal
                delegate: Image {
                    width: height
                    height: parent.height
                    source: "file:" + encodeURIComponent(folderPath + "/" + modelData)
                }
            }
            Text {
                anchors.fill: parent
                id: fileEntry
                color: "white"
                font.bold: file === modelData
                font.pointSize: titleText.font.pointSize
                text: modelData
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                style: Text.Outline
                styleColor: "black"
                ClickableArea {
                    hoverEnabled: toolbar.file !== modelData
                    onClicked: {
                        toolbar.file = modelData
                    }
                }
            }
        }
    }

    ScrollBar {
        view: fileList
    }
}

