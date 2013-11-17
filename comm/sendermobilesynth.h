#ifndef SENDERMOBILESYNTH_H
#define SENDERMOBILESYNTH_H

#include "rc1.h"
#include "isender.h"
#include "../mobilesynth/mobilesynth/Classes/mobilesynthviewcontrollerrc1.hpp"

class SenderMobileSynth : public ISender
{
public:
    SenderMobileSynth(RC1 * rc1);
    virtual void cc(int c, int voiceId, int cc, double v1);
    virtual void pc(int c, int v1);
    virtual void note(int c, int voiceId, double f, int vel);
    virtual void pitch(int c, int voiceId, double f);
    virtual void setDestination(QHostAddress a,int p);
    
private:
    mobilesynthview::Widget * sy;
};

#endif // SENDERMOBILESYNTH_H
