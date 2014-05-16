// oscillator.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/oscillator.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "synth/parameter.h"
#include <QDebug>

namespace synth {
    
    Oscillator::Oscillator()
    : wave_type_(SINE),
    frequency_(0),
    sample_rate_(kDefaultSampleRate),
    pulse_width_(0.5),
    sample_num_norm_(0),
    sample_num_(0),
    rise_(true){

    }
    
    Oscillator::~Oscillator() { }
    
    void Oscillator::set_sample_rate(long sample_rate) {
        sample_rate_ = sample_rate;
    }
    
    void Oscillator::set_wave_type(WaveType wave_type) {
        wave_type_ = wave_type;
    }
    
    void Oscillator::set_wave_type(int w) {
        switch (w) {
            case 0:
                wave_type_ = Oscillator::SQUARE;
                break;
            case 1:
                wave_type_ = Oscillator::SAWTOOTH;
                break;
            case 2:
                wave_type_ = Oscillator::SINE;
                break;
            case 3:
                wave_type_ = Oscillator::TRIANGLE;
                break;
            case 4:
                wave_type_ = Oscillator::NOISE;
                break;
            case 5:
                wave_type_ = Oscillator::WAVETABLE;
                break;
            default:
                wave_type_ = Oscillator::REVERSE_SAWTOOTH;
                break;
        }
    }

    void Oscillator::set_frequency(float frequency) {
        if(frequency_==0 ) {
            frequency_ = frequency;
            calc_all();
        }
        frequency_new_ = frequency;
        if(wave_type_ ==WAVETABLE) {
            sample_num_=0;
        }
    }
    
    void Oscillator::set_mod_f(float mod) {
        if(mod<-1) {
            mod_f_new_=-1;
        } else if(mod>1) {
            mod_f_new_=1;
        } else {
            mod_f_new_=mod;
        }
    }
    
    void Oscillator::set_mod_pw(float mod) {
        mod_pw_new_=mod;
    }
    
    void Oscillator::calc_all() {
        if(mod_f_>0) {
            frequency_mod_=frequency_*(1+mod_f_);
        } else if(mod_f_<0) {
            frequency_mod_=frequency_/(1+mod_f_);
        } else {
            frequency_mod_=frequency_;
            
        }
        calc_steps();
        calc_edges();
    }
    
    float Oscillator::GetValue() {
        /*
        if (frequency_ == 0) {
            return 0.0f;
        }
         */
        if (frequency_ < 0.01f) {
            return 0.0f;
        }
        if (period_samples_ == 0) {
            return 0.0f;
        }
        int sample_num_wt;
        switch (wave_type_) {
            case SINE:
                value = sinf(sample_num_norm_);
                break;
            case SQUARE:
                if (sample_num_norm_ < (pulse_width_mod_)) {
                    value = 1.0f;
                } else {
                    value = -1.0f;
                }
                break;
            case TRIANGLE:
//                value = (2.0f * fabs(2.0f * x - 2.0f * floorf(x) - 1.0f) - 1.0f);
                if(rise_) {
                    value+=rise_val_;
                    if(value>=1.0) {
                        value=1.0;
                        rise_=false;
                    }
                } else {
                    value-=fall_val_;
                    if(value<=-1.0) {
                        value=-1.0;
                        rise_=true;
                    }
                }
                break;
            case SAWTOOTH:
                value = 2.0f * (sample_num_norm_ - floorf(sample_num_norm_) - 0.5f);
                break;
            case NOISE:
                value = -1.0f + (float)rand()/((float)RAND_MAX/2.0f);
                break;
            case WAVETABLE:
                if(frequency_new_!=frequency_) {
                    sample_num_trans_=frequency_new_/65.4064f;
                    frequency_=frequency_new_;
                    //qDebug() << "sample_num_trans " << sample_num_trans_ << " f " << frequency_;
                }
                sample_num_wt =(int)((float)sample_num_*sample_num_trans_);
                if(sample_num_wt>waveform_->getLoop()) {
                    //qDebug() << "sample_num_wt " << sample_num_wt << " loop " << waveform_->getLoop();
                    sample_num_=waveform_->getAttack();
                    sample_num_wt =(int)((float)sample_num_*sample_num_trans_);
                }
                value = waveform_->getValue(sample_num_wt);
                sample_num_++;
                break;
            case REVERSE_SAWTOOTH:
                value = 2.0f * (floorf(sample_num_norm_) - sample_num_norm_ + 0.5f);
                break;
            default:
                assert(false);
                break;
        }
        if(wave_type_ !=WAVETABLE) {
            sample_num_++;
            sample_num_norm_+=sample_step_norm_;
            if(sample_num_>= (long)period_samples_) {
                sample_num_=0;
                sample_num_norm_=0;
                rise_=true;
                value=0;
                bool recal_freq_mod=false;
                if(frequency_!=frequency_new_) {
                    frequency_=frequency_new_;
                    recal_freq_mod=true;
                }
                if(mod_f_!=mod_f_new_) {
                    mod_f_=mod_f_new_;
                    recal_freq_mod=true;
                }
                if(mod_pw_!=mod_pw_new_) {
                    mod_pw_=mod_pw_new_;
                    recal_freq_mod=true;
                }
                if(recal_freq_mod) {
                    calc_all();
                }
            }
        }

        return value;
    }
    
    void Oscillator::set_pulse_width(float p) {
        pulse_width_ = p;
        calc_edges();
    }

    void Oscillator::set_waveform(waveform *w)
    {
        waveform_=w;
    }
    
    void Oscillator::calc_edges() {
        if(mod_pw_!=0) {
            pulse_width_mod_=(pulse_width_+pulse_width_*mod_pw_)/2;
        } else {
            pulse_width_mod_=pulse_width_;
        }

        if(pulse_width_mod_<sample_num_norm_) {
            pulse_width_mod_=sample_num_norm_;
            rise_val_=1.0;
            fall_val_=2.0/period_samples_;
        } else if(pulse_width_mod_>=1.0) {
            pulse_width_mod_=1.0-sample_num_norm_;
            rise_val_=1.0/period_samples_;
            fall_val_=2.0;
        } else {
            rise_val_=2.0/(period_samples_*pulse_width_mod_);
            fall_val_=2.0/(period_samples_*(1-pulse_width_mod_));
        }
        /*
        rise_val_=1.0/(1+(period_samples_*pulse_width_mod_/4));
        if(rise_val_>1.0) {
            rise_val_=1.0;
        }
        fall_val_=1.0/(1+(period_samples_*(2-pulse_width_mod_)/4));
        //fall_val_=4.0/period_samples_*(1-pulse_width_mod_);
        if(fall_val_>1.0) {
            fall_val_=1.0;
        }
         */
    }
    
    void Oscillator::calc_steps() {
        period_samples_=sample_rate_/frequency_mod_;
        if(wave_type_==SINE) {
            sample_step_norm_ = (2.0f * M_PI / (float)period_samples_);
        } else {
            sample_step_norm_ = (1.0 / (float)period_samples_);
        }
    }

}  // namespace synth
