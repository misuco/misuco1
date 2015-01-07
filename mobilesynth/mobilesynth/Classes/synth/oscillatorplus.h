#ifndef OSCILLATORPLUS_H
#define OSCILLATORPLUS_H

#include "synth/parameter.h"
#include "synth/lag_processor.h"
#include "synth/waveform.h"

namespace synth {

    static const long kDefaultSampleRate = 44100;

    class OscillatorPlus : public Parameter {
    public:
        OscillatorPlus();
        virtual ~OscillatorPlus();

        enum WaveType {
            SINE,
            SQUARE,
            TRIANGLE,
            SAWTOOTH,
            REVERSE_SAWTOOTH,
            NOISE,
            WAVETABLE,
        };
        void set_wave_type(WaveType wave_type);
        void set_wave_type(int w);

        void set_frequency(float frequency);
        float get_frequency() {return frequency_new_;}
        void set_mod_f(float mod);
        void set_mod_pw(float mod);
        void set_period_samples(long p);
        void set_pulse_width(float p);

        void set_waveform(waveform * w);

        // Override the default sample rate
        void set_sample_rate(long sample_rate);

        // Returns the value at the specific time [0.0, 1.0].  The returned value
        // returned value is in the range [-1.0, 1.0].
        virtual float GetValue();

        // Start at the beginning of the period
        void Reset() { sample_num_ = 0; }

        // Returns true if this is the first sample in the period
        bool IsStart() { return (sample_num_ == 0); }

    private:
        WaveType wave_type_;
        waveform * waveform_;
        float frequency_;
        float frequency_new_;
        float mod_f_;
        float mod_f_new_;
        float mod_pw_;
        float mod_pw_new_;
        float frequency_mod_;   // the frequency including all modulations
        long period_samples_;
        float pulse_width_;
        float pulse_width_mod_; // the pw including modulation
        long sample_rate_;
        long sample_num_;
        float sample_num_trans_;
        float sample_num_norm_;
        float sample_step_norm_;

        float rise_val_;
        float fall_val_;
        bool rise_;
        float value;
        void calc_all();
        void calc_edges();
        void calc_steps();

    };

}  // namespace synth

#endif // OSCILLATORPLUS_H
