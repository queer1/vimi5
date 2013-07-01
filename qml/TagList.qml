import QtQuick 2.0

Rectangle {
    id: tagList
    /*anchors.top: titleText.bottom
    anchors.right: parent.right
    anchors.left: parent.left*/
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
                font.bold: true
                font.pointSize: 8
                renderType: Text.NativeRendering
                style: Text.Outline
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
        color: "#55000000"
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
                anchors.top: toolbar.bottom
                anchors.bottom: seekbar.top
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
                anchors.top: toolbar.bottom
                anchors.bottom: seekbar.top
                anchors.left: rect.left
                anchors.right: undefined
            }
        },
        State {
            name: "normal"
            PropertyChanges { target: tagList; opacity: 1; width: undefined }
            PropertyChanges { target: newTagInput; focus: false }
            AnchorChanges {
                target: tagList
                anchors.top: titleText.bottom
                anchors.bottom: undefined
                anchors.left: rect.left
                anchors.right: rect.right
            }
        }

    ]
}
