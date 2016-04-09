/*

Copyright (C) 2013 by Claudio Zopfi, Zurich, Suisse, z@x21.ch

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <QDebug>
#include "sendersfmidi.h"

SenderSfMidi::SenderSfMidi() :   QIODevice()
  ,   m_pullTimer(new QTimer(this))
  ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
{
    //if (!_synth) {
        initSynth();
    //}

    ccstate=new int[1024];
    for(int i=0;i<1024;i++) {
        ccstate[i]=0;
    }

    stack_top=0;
    nextChannel=0;
    for(int i=0;i<STACK_SIZE;i++) {
        vstack[i]=-1;
        cstack[i]=-1;
        nstack[i]=-1;
    }

    DataSampleRateHz  = 44100;
    //const int BufferSize        = 32768;
    //BufferSize        = 8192;
    BufferSize        = 32768;
    //BufferSize        = 16384;

    m_buffer.resize(BufferSize);
    m_buffer.fill(0);
    m_format.setSampleRate(DataSampleRateHz);
    m_format.setChannelCount(2);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    m_format=info.preferredFormat();
    qDebug() << "Preferred format is sr " << m_format.sampleRate() << " sn " << m_format.sampleSize() << " ch " << m_format.channelCount();

    if (!info.isFormatSupported(m_format)) {
        qDebug() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
        info.preferredFormat();
    }

    //prog=0;

    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    m_audioOutput->setBufferSize(BufferSize);
//    connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
//    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));
    this->start();

    // way 1: push mode
    //m_audioOutput->start(this);

    // way 2: pull mode
    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));
    m_output=m_audioOutput->start();
    m_pullTimer->start(20);

}

SenderSfMidi::~SenderSfMidi()
{
    delete(ccstate);
    delete(_synth);
    //delete(oscout);
    m_pullTimer->stop();
    m_audioOutput->stop();
    this->stop();
//    delete(syctl);
    delete(m_audioOutput);
}

void SenderSfMidi::start()
{
    open(QIODevice::ReadOnly);
}

void SenderSfMidi::stop()
{
    close();
}

qint64 SenderSfMidi::readData(char *data, qint64 len)
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
    //qDebug() << "SenderSfMidi::readData len " << len;
//TODO: why this dirty hack? why does windows request odd lens
    if(len%2!=0) len-=1;
//    syctl->GetCharSamples(data,len);
//    for(int i=0;i<len;i++) {
//        data[i]=0;
//    }

    qint16 valueil;
    qint16 valueir;
    qint16 valueilc;
    qint16 valueirc;
    float valuefl;
    float valuefr;

    fluid_synth_write_s16(_synth,len/4,data,0,2,data,1,2);
    //fluid_synth_write_float(_synth,len/4,)

    for(int i=0;i<len;i+=4) {
        fluid_synth_write_s16(_synth,1,&valueil,0,1,&valueir,0,1);
        fluid_synth_write_float(_synth,1,&valuefl,0,1,&valuefr,0,1);

        valueilc=valuefl*32767;
        valueirc=valuefr*32767;

        //qDebug() << "l " << valueil << " " << valueilc << " " << valuefl;
        //qDebug() << "r " << valueir << " " << valueirc << " " << valuefr;

        data[i]=valueilc&0x00FF;
        data[i+1]=valueilc>>0x0008;
        data[i+2]=valueirc&0x00FF;
        data[i+3]=valueirc>>0x0008;

        /*
        char m;
        m=data[i];
        data[i]=data[i+1];
        data[i+1]=m;
        m=data[i+2];
        data[i+2]=data[i+3];
        data[i+3]=m;
        */
    }


//    return total;
    //qDebug() << "SenderSfMidi::readData written " << len;
    return len;

}

qint64 SenderSfMidi::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 SenderSfMidi::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}


void SenderSfMidi::noteOn(int chan, int vid, float, int midinote, int pitch, int scalenote, int)
{
    qDebug() << "SenderSfMidi::noteOn v " << vid << " c " << chan << " note " << midinote << " pitch " << pitch;
    int sid=getStackId(vid);
    nstack[sid]=midinote;
    cstack[sid]=nextChannel;
    nextChannel++;
    if(nextChannel>15) nextChannel=0;
    fluid_synth_noteon(_synth, cstack[sid], midinote, 127);
    fluid_synth_pitch_bend(_synth, cstack[sid], pitch+8192);
}

void SenderSfMidi::noteOff(int chan, int vid, int midinote)
{
    /* Release the note on key 60 */
    qDebug() << "SenderSfMidi::noteOff v " << vid << " c " << chan << " note " << midinote << " ";
    int sid=getStackId(vid);
    qDebug() << "fluid_synth_noteoff" << vid << " c " << cstack[sid] << nstack[sid];
    fluid_synth_noteoff(_synth, cstack[sid], nstack[sid]);
    deleteStackId(sid);
}

void SenderSfMidi::pitch(int chan, int vid, float f, int midinote, int pitch, int scalenote)
{
    qDebug() << "SenderSfMidi::pitch " << pitch << " n " << midinote;
    int sid=getStackId(vid);
    if(midinote!=nstack[sid]) {
        fluid_synth_noteoff(_synth, cstack[sid], nstack[sid]);
        nstack[sid]=midinote;
        fluid_synth_noteon(_synth, cstack[sid], midinote, 127);
    }
    fluid_synth_pitch_bend(_synth, cstack[sid], pitch+8192);
}

void SenderSfMidi::setDestination(char * a, int p)
{
}

void SenderSfMidi::reconnect()
{
}

void SenderSfMidi::pc(int chan, int v1)
{
    qDebug() <<  "SenderSfMidi::pc ch " << chan << " v1 " << v1;
    for(int i=0;i<16;i++) {
        fluid_synth_program_select(_synth,i,1,0,v1);
    }
}

void SenderSfMidi::cc(int chan, int, int cc, float, float v1avg)
{
    //qDebug() <<  "SenderOscPuredata::cc " << cc << " v1 " << v1;

    // translate value to midi
    int v1mid=(float)127*v1avg;

    // translate cc numbers
    if(v1mid!=ccstate[cc]) {
//        sendOsc(path,v);
    }
}

void SenderSfMidi::pullTimerExpired()
{
    if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
        int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
        //qDebug() << "***** pullTimerExpired ***** chunks " << chunks << " bytes free " << m_audioOutput->bytesFree();
        while (chunks) {
           qint64 ps=m_audioOutput->periodSize();
           //qDebug() << "pull timer read " << ps;
           qint64 len = this->read(m_buffer.data(), ps);
           //qDebug() << "pull timer loop len " << len << " chunks " << chunks;
           if (len) {
               m_output->write(m_buffer.data(), ps);
               //qDebug() << "pull timer written " << ps;
           }
           if (len != ps) {
               //qDebug() << "pull timer break " << len << " != " << ps;
               break;
           }
           --chunks;
        }
        //qDebug() << "***** pullTimerExpired DONE *****  ";
    }
}

int SenderSfMidi::getStackId(int vid)
{
    int sid=-1;
    for(int i=0;i<stack_top;i++) {
        if(vstack[i]==vid) {
            sid=i;
        }
    }
    if(sid==-1) {
        sid=stack_top;
        vstack[sid]=vid;
        stack_top++;
    }
    return sid;
}

void SenderSfMidi::deleteStackId(int sid)
{
    for(int i=0;i<stack_top-1;i++) {
        if(i>=sid) {
            vstack[i]=vstack[i+1];
            cstack[i]=cstack[i+1];
            nstack[i]=nstack[i+1];
        }
    }
    stack_top--;
    vstack[stack_top]=-1;
    cstack[stack_top]=-1;
    nstack[stack_top]=-1;
}
