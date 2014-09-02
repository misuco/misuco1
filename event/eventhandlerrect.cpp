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
    if(p->getX()<=0 || p->getY()<=0 ) return;

    ISender * snd=rc1->getSender();
    LayoutModel * layout=rc1->getLayout();
    
    // 1. figure out, at which index (evptr) the data for this touch point is stored
    qint16 evptr=-1; //p->getGid()%ntp;
    
    for(int i=0;i<evptr_stack_size;i++) {
        if(evptr_stack[i]==p->getGid()) {
            evptr=i;
            break;
        }
    }
    if(evptr==-1) {
        evptr_stack[evptr_stack_size]=p->getGid();
        evptr=evptr_stack_size;
        evptr_stack_size++;
    }
    
    //qDebug() << "processPoint ptr " << evptr << " id " << p->getGid();
    if(p->getState()==Qt::TouchPointPressed) {
        if(act[evptr]!=true) {  // if needed, else ieventsub will be set to 0 by accitent -> hanging note
            act[evptr]=true;
            ievent[evptr]=p->getGid();
            ieventout[evptr]=ieventoutnext;
            ieventoutnext++;
        }
    }
    
    // 2.b. translate to MisuEvent index iseg
    int iy=0;
    int ix=0;
    int iseg=0;
    int ysum=0;
    int xsum=0;
    while(p->getY()>ysum && iy<layout->getNrows()) {
        ysum+=layout->getRowheightpx(iy);
        iseg+=layout->getNseg(iy);
        iy++;
        //qDebug() << "loop1 iy:" << iy << " iseg:" << iseg << " ysum: " << ysum;
    }
    if(iy>0) {
        iy--;
        iseg-=layout->getNseg(iy);
        while(p->getX()>xsum && ix<layout->getNseg(iy)) {
            xsum+=layout->getSegwidthpx(iseg);
            iseg++; ix++;
            //qDebug() << "loop2 iseg:" << iseg << " xsum: " << xsum;
        }
        if(iseg>0) {
            iseg--;
            ix--;
        }
    }
    
    //qDebug() << "iseg " << iseg;
    
    if( p->getState() == Qt::TouchPointPressed ||
       p->getState() == Qt::TouchPointMoved ) {
        
        // have moved to another segment?
        // and are we in a note key segment?
        // isegb: id of previos selected segment; -1 means no previos
        if(isegb[evptr]!=iseg && layout->getSegtype(iseg)<2) {
            // turn off pressed if necessary (pressed >0 highlights segment)
            if(isegb[evptr]!=-1) {
                layout->decPressed(isegb[evptr]);
                //qDebug() << "event " << evptr << "decPressed " << isegb[evptr];
            }
            // turn on pressed for new segment
            layout->incPressed(iseg);
            //qDebug() << "event " << evptr << "incPressed " << iseg;
        }

        if(layout->getSegtype(iseg)!=11) {
            layResize=false;
        }

        /*
         * controller handling
         *
         */
        if(layout->getSegtype(iseg)==0 || layout->getSegtype(iseg)==1) {
            if(layout->getCtlx(iseg)>0) {
                if(p->getX()!=ccval1[evptr]) {
                    ccval1[evptr]=p->getX();
                    float xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                    xrel=xrel/(float)layout->getSegwidthpx(iseg);
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
                    //float yrel=p->getY()-(ysum-layout->getRowheightpx(iy));
                    float yrel=calcYrel(p->getY(),ysum,layout->getRowheightpx(iy));
                    yrel=1-(yrel/(float)layout->getRowheightpx(iy));
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
            int v1=layout->getValueInt(iseg);
            float v2=layout->getValue(iseg);
            p->setHue(30*(layout->getValueInt(iseg)%12));
            if(note[evptr]!=v2) {
                if(transitionMode) {
                    if(note[evptr]>0) {
                        snd->pitch(layout->getChan(iseg), ieventout[evptr],v2,v1, layout->getPitch(iseg));
                    } else {
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->noteOn(layout->getChan(iseg), ieventout[evptr], v2, v1, layout->getPitch(iseg), veldef);
                    }
                } else {
                    if(note[evptr]>0 && layout->getSegtype(isegb[evptr])==1) {
                        snd->pitch(layout->getChan(iseg), ieventout[evptr],v2,v1, layout->getPitch(iseg));
                    } else {
                        if(note[evptr]>0) {
                            snd->noteOff(chan[evptr], ieventout[evptr]);
                        }
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->noteOn(layout->getChan(iseg), ieventout[evptr], v2, v1, layout->getPitch(iseg), veldef);
                    }
                }
                note[evptr]=v2;
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
                    float xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                    xrel=xrel/(float)layout->getSegwidthpx(iseg);
                    // 3b. calculate frequency difference
                    float fdiff=layout->getValue(iseg+1)-layout->getValue(iseg-1);
                    //float mndiff=layout->getValueInt(iseg+1)%12-layout->getValueInt(iseg-1)%12;
                    // 3c. calculate relative frequency
                    float frel=fdiff*xrel;
                    frel+=layout->getValue(iseg-1);
                    //float hue=(float)(layout->getValueInt(iseg-1)%12)+(mndiff*(float)xrel);
                    //hue*=30;
                    //layout->setSegH(iseg, hue);    // store value for painter
                    //p->setHue(hue);

                    float pitchdiff=4096*(layout->getValueInt(iseg+1)-layout->getValueInt(iseg-1));
                    pitchdiff+=layout->getPitch(iseg+1);
                    pitchdiff-=layout->getPitch(iseg-1);
                    pitchdiff*=xrel;
                    float pitchednote=layout->getValueInt(iseg-1)*4096+layout->getPitch(iseg-1)+pitchdiff;
                    int midinote=round(pitchednote/4096);
                    int pitch=(pitchednote-midinote*4096)/2;

                    if(note[evptr]>0) {
                        snd->pitch(layout->getChan(iseg), ieventout[evptr],frel,midinote,pitch);
                    } else {
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->noteOn(layout->getChan(iseg), ieventout[evptr],frel,midinote,pitch,veldef);
                    }
                    note[evptr]=frel;
                }
            }
        } else {
            // turn off note if moved out of note into functional field
            bool movedin=false;
            if(note[evptr]>0) {
                snd->noteOff(chan[evptr], ieventout[evptr]);
                note[evptr]=-1;
                layout->decPressed(isegb[evptr]);
                //qDebug() << "event " << evptr << " decpressed " << isegb[evptr] << " if moved from note- into control-field " << iseg;
//                isegb[evptr]=iseg;
                movedin=true;
            }
            //isegb[evptr]=iseg;
            if(layout->getSegtype(iseg)==2) {
                // push button
                if(layout->getCtly(iseg)==-1) {
                    // basenote button
//                    layout->decPressed(isegb[evptr]);
//                    layout->incPressed(iseg);
                    layout->setBasenote(layout->getValueInt(iseg));
                    layout->updateLayout();
                } else if(layout->getCtly(iseg)==-2) {
                    rc1->setActProgmem(layout->getValueInt(iseg));
                } else if(layout->getCtly(iseg)==-3) {
                    //qDebug() << " segtype 2 chan 2 pressed " << layout->getPressed(iseg) ;
                    if(layout->getValueInt(iseg)==0) {
                        layout->setValueInt(iseg,1);
                        layout->setPressed(iseg,1);
                    } else {
                        layout->setRowheight(layout->getScalerow()-1,10);
                        layout->setRowheight(layout->getScalerow(),50);
                        layout->setValueInt(iseg,0);
                        layout->setPressed(iseg,0);
                    }
                    for(int i=0;i<layout->getNrows();i++) {
                        if(layout->getValueInt(iseg)==1) {
                            layout->setRowheight(i,10);
                        } else {
                            if(i<layout->getScalerow()-1) {
                                layout->setRowheight(i,0);
                            }
                        }
                    }
                    layout->calcGeo(layout->getWidth(),layout->getHeight());
                }
            } else if(layout->getSegtype(iseg)==3) {
                // toggle button
                 if( p->getState() == Qt::TouchPointPressed || movedin) {
                     //qDebug() << " seg type 3 " << iseg << " pressed " << layout->getPressed(iseg);
                     if(layout->getPressed(iseg)>0) {
                         layout->decPressed(iseg);
                         if(layout->getCtly(iseg)==-1) {
                             layout->setBscale(layout->getCtlx(iseg),false);
                             //qDebug() << " bscale off " << iseg;
                         } else if(layout->getCtly(iseg)==-2) {
                             layout->setTransMode(false);
                         }
                         layout->updateLayout();
                     } else {
                         layout->incPressed(iseg);
                         if(layout->getCtly(iseg)==-1) {
                             layout->setBscale(layout->getCtlx(iseg),true);
                             //qDebug() << " bscale on " << iseg;

                             // play note if not yet selected
                             /*
                             note[evptr]=layout->getValue(iseg);
                             snd->noteOn(ieventout[evptr],note[evptr],veldef);
                             */
                             //qDebug() << "snd->note(" << chan[evptr] << " " << ieventout[evptr] << " " << note[evptr];
                         } else if(layout->getCtly(iseg)==-2) {
                             layout->setTransMode(true);
                         }
                         layout->updateLayout();
                     }
                 }
            } else if(layout->getSegtype(iseg)==4) {
                // x-slider
                float xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                xrel=xrel/(float)layout->getSegwidthpx(iseg);
                int xrelquant=0;    // quantized by steps
                if(layout->getCtlx(iseg)>0) {
                    xrelquant=xrel*(float)layout->getCtlx(iseg);
                    //xrel=(float)xrelquant/(float)layout->getCtlx(iseg);
                }
                layout->setValue(iseg,xrel);
                layout->setValueInt(iseg,xrelquant);
                if(layout->getCtly(iseg)==-1) {
                    layout->setBasenote(layout->getValueInt(iseg));
                    layout->updateLayout();
                } else if(layout->getCtly(iseg)==-2) {
                    rc1->setActProgmem(layout->getValueInt(iseg));
                } else if(layout->getCtly(iseg)==-3) {
                    snd->pc(layout->getChan(iseg), xrelquant);
                } else if(layout->getCtly(iseg)==-4) {


                } else {
                    snd->cc(0, 0, layout->getCtly(iseg), layout->getValueInt(iseg));
                }
            } else if(layout->getSegtype(iseg)==5) {
                // y-slider
                float yrel=p->getY()-(ysum-layout->getRowheightpx(iy));
                yrel=yrel/(float)layout->getRowheightpx(iy);
                int yrelquant=0;    // quantized by steps
                if(layout->getCtlx(iseg)>0) {
                    yrelquant=yrel*(float)layout->getCtlx(iseg);
                    //xrel=(float)xrelquant/(float)layout->getCtlx(iseg);
                }
                layout->setValue(iseg,yrel);
                layout->setValueInt(iseg,yrelquant);
                snd->cc(0, 0,layout->getCtly(iseg),layout->getValueInt(iseg));
            } else if(layout->getSegtype(iseg)==6) {
                // x-twin-slider
                float xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                xrel=xrel/(float)layout->getSegwidthpx(iseg);
                int xrelquant=0;    // quantized by steps
                if(layout->getCtlx(iseg)>0) {
                    xrelquant=xrel*(float)layout->getCtlx(iseg);
                    xrel=(float)xrelquant/(float)layout->getCtlx(iseg);
                }
                //qDebug() << "x-float-slider " << xrel  << " : " << xrelquant;
                //qDebug() << "x-float-slider " << layout->getValueInt(iseg) << " : " << layout->getValueInt(iseg+1);

                if(xrelquant>layout->getValueInt(iseg+1)) {
                    layout->setValue(iseg+1,xrel);
                    layout->setValueInt(iseg+1,xrelquant);
                } else if(xrelquant<layout->getValueInt(iseg)) {
                    layout->setValue(iseg,xrel);
                    layout->setValueInt(iseg,xrelquant);
                } else if(xrelquant<=layout->getValueInt(iseg+1) && xrelquant>=layout->getValueInt(iseg)) {
                    int padDiff=layout->getValueInt(iseg+1)-layout->getValueInt(iseg)+1;
                    //qDebug() << "pad diff " << padDiff;
                    if( padDiff<=2 ) {
                        if(xrelquant==layout->getValueInt(iseg+1)) {
                            layout->setValue(iseg,xrel);
                            layout->setValueInt(iseg,xrelquant);
                        } else if(xrelquant==layout->getValueInt(iseg)) {
                            layout->setValue(iseg+1,xrel);
                            layout->setValueInt(iseg+1,xrelquant);
                        }
                    } else if(layout->getValueInt(iseg+1)-xrelquant < xrelquant - layout->getValueInt(iseg)) {
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
                //qDebug() << "x-twin-slider " << layout->getValueInt(iseg) << " : " << layout->getValueInt(iseg+1);
            } else if(layout->getSegtype(iseg)==12) {
                if( p->getState() == Qt::TouchPointPressed || movedin) {
                    /* ordered
                    QString link = "";
                    QString link_pre="http://scales.misuco.org/";
                    link.append(layout->getMidi2TextUrl(layout->getBasenote()));
                    link.append("-");
                    for(int i=0;i<11;i++) {
                        if(layout->getBscale(i)) {
                            int currnote=layout->getBasenote()+i+1;
                            if(currnote>11) {
                                link_pre.append(layout->getMidi2TextUrl(currnote%12));
                                link_pre.append("-");
                            } else {
                                link.append(layout->getMidi2TextUrl(currnote));
                                link.append("-");
                            }
                        }
                    }
                    link.chop(1);
                    link_pre.append(link);
                    link_pre.append("/#");
                    link_pre.append(layout->getMidi2TextUrl(layout->getBasenote()));
                    */
                    QString link ="http://scales.misuco.org/";
                    link.append(layout->getMidi2TextUrl(layout->getBasenote()));
                    link.append("-");
                    for(int i=0;i<11;i++) {
                        if(layout->getBscale(i)) {
                            int currnote=layout->getBasenote()+i+1;
                            link.append(layout->getMidi2TextUrl(currnote%12));
                            link.append("-");
                        }
                    }
                    link.chop(1);
                    QDesktopServices::openUrl(QUrl(link));
                }
            }
        }
        isegb[evptr]=iseg;
    } else if( p->getState() == Qt::TouchPointReleased ) {
        p->setHue(-1);
        act[evptr]=false;
        layResize=false;
        if(layout->getSegtype(iseg)<2) {
            layout->decPressed(isegb[evptr]);
        }
        if(layout->getSegtype(iseg)==0 || layout->getSegtype(iseg)==1 || layout->getSegtype(iseg)==3) {
            snd->noteOff(chan[evptr],ieventout[evptr]);
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
            //qDebug() << "evptr_stack underflow.";
        }
    }
}

void EventHandlerRect::init()
{
    veldef = 1;
    layResize=false;
    layResizeDiff=0;
    layResizePnt=0;
    
    ieventoutnext=1;
    
    useCCCVal=true;
    cccvalAvg=10;
    cccval1=0;
    cccval2=0;
    
    transitionMode=false;
    ntp=32;
    
    act=new bool[ntp];
    ievent=new int[ntp];
    ieventout=new int[ntp];
    ccval1=new int[ntp];
    ccval2=new int[ntp];
    note=new float[ntp];
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

float EventHandlerRect::calcYrel(int y, int ysum, int height)
{
    return y-(ysum-height);
}
