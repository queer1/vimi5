import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    anchors.centerIn: parent
    width: 800
    height: 500
    color: "black"
    border.color: "white"
    radius: 15
    opacity: 0
    visible: false
    onOpacityChanged: if (opacity < 0.1) visible = false; else visible = true

    Behavior on opacity { NumberAnimation { duration: 100; } }

    Text {
        id: helpTitle
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 20
        text: "help"
        font.pointSize: 20
        font.bold: true
        color: "white"
    }

    Text {
        id: videoKeyboardTitle
        text: "keyboard shortcuts while playing video:"
        font.bold: true
        anchors.top: helpTitle.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "white"
    }

    Text {
        id: keyboardHelp
        anchors.left: parent.left
        anchors.top: videoKeyboardTitle.bottom
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "white"
        text: "\
escape:\tpause and go back to overview\n\
arrow right:\tskip forward 3 seconds\n\
arrow left:\tskip backward 3 seconds\n\
arrow up:\tskip forward 30 seconds\n\
arrow down:\tskip backward 30 seconds\n\
page up:\tskip forward 5 minutes\n\
page down:\tskip backward 5 minutes\n\
s:\tgenerate screenshot previews for seek bar\n\
t:\tedit tags\n\
b:\tadd bookmark at current position\n\
c:\tscreenshot video for cover at current position\n\
"
    }

    Text {
        id: helpTextTitle
        text: "general help:"
        font.bold: true
        anchors.top: helpTitle.bottom
        anchors.left: keyboardHelp.right
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "white"
    }

    Text {
        anchors.left: keyboardHelp.right
        anchors.right: parent.right
        anchors.top: helpTextTitle.bottom
        wrapMode: Text.WordWrap
        anchors.topMargin: 10
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        color: "white"
        text: "\
Vimi assumes that each movie consists of a single directory, containing the \
individual video files, an optional cover image and an optional file named \
\"tags.txt\" containing tags for this move, one on each line.

When you add new tags in this application, these will also be written to the \"tags.txt\" file in \
the movie directory, as well as being written to the internal Vimi cache.

You can move around in the main view with the arrow keys, and start a video with enter. \
To quit the app, press escape while in the main view, or simply close the window.

You can also point Vimi to a folder with thumbs/headshot of your favourite starlets, then they \
will automatically be shown in the field to the right, and you can click on them to filter \
the videos by them (assuming the videos are tagged with their names).
"
    }
    MouseArea {
        anchors.fill: parent
        enabled: parent.visible
        onClicked: parent.opacity = 0
        cursorShape: "PointingHandCursor"
    }
}
