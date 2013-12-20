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
    : modulation_source_(LFO_SRC_SQUARE),
    modulation_destination_(LFO_DEST_WAVE),
    modulation_frequency_(0.0f),
    modulation_amount_(0.0f) {
        modulation_osc_.set_frequency(modulation_frequency_.GetValue());
        modulation_.set_oscillator(&modulation_osc_);
        modulation_.set_level(&modulation_amount_);
        key_stack_.setADSR(0, 1000, 1000, 0.8, 80000);
        key_stack_.setADSR(1, 80000,   0,   1, 80000);
        format=0;
        reset_routing();
    }
    void Controller::set_sample_rate(float sample_rate) {
        osc1_.set_sample_rate(sample_rate);
        sample_rate_=sample_rate;
        key_stack_.SetSampleRate(sample_rate);
    }
    
    void Controller::NoteOn(int note, float freq) {
        key_stack_.NoteOn(note, freq);
    }
    
    /*    void Controller::NoteOnFrequency(float frequency) {
     //  key_frequency_.set_value(frequency);
     //  volume_envelope()->NoteOn();
     //  filter_envelope()->NoteOn();
     }
     */
    void Controller::NoteOff(int note) {
        key_stack_.NoteOff(note);
        if (key_stack_.size() == 0) {
            // All notes were release, so start the release phase of the envelope
            NoteOff();
        }
    }
    
    void Controller::NoteOff() {
        key_stack_.clear();
    }
    
    void Controller::set_osc1_wave_type(Oscillator::WaveType wave_type) {
        osc1_.set_wave_type(wave_type);
    }
    
    void Controller::set_osc1_wave_type_int(int w) {
        switch (w) {
            case 0:
                osc1_.set_wave_type(Oscillator::SINE);
                break;
            case 1:
                osc1_.set_wave_type(Oscillator::SQUARE);
                break;
            case 2:
                osc1_.set_wave_type(Oscillator::SAWTOOTH);
                break;
            case 3:
                osc1_.set_wave_type(Oscillator::TRIANGLE);
                break;
            default:
                osc1_.set_wave_type(Oscillator::REVERSE_SAWTOOTH);
                break;
        }
    }
    
    void Controller::set_modulation_amount(float amount) {
        modulation_amount_.set_value(amount);
    }
    
    void Controller::set_modulation_frequency(float frequency) {
        modulation_frequency_.set_value(frequency);
    }
    
    void Controller::set_modulation_source(ModulationSource src) {
        modulation_source_ = src;
        reset_routing();
    }
    
    void Controller::set_modulation_destination(ModulationDestination dest) {
        modulation_destination_ = dest;
        reset_routing();
    }
    
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
        
        // Reset the destinations
        //volume_.set_modulation(NULL);
        //filter_cutoff_.set_modulation(NULL);
        //  combined_osc_.set_frequency_modulation(NULL);
        
        // Route modulation into the correct pipeline
        switch (modulation_destination_) {
            case LFO_DEST_WAVE:
                // Modulate the volume (tremelo)
                //      volume_.set_modulation(&modulation_);
                break;
            case LFO_DEST_PITCH:
                // Modulate the frequency (vibrato)
                //      combined_osc_.set_frequency_modulation(&modulation_);
                break;
            case LFO_DEST_FILTER:
                // Modulate the cutoff frequency
                // filter_cutoff_.set_modulation(&modulation_);
                break;
            default:
                assert(false);
        }
    }
    
    void Controller::set_filter_cutoff(float frequency) {
    }
    
    void Controller::set_filter_resonance(float value) {
    }
    
    void Controller::GetFloatSamples(float* buffer, int size) {
        for (int i = 0; i < size; ++i) {
            buffer[i] = GetSample();
        }
    }
    
    void Controller::setADSR(int n, long a, long d, float s, long r) {
        key_stack_.setADSR(n,a,d,s,r);
    }
    
    void Controller::GetCharSamples(char* buffer, int size) {
        
        if(format!=0) {
            Q_ASSERT(size % sampleBytes == 0);
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
        for(int i=0;i<key_stack_.GetSize();i++) {
            if(key_stack_.GetFreq(i)!=key_stack_.GetFreq1(i)) {
                float oldPos=key_stack_.GetPos(i);
                float divisor=key_stack_.GetFreq1(i)/key_stack_.GetFreq(i);
                float newPos=oldPos*divisor;
                long newPosL=(long)newPos;
                key_stack_.SetPos(i, newPosL);
                key_stack_.SetFreq1(i, key_stack_.GetFreq(i));
            }
            //    key_frequency_.set_value(key_stack_.GetFreq(i));
            osc1_.set_frequency(key_stack_.GetFreq(i));
            
            value += key_stack_.getFilter(i)->GetValue(osc1_.GetValue(key_stack_.GetPos(i)));
            
            // Clip!
            value = fmaxf(-1.0f, value);
            value = fminf(1.0f, value);
            //    long period_samples = sample_rate_ / key_stack_.GetFreq(i);
            value*=key_stack_.getEnvelope(0, i)->GetValue();
            
            key_stack_.SetPos(i, key_stack_.GetPos(i)+1 );
            
        }
        // Clip!
        value = fmaxf(-1.0f, value);
        value = fminf(1.0f, value);
        // Adjust volume
        for(int i=0;i<key_stack_.GetSize();i++) {
            if(key_stack_.getEnvelope(0, i)->released()) {
                key_stack_.NoteClear(key_stack_.GetNote(i));
            }
        }
        return value;
    }
}  // namespace synth
