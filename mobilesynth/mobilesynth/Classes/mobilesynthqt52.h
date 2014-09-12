#ifndef MOBILESYNTHQT52_H
#define MOBILESYNTHQT52_H

#include <QAudioOutput>
#include <QByteArray>
#include <QIODevice>
#include <QMainWindow>
#include <QObject>
#include <QTimer>

#include "synth/controller.h"

class mobileSynthQT52 : public QIODevice
{
    Q_OBJECT

public:
    mobileSynthQT52();
    ~mobileSynthQT52();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

    void noteOn(int vid, float f);
    void noteOff(int vid);

    synth::Controller * getSyctl() {return syctl;}
public slots:
    void pullTimerExpired();

private:
//    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);
//    qint64 m_pos;
    int DataSampleRateHz;
    int BufferSize;
    QTimer *m_pullTimer;
    QByteArray m_buffer;
    synth::Controller * syctl;
    QAudioOutput *m_audioOutput;
    QIODevice *m_output; // not owned
    QAudioFormat m_format;
    QAudioDeviceInfo m_device;
};

#endif // MOBILESYNTHQT52_H
