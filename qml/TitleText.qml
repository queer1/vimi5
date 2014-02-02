import QtQuick 2.0
import QtMultimedia 5.0
import QtGraphicalEffects 1.0

Text {
    z: 1
    id: titleText
    text: name
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    color: "white"
    font.bold: true
    height: 40
    wrapMode: Text.WordWrap
    
    elide: Text.ElideRight
    width: 200
    Behavior on color { ColorAnimation { duration: 1000 } }
    anchors.top: rect.top
    anchors.left: rect.left
    anchors.right: rect.right
    anchors.margins: 15
    
    MouseArea {
        propagateComposedEvents: true
        anchors.fill: parent
        onDoubleClicked: {
            titleEditRectangle.visible = true
            titleEdit.focus = true
            console.log("caek")
        }
    }
    
    Rectangle {
        id: titleEditRectangle
        anchors.fill: parent
        visible: false
        color: "black"
        TextInput {
            id: titleEdit
            anchors.fill: parent
            wrapMode: TextInput.WordWrap
            text: titleText.text
            color: "white"
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            onAccepted: {
                videoModel.renameVideo(index, text)
                titleEditRectangle.visible = false
            }
        }
    }
}
