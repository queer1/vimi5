import QtQuick 2.0
import QtGraphicalEffects 1.0

Image {
    id: mainView
    width: 640
    height: 480
    source: "images/bg2.png"
    fillMode: Image.Tile

    Keys.onEscapePressed: {
        Qt.quit()
    }

    SideBar {
        id: sideBar
    }

    GridView {
        id: gridView
        anchors.left: sideBar.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: scrollbar.left
        model: videoModel
        cellHeight: 300
        cellWidth: 200
        delegate: VideoElement {
        }
        focus:true
        highlight: RectangularGlow {color:"white"; glowRadius: 15; spread:0.001}
    }
    ScrollBar {
        id: scrollbar
        view: gridView
    }

    Rectangle {
        z: 100
        id: notification
        height: 150
        width: 300
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
        color: "black"
        radius: 25
        smooth: true
        opacity: 0

        Text {
            color: "white"
            anchors.fill: parent
            anchors.margins: 25
            font.pointSize: 12
            text: videoModel.status
            onTextChanged: {
                parent.opacity = 1
                notifyTimer.restart()
            }
            wrapMode: Text.WordWrap
            elide: Text.ElideRight

        }

        RectangularGlow {
            z:-1
            anchors.fill: notification
            glowRadius: 10
            spread: 0.2
            color: "white"
            cornerRadius: notification.radius + glowRadius
        }

        Timer {
            id: notifyTimer
            interval: 1000
            onTriggered: parent.opacity = 0
        }

        Behavior on opacity { NumberAnimation { duration: 1000; } }
    }

    Rectangle {
        id: aboutBox
        anchors.fill: parent
        visible: false
        color: "#00000000"
        Behavior on color {
            ColorAnimation { duration: 500 }
        }

        onVisibleChanged: {
            if (visible) color = "#e0000000"
            else color = "#00000000"
        }

        Text {
            id: aboutLogo
            //anchors.centerIn: parent
            anchors.fill: parent
            text: "Vimi"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 200
            color: "black"
            opacity: 1
        }
        Glow {
            id: aboutLogoGlow
            anchors.fill: aboutLogo
            radius: 8
            samples: 8
            color: "white"
            source: aboutLogo
            SequentialAnimation {
                running: aboutBox.visible
                loops: Animation.Infinite
                NumberAnimation { target: aboutLogoGlow; property: "radius"; duration: 5000; from: 8; to:100; easing.type: Easing.InOutQuad }
                NumberAnimation { target: aboutLogoGlow; property: "radius"; duration: 5000; from: 100; to:8; easing.type: Easing.InOutQuad }
            }
        }


        MouseArea {
            anchors.fill: parent
            onClicked: {
                aboutBox.color = "#00000000"
                aboutBox.visible = false
            }
        }
    }


}
