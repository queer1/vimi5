import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    property string helpText
    property string text: textInput.text
    id: inputBox
    anchors.left: parent.left
    anchors.right: parent.right
    height: 30
    border.color: "white"
    border.width: 1
    color: "black"
    visible: opacity > 0.1


    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked()
        hoverEnabled: true
        onEntered: {
            inputBox.color = "white"
            textInput.color = "black"
        }
        onExited: {
            inputBox.color = "black"
            textInput.color = "white"
        }
        cursorShape: Qt.IBeamCursor
    }

    TextInput {
        id: textInput
        color: "white"
        anchors.fill: parent; anchors.leftMargin: 10
        font.pixelSize: 24
        onTextChanged: inputBox.textChanged()
        
        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            text: "[" + inputBox.helpText + "]"
            visible: !(parent.cursorVisible || parent.text != "")
            color: parent.color
        }
    }
}
