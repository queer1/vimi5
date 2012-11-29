# cmake stuff
cmake_minimum_required(VERSION 2.6)
project(Vimi)

# Qt stuff
find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

# ffmpeg stuff
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE)
include_directories(${FFMPEG_INCLUDE_DIRS})
add_definitions(-D__STDC_CONSTANT_MACROS)

# our sources
set(sources
    src/aboutdialog.cpp
    src/collection.cpp
    src/collectionview.cpp
    src/config.cpp
    src/cookiestorage.cpp
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

set(moc_hdrs
    src/aboutdialog.h
    src/collection.h
    src/collectionview.h
    src/cookiestorage.h
    src/coverlabel.h
    src/coverloader.h
    src/covermaker.h
    src/favouritetagdialog.h
    src/infopanel.h
    src/mainwindow.h
    src/settingsdialog.h
    src/tagdialog.h
    src/tagreplacementdialog.h
    src/videofilterproxymodel.h
    src/video.h
    src/videowidget.h
    )

# magical Qt tools
qt4_wrap_cpp(moc_sources ${moc_hdrs})
qt4_add_resources(qrc_sources images/images.qrc)

# actually put together our executable
add_executable(vimi ${sources} ${moc_sources} ${qrc_sources})
target_link_libraries(vimi ${QT_LIBRARIES} ${FFMPEG_LIBRARIES})
    
