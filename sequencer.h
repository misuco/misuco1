#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <list>
#include "rc1.h"
#include "conf/sequence.h"

struct noteEvent {
    int note;
    int eventId;
    int channel;
};

class Sequencer
{
public:
    Sequencer(RC1 *rc1, float bpm, Sequence * seq);
    ~Sequencer();
    void doNow(long now);
    void play();
    void stop();
    void setBPM(int b);
    void setNbars(int n);
    void setStep(int n);
    int getStep() { if(currStep==0) {
            return seq->getNsteps()-1;
        } else
        return currStep-1;}

private:
    RC1 * rc1;
    Sequence * seq;
    long nowInit;
    float nextStepAt;
    float stepDiff;
    float bpm;
    bool run;
    bool rec;
    int currStep;
    std::list<noteEvent *> onNotes;

    void alloff();

};

#endif // SEQUENCER_H
