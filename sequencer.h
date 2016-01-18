#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <list>
#include "rc1.h"
#include "conf/sequence.h"

class RC1;

struct noteEvent {
    int note;
    int eventId;
    int channel;
};

class Sequencer
{
public:
    Sequencer(RC1 *rc1, float bpm);
    ~Sequencer();
    bool syncMaster;
    int noSyncMasterFor;

    void doNow(long now);
    void play();
    void netPlay();
    void record();
    void stop();
    void netStop();
    void setBPM(int b);
    void setBar(int b);
    void setNbars(int n);
    void setStep(int n);
    int getStep() { return currStep;}
    bool isSyncMaster() {return syncMaster;}
    void noteOn(int chan, int voiceId, float f, int midinote, int pitch, int scalenote, int v);
    void noteOff(int chan, int voiceId);
    bool isOn(int scalenote);
    bool isManuStop() {return manuStop;}
    void setEdit(bool e) {edit=e;}

private:
    RC1 * rc1;
    long nowInit;
    float nextStepAt;
    float stepDiff;
    float bpm;
    bool run;
    bool rec;
    bool manuStop;
    bool edit;
    int currStep;
    std::list<noteEvent *> onNotes;
    std::list<noteEvent *> onRecNotes;

    void alloff();
    void playInt();

};

#endif // SEQUENCER_H
