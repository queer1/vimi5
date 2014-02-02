import QtQuick 2.0
import QtQuick.Dialogs 1.0

Rectangle {
    signal clicked
    property string text
    id: button
    height: 25
    anchors.right: parent.right
    anchors.left: parent.left
    anchors.bottomMargin: 1
    border.color: "white"
    border.width: 1
    color: "black"
    Text {
        id: text
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: parent.text
        color: "white"
    }
    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked()
        hoverEnabled: true
        onEntered: {
            parent.color = "white"
            text.color = "black"
        }
        onExited: {
            parent.color = "black"
            text.color = "white"
        }
    }
}
