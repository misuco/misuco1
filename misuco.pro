#-------------------------------------------------
#
# Project created by QtCreator 2013-07-19T17:04:33
#
#-------------------------------------------------

QT       += core gui opengl network multimedia xml quick qml
CCFLAG += -lpthread -lgthread-2.0 -lglib-2.0
QMAKE_CXXFLAGS += -lpthread -lgthread-2.0 -lglib-2.0
LIBS += -lpthread -lgthread-2.0 -lglib-2.0

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = misuco
TEMPLATE = app

INCLUDEPATH += comm
INCLUDEPATH += comm/libofqf
INCLUDEPATH += mobilesynth/mobilesynth/Classes/synth
INCLUDEPATH += mobilesynth/mobilesynth/Classes
INCLUDEPATH += mobilesynth/lib
INCLUDEPATH += fluidsynth-1.1.6/include
INCLUDEPATH += fluidsynth-1.1.6/src/utils
INCLUDEPATH += fluidsynth-1.1.6/src/synth
INCLUDEPATH += fluidsynth-1.1.6/src/midi
INCLUDEPATH += fluidsynth-1.1.6/src/rvoice
INCLUDEPATH += fluidsynth-1.1.6/src/sfloader
INCLUDEPATH += fluidsynth-1.1.6/src/bindings
INCLUDEPATH += fluidsynth-1.1.6/src/drivers
INCLUDEPATH += fluidsynth-1.1.6/src
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include

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
    paint/paintwave.cpp \
    touchstat.cpp \
    paint/painthistogram.cpp \
    conf/sequence.cpp \
    sequencer.cpp \
    conf/seqmem.cpp \
    fluidsynth-1.1.6/src/drivers/fluid_adriver.c \
    fluidsynth-1.1.6/src/drivers/fluid_dart.c \
    fluidsynth-1.1.6/src/drivers/fluid_mdriver.c \
    fluidsynth-1.1.6/src/drivers/fluid_midishare.c \
    fluidsynth-1.1.6/src/drivers/fluid_portaudio.c \
    fluidsynth-1.1.6/src/drivers/fluid_sndmgr.c \
    fluidsynth-1.1.6/src/midi/fluid_midi.c \
    fluidsynth-1.1.6/src/midi/fluid_midi_router.c \
    fluidsynth-1.1.6/src/midi/fluid_seq.c \
    fluidsynth-1.1.6/src/midi/fluid_seqbind.c \
    fluidsynth-1.1.6/src/rvoice/fluid_adsr_env.c \
    fluidsynth-1.1.6/src/rvoice/fluid_chorus.c \
    fluidsynth-1.1.6/src/rvoice/fluid_iir_filter.c \
    fluidsynth-1.1.6/src/rvoice/fluid_lfo.c \
    fluidsynth-1.1.6/src/rvoice/fluid_rev.c \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice.c \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice_dsp.c \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice_event.c \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice_mixer.c \
    fluidsynth-1.1.6/src/sfloader/fluid_defsfont.c \
    fluidsynth-1.1.6/src/sfloader/fluid_ramsfont.c \
    fluidsynth-1.1.6/src/synth/fluid_chan.c \
    fluidsynth-1.1.6/src/synth/fluid_event.c \
    fluidsynth-1.1.6/src/synth/fluid_gen.c \
    fluidsynth-1.1.6/src/synth/fluid_mod.c \
    fluidsynth-1.1.6/src/synth/fluid_synth.c \
    fluidsynth-1.1.6/src/synth/fluid_tuning.c \
    fluidsynth-1.1.6/src/synth/fluid_voice.c \
    fluidsynth-1.1.6/src/utils/fluid_conv.c \
    fluidsynth-1.1.6/src/utils/fluid_hash.c \
    fluidsynth-1.1.6/src/utils/fluid_list.c \
    fluidsynth-1.1.6/src/utils/fluid_ringbuffer.c \
    fluidsynth-1.1.6/src/utils/fluid_settings.c \
    fluidsynth-1.1.6/src/utils/fluid_sys.c \
    comm/sendersfmidi.cpp

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
    paint/paintwave.h \
    touchstat.h \
    paint/painthistogram.h \
    conf/sequence.h \
    sequencer.h \
    conf/seqmem.h \
    fluidsynth-1.1.6/include/fluidsynth/audio.h \
    fluidsynth-1.1.6/include/fluidsynth/event.h \
    fluidsynth-1.1.6/include/fluidsynth/gen.h \
    fluidsynth-1.1.6/include/fluidsynth/log.h \
    fluidsynth-1.1.6/include/fluidsynth/midi.h \
    fluidsynth-1.1.6/include/fluidsynth/misc.h \
    fluidsynth-1.1.6/include/fluidsynth/mod.h \
    fluidsynth-1.1.6/include/fluidsynth/ramsfont.h \
    fluidsynth-1.1.6/include/fluidsynth/seq.h \
    fluidsynth-1.1.6/include/fluidsynth/seqbind.h \
    fluidsynth-1.1.6/include/fluidsynth/settings.h \
    fluidsynth-1.1.6/include/fluidsynth/sfont.h \
    fluidsynth-1.1.6/include/fluidsynth/shell.h \
    fluidsynth-1.1.6/include/fluidsynth/synth.h \
    fluidsynth-1.1.6/include/fluidsynth/types.h \
    fluidsynth-1.1.6/include/fluidsynth/voice.h \
    fluidsynth-1.1.6/include/fluidsynth.h \
    fluidsynth-1.1.6/src/drivers/fluid_adriver.h \
    fluidsynth-1.1.6/src/drivers/fluid_mdriver.h \
    fluidsynth-1.1.6/src/midi/fluid_midi.h \
    fluidsynth-1.1.6/src/midi/fluid_midi_router.h \
    fluidsynth-1.1.6/src/rvoice/fluid_adsr_env.h \
    fluidsynth-1.1.6/src/rvoice/fluid_chorus.h \
    fluidsynth-1.1.6/src/rvoice/fluid_iir_filter.h \
    fluidsynth-1.1.6/src/rvoice/fluid_lfo.h \
    fluidsynth-1.1.6/src/rvoice/fluid_phase.h \
    fluidsynth-1.1.6/src/rvoice/fluid_rev.h \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice.h \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice_event.h \
    fluidsynth-1.1.6/src/rvoice/fluid_rvoice_mixer.h \
    fluidsynth-1.1.6/src/sfloader/fluid_defsfont.h \
    fluidsynth-1.1.6/src/sfloader/fluid_ramsfont.h \
    fluidsynth-1.1.6/src/sfloader/fluid_sfont.h \
    fluidsynth-1.1.6/src/synth/fluid_chan.h \
    fluidsynth-1.1.6/src/synth/fluid_event_priv.h \
    fluidsynth-1.1.6/src/synth/fluid_event_queue.h \
    fluidsynth-1.1.6/src/synth/fluid_gen.h \
    fluidsynth-1.1.6/src/synth/fluid_mod.h \
    fluidsynth-1.1.6/src/synth/fluid_synth.h \
    fluidsynth-1.1.6/src/synth/fluid_tuning.h \
    fluidsynth-1.1.6/src/synth/fluid_voice.h \
    fluidsynth-1.1.6/src/utils/fluid_conv.h \
    fluidsynth-1.1.6/src/utils/fluid_hash.h \
    fluidsynth-1.1.6/src/utils/fluid_list.h \
    fluidsynth-1.1.6/src/utils/fluid_ringbuffer.h \
    fluidsynth-1.1.6/src/utils/fluid_settings.h \
    fluidsynth-1.1.6/src/utils/fluid_sys.h \
    fluidsynth-1.1.6/src/utils/fluidsynth_priv.h \
    comm/sendersfmidi.h \
    fluidsynth-1.1.6/include/fluidsynth/version.h

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
    launch_images.files = $$PWD/ios/Launch.xib $$files($$PWD/icons/pro/LaunchImage*.png)
    QMAKE_BUNDLE_DATA += launch_images
}

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
    conf/sound.xml \
    NetDialog.qml \
    conf/lp6.xml \
    conf/sYnerGi-8Mb.sf2

