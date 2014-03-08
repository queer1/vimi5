import QtQuick 2.0
import QtGraphicalEffects 1.0


Rectangle {
    id: busyWidget
    anchors.fill: parent
    color: "black"

    Item {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        height: 200
        width: 200

        Rectangle {
            id: spinnerContainer
            anchors.centerIn: parent
            height: 100
            width: 100
            radius: 10
            border.width: 5
            border.color: "#fff"
            color: "#222"
            RotationAnimation on rotation {
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 3000
                running: busyWidget.visible
            }
        }
        Text {
            smooth: true
            anchors.centerIn: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            color: "white"
            text: "working..."
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
