import QtQuick 2.0
Rectangle {
    id: toolbar
    anchors.top: parent.top
    color: "#f0000000"
    border.color: "white"
    property var model
    property var screenshots
    height: Math.min((fileList.count) * 75 + 10, parent.height / 2)//titleText.contentHeight + 5
    property string folderPath
    property string file
    opacity: 0
    Behavior on opacity { NumberAnimation { duration: 300 } }

    ListView {
        id: fileList
        anchors.fill: parent
        anchors.margins: 5
        model: parent.model
        clip: true
        interactive: false
        delegate: Item {
            width: parent.width
            height: 75
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
                    fillMode: Image.PreserveAspectCrop
                    source: "file:" + encodeURIComponent(folderPath + "/" + modelData)
                }
            }
            Text {
                anchors.fill: parent
                id: fileEntry
                color: "white"
                font.bold: file === modelData
                font.pointSize: 20
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

