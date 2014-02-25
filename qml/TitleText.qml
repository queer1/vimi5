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
    height: visible ? parent.height / 7 : 0
    wrapMode: Text.Wrap
    font.pointSize: height / 5 > 0 ?  height / 5 : 5
    
    elide: Text.ElideRight
    Behavior on color { ColorAnimation { duration: 1000 } }
    anchors.top: rect.top
    anchors.left: rect.left
    anchors.right: rect.right
    anchors.margins: 5
    
    MouseArea {
        propagateComposedEvents: true
        anchors.fill: parent
        onDoubleClicked: {
            titleEditRectangle.visible = true
            titleEdit.focus = true
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
