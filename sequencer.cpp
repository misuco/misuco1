#include "sequencer.h"


Sequencer::Sequencer(RC1 * rc1, float bpm, Sequence *seq)
{
    this->rc1=rc1;
    this->seq=seq;
    this->bpm=bpm;
    this->currStep=0;
    this->run=false;
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
        stepNNotes=seq->getStepNNotes(currStep);
        qDebug() << "do step " << currStep << " stepNotes " << stepNNotes;
        l=rc1->getLayout();
        for(int i=0;i<stepNNotes;i++) {
            note=seq->getStepNote(currStep,i);
            qDebug() << "check note " << note;
            alreadyOn=false;
            for (it=onNotes.begin(); it!=onNotes.end(); ++it) {
                if((*it)->note==note) alreadyOn=true;
            }
            if(!alreadyOn) {
                qDebug() << "not on ";
                int seg=note+l->getScaleStartSeg();
                float freq=l->getFreq(seg);
                int pitch=l->getPitch(seg);
                int midinote=l->getMidinote(seg);
                int evid=rc1->getIEventOut();
                rc1->sender->noteOn(1,evid,freq,midinote,pitch,1);
                qDebug() << "note on " << freq << " evid " << evid << " midinote " << midinote;
                noteEvent * ne = new noteEvent;
                ne->note=note;
                ne->eventId=evid;
                onNotes.push_back(ne);
                qDebug() << "pushed back ";
            }
        }
        for (it=onNotes.begin(); it!=onNotes.end(); ++it) {
            nolongerOn=true;
            noteEvent * ne;
            qDebug() << "checking noLongerOn note ";
            for(int i=0;i<stepNNotes;i++) {
                note=seq->getStepNote(currStep,i);
                ne = *it;
                if(ne->note==note) nolongerOn=false;
                qDebug() << "whichis " << ne->note << "against " << i << " which is " << note << " gives " << nolongerOn;
            }
            if(nolongerOn) {
                qDebug() << "no longer on";
                rc1->sender->noteOff(1,ne->eventId,0);
                qDebug() << " note off " << ne->eventId;
                delete(*it);
                onNotes.erase(it);
                it=onNotes.begin();
                qDebug() << "erased ";
            }
        }
        nextStepAt+=stepDiff;
        currStep++;
        if(currStep>=seq->getNsteps()) currStep=0;
        qDebug() << "next step " << currStep << " at " << nextStepAt << " diff " << stepDiff;
    }
}

void Sequencer::play()
{
    nextStepAt=0;
    nowInit=QDateTime::currentMSecsSinceEpoch();
    run=true;
}

void Sequencer::stop()
{
    run=false;
}

void Sequencer::setBPM(int b)
{
    this->bpm=b;
    this->stepDiff=60/bpm/seq->getNbars()*1000;
}

void Sequencer::setNbars(int n)
{
    seq->nbars=n;
    setBPM(bpm);
}

void Sequencer::setStep(int n)
{
    currStep=n;
}
