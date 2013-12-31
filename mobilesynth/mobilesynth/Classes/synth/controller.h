// controller.h
// Author: Allen Porter <allen@thebends.org>
// Polyphony by Claudio Zopfi <z@x21.ch>
//
// The controller module returns samples and drives the oscillator and envelope.

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <QAudioFormat>

#include "synth/arpeggio.h"
#include "synth/envelope.h"
#include "synth/filter.h"
#include "synth/lag_processor.h"
#include "synth/modulation.h"
#include "synth/oscillator.h"
#include "synth/parameter.h"
#include "synth/key_stack.h"

namespace synth {
    
    class Controller {
    public:
        Controller();
        
        // Volume [0, 1.0]
        void set_volume(float volume);
        
        // Start/Stop playing a note.  These may trigger the Attack and Release of the
        // volume and filter envelopes, depending on the order of the on/off events.
        // It is an error to call NoteOff() for a note that was never the argument of
        // NoteOn();
        void NoteOn(int midi_note, float freq);
        void NoteOff(int midi_note);
        //        void NoteOnFrequency(float frequency);  // For testing
        void NoteOff();  // Invoked when all notes have been released as a fallback
        
        // True when nothing is playing
        bool released() {
            return key_stack_.GetSize()==0;
        }
        
        void set_sample_rate(float sample_rate);  // For testing
        void setADSR(int n, long a, long d, float s, long r);
        
        // A shift in frequency by the specified amount.  The frequency gets
        // multiplied by 2^n
        enum OctaveShift {
            OCTAVE_1 = 1,  // No shift
            OCTAVE_2 = 2,
            OCTAVE_4 = 4,
            OCTAVE_8 = 8,
            OCTAVE_16 = 16
        };
        
        // OSC 1
        
        // Set the volume of oscillator
        // void set_osc1_level(float level);
        // Set the wave form of oscillator
        void set_osc1_wave_type(Oscillator::WaveType wave_type);
        void set_osc1_wave_type_int(int w);
        
        enum ModulationSource {
            LFO_SRC_SQUARE,
            LFO_SRC_TRIANGLE,
            LFO_SRC_SAWTOOTH,
            LFO_SRC_REVERSE_SAWTOOTH,
        };
        enum ModulationDestination {
            LFO_DEST_WAVE,  // Tremelo
            LFO_DEST_PITCH,  // Vibrato
            LFO_DEST_FILTER,
        };
        void set_modulation_source(ModulationSource source);
        void set_modulation_destination(ModulationDestination dest);
        void set_modulation_amount(float amount);
        void set_modulation_frequency(float frequency);
        void set_osc_pw(float p);
        void set_filter_cutoff(float frequency);
        
        // [0.0, 1.0]
        void set_filter_resonance(float value);
        
        // Get a single sample
        float GetSample();
        
        void GetFloatSamples(float* buffer, int size);
        void GetCharSamples(char *buffer, int size);
        
        void setFormat(QAudioFormat * f);
        
    private:
        // Invoked when one of the routing parameters changes, such as the source
        // or destination of modulation.
        void reset_routing();
        
        KeyStack key_stack_;
        
//        Oscillator osc1_;
        
        bool osc_sync_;
        float sample_rate_;
        
        ModulationSource modulation_source_;
        ModulationDestination modulation_destination_;
        MutableParameter modulation_frequency_;
        Oscillator modulation_osc_;
        MutableParameter modulation_amount_;
        LFO modulation_;
        long modulation_osc_pos_;
        long modulation_osc_period_;
        
        QAudioFormat * format;
        int channelBytes;
        int sampleBytes;
        
    };
    
}  // namespace synth

#endif // __CONTROLLER_H__
