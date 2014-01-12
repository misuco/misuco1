// key_stack.cpp
// Author: Allen Porter <allen@thebends.org>

#include "key_stack.h"
#include <math.h>
#include <assert.h>
#include <QDebug>

using namespace std;

namespace synth {
    
    KeyStack::KeyStack() : size_(0) {
        for(int i=0;i<kMaxSize;i++) {
            for(int j=0;j<kNumEnv;j++) {
                envelopes[j][i]=new Envelope();
            }
            oscs[i]=new Oscillator();
            lfos[i]=new Oscillator();
            lfos[i]->set_frequency(2.0);
            mod_amt_[i]=0;
            cutoffs[i]=new FilterCutoff();
            cutoffs[i]->set_envelope(envelopes[1][i]);
            cutoffs[i]->set_cutoff(5000);
            filters[i]=new ResonantFilter();
            filters[i]->set_cutoff(cutoffs[i]);
        }
        mod_amt_init_=0;
        lfo_freq_init_=2.0;
    }
    
    KeyStack::~KeyStack() { }
    
    bool KeyStack::NoteOn(int note, float freq) {
        //qDebug() << "key stack note on " << note << " f: " << freq << " size: " << size_;
        
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                oscs[i]->set_frequency(freq);
                //qDebug() << "F   stack note on " << note << " f: " << freq << " size: " << size_;
                return false;
            }
        }
        
        
        // if stack full, clear bottom of stack
        // => kill oldest note
        if(size_ >= kMaxSize) {
            NoteClear(notes_[0]);
            //qDebug() << "key stack full, NoteClear " << notes_[0];
        }
        
        // put new note on top of stack
        notes_[size_] = note;
        oscs[size_]->set_frequency(freq);
        oscs[size_]->set_pulse_width(osc_pw);
        oscs[size_]->set_wave_type(osc_wave);
        lfos[size_]->set_frequency(lfo_freq_init_);
        mod_amt_[size_]=mod_amt_init_;

        for(int i=0;i<kNumEnv;i++) {
            envelopes[i][size_]->set_attack(env_a[i]);
            envelopes[i][size_]->set_decay(env_d[i]);
            envelopes[i][size_]->set_sustain(env_s[i]);
            envelopes[i][size_]->set_release(env_r[i]);
            envelopes[i][size_]->NoteOn();
        }
        size_++;
        //qDebug() << "T   stack note on " << note << " f: " << freq << " size: " << size_;
        return true;
    }
    
    bool KeyStack::NoteOff(int note) {
        //qDebug() << "key stack note off " << note << " size: " << size_;
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                for(int k=0;k<kNumEnv;k++) {
                    envelopes[k][i]->NoteOff();
                }
                //qDebug() << "T   stack note off " << note << " size: " << size_;
                return true;
            }
        }
        // The note wasn't on the stack.  The multi-touch events on the iphone seem
        // to be flaky, so we don't worry if we were asked to remove something that
        // was not on the stack.  The controller also calls our clear() method when
        // no touch events are left as a fallback.
        //qDebug() << "F   stack note off " << note << " size: " << size_;
        return false;
    }
    
    bool KeyStack::NoteClear(int note) {
        //qDebug() << "-  NoteClear " << note;
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                // Remove this element from the stack -- copy all elements above
                Envelope * ex[kNumEnv];
                Filter * fi;
                FilterCutoff * cu;
                Oscillator * osc;
                Oscillator * lfo;
                float mod_amt = mod_amt_[i];
                for(int k=0;k<kNumEnv;k++) {
                    ex[k]=envelopes[k][i];
                }
                fi=filters[i];
                cu=cutoffs[i];
                osc=oscs[i];
                lfo=lfos[i];
                for (int j = i; j < size_ - 1; ++j) {
                    notes_[j] = notes_[j + 1];
                    for(int k=0;k<kNumEnv;k++) {
                        envelopes[k][j]=(envelopes[k][j + 1]);
                    }
                    filters[j]=(filters[j + 1]);
                    cutoffs[j]=(cutoffs[j + 1]);
                    oscs[j]=(oscs[j + 1]);
                    lfos[j]=(lfos[j + 1]);
                    mod_amt_[j]=(mod_amt_[j + 1]);
                }
                for(int k=0;k<kNumEnv;k++) {
                    envelopes[k][size_-1]=ex[k];
                }
                cutoffs[size_-1]=cu;
                filters[size_-1]=fi;
                oscs[size_-1]=osc;
                lfos[size_-1]=lfo;
                mod_amt_[size_-1]=mod_amt;
                size_--;
                //qDebug() << "-T NoteClear " << note;
                return true;
            }
        }
        // The note wasn't on the stack.  The multi-touch events on the iphone seem
        // to be flaky, so we don't worry if we were asked to remove something that
        // was not on the stack.  The controller also calls our clear() method when
        // no touch events are left as a fallback.
        // qDebug() << "-F NoteClear " << note;
        return false;
    }
    /*
    bool KeyStack::IsNoteInStack(int note) {
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                return true;
            }
        }
        return false;
    }
    */
    int KeyStack::size() {
        return size_;
    }
    
    int KeyStack::GetCurrentNote() {
        if (size_ > 0) {
            return notes_[size_ - 1];
        }
        return 0;
    }
    
    int KeyStack::GetSize() {
        return size_;
    }
    
    int KeyStack::GetNote(int num) {
        if (num >= size_) {
            return 0;
        }
        return notes_[num];
    }

    void KeyStack::SetSampleRate(float s) {
        sample_rate_=s;
        for(int i=0;i<kMaxSize;i++) {
            oscs[i]->set_sample_rate(s);
            lfos[i]->set_sample_rate(s);
        }
    }
    
    void KeyStack::setOscPW(int note, float pw) {
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                oscs[i]->set_pulse_width(pw);
                i=size_;
            }
        }
        osc_pw=pw;
    }
    
    void KeyStack::setModAmt(int note, float v ) {
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                mod_amt_[i]=v;
                i=size_;
            }
        }
    }
    
    void KeyStack::setLfoFreq(int note, float v ) {
        for (int i = 0; i < size_; ++i) {
            if (notes_[i] == note) {
                lfos[i]->set_frequency(v);
                i=size_;
            }
        }
    }
    
}  // namespace synth
