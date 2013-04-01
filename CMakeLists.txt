# cmake stuff
cmake_minimum_required(VERSION 2.8.8)
project(Vimi)
set(CMAKE_AUTOMOC ON)

# Qt stuff
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Widgets REQUIRED)

# ffmpeg stuff
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE)
include_directories(${FFMPEG_INCLUDE_DIRS})
add_definitions(-D__STDC_CONSTANT_MACROS)

# our sources
set(sources
    src/aboutdialog.cpp
    src/collection.cpp
    src/config.cpp
    src/coverlabel.cpp
    src/coverloader.cpp
    src/covermaker.cpp
    src/favouritetagdialog.cpp
    src/infopanel.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/settingsdialog.cpp
    src/tagdialog.cpp
    src/tagreplacementdialog.cpp
    src/video.cpp
    src/videofilterproxymodel.cpp
    src/videowidget.cpp
    )

# create our resource file
qt5_add_resources(qrc_sources images/images.qrc)

# actually put together our executable
add_executable(vimi ${sources} ${qrc_sources})
target_link_libraries(vimi ${FFMPEG_LIBRARIES})
qt5_use_modules(vimi Widgets Concurrent)

