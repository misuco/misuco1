#-------------------------------------------------
#
# Project created by QtCreator 2013-07-19T17:04:33
#
#-------------------------------------------------

QT       += core gui opengl network multimedia xml quick qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = misuco
TEMPLATE = app

INCLUDEPATH += comm
INCLUDEPATH += comm/libofqf
INCLUDEPATH += mobilesynth/mobilesynth/Classes/synth
INCLUDEPATH += mobilesynth/mobilesynth/Classes
INCLUDEPATH += mobilesynth/lib

SOURCES += main.cpp\
    storage.cpp \
    conf/layoutmodel.cpp \
    event/eventhandlerrect.cpp \
    comm/senderdebug.cpp \
    test/unittest.cpp \
    point.cpp \
    paint/paintbgshapes.cpp \
    paint/paintshapes.cpp \
    paint/paintstat.cpp \
    paint/pointpaintshape.cpp \
    comm/libofqf/qosctypes.cpp \
    comm/libofqf/qosctcpserverthread.cpp \
    comm/libofqf/qosctcpserver.cpp \
    comm/libofqf/qosctcpclient.cpp \
    comm/libofqf/qoscserver.cpp \
    comm/libofqf/qoscproxyinterface.cpp \
    comm/libofqf/qoscproxy.cpp \
    comm/libofqf/qoscmsgparser.cpp \
    comm/libofqf/qoscclient.cpp \
    comm/senderoscpuredata.cpp \
    rc1.cpp \
    eventstat.cpp \
    paint/pointpaintsphere.cpp \
    comm/sendersupercollider.cpp \
    mobilesynth/mobilesynth/Classes/synth/arpeggio.cpp \
    mobilesynth/mobilesynth/Classes/synth/controller.cpp \
    mobilesynth/mobilesynth/Classes/synth/envelope.cpp \
    mobilesynth/mobilesynth/Classes/synth/filter.cpp \
    mobilesynth/mobilesynth/Classes/synth/key_stack.cpp \
    mobilesynth/mobilesynth/Classes/synth/lag_processor.cpp \
    mobilesynth/mobilesynth/Classes/synth/modulation.cpp \
    mobilesynth/mobilesynth/Classes/synth/oscillator.cpp \
    mobilesynth/mobilesynth/Classes/synth/parameter.cpp \
    comm/sendermobilesynth.cpp \
    mobilesynth/mobilesynth/Classes/mobilesynthqt52.cpp \
    conf/layoutxml.cpp \
    comm/senderoscxy.cpp \
    paint/paintbgbitmap.cpp \
    paint/paintblocker.cpp \
    conf/scales.cpp \
    mobilesynth/mobilesynth/Classes/synth/waveform.cpp \
    comm/sendermulti.cpp \
    comm/senderreaktor.cpp \
    comm/senderoscmidigeneric.cpp \
    conf/progmem.cpp \
    dialognet.cpp \
    mainwindow.cpp \
    conf/soundmem.cpp \
    qdialog.cpp \
    qqdialog.cpp \
    mobilesynth/mobilesynth/Classes/synth/oscillatorplus.cpp \
    paint/paintwave.cpp

HEADERS  += \
    storage.h \
    conf/layoutmodel.h \
    event/eventhandlerrect.h \
    comm/isender.h \
    event/ieventhandler.h \
    comm/senderdebug.h \
    test/unittest.h \
    point.h \
    paint/ipaint.h \
    paint/ipointpaint.h \
    paint/paintbgshapes.h \
    paint/paintshapes.h \
    paint/paintstat.h \
    paint/pointpaintshape.h \
    comm/libofqf/qosctypes.h \
    comm/libofqf/qosctcpserverthread.h \
    comm/libofqf/qosctcpserver.h \
    comm/libofqf/qosctcpclient.h \
    comm/libofqf/qoscserverinterface.h \
    comm/libofqf/qoscserver.h \
    comm/libofqf/qoscproxyinterface.h \
    comm/libofqf/qoscproxy.h \
    comm/libofqf/qoscmsgparser.h \
    comm/libofqf/qoscclientinterface.h \
    comm/libofqf/qoscclient.h \
    comm/senderoscpuredata.h \
    rc1.h \
    eventstat.h \
    paint/pointpaintsphere.h \
    comm/sendersupercollider.h \
    mobilesynth/mobilesynth/Classes/AudioOutput.h \
    mobilesynth/mobilesynth/Classes/mobilesynthViewControllerRc1.h \
    mobilesynth/mobilesynth/Classes/synth/arpeggio.h \
    mobilesynth/mobilesynth/Classes/synth/controller.h \
    mobilesynth/mobilesynth/Classes/synth/envelope.h \
    mobilesynth/mobilesynth/Classes/synth/filter.h \
    mobilesynth/mobilesynth/Classes/synth/key_stack.h \
    mobilesynth/mobilesynth/Classes/synth/lag_processor.h \
    mobilesynth/mobilesynth/Classes/synth/modulation.h \
    mobilesynth/mobilesynth/Classes/synth/oscillator.h \
    mobilesynth/mobilesynth/Classes/synth/parameter.h \
    mobilesynth/mobilesynth/Classes/synth/test_util.h \
    comm/sendermobilesynth.h \
    mobilesynth/mobilesynth/Classes/mobilesynthviewcontrollerrc1.hpp \
    mobilesynth/mobilesynth/Classes/mobilesynthqt52.h \
    conf/layoutxml.h \
    comm/senderoscxy.h \
    platform.h \
    paint/paintbgbitmap.h \
    paint/paintblocker.h \
    conf/scales.h \
    mobilesynth/mobilesynth/Classes/synth/waveform.h \
    comm/sendermulti.h \
    comm/senderreaktor.h \
    comm/senderoscmidigeneric.h \
    conf/progmem.h \
    dialognet.h \
    mainwindow.h \
    conf/soundmem.h \
    qdialog.h \
    qqdialog.h \
    mobilesynth/mobilesynth/Classes/synth/oscillatorplus.h \
    paint/paintwave.h

OTHER_FILES += \
    puredata/osc2midi.pd \
    puredata/xdotool.sh \
    mobilesynth/mobilesynth/Classes/synth/Makefile \
    mobilesynth/mobilesynth/MainWindow.xib \
    mobilesynth/mobilesynth/mobilesynthViewController.xib \
    mobilesynth/mobilesynth/main.m \
    mobilesynth/mobilesynth/Info.plist \
    mobilesynth/mobilesynth/mobilesynth_Prefix.pch \
    mobilesynth/mobilesynth/mobilesynth.xcodeproj \
    Info.plist \
    misuco.xml \
    scales.xml \
    misuco-logo.jpg \
    conf/l1.xml \
    conf/l2.xml \
    conf/l3.xml \
    conf/l4.xml \
    conf/prog.xml \
    android/AndroidManifest.xml \
    conf/lp1.xml \
    conf/lp2.xml \
    conf/lp3.xml \
    conf/lp4.xml \
    conf/lp5.xml

OBJECTIVE_SOURCES += \
    mobilesynth/mobilesynth/Classes/mobilesynthViewControllerRc1.mm \
    mobilesynth/mobilesynth/Classes/AudioOutput.m \

RESOURCES += \
    res.qrc

ios {
    QMAKE_INFO_PLIST = Info.plist
    ios_icon.files = $$files($$PWD/icons/pro/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    launch_images.files = $$files($$PWD/icons/pro/LaunchImage*.png)
    QMAKE_BUNDLE_DATA += launch_images
}

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
    conf/sound.xml \
    NetDialog.qml

