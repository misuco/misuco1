#include "sequencer.h"

Sequencer::Sequencer(RC1 * rc1, float bpm)
{
    this->rc1=rc1;
    this->bpm=bpm;
    this->currStep=0;
    this->run=false;
    this->edit=false;
    this->syncMaster=true;
    this->noSyncMasterFor=0;
}

Sequencer::~Sequencer()
{

}

void Sequencer::doNow(long now)
{
    int note;
    int stepNNotes;
    bool alreadyOn;
    bool nolongerOn;
    LayoutModel * l;

    std::list<noteEvent *>::iterator it;
    if(now-nowInit>=nextStepAt && this->run) {
        Sequence * seq=rc1->layout->getCurrentSeq();
        noSyncMasterFor++;
        if(noSyncMasterFor>20) {
            syncMaster=true;
        }

        currStep++;
        if(currStep>=seq->getNsteps()) currStep=0;

        if(syncMaster) {
            if(currStep%seq->getNbars()==0) {
                int bar=currStep/seq->getNbars();
                rc1->sender->sync(1,bar,bpm);
                qDebug() << "send sync " << bar << " " << bpm;
            }
        }

        if(rec) {
            for (it=onRecNotes.begin(); it!=onRecNotes.end(); ++it) {
                nolongerOn=true;
                noteEvent * ne = *it;
                seq->addNote(currStep,ne->note);
            }

        }

        stepNNotes=seq->getStepNNotes(currStep);

        //qDebug() << "do step " << currStep << " stepNotes " << stepNNotes;
        l=rc1->getLayout();
        for(int i=0;i<stepNNotes;i++) {
            note=seq->getStepNote(currStep,i);
            //qDebug() << "check note " << note;
            alreadyOn=false;
            for (it=onNotes.begin(); it!=onNotes.end(); ++it) {
                if((*it)->note==note) alreadyOn=true;
            }
            if(!alreadyOn) {
                //qDebug() << "not on ";
                noteEvent * ne = new noteEvent;
                ne->note=note;
                ne->eventId=rc1->getIEventOut();
                ne->channel=rc1->layout->getChannel()*-1;
                onNotes.push_back(ne);

                int seg=note+l->getScaleStartSeg();
                float freq=l->getFreq(seg);
                int pitch=l->getPitch(seg);
                int midinote=l->getMidinote(seg);
                rc1->sender->noteOn(ne->channel,ne->eventId,freq,midinote,pitch,note,1);
                //qDebug() << "note on " << freq << " evid " << ne->eventId << " midinote " << midinote << " ch " << ne->channel;
                //qDebug() << "pushed back ";
            }
        }
        for (it=onNotes.begin(); it!=onNotes.end(); ++it) {
            nolongerOn=true;
            noteEvent * ne = *it;
            //qDebug() << "checking noLongerOn note ";
            for(int i=0;i<stepNNotes;i++) {
                note=seq->getStepNote(currStep,i);
                //ne = ;
                if(ne->note==note) nolongerOn=false;
                //qDebug() << "whichis " << ne->note << "against " << i << " which is " << note << " gives " << nolongerOn;
            }
            if(nolongerOn) {
                //qDebug() << "no longer on";
                rc1->sender->noteOff(ne->channel,ne->eventId,0);
                //qDebug() << " note off " << ne->eventId << " ch " << ne->channel;
                delete(*it);
                onNotes.erase(it);
                it=onNotes.begin();
                //qDebug() << "erased ";
            }
        }
        nextStepAt+=stepDiff;
        //qDebug() << "next step " << currStep << " at " << nextStepAt << " diff " << stepDiff;
    }
}

void Sequencer::play()
{
    rec=false;
    playInt();
}

void Sequencer::record()
{
    rec=true;
    playInt();
}

void Sequencer::stop()
{
    run=false;
    rec=false;
    alloff();
    if(syncMaster) {
        rc1->sender->sync(0,0,bpm);
    }
}

void Sequencer::setBPM(int b)
{
    this->bpm=b;
    this->stepDiff=60/bpm/rc1->layout->getCurrentSeq()->getNbars()*1000;
}

void Sequencer::setBar(int b)
{
    int step=b*rc1->layout->getCurrentSeq()->getNbars()-1;
    if(step<0) step=rc1->layout->getCurrentSeq()->getNsteps();
    qDebug() << "setBar " << b << " step " << step;
}

void Sequencer::setNbars(int n)
{
    rc1->layout->getCurrentSeq()->nbars=n;
    setBPM(bpm);
}

void Sequencer::setStep(int n)
{
    currStep=n;
}

void Sequencer::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int scalenote, int v)
{
    //qDebug() << "sequencer noteOn ch " << chan << " scalenote " << scalenote;
    Sequence * seq;
    if(rec) {
        noteEvent * ne = new noteEvent;
        ne->note=scalenote;
        ne->eventId=voiceId;
        ne->channel=rc1->layout->getChannel()*-1;
        onRecNotes.push_back(ne);

        seq=rc1->layout->getCurrentSeq();
        //qDebug() << " step " << currStep << " note " << scalenote;
        seq->addNote(currStep,scalenote);

        ne = new noteEvent;
        ne->note=scalenote;
        ne->eventId=rc1->getIEventOut();
        ne->channel=rc1->layout->getChannel()*-1;
        onNotes.push_back(ne);
    } else if(!run && edit) {
        seq=rc1->layout->getCurrentSeq();
        if(isOn(scalenote)) {
            seq->removeScaleNote(currStep,scalenote);
        } else {
            seq->addNote(currStep,scalenote);
        }

    }
}

void Sequencer::noteOff(int chan, int voiceId)
{
    if(rec) {
        std::list<noteEvent *>::iterator it;
        for (it=onRecNotes.begin(); it!=onRecNotes.end(); ++it) {
            noteEvent * ne = *it;
            if(ne->eventId==voiceId) {
                delete(*it);
                onRecNotes.erase(it);
                it=onRecNotes.begin();
                //qDebug() << "erased ";
            }
        }
    }
}

bool Sequencer::isOn(int scalenote)
{
    Sequence * seq=rc1->layout->getCurrentSeq();
    int nnotes=seq->getStepNNotes(currStep);
    if(nnotes>0) {
        for (int i=0;i<nnotes;i++) {
            if(seq->getStepNote(currStep,i)==scalenote) return true;
        }
    }
    return false;
}

void Sequencer::alloff()
{
    std::list<noteEvent *>::iterator it;
    for (it=onNotes.begin(); it!=onNotes.end(); ++it) {
        noteEvent * ne;
        ne = *it;
        rc1->sender->noteOff(ne->channel,ne->eventId,0);
        delete(*it);
        onNotes.erase(it);
        it=onNotes.begin();
    }
}

void Sequencer::playInt()
{
    setNbars(rc1->layout->getCurrentSeq()->nbars);
    nextStepAt=0;
    nowInit=QDateTime::currentMSecsSinceEpoch();
    run=true;
}
