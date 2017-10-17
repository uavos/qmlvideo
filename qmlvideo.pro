#-------------------------------------------------
#
# Project created by QtCreator 2014-01-09T12:48:26
#
#-------------------------------------------------

QT += core widgets network multimedia

TEMPLATE = lib
CONFIG += plugin c++11 silent

DESTDIR = bin
TARGET = qmlvideo
INSTALLS += target qml
OBJECTS_DIR = obj
UI_DIR = obj
MOC_DIR = obj
target.path = ~/.gcu/plugins
qml.files = qmlvideo_full.qml
qml.path = ~/.gcu/plugins

SOURCES += \
    videothread.cpp \
    ffmpegplayer.cpp \
    qmlvideoplugin.cpp \
    videocoords.cpp \
    videocontroller.cpp \
    ffmpegvideowriter.cpp \
    ffmpegvideoreader.cpp \
    osdmenu.cpp \
    smartffmpeg.cpp \
    cameracontrol.cpp

INCLUDEPATH += /usr/share/uavos/gcu/sdk/inc/

LIBS += -lavcodec -lavformat -lavutil -lswscale -lavdevice -lavfilter

contains(DEFINES, WITH_STAB) {
    INCLUDEPATH += /usr/include/uavos-opencv

    LIBS += -L/usr/lib/uavos-opencv \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_videoio \
        -lopencv_highgui \
        -lopencv_videostab \
        -lopencv_optflow \
        -lopencv_video \
        -lopencv_features2d \
        -lopencv_imgcodecs

SOURCES += \
    kalmanfilter.cpp

HEADERS += \
    kalmanfilter.h
}

HEADERS  += \
    /usr/share/uavos/gcu/sdk/inc/plugin_interface.h \
    videothread.h \
    ffmpegplayer.h \
    qmlvideoplugin.h \
    videocoords.h \
    videocontroller.h \
    ffmpegvideowriter.h \
    ffmpegvideoreader.h \
    osdmenu.h \
    smartffmpeg.h \
    cameracontrol.h

DISTFILES += \
    qmlvideo_lite.qml \
    qmlvideo_full.qml
