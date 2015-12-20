#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <list>
#include "rc1.h"
#include "conf/sequence.h"

struct noteEvent {
    int note;
    int eventId;
};

class Sequencer
{
public:
    Sequencer(RC1 *rc1, float bpm, Sequence * seq);
    ~Sequencer();
    void doNow(long now);
    void play();

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

};

#endif // SEQUENCER_H
