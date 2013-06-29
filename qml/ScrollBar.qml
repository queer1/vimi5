import QtQuick 2.0

Item {
    id: scrollBar
    // The properties that define the scrollbar's state.
    // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
    // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
    // of the height of the view.
    // orientation can be either 'Vertical' or 'Horizontal'
    property real position
    property real pageSize
    property alias bgColor: background.color
    property alias fgColor: thumb.color
    opacity: 0.7
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.top: parent.top
    width: 20
    // A light, semi-transparent background
    Rectangle {
        id: background
        color: "white";
        anchors.fill: parent
    }
    // Size the bar to the required size, depending upon the orientation.
    Rectangle {
        id: thumb
        color: "black"
        radius: width/2 - 1
        x: 1
        y: scrollBar.position * (scrollBar.height-2) + 1
        width: parent.width-2
        height: scrollBar.pageSize * (scrollBar.height-2)
    }
}
