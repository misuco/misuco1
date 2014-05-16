#ifndef WAVEFORM_H
#define WAVEFORM_H

class waveform
{
public:
    waveform();
    float getValue(int i);
    int getAttack();
    int getLoop();
    int getRelease();

private:
    float * values;
    int attack;
    int loop;
    int release;
};

#endif // WAVEFORM_H
