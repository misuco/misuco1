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

#ifndef SENDERSFMIDI_H
#define SENDERSFMIDI_H

#include <QDebug>
#include <QAudioOutput>
#include <QByteArray>
#include <QIODevice>
#include <QTimer>
#include "isender.h"
#include "fluidsynth-1.1.6/include/fluidsynth.h"

#define STACK_SIZE 256


class SenderSfMidi : public QIODevice, ISender
{
    Q_OBJECT

public:
    SenderSfMidi();
    ~SenderSfMidi();
    virtual void cc(int chan, int voiceId, int cc, float, float v1avg);
    virtual void pc(int chan, int v1);
    virtual void noteOn(int chan, int, float f, int midinote, int pitch,int scalenote, int vel);
    virtual void noteOff(int chan, int vid, int midinote);
    virtual void pitch(int chan, int vid, float f, int midinote, int pitch, int scalenote);
    virtual void setDestination(char * a,int p);
    virtual void reconnect();
    virtual int getPort() {return 0;}
    virtual char* getAddress() {return "";}
    virtual bool voiceBased() {return false;}

    void initSynth() {
        _settings = new_fluid_settings();
        _synth = new_fluid_synth(_settings);

        int ret=fluid_synth_sfload(_synth,"/usr/share/sounds/sf2/FluidR3_GM.sf2",true);
        //int ret=fluid_synth_sfload(_synth,"/media/c1/3764-3838/owncloud/code/sYnerGi8/sYnerGi-8Mb.sf2",true);
        //int ret=fluid_synth_sfload(_synth,":/conf/sYnerGi-8Mb.sf2",true);
        //int ret=fluid_synth_sfload(_synth,":/conf/FluidR3_GM.sf2",true);
        qDebug() << "loaded sf " << ret;
        ret = fluid_synth_program_select 	( 	_synth,
                0,
                1,
                0,
                0
            ) ;
        qDebug() << "program select " << ret;
    }

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

public slots:
    void pullTimerExpired();

private:
    //QOscClientInterface* oscout;
    //quint8 * notestate;   // currently played notes
    int * ccstate;       // current ccval;
    fluid_settings_t* _settings;
    fluid_synth_t* _synth;
    fluid_audio_driver_t* _adriver;

    int DataSampleRateHz;
    int BufferSize;
    QTimer *m_pullTimer;
    QByteArray m_buffer;

    QAudioOutput *m_audioOutput;
    QIODevice *m_output; // not owned
    QAudioFormat m_format;
    QAudioDeviceInfo m_device;

    int vstack[STACK_SIZE]; // voice id stack
    int cstack[STACK_SIZE]; // channel of voice
    int nstack[STACK_SIZE]; // note of voice
    int stack_top;

    int getStackId(int vid);
    void deleteStackId(int sid);
    int nextChannel;
};

#endif // SENDERSFMIDI_H
