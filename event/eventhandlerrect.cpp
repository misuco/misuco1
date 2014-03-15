/*
 
 Copyright (C) 2013 by Claudio Zopfi, Zurich, Suisse, z@x21.ch
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */
#include <QDebug>
#include "eventhandlerrect.h"
#include "../comm/senderdebug.h"
#include "../comm/senderoscpuredata.h"

EventHandlerRect::EventHandlerRect()
{
    init();
}

//void EventHandlerRect::processPoint(int p->getGid(), Qt::TouchPointState touchPointState, quint16 x1, quint16 y1)
void EventHandlerRect::processPoint(Point * p, RC1 *rc1)
{
    ISender * snd=rc1->getSender();
    LayoutModel * layout=rc1->getLayout();
    
    // 1. figure out, at which index (evptr) the data for this touch point is stored
    qint16 evptr=-1; //p->getGid()%ntp;
    
    for(int i=0;i<evptr_stack_size;i++) {
        if(evptr_stack[i]==p->getGid()) {
            evptr=i;
            i=evptr_stack_size;
        }
    }
    if(evptr==-1) {
        evptr_stack[evptr_stack_size]=p->getGid();
        evptr=evptr_stack_size;
        evptr_stack_size++;
    }
    
    // qDebug() << "processPoint ptr " << evptr << " id " << p->getGid();
    if(p->getState()==Qt::TouchPointPressed) {
        if(act[evptr]!=true) {  // if needed, else ieventsub will be set to 0 by accitent -> hanging note
            act[evptr]=true;
            ievent[evptr]=p->getGid();
            ieventout[evptr]=ieventoutnext;
            ieventoutnext++;
        }
    }
    if(p->getState() == Qt::TouchPointPressed) {
        rc1->getEvstat()->incTouchbegincount();
    }
    if(p->getState() == Qt::TouchPointMoved) {
        rc1->getEvstat()->incTouchmovecount();
    }
    if(p->getState() == Qt::TouchPointReleased) {
        rc1->getEvstat()->incTouchendcount();
    }
    
    // 2.b. translate to MisuEvent index
    int iy=0;
    int ix=0;
    int iseg=0;
    int ysum=0;
    int xsum=0;
    while(p->getY()>ysum && iy<layout->getNrows()) {
        ysum+=layout->getRowheightpx(iy);
        iseg+=layout->getNseg(iy);
        iy++;
        //            qDebug() << "loop1 iy:" << iy << " iseg:" << iseg << " ysum: " << ysum;
    }
    if(iy>0) {
        iy--;
        iseg-=layout->getNseg(iy);
        while(p->getX()>xsum && ix<layout->getNseg(iy)) {
            xsum+=layout->getSegwidthpx(iseg);
            iseg++; ix++;
            //                qDebug() << "loop2 iseg:" << iseg << " xsum: " << xsum;
        }
        if(iseg>0) {
            iseg--;
            ix--;
        }
    }
    
    //qDebug() << "iseg " << iseg;
    
    if( p->getState() == Qt::TouchPointPressed ||
       p->getState() == Qt::TouchPointMoved ) {
        
        if(isegb[evptr]!=iseg) {
            if(isegb[evptr]!=-1) {
                layout->decPressed(isegb[evptr]);
                // increase statistics for transitions
                rc1->getEvstat()->incTransitioncount();
            }
            isegb[evptr]=iseg;
            if(layout->getSegtype(iseg)!=3) {
                layout->incPressed(isegb[evptr]);
            }
        }
        
        if(layout->getSegtype(iseg)==0 || layout->getSegtype(iseg)==1) {
            if(layout->getCtlx(iseg)>0) {
                if(p->getX()!=ccval1[evptr]) {
                    ccval1[evptr]=p->getX();
                    double xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                    xrel=xrel/(double)layout->getSegwidthpx(iseg);
                    if(useCCCVal==true) {
                        cccval1=xrel/cccvalAvg+(cccvalAvg-1)*cccval1/cccvalAvg;
                        snd->cc(layout->getChan(iseg), ieventout[evptr], layout->getCtlx(iseg), cccval1);
                    } else {
                        snd->cc(layout->getChan(iseg), ieventout[evptr], layout->getCtlx(iseg), xrel);
                    }
                }
            }
            if(layout->getCtly(iseg)>0) {
                if(p->getY()!=ccval2[evptr]) {
                    ccval2[evptr]=p->getY();
                    double yrel=p->getY()-(ysum-layout->getRowheightpx(iy));
                    yrel=1-(yrel/(double)layout->getRowheightpx(iy));
                    //                ysum+=layout->getRowheightpx(iy);
                    if(useCCCVal==true) {
                        cccval2=yrel/cccvalAvg+(cccvalAvg-1)*cccval2/cccvalAvg;
                        snd->cc(layout->getChan(iseg), ieventout[evptr], layout->getCtly(iseg), cccval2);
                    } else {
                        snd->cc(layout->getChan(iseg), ieventout[evptr], layout->getCtly(iseg), yrel);
                    }
                }
            }
        }
        
        if(layout->getSegtype(iseg)==0) {
            //            double v1=layout->getNote(iseg);
            double v1=layout->getValue(iseg);
            p->setHue(30*(layout->getValueInt(iseg)%12));
            if(note[evptr]!=v1) {
                if(transitionMode) {
                    if(note[evptr]>0) {
                        snd->pitch(layout->getChan(iseg),ieventout[evptr],v1);
                    } else {
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->note(layout->getChan(iseg),ieventout[evptr], v1, veldef);
                    }
                } else {
                    if(note[evptr]>0) {
                        snd->note(chan[evptr],ieventout[evptr],note[evptr],0);
                    }
                    ieventout[evptr]=ieventoutnext;
                    ieventoutnext++;
                    snd->note(layout->getChan(iseg),ieventout[evptr], v1, veldef);
                }
                note[evptr]=v1;
                chan[evptr]=layout->getChan(iseg);
            }
        } else if(layout->getSegtype(iseg)==1) {
            // for transition segments calculate the frequency value between neighbour segments
            
            // 1. make sure, to be in range to have two neighbours
            if(iseg>0 && iseg<layout->getNsegs()-1) {
                // 2. make sure, two neighbours are notes
                if(layout->getSegtype(iseg+1)==0 && layout->getSegtype(iseg-1)==0) {
                    // 3. calculate frequency
                    // 3a. calculate relative xposition in field
                    double xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                    xrel=xrel/(double)layout->getSegwidthpx(iseg);
                    // 3b. calculate frequency difference
                    double fdiff=layout->getValue(iseg+1)-layout->getValue(iseg-1);
                    float mndiff=layout->getValueInt(iseg+1)%12-layout->getValueInt(iseg-1)%12;
                    // 3c. calculate relative frequency
                    double frel=fdiff*xrel;
                    frel+=layout->getValue(iseg-1);
                    float hue=(float)(layout->getValueInt(iseg-1)%12)+(mndiff*(float)xrel);
                    hue*=30;
                    layout->setSegH(iseg, hue);    // store value for painter
                    p->setHue(hue);
                    
                    if(note[evptr]>0) {
                        snd->pitch(layout->getChan(iseg),ieventout[evptr],frel);
                    } else {
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->note(layout->getChan(iseg),ieventout[evptr], frel, veldef);
                    }
                    note[evptr]=frel;
                }
            }
        } else {
            // turn off note if moved out of note into functional field
            bool movedin=false;
            if(note[evptr]>0) {
                snd->note(chan[evptr],ieventout[evptr],note[evptr],0);
                note[evptr]=-1;
                layout->decPressed(isegb[evptr]);
                isegb[evptr]=-1;
                movedin=true;
            }
            if(layout->getSegtype(iseg)==2) {
                // button
                layout->setBasenote(layout->getValueInt(iseg));
                layout->updateLayout();
/*
                if(layout->getChan(iseg)==0) {
                    rc1->getLayout()->setFactoryLayout(layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==1) {
                    rc1->getLayout()->setBasenote(layout->getValueInt(iseg));
                    layout->updateLayout();
                } else if(layout->getChan(iseg)==2) {
                    rc1->getLayout()->setBasescale(layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==3) {
                    rc1->getLayout()->setNoct(layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==4) {
                    rc1->setProg(layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==5) {
                    QDesktopServices::openUrl(QUrl(*layout->getSegText(iseg)));
                }
                */

            } else if(layout->getSegtype(iseg)==3) {
                // toggle button
                 if( p->getState() == Qt::TouchPointPressed ) {
                     //qDebug() << " seg type 8 " << isegb[evptr];
                     if(layout->getPressed(isegb[evptr])>0) {
                         layout->decPressed(isegb[evptr]);
                         if(layout->getChan(isegb[evptr])==0) {
                             layout->setBscale(isegb[evptr],false);
                         } else if(layout->getChan(isegb[evptr])==1) {
                             layout->setTransMode(false);
                         }
                         layout->updateLayout();
                         //qDebug() << " bscale off " << isegb[evptr];
                     } else {
                         layout->incPressed(isegb[evptr]);
                         if(layout->getChan(isegb[evptr])==0) {
                             layout->setBscale(isegb[evptr],true);
                             //qDebug() << " bscale on " << isegb[evptr];

                             // play note if not yet selected
                             note[evptr]=layout->getValue(iseg);
                             snd->note(chan[evptr],ieventout[evptr],note[evptr],veldef);
                             //qDebug() << "snd->note(" << chan[evptr] << " " << ieventout[evptr] << " " << note[evptr];
                         } else if(layout->getChan(isegb[evptr])==1) {
                             layout->setTransMode(true);
                         }
                         layout->updateLayout();
                     }
                 }
            } else if(layout->getSegtype(iseg)==6) {
                // x-double-slider
                double xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                xrel=xrel/(double)layout->getSegwidthpx(iseg);
                int xrelquant=0;    // quantized by steps
                if(layout->getCtlx(iseg)>0) {
                    xrelquant=0.5+xrel*(double)layout->getCtlx(iseg);
                    xrel=(double)xrelquant/(double)layout->getCtlx(iseg);
                }
                if(xrel>layout->getValue(iseg+1)) {
                    layout->setValue(iseg+1,xrel);
                    layout->setValueInt(iseg+1,xrelquant);
                } else if(xrel<layout->getValue(iseg)) {
                    layout->setValue(iseg,xrel);
                    layout->setValueInt(iseg,xrelquant);
                } else if(xrel<layout->getValue(iseg+1) && xrel>layout->getValue(iseg)) {
                    if(layout->getValue(iseg+1)-xrel < xrel - layout->getValue(iseg)) {
                        layout->setValue(iseg+1,xrel);
                        layout->setValueInt(iseg+1,xrelquant);
                    } else {
                        layout->setValue(iseg,xrel);
                        layout->setValueInt(iseg,xrelquant);
                    }
                }
                layout->setBaseoct(layout->getValueInt(iseg));
                layout->setTopoct(layout->getValueInt(iseg+1));
                layout->updateLayout();
//                qDebug() << "x-double-slider " << layout->getValueInt(iseg) << " : " << layout->getValueInt(iseg+1);
            }
        }
    } else if( p->getState() == Qt::TouchPointReleased ) {
        p->setHue(-1);
        act[evptr]=false;
        if(layout->getSegtype(iseg)!=3) {
            layout->decPressed(isegb[evptr]);
        }
        if(layout->getSegtype(iseg)==0 || layout->getSegtype(iseg)==1 || layout->getSegtype(iseg)==3) {
            snd->note(chan[evptr],ieventout[evptr],note[evptr],0);
            note[evptr]=-1;
            isegb[evptr]=-1;
        }
        for(int i=evptr;i<evptr_stack_size;i++) {
            act[i]=act[i+1];
            ievent[i]=ievent[i+1];
            ieventout[i]=ieventout[i+1];
            ccval1[i]=ccval1[i+1];
            ccval2[i]=ccval2[i+1];
            note[i]=note[i+1];
            chan[i]=chan[i+1];
            isegb[i]=isegb[i+1];
            evptr_stack[i]=evptr_stack[i+1];
        }
        evptr_stack[evptr_stack_size]=-1;
        if(evptr_stack_size>0) {
            evptr_stack_size--;
        } else {
            qDebug() << "evptr_stack underflow.";
        }
    }
}

void EventHandlerRect::init()
{
    veldef = 1;
    
    ieventoutnext=1;
    
    useCCCVal=false;
    cccvalAvg=10;
    cccval1=0;
    cccval2=0;
    
    transitionMode=true;
    ntp=32;
    
    act=new bool[ntp];
    ievent=new int[ntp];
    ieventout=new int[ntp];
    ccval1=new int[ntp];
    ccval2=new int[ntp];
    note=new double[ntp];
    chan=new int[ntp];
    isegb=new int[ntp];
    
    evptr_stack=new int[ntp];
    evptr_stack_size=0;
    
    for(int i=0;i<ntp;i++) {
        ievent[i]=-1;
        ieventout[i]=0;
        act[i]=false;
        note[i]=-1;
        chan[i]=-1;
        ccval1[i]=-1;
        ccval2[i]=-1;
        isegb[i]=-1;
        evptr_stack[i]=-1;
    }
}
