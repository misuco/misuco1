#ifndef SENDERMOBILESYNTH_H
#define SENDERMOBILESYNTH_H

#include "rc1.h"
#include "isender.h"
#ifdef IOS
#include "../mobilesynth/mobilesynth/Classes/mobilesynthviewcontrollerrc1.hpp"
#else
#include "../mobilesynthqt52.h"
#endif

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
#ifdef IOS
    mobilesynthview::Widget * sy;
#else
    mobileSynthQT52 * sy;
#endif
};

#endif // SENDERMOBILESYNTH_H
