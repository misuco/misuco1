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
    sample_num_(0) { }
    
    Oscillator::~Oscillator() { }
    
    void Oscillator::set_sample_rate(long sample_rate) {
        sample_rate_ = sample_rate;
    }
    
    void Oscillator::set_wave_type(WaveType wave_type) {
        wave_type_ = wave_type;
    }
    
    void Oscillator::set_frequency(float frequency) {
        frequency_ = frequency;
    }
    
    void Oscillator::set_period_samples(long p) {
        period_samples_ = p;
    }
    
    float Oscillator::GetValue(int sample_num) {
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
        float x = (sample_num / (float)period_samples_);
        float value = 0;
        switch (wave_type_) {
            case SINE:
                value = sinf(2.0f * M_PI * x);
                break;
            case SQUARE:
                if (sample_num < (period_samples_ / 2)) {
                    value = 1.0f;
                } else {
                    value = -1.0f;
                }
                break;
            case TRIANGLE:
                value = (2.0f * fabs(2.0f * x - 2.0f * floorf(x) - 1.0f) - 1.0f);
                break;
            case SAWTOOTH:
                value = 2.0f * (x - floorf(x) - 0.5f);
                break;
            case REVERSE_SAWTOOTH:
                value = 2.0f * (floorf(x) - x + 0.5f);
                break;
            default:
                assert(false);
                break;
        }
        //  sample_num_ = (sample_num_ + 1) % (long)period_samples;
        return value;
    }
}  // namespace synth
