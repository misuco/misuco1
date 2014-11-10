// controller.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/controller.h"

#include <math.h>
#include <assert.h>
#include "synth/envelope.h"
#include "synth/filter.h"
#include "synth/modulation.h"
#include "synth/oscillator.h"

#include <QDebug>
#include <qendian.h>

namespace synth {
    
    Controller::Controller()
    : //modulation_source_(LFO_SRC_TRIANGLE),
    modulation_destination_(LFO_DEST_NONE) //,
//    modulation_frequency_(2.0f),
//    modulation_amount_(0.0f)
    {
//        modulation_osc_.set_frequency(modulation_frequency_.GetValue());
//        modulation_.set_oscillator(&modulation_osc_);
//        modulation_.set_level(&modulation_amount_);
        key_stack_.setADSR(0, 1000, 1000, 0.8, 80000);
        key_stack_.setADSR(1, 0,   0,   1, 1000);
        format=0;
        volume_=0.5;
        //        reset_routing();
    }

    void Controller::set_volume(float volume)
    {
        volume_=volume;
    }
    void Controller::set_sample_rate(float sample_rate) {
//        osc1_.set_sample_rate(sample_rate);
        sample_rate_=sample_rate;
        key_stack_.SetSampleRate(sample_rate);
    }
    
    void Controller::NoteOn(int note, float freq) {
        //qDebug() << "controller::noteon " << note << " freq " << freq;
        key_stack_.NoteOn(note, freq);
    }
    
    void Controller::NoteOff(int note) {
        //qDebug() << "controller::noteoff " << note;
        key_stack_.NoteOff(note);
        /*        if (key_stack_.size() == 0) {
         // All notes were release, so start the release phase of the envelope
         NoteOff();
         }*/
    }
    
    /*    void Controller::NoteOff() {
     key_stack_.clear();
     }*/
    
//    void Controller::set_osc1_wave_type(Oscillator::WaveType wave_type) {
//        osc1_.set_wave_type(wave_type);
//    }
    
    void Controller::set_osc_pw(int voice, float p) {
        key_stack_.setOscPW(voice, p);
    }
    
    void Controller::set_osc1_wave_type_int(int w) {
        key_stack_.setOscWave(w);
    }
    
    void Controller::set_lfo_wave_type_int(int w) {
        key_stack_.setLfoWave(w);
    }
    
    void Controller::set_modulation_amount(float amount) {
        key_stack_.setModAmtInit(amount);
    }
    
    void Controller::set_modulation_amount(int voice, float amount) {
        key_stack_.setModAmt(voice, amount);
    }
    
    void Controller::set_modulation_frequency(float frequency) {
        key_stack_.setLfoFreqInit(frequency);
    }
    
    void Controller::set_modulation_frequency(int voice, float frequency) {
        key_stack_.setLfoFreq(voice, frequency);
    }
    
    void Controller::set_modulation_mod_f(int voice, float frequency) {
        key_stack_.setLfoModFreq(voice, frequency);
    }
    
/*    void Controller::set_modulation_source(ModulationSource src) {
        modulation_source_ = src;
        reset_routing();
    }*/
    
    void Controller::set_modulation_destination(ModulationDestination dest) {
        modulation_destination_ = dest;
        //        reset_routing();
    }
    
    void Controller::set_modulation_destination(int dest) {
        switch (dest) {
            case 1:
                modulation_destination_ = LFO_DEST_AMP;
                break;
                
            case 2:
                modulation_destination_ = LFO_DEST_FILTER;
                break;
                
            case 3:
                modulation_destination_ = LFO_DEST_PITCH;
                break;
                
            case 4:
                modulation_destination_ = LFO_DEST_PW;
                break;
                
            default:
                modulation_destination_ = LFO_DEST_NONE;
                break;
        }
        //        reset_routing();
    }
    /*
    void Controller::reset_routing() {
        switch (modulation_source_) {
            case LFO_SRC_SQUARE:
                modulation_osc_.set_wave_type(Oscillator::SQUARE);
                break;
            case LFO_SRC_TRIANGLE:
                modulation_osc_.set_wave_type(Oscillator::TRIANGLE);
                break;
            case LFO_SRC_SAWTOOTH:
                modulation_osc_.set_wave_type(Oscillator::SAWTOOTH);
                break;
            case LFO_SRC_REVERSE_SAWTOOTH:
                modulation_osc_.set_wave_type(Oscillator::REVERSE_SAWTOOTH);
                break;
            default:
                assert(false);
        }
    }
     */

    void Controller::set_filter_cutoff(float frequency) {
        key_stack_.setFilterCutoff(frequency);
    }

    void Controller::set_mod_filter_cutoff(float v)
    {
        key_stack_.setModCutoff(v);
    }

    void Controller::set_mod_filter_resonance(float v)
    {
        key_stack_.setModRes(v);
    }

    void Controller::set_modulation(int voice, float v)
    {
        key_stack_.setModulation(voice,v);
    }

    void Controller::set_filter_cutoff(int voice, float frequency) {
        key_stack_.setFilterCutoff(voice, frequency);
    }

    void Controller::set_filter_resonance(int voice, float value) {
        key_stack_.setFilterRes(voice, value);
    }
    
    void Controller::set_filter_resonance(float value) {
        key_stack_.setFilterRes(value);
    }
    
    void Controller::GetFloatSamples(float* buffer, int size) {
        for (int i = 0; i < size; ++i) {
            buffer[i] = GetSample();
        }
    }
    
    void Controller::GetInt32Sapmles(int* buffer, int size) {
        //qDebug() << "get samples " <<  size << " from " <<  buffer;
        for (int i = 0; i < size; ++i) {
            buffer[i] = GetSample()* 16777216L;
        }
    }
    
    void Controller::setADSR(int n, long a, long d, float s, long r) {
        key_stack_.setADSR(n,a,d,s,r);
    }

    void Controller::setAttack(int n, long a) {
        key_stack_.setAttack(n,a);
    }

    void Controller::setDecay(int n, long d) {
        key_stack_.setDecay(n,d);
    }

    void Controller::setSustain(int n, float s) {
        key_stack_.setSustain(n,s);
    }

    void Controller::setRelease(int n, int r) {
        key_stack_.setRelease(n,r);
    }

    void Controller::GetCharSamples(char* buffer, int size) {
        
        if(format!=0) {
            //qDebug() << " size " << size << " sampleBytes " << sampleBytes;
            //Q_ASSERT(size % sampleBytes == 0);
            Q_UNUSED(sampleBytes) // suppress warning in release builds
            unsigned char *ptr = reinterpret_cast<unsigned char *>(buffer);
            while (size) {
                qreal x=GetSample();
                for (int i=0; i<format->channelCount(); ++i) {
                    if (format->sampleSize() == 8 && format->sampleType() == QAudioFormat::UnSignedInt) {
                        const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                        *reinterpret_cast<quint8*>(ptr) = value;
                    } else if (format->sampleSize() == 8 && format->sampleType() == QAudioFormat::SignedInt) {
                        const qint8 value = static_cast<qint8>(x * 127);
                        *reinterpret_cast<quint8*>(ptr) = value;
                    } else if (format->sampleSize() == 16 && format->sampleType() == QAudioFormat::UnSignedInt) {
                        quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                        if (format->byteOrder() == QAudioFormat::LittleEndian)
                            qToLittleEndian<quint16>(value, ptr);
                        else
                            qToBigEndian<quint16>(value, ptr);
                    } else if (format->sampleSize() == 16 && format->sampleType() == QAudioFormat::SignedInt) {
                        qint16 value = static_cast<qint16>(x * 32767);
                        if (format->byteOrder() == QAudioFormat::LittleEndian)
                            qToLittleEndian<qint16>(value, ptr);
                        else
                            qToBigEndian<qint16>(value, ptr);
                    }
                    
                    ptr += channelBytes;
                    size -= channelBytes;
                }
            }
        }
    }
    
    void Controller::setFormat(QAudioFormat *f)
    {
        format=f;
        channelBytes = format->sampleSize() / 8;
        sampleBytes = format->channelCount() * channelBytes;
    }
    
    float Controller::GetSample() {
        
        float value=0;
        float amp_mod=1;
        float mod_value=0;
        
        for(int i=0;i<key_stack_.GetSize();i++) {
            switch (modulation_destination_) {
                case LFO_DEST_AMP:
                    amp_mod-=(key_stack_.getLfo(i)->GetValue()+1)/2*key_stack_.getModAmt(i);
                    break;
                case LFO_DEST_PW:
                    mod_value=key_stack_.getLfo(i)->GetValue()*key_stack_.getModAmt(i);
                    key_stack_.getOsc(i)->set_mod_pw(mod_value);
                    break;
                case LFO_DEST_PITCH:
                    mod_value=key_stack_.getLfo(i)->GetValue()*key_stack_.getModAmt(i);
                    key_stack_.getOsc(i)->set_mod_f(mod_value);
                    break;
                case LFO_DEST_FILTER:
                    break;
                case LFO_DEST_NONE:
                    break;
            }
            
            value += key_stack_.getFilter(i)->GetValue(key_stack_.getOsc(i)->GetValue());
            // value+=key_stack_.getOsc(i)->GetValue();
            
            // Clip!
            value*=key_stack_.getEnvelope(0, i)->GetValue();
            value*=amp_mod;
            value = fmaxf(-1.0f, value);
            value = fminf(1.0f, value);
        }
        // Clip!
        value = fmaxf(-1.0f, value);
        value = fminf(1.0f, value);
        value *= volume_;
        // Adjust volume
        for(int i=0;i<key_stack_.GetSize();i++) {
            if(key_stack_.getEnvelope(0, i)->released()) {
                //qDebug() << "controller: clear note " << key_stack_.GetNote(i);
                key_stack_.NoteClear(key_stack_.GetNote(i));
            }
        }
        return value;
    }
}  // namespace synth
