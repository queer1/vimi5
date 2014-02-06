import QtQuick 2.0
import QtGraphicalEffects 1.0


Rectangle {
    id: busyWidget
    anchors.fill: parent
    color: "black"
    
    Rectangle {
        id: spinnerContainer
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        height: 100
        width: 100
        border.color: "white"
        color: "transparent"
        Rectangle {
            smooth: true
            height: 10
            width: 100
            anchors.centerIn: parent
            color: "white"
            RotationAnimation on rotation {
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 1500
                running: busyWidget.visible
            }
        }
    }
    
    Text {
        opacity: 1
        text: videoModel.status
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        color: "white"
        font.pointSize: 20
        font.bold: true
    }
}
