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
    
    //qDebug() << "processPoint ptr " << evptr << " id " << p->getGid();
    if(p->getState()==Qt::TouchPointPressed) {
        if(act[evptr]!=true) {  // if needed, else ieventsub will be set to 0 by accitent -> hanging note
            act[evptr]=true;
            ievent[evptr]=p->getGid();
            ieventout[evptr]=ieventoutnext;
            ieventoutnext++;
        }
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
            isegb[evptr]=iseg;
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
                    //double yrel=p->getY()-(ysum-layout->getRowheightpx(iy));
                    double yrel=calcYrel(p->getY(),ysum,layout->getRowheightpx(iy));
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
            int v1=layout->getValueInt(iseg);
            double v2=layout->getValue(iseg);
            int pitch=0;
            p->setHue(30*(layout->getValueInt(iseg)%12));
            if(note[evptr]!=v2) {
                if(transitionMode) {
                    if(note[evptr]>0) {
                        snd->pitch(layout->getChan(iseg), ieventout[evptr],v2,v1, pitch);
                    } else {
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->noteOn(layout->getChan(iseg), ieventout[evptr], v2, v1, pitch, veldef);
                    }
                } else {
                    if(note[evptr]>0) {
                        snd->noteOff(chan[evptr], ieventout[evptr]);
                    }
                    ieventout[evptr]=ieventoutnext;
                    ieventoutnext++;
                    snd->noteOn(layout->getChan(iseg), ieventout[evptr], v2, v1, pitch, veldef);
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
                    
                    int pitch=0; // TODO: calculate properly
                    if(note[evptr]>0) {
                        snd->pitch(layout->getChan(iseg), ieventout[evptr],frel,frel,pitch);
                    } else {
                        ieventout[evptr]=ieventoutnext;
                        ieventoutnext++;
                        snd->noteOn(layout->getChan(iseg), ieventout[evptr],frel,frel,pitch,veldef);
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
                if(layout->getChan(iseg)==0) {
                    // basenote button
                    layout->decPressed(isegb[evptr]);
                    layout->incPressed(iseg);
                    layout->setBasenote(layout->getValueInt(iseg));
                    layout->updateLayout();
                } else if(layout->getChan(iseg)==1) {
                    rc1->setActProgmem(layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==2) {
                    //qDebug() << " segtype 2 chan 2 pressed " << layout->getPressed(iseg) ;
                    if(layout->getPressed(iseg)==0) {
                        layout->setRowheight(0,10);
                        layout->setRowheight(1,10);
                        layout->setRowheight(2,10);
                        layout->setRowheight(3,10);
                        layout->setRowheight(4,10);
                        layout->setRowheight(5,10);
                        layout->incPressed(iseg);
                        layout->incPressed(iseg);
                    } else {
                        layout->setRowheight(0,0);
                        layout->setRowheight(1,0);
                        layout->setRowheight(2,0);
                        layout->setRowheight(3,0);
                        layout->setRowheight(4,10);
                        layout->setRowheight(5,50);
                        layout->decPressed(iseg);
                    }
                    layout->calcGeo(layout->getWidth(),layout->getHeight());
                }
            } else if(layout->getSegtype(iseg)==3) {
                // toggle button
                 if( p->getState() == Qt::TouchPointPressed || movedin) {
                     //qDebug() << " seg type 3 " << iseg << " pressed " << layout->getPressed(iseg);
                     if(layout->getPressed(iseg)>0) {
                         layout->decPressed(iseg);
                         if(layout->getChan(iseg)==0) {
                             layout->setBscale(iseg,false);
                             //qDebug() << " bscale off " << iseg;
                         } else if(layout->getChan(iseg)==1) {
                             layout->setTransMode(false);
                         }
                         layout->updateLayout();
                     } else {
                         layout->incPressed(iseg);
                         if(layout->getChan(iseg)==0) {
                             layout->setBscale(iseg,true);
                             //qDebug() << " bscale on " << iseg;

                             // play note if not yet selected
                             /*
                             note[evptr]=layout->getValue(iseg);
                             snd->noteOn(ieventout[evptr],note[evptr],veldef);
                             */
                             //qDebug() << "snd->note(" << chan[evptr] << " " << ieventout[evptr] << " " << note[evptr];
                         } else if(layout->getChan(iseg)==1) {
                             layout->setTransMode(true);
                         }
                         layout->updateLayout();
                     }
                 }
            } else if(layout->getSegtype(iseg)==4) {
                // x-slider
                double xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                xrel=xrel/(double)layout->getSegwidthpx(iseg);
                int xrelquant=0;    // quantized by steps
                if(layout->getCtlx(iseg)>0) {
                    xrelquant=xrel*(double)layout->getCtlx(iseg);
                    //xrel=(double)xrelquant/(double)layout->getCtlx(iseg);
                }
                layout->setValueInt(iseg,xrelquant);
                if(layout->getChan(iseg)==0) {
                    layout->setBasenote(layout->getValueInt(iseg));
                    layout->updateLayout();
                } else if(layout->getChan(iseg)==1) {
                    rc1->setActProgmem(layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==2) {
                    snd->pc(layout->getChan(25), xrelquant);
                } else if(layout->getChan(iseg)==3) {
                    // waveform
                    snd->cc(0, 0, 100, layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==4) {
                    // envelope
                    snd->cc(0, 0,103,layout->getValueInt(iseg));
                } else if(layout->getChan(iseg)==5) {
                    // resonance
                    snd->cc(0, 0,104,layout->getValueInt(iseg));
                }
            } else if(layout->getSegtype(iseg)==6) {
                // x-double-slider
                double xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));
                xrel=xrel/(double)layout->getSegwidthpx(iseg);
                int xrelquant=0;    // quantized by steps
                if(layout->getCtlx(iseg)>0) {
                    xrelquant=xrel*(double)layout->getCtlx(iseg);
                    xrel=(double)xrelquant/(double)layout->getCtlx(iseg);
                }
                //qDebug() << "x-double-slider " << xrel  << " : " << xrelquant;
                //qDebug() << "x-double-slider " << layout->getValueInt(iseg) << " : " << layout->getValueInt(iseg+1);

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
                //qDebug() << "x-double-slider " << layout->getValueInt(iseg) << " : " << layout->getValueInt(iseg+1);
            } else if(layout->getSegtype(iseg)==10) {
                if( p->getState() == Qt::TouchPointPressed ) {
                    if(layout->getChan(iseg)==0) {
                        int newwaveform=layout->getValueInt(iseg)+1;
                        if(newwaveform>5) {
                            newwaveform=0;
                        }
                        layout->setValueInt(iseg,newwaveform);
                        switch(newwaveform) {
                            case 0:
                                layout->getSegText(iseg)->sprintf("SQR");
                                break;
                            case 1:
                                layout->getSegText(iseg)->sprintf("SAW");
                                break;
                            case 2:
                                layout->getSegText(iseg)->sprintf("SIN");
                                break;
                            case 3:
                                layout->getSegText(iseg)->sprintf("TRI");
                                break;
                            case 4:
                                layout->getSegText(iseg)->sprintf("NOI");
                                break;
                            case 5:
                                layout->getSegText(iseg)->sprintf("WTAB");
                                break;
                        }
                        snd->cc(0,0,200,newwaveform);
                    } else if(layout->getChan(iseg)==1) {
                        int newwaveform=layout->getValueInt(iseg)+1;
                        if(newwaveform>4) {
                            newwaveform=0;
                        }
                        layout->setValueInt(iseg,newwaveform);
                        switch(newwaveform) {
                            case 0:
                                layout->getSegText(iseg)->sprintf("LFOSQR");
                                break;
                            case 1:
                                layout->getSegText(iseg)->sprintf("LFOSAW");
                                break;
                            case 2:
                                layout->getSegText(iseg)->sprintf("LFOSIN");
                                break;
                            case 3:
                                layout->getSegText(iseg)->sprintf("LFOTRI");
                                break;
                            case 4:
                                layout->getSegText(iseg)->sprintf("LFONOI");
                                break;
                        }
                        snd->cc(0,0,201,newwaveform);
                    } else if(layout->getChan(iseg)==2) {
                        int newwaveform=layout->getValueInt(iseg)+1;
                        if(newwaveform>3) {
                            newwaveform=0;
                        }
                        layout->setValueInt(iseg,newwaveform);
                        switch(newwaveform) {
                            case 0:
                                layout->getSegText(iseg)->sprintf("NONE");
                                break;
                            case 1:
                                layout->getSegText(iseg)->sprintf("AMP");
                                break;
                            case 2:
                                layout->getSegText(iseg)->sprintf("FREQ");
                                break;
                            case 3:
                                layout->getSegText(iseg)->sprintf("FILT");
                                break;
                        }
                        snd->cc(0,0,202,newwaveform);

                    } else if(layout->getChan(iseg)==3) {
                        int newenv=layout->getValueInt(iseg)+1;
                        if(newenv>3) {
                            newenv=0;
                        }
                        layout->setValueInt(iseg,newenv);
                        switch(newenv) {
                        case 0:
                            layout->getSegText(iseg)->sprintf("-__");
                            break;
                        case 1:
                            layout->getSegText(iseg)->sprintf("--_");
                            break;
                        case 2:
                            layout->getSegText(iseg)->sprintf("---");
                            break;
                        case 3:
                            layout->getSegText(iseg)->sprintf("_-_");
                                break;
                        }
                        snd->cc(0,0,203,newenv);
                    } else if(layout->getChan(iseg)==4) {
                        int newenv=layout->getValueInt(iseg)+1;
                        if(newenv>3) {
                            newenv=0;
                        }
                        layout->setValueInt(iseg,newenv);
                        switch(newenv) {
                            case 0:
                                layout->getSegText(iseg)->sprintf("XMOD1");
                                break;
                            case 1:
                                layout->getSegText(iseg)->sprintf("XMOD2");
                                break;
                            case 2:
                                layout->getSegText(iseg)->sprintf("XMOD3");
                                break;
                            case 3:
                                layout->getSegText(iseg)->sprintf("XMOD4");
                                break;
                        }
                        //snd->cc(0,0,202,newenv);
                    } else if(layout->getChan(iseg)==5) {
                        int newenv=layout->getValueInt(iseg)+1;
                        if(newenv>3) {
                            newenv=0;
                        }
                        layout->setValueInt(iseg,newenv);
                        switch(newenv) {
                            case 0:
                                layout->getSegText(iseg)->sprintf("YMOD1");
                                break;
                            case 1:
                                layout->getSegText(iseg)->sprintf("YMOD2");
                                break;
                            case 2:
                                layout->getSegText(iseg)->sprintf("YMOD3");
                                break;
                            case 3:
                                layout->getSegText(iseg)->sprintf("YMOD4");
                                break;
                        }
                        //snd->cc(0,0,202,newenv);
                    }
                }
            } else if(layout->getSegtype(iseg)==12) {
                if( p->getState() == Qt::TouchPointPressed || movedin) {
                    QString link = "";
                    QString link_pre="http://scales.misuco.org/";
                    //digit.sprintf("%d",layout->getBasenote()+1);
                    link.append(layout->getMidi2TextUrl(layout->getBasenote()));
                    link.append("-");
                    for(int i=0;i<11;i++) {
                        if(layout->getBscale(i)) {
                            int currnote=layout->getBasenote()+i+1;
                            if(currnote>11) {
                                /*
                                if(link_pre.length()==0) {
                                    digit.sprintf("%d",currnote%12+1);
                                } else {
                                    digit.sprintf("-%d",currnote%12+1);
                                }
                                link_pre.append(digit);
                                */
                                link_pre.append(layout->getMidi2TextUrl(currnote%12));
                                link_pre.append("-");
                            } else {
                                /*
                                digit.sprintf("-%d",currnote+1);
                                link.append(digit);
                                */
                                link.append(layout->getMidi2TextUrl(currnote));
                                link.append("-");
                            }
                        }
                    }
                    link.chop(1);
                    link_pre.append(link);
                    link_pre.append("/#");
                    link_pre.append(layout->getMidi2TextUrl(layout->getBasenote()));
                    QDesktopServices::openUrl(QUrl(link_pre));
                }
            } else if(layout->getSegtype(iseg)==11) {
                if( p->getState() == Qt::TouchPointMoved ) {
                    if(!layResize) {
                        layResize=true;
                        layResizeDiff=p->getY();
                    } else {
                        int moved=p->getY()-layResizeDiff;
                        while(moved>0) {
                            int index=layResizePnt%iy;
                            layout->setRowheightpx(index,layout->getRowheightpx(index)+1);
                            layout->setRowheightpx(iy+1,layout->getRowheightpx(iy+1)-1);
                            moved--;
                            layResizePnt++;
                        }
                        while(moved<0) {
                            int index=layResizePnt%iy;
                            layout->setRowheightpx(index,layout->getRowheightpx(index)-1);
                            layout->setRowheightpx(iy+1,layout->getRowheightpx(iy+1)+1);
                            moved++;
                            layResizePnt--;
                        }
                        layResizeDiff=p->getY();


                        /*
                         *
                         *
                         * ----------------
                        if(moved>=iy) {
                            int heightnew=layout->getRowheightpx(0)+1;
                            int heightnewmax=layout->getHeight()/(iy+2);
                            //qDebug() << "heightnewmax " << heightnewmax;
                            if(heightnew<heightnewmax) {
                                for(int i=0;i<iy;i++) {
                                    layout->setRowheightpx(i,heightnew);
                                }
                            }
                            int scaelheightnew=layout->getHeight()-iy*heightnew-layout->getRowheightpx(iy);
                            layout->setRowheightpx(iy+1,scaelheightnew);
                            layResizeDiff=p->getY();
                        } else if(moved<=iy*-1) {
                            int heightnew=layout->getRowheightpx(0)-1;
                            if(heightnew>1) {
                                for(int i=0;i<iy;i++) {
                                    layout->setRowheightpx(i,heightnew);
                                }
                            }
                            int scaelheightnew=layout->getHeight()-iy*heightnew-layout->getRowheightpx(iy);
                            layout->setRowheightpx(iy+1,scaelheightnew);
                            layResizeDiff=p->getY();
                        }
                        */
                    }
                }
            }
            isegb[evptr]=iseg;
        }
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

double EventHandlerRect::calcYrel(int y, int ysum, int height)
{
    return y-(ysum-height);
}
