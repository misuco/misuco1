// key_stack.h
// Author: Allen Porter <allen@thebends.org>
//
// The key stack keeps track of notes pressed and released, determining when
// the attack phase should be entered or when the note should just change
// without starting a new note attack.

#ifndef __KEY_STACK_H__
#define __KEY_STACK_H__

#include "envelope.h"
#include "filter.h"
#include "oscillator.h"

namespace synth {
    
    class KeyStack {
    public:
        // We can't push more notes on the stack than this
        static const int kMaxSize = 4;
        static const int kNumEnv = 1;
        
        KeyStack();
        ~KeyStack();
        
        // Returns true if this was the first note pushed on to the key stack
        bool NoteOn(int note, float freq);
        
        // Returns true if this was the last note removed from the key stack
        bool NoteOff(int note);
        bool NoteClear(int note);
        
        // Returns the current not, or 0 if no note is playing.
        int GetCurrentNote();
        
        // Return the note at the specified position in the stack.  num must be less
        // than size.
        int GetNote(int num);
        
        int GetSize();
        float GetFreq(int num);
/*        float GetFreq1(int num);
        void SetFreq1(int num, float value);*/
        long GetPos(int num);
        long GetPeriodSamples(int num);
        void SetPos(int num,long value);
        void SetSampleRate(float s);
        
        bool IsNoteInStack(int note);
        
        int size();
        
        Envelope * getEnvelope(int i,int j) {
            return envelopes[i][j];
        }
        
        Filter * getFilter(int i) {
            return filters[i];
        }
        
        Oscillator * getOsc(int i) {
            return oscs[i];
        }
        
        void setOscPW(float pw) {
            osc_pw=pw;
        }
        
        void setOscWave(int w) {
            osc_wave=w;
        }
        
        void setADSR(int n, long a, long d, float s, long r) {env_a[n]=a;env_d[n]=d;env_s[n]=s;env_r[n]=r;};        
//        void clear() { size_ = 0; }
        
    private:
        int size_;
        int notes_[kMaxSize];
//        float freqs_[kMaxSize];
//        float freqs1_[kMaxSize];
//        long pos_[kMaxSize];
//        long period_samples_[kMaxSize];
        Envelope * envelopes[kNumEnv][kMaxSize];
        Filter * filters[kMaxSize];
        FilterCutoff * cutoffs[kMaxSize];
        Oscillator * oscs[kMaxSize];
        
        // Number of times the note at the position was pressed
        // int count_[kMaxSize];
        float sample_rate_;
        
        long env_a[kNumEnv];
        long env_d[kNumEnv];
        float env_s[kNumEnv];
        long env_r[kNumEnv];
        
        float osc_pw;
        int osc_wave;
    };
    
    float KeyToFrequency(int key);
    
}  // namespace synth

#endif  // __KEY_STACK_H__
