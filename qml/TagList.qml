import QtQuick 2.0

Rectangle {
    id: tagList
    anchors.margins: 5
    height: Math.min(taglistlist.count * 10 + 30, rect.height)


    color: "#80000000"
    radius: 10
    ListView {
        id: taglistlist
        anchors.fill: parent
        interactive: false

        anchors.margins: 10
        anchors.bottomMargin: 15
        model: tags
        delegate: Row {
            height:10; width: tagList.width
            spacing: 10
            Text {
                text: "x"
                color: "red"
                font.pointSize: 8
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: videoModel.removeTag(rect.index, modelData)
                }
            }

            Text {
                text: modelData;
                color: "white";
                styleColor: "black"
                font.pointSize: 8
                renderType: Text.NativeRendering
                style: Text.Outline

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
                        console.log(parent.selected)
                        parent.selected = !parent.selected
                        console.log(parent.selected)
                    }
                }

            }
        }
    }

    Rectangle {
        id: newTagRect
        anchors.bottom: parent.bottom
        anchors.left: tagList.left
        anchors.right: tagList.right
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        height: 16
        color: "#55ffffff"
        border.color: "white"
        border.width: 0
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor
        }

        TextInput {
            font.pointSize: 8
            anchors.fill: parent
            anchors.margins: 2
            color: "white"
            id: newTagInput

            selectByMouse: true
            onCursorVisibleChanged: {
                if (cursorVisible)
                    newTagRect.border.width = 1
                else
                    newTagRect.border.width = 0
            }

            Text {
                font.pointSize: parent.font.pointSize
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "New tag..."
                visible: !(parent.cursorVisible || parent.text != "")
                color: "white"
            }

            onAccepted: {
                videoModel.addTag(index, text)
                text = ""
            }
        }
    }

    Behavior on opacity { NumberAnimation { duration: 1000 } }
    Behavior on width { SmoothedAnimation { duration: 1000 } }

    state: "normal"
    states: [
        State {
            name: "hidden"
            PropertyChanges { target: tagList; opacity: 0; width: 0 }
            PropertyChanges { target: newTagInput; focus: false }
            AnchorChanges {
                target: tagList
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: rect.left
                anchors.right: undefined
            }
        },
        State {
            name: "maximized"
            PropertyChanges { target: tagList; opacity: 0.75; width: 200 }
            PropertyChanges { target: newTagInput; focus: false }
            AnchorChanges {
                target: tagList
                anchors.verticalCenter: rect.verticalCenter
                anchors.left: rect.left
                anchors.right: undefined
            }
        },
        State {
            name: "normal"
            PropertyChanges { target: tagList; width: undefined }
            PropertyChanges { target: newTagInput; focus: false }
            AnchorChanges {
                target: tagList
                anchors.top: titleText.bottom
                anchors.verticalCenter: undefined
                anchors.bottom: undefined
                anchors.left: rect.left
                anchors.right: rect.right
            }
        }

    ]
}
