import QtQuick 2.0

Rectangle {
    id: tagList
    anchors.margins: 5
    width: 200
    color: "#c0000000"
    border.color: "white"
    property var tags
    property int index
    opacity: 0
    visible: false
    onOpacityChanged: {
        if (opacity < 0.1)
            visible = false
        else
            visible = true
    }
    onVisibleChanged: {
        if (visible)
            newTagInput.forceActiveFocus()
        else
            parent.forceActiveFocus()
    }

    ListView {
        id: taglistlist
        anchors.top: parent.top
        anchors.bottom: newTagRect.top
        anchors.right: parent.right
        anchors.left: parent.left
        //interactive: false
        boundsBehavior: Flickable.StopAtBounds

        anchors.margins: 10
        anchors.bottomMargin: 15
        model: tags
        opacity: 1.0
        delegate: Row {
            height: tag.height + 10
            width: tagList.width
            spacing: 10
            Text {
                text: "x"
                color: "red"
                visible: newTagRect.visible
                font.pointSize: tag.font.pointSize
                font.bold: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: videoModel.removeTag(tagList.index, modelData)
                }
            }

            Text {
                id: tag
                text: modelData;
                color: "white";
                styleColor: "black"
                font.pointSize: 12
                style: Text.Outline
                width: 150
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter

                property bool selected: videoModel.filterTagsContains(modelData)
                font.bold: selected
            }
        }
    }

    ScrollBar {
        view: taglistlist
    }

    Rectangle {
        id: newTagRect
        anchors.bottom: parent.bottom
        anchors.left: tagList.left
        anchors.right: tagList.right
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        height: 24
        color: "black"
        border.color: "white"
        border.width: 1
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor
        }

        TextInput {
            font.pointSize: 12
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

            Keys.onEscapePressed: tagList.opacity = 0

            Text {
                font.pointSize: parent.font.pointSize
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "Add new tag..."
                visible: !(parent.cursorVisible || parent.text != "")
                color: "white"
            }

            onAccepted: {
                videoModel.addTag(tagList.index, text)
                text = ""
            }
        }
    }

    Behavior on opacity { NumberAnimation { duration: 300 } }
}
