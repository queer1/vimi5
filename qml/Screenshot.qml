import QtQuick 2.0
import QtMultimedia 5.0
import QtGraphicalEffects 1.0

Image {
    id: screenshot
    height: 200
    width: 300
    y: 0
    anchors.bottom: seekbar.top
    fillMode: Image.PreserveAspectFit
    property int position: 0
    property int duration
    property string path
    property string file
    property var screenshots
    //x: visible ? position - (width * (position/parent.width)) : 0
    x: position - (width * (position/parent.width))
    onXChanged: {
        var screenshots = screenshot.screenshots
        if (screenshots === undefined) return;
        if (screenshots.length < 2) return;
        
        var file = screenshot.file
        var position = (screenshot.position * duration) / parent.width
        var lastScreenshotPos = -1000000000
        var lastScreenshot =""
        
        for (var i=0; i<screenshots.length; i++) {
            if (screenshots[i].indexOf(file) === -1)
                continue
            
            var screenshotPos = screenshots[i].split("_")[1]
            
            if (screenshotPos > position) {
                if (Math.abs(screenshotPos - position) < Math.abs(lastScreenshotPos - position)) {
                    screenshot.source = "file:" + encodeURIComponent(path + "/" + screenshots[i])
                } else if (lastScreenshot !== ""){
                    screenshot.source = "file:" + encodeURIComponent(path + "/" + lastScreenshot)
                }
                
                return
            }
            lastScreenshotPos = screenshotPos
            lastScreenshot = screenshots[i]
        }
    }
    onVisibleChanged: if (!visible) { source = ""; }
}
