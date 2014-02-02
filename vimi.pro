SOURCES = \
    src/collection.cpp \
    src/config.cpp \
    src/main.cpp \
    src/videoframedumper.cpp
HEADERS = \
    src/collection.h \
    src/videoframedumper.h \
    src/config.h \

QT += quick qml widgets

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    qml/ControlBar.qml \
    qml/VideoElement.qml \
    qml/main.qml \
    qml/TagList.qml \
    qml/Toolbar.qml \
    qml/ScrollBar.qml \
    qml/SideBar.qml \
    qml/SeekBar.qml \
    qml/ActressPanel.qml \
    qml/TitleText.qml \
    qml/ClickableArea.qml \
    qml/Screenshot.qml \
    qml/VideoPlayer.qml \
    qml/Button.qml \
    qml/InputBox.qml

LIBS += -lavcodec -lavformat -lavutil -lswscale
