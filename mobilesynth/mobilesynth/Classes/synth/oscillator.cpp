// oscillator.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/oscillator.h"
#include <assert.h>
#include <math.h>
#include "synth/parameter.h"

namespace synth {
    
    Oscillator::Oscillator()
    : wave_type_(SINE),
    frequency_(0),
    sample_rate_(kDefaultSampleRate),
    sample_num_(0),
    sample_num_norm_(0),
    pulse_width_(0.5),
    rise_(true){ }
    
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
                wave_type_ = Oscillator::SINE;
                break;
            case 1:
                wave_type_ = Oscillator::SQUARE;
                break;
            case 2:
                wave_type_ = Oscillator::SAWTOOTH;
                break;
            case 3:
                wave_type_ = Oscillator::TRIANGLE;
                break;
            default:
                wave_type_ = Oscillator::REVERSE_SAWTOOTH;
                break;
        }
    }

    void Oscillator::set_frequency(float frequency) {
        if(frequency_==0 ) {
            frequency_ = frequency;
            calc_steps();
            calc_edges();
        }
        frequency_new_ = frequency;
    }
    
    float Oscillator::GetValue() {
        if (frequency_ == 0) {
            return 0.0f;
        }
        //  float freq = frequency_->GetValue();
        if (frequency_ < 0.01f) {
            return 0.0f;
        }
        /*
         long period_samples = sample_rate_ / frequency_;
         sample_num = sample_num % (long)period_samples;
         */
        if (period_samples_ == 0) {
            return 0.0f;
        }
        switch (wave_type_) {
            case SINE:
                value = sinf(sample_num_norm_);
                break;
            case SQUARE:
                if (sample_num_norm_ < (pulse_width_)) {
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
            case REVERSE_SAWTOOTH:
                value = 2.0f * (floorf(sample_num_norm_) - sample_num_norm_ + 0.5f);
                break;
            default:
                assert(false);
                break;
        }
        sample_num_++;
        sample_num_norm_+=sample_step_norm_;
        if(sample_num_>= (long)period_samples_) {
            sample_num_=0;
            sample_num_norm_=0;
            rise_=true;
            value=0;
            if(frequency_!=frequency_new_) {
                frequency_=frequency_new_;
                calc_edges();
                calc_steps();
            }
        }
        return value;
    }
    
    void Oscillator::set_pulse_width(float p) {
        pulse_width_ = p;
        calc_edges();
    }
    
    void Oscillator::calc_edges() {
        rise_val_=4.0/period_samples_*pulse_width_;
        if(rise_val_>1.0) {
            rise_val_=1.0;
        }
        fall_val_=4.0/period_samples_*(1-pulse_width_);
        if(fall_val_>1.0) {
            fall_val_=1.0;
        }
    }
    
    void Oscillator::calc_steps() {
        period_samples_=sample_rate_/frequency_;
        if(wave_type_==SINE) {
            sample_step_norm_ = (2.0f * M_PI / (float)period_samples_);
        } else {
            sample_step_norm_ = (1.0 / (float)period_samples_);
        }
    }
    
}  // namespace synth
