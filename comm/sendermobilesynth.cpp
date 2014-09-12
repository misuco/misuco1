#include "platform.h"
#include "sendermobilesynth.h"

#ifdef RC1_IOS

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    
    syco=new mobilesynthview::Widget();
    sy=new synth::Controller();
    syco->setController(sy);
}

void SenderMobileSynth::cc(int chan, int voiceId, int cc, float v1)
{
//    qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;

    if(cc==2) {
//        qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;
        sy->set_filter_cutoff(voiceId,v1);
    } else if(cc==1) {
        sy->set_filter_resonance(voiceId,v1);
    } else if(cc==3) {
        sy->set_modulation_amount(voiceId, v1);
    } else if(cc==4) {
        sy->set_modulation_mod_f(voiceId, v1);
    } else if(cc==100) {
        sy->set_osc1_wave_type_int(v1);
    } else if(cc==101) {
        sy->set_lfo_wave_type_int(v1);
    } else if(cc==102) {
        sy->set_modulation_destination(v1);
    } else if(cc==103) {
        switch ((int)v1%4) {
            case 0:
                sy->setADSR(0, 10, 300, 0.5, 500);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_NONE);
                break;
                
            case 1:
                sy->setADSR(0, 10, 5000, 0, 5000);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_AMP);
                break;
                
            case 2:
                sy->setADSR(0, 10, 300, 0.5, 40000);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_PW);
                break;
                
            case 3:
                sy->setADSR(0, 40000, 10, 1, 80000);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_PITCH);
                break;
                
            default:
                break;
        }
    } else if(cc==104) {
//        qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;
        sy->set_filter_resonance(v1);
    }
    
    /*
    if(cc==10) {
        sy->set_osc_pw(voiceId,v1);
    }
    if(cc==11) {
        sy->set_modulation_amount(voiceId,v1);
    }
    if(cc==12) {
        sy->set_modulation_frequency(voiceId,1+v1*20);
    }
     */
}

void SenderMobileSynth::pc(int chan, int v1)
{
    /*
//    sy->pc(v1);
    sy->set_osc1_wave_type_int(v1%5);
    sy->set_modulation_destination(synth::Controller::LFO_DEST_NONE);
    switch (v1%4) {
        case 0:
            sy->setADSR(0, 10, 300, 0.5, 500);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_NONE);
            break;
            
        case 1:
            sy->setADSR(0, 10, 300, 0.5, 5000);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_AMP);
            break;
            
        case 2:
            sy->setADSR(0, 10, 300, 0.5, 40000);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_PW);
            break;
            
        case 3:
            sy->setADSR(0, 10, 300, 0.5, 80000);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_PITCH);
            break;
            
        default:
            break;
    }
    sy->set_modulation_amount(0);

    if(v1>7) {
        sy->set_modulation_amount(1.0);
    } else if(v1>3) {
        sy->set_modulation_amount(0.5);
    } else {
        sy->set_modulation_amount(0);
    } */
}

void SenderMobileSynth::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int vel)
{
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int chan,int voiceId)
{
    sy->NoteOff(voiceId);
}

void SenderMobileSynth::pitch(int chan, int voiceId, float f, int midinote, int pitch) {
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#else

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    sy= new mobileSynthQT52();
    //qDebug() << "mobileSynthQt52 created";
}

void SenderMobileSynth::cc(int chan, int voiceId, int cc, float v1)
{
    if(cc==2) {
//        qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;
        sy->getSyctl()->set_modulation(voiceId,v1);
//    } else if(cc==1) {
//        sy->getSyctl()->set_filter_resonance(voiceId,v1);
//    } else if(cc==3) {
//        sy->getSyctl()->set_modulation_amount(voiceId, v1);
//    } else if(cc==4) {
//        sy->getSyctl()->set_modulation_mod_f(voiceId, v1);
    } else if(cc==100) {
        sy->getSyctl()->set_osc1_wave_type_int(v1);
//    } else if(cc==101) {
//        sy->getSyctl()->set_lfo_wave_type_int(v1);
//    } else if(cc==102) {
//        sy->getSyctl()->set_modulation_destination(v1);
 /*   } else if(cc==103) {
        switch ((int)v1%4) {
            case 0:
                sy->getSyctl()->setADSR(0, 10, 300, 0.5, 500);
                break;
            case 1:
                sy->getSyctl()->setADSR(0, 10, 5000, 0, 5000);
                break;
            case 2:
                sy->getSyctl()->setADSR(0, 10, 300, 0.5, 40000);
                break;
            case 3:
                sy->getSyctl()->setADSR(0, 40000, 10, 1, 80000);
                break;

            default:
                break;
        }*/
    } else if(cc==104) {
        sy->getSyctl()->set_filter_resonance(v1);
    } else if(cc==105) {
        sy->getSyctl()->setAttack(voiceId, v1*700.0f);
    } else if(cc==106) {
        sy->getSyctl()->setDecay(voiceId, v1*700.0f);
    } else if(cc==107) {
        sy->getSyctl()->setSustain(voiceId, v1/127.0f);
    } else if(cc==108) {
        sy->getSyctl()->setRelease(voiceId, v1*700.0f);
    } else if(cc==109) {
        sy->getSyctl()->set_filter_cutoff(v1/127.0f);
    } else if(cc==110) {
        sy->getSyctl()->set_filter_resonance(v1/32.0f);
    } else if(cc==111) {
        sy->getSyctl()->set_mod_filter_cutoff(v1/127.0f);
    } else if(cc==112) {
        sy->getSyctl()->set_mod_filter_resonance(v1/127.0f);
    } else if(cc==113) {
        sy->getSyctl()->set_volume(v1/127.0f);
    }
}

void SenderMobileSynth::pc(int chan, int v1)
{
}

void SenderMobileSynth::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int vel)
{
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int chan, int voiceId)
{
    sy->noteOff(voiceId);
}

void SenderMobileSynth::pitch(int chan, int voiceId, float f, int midinote, int pitch) {
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#endif
