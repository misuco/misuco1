#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QDebug>
#include <QVBoxLayout>
#include <qmath.h>
#include <qendian.h>

#include "mobilesynthqt52.h"

const int DataSampleRateHz  = 44100;
const int BufferSize        = 32768;

mobileSynthQT52::mobileSynthQT52()
    :   QIODevice()
    ,   m_pullTimer(new QTimer(this))
    ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_buffer(BufferSize, 0)

{
//    generateData(format, durationUs, sampleRate);
    syctl = new synth::Controller();
    syctl->set_modulation_amount(0.7);
    syctl->set_modulation_frequency(0.3);
//    syctl->set_modulation_source(synth::Controller::LFO_SRC_TRIANGLE);
    syctl->set_modulation_destination(synth::Controller::LFO_DEST_FILTER);
    syctl->set_osc1_wave_type(synth::Oscillator::SAWTOOTH);
//    syctl->set_filter_cutoff(2000);
//    syctl->set_filter_resonance(0.9);
    syctl->set_sample_rate(DataSampleRateHz);

    m_format.setSampleRate(DataSampleRateHz);
    m_format.setChannelCount(1);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    syctl->setFormat(&m_format);

    //delete m_audioOutput;
    //m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    m_audioOutput->setBufferSize(2048);
//    connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
//    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));
    this->start();

    // way 1: push mode
    // m_audioOutput->start(this);

    // way 2: pull mode
    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));
    m_output=m_audioOutput->start();
    m_pullTimer->start(5);

}

mobileSynthQT52::~mobileSynthQT52()
{

}

void mobileSynthQT52::start()
{
    open(QIODevice::ReadOnly);
}

void mobileSynthQT52::stop()
{
    close();
}

qint64 mobileSynthQT52::readData(char *data, qint64 len)
{
    /*
    qint64 total = 0;
    while (len - total > 0) {
        const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
        memcpy(data + total, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk) % m_buffer.size();
        total += chunk;
    }
    */
    syctl->GetCharSamples(data,len);
//    return total;
    return len;

}

qint64 mobileSynthQT52::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 mobileSynthQT52::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

void mobileSynthQT52::noteOn(int vid, float f)
{
    syctl->NoteOn(vid,f);
}

void mobileSynthQT52::noteOff(int vid)
{
    syctl->NoteOff(vid);
}


void mobileSynthQT52::pullTimerExpired()
{
    if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
        int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
        while (chunks) {
           const qint64 len = this->read(m_buffer.data(), m_audioOutput->periodSize());
           if (len)
               m_output->write(m_buffer.data(), len);
           if (len != m_audioOutput->periodSize())
               break;
           --chunks;
        }
    }
}
