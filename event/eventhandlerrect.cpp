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
#include <QInputDialog>
#include <QMessageBox>
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
    SenderMulti * snd=rc1->getSender();
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

    // check if iseg valid
    if(iseg<layout->getNsegs() ) {
        //qDebug() << "valid iseg " << iseg;

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
                yori[evptr]=-1;
                //qDebug() << "event " << evptr << "incPressed " << iseg;
            }

            if(layout->getSegtype(iseg)!=11) {
                layResize=false;
            }

            /*
             * calculation, quantisation and storage of segment relative x/y values
             *
             */
            float xrel=p->getX()-(xsum-layout->getSegwidthpx(iseg));

            // for non-transistion segments calculate with border
            if(layout->getSegtype(iseg)!=1) {
                xrel-=2*layout->getSegBorder();
                xrel=xrel/(float)(layout->getSegwidthpx(iseg)-4*layout->getSegBorder());
            } else {
                xrel=xrel/(float)(layout->getSegwidthpx(iseg));
            }

            if(xrel>1.0f) {
                xrel=1.0f;
            }
            if(xrel<0) {
                xrel=0;
            }
            layout->setXrel(iseg,xrel);

            float yrel=p->getY()-(ysum-layout->getRowheightpx(iy))-2*layout->getSegBorder();
            yrel=yrel/(float)(layout->getRowheightpx(iy)-4*layout->getSegBorder());
            if(yori[evptr]==-1) {
                yori[evptr]=yrel;
                yrelori[evptr]=1.0f-layout->getYrel(iseg);
            }
            //qDebug() << "yrel " << yrel << " yori " << yori[evptr] << " layout->yrel " << layout->getYrel(iseg);
            // apply relative fadder move for vertical fadders
            if((layout->getSegtype(iseg)==0 && layout->getCtly(iseg)==-3) || layout->getSegtype(iseg)==5) {
                yrel=(yrelori[evptr])+(yrel-yori[evptr]);
            }
            //qDebug() << "yrel " << yrel;
            if(yrel>1.0f) {
                yrel=1.0f;
            }
            if(yrel<0) {
                yrel=0;
            }
            yrel=1.0f-yrel;
            layout->setYrel(iseg,yrel);

            int xrelquant=0;    // quantized by steps
            int yrelquant=0;    // quantized by steps
            if(layout->getCtly(iseg)<0 || layout->getSegtype(iseg)>3) {
                xrelquant=xrel*(float)layout->getCtlx(iseg);
                yrelquant=yrel*(float)layout->getCtlx(iseg);
                //qDebug() << "xrelquant " << xrelquant << " yrelquant " << yrelquant;
            }
            layout->setXrelq(iseg,xrelquant);
            layout->setYrelq(iseg,yrelquant);

            /*
             * controller handling
             *
             */
            if(layout->getSegtype(iseg)==0 || layout->getSegtype(iseg)==1) {
                if(layout->getCtlx(iseg)>0 && layout->getCtly(iseg)>=0) {
                    if(p->getX()!=ccval1[evptr]) {
                        ccval1[evptr]=p->getX();
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
                if(layout->getCtly(iseg)==-3) {
                    layout->setMidi2fcent(layout->getCtlx(iseg),(layout->getYrel(iseg)-0.5f)*200.0f);
                    layout->updateLayout();
                    // prevent re-trigger due to freq. difference
                    if(p->getState()==Qt::TouchPointMoved) {
                        freq[evptr]=layout->getFreq(iseg);
                        snd->pitch(layout->getChan(iseg), ieventout[evptr],freq[evptr], layout->getMidinote(iseg), layout->getPitch(iseg));
                    }
                }
                int midinote=layout->getMidinote(iseg);
                float f=layout->getFreq(iseg);

                //qDebug() << "f " << f;

                //p->setHue(30*(layout->getValueInt(iseg)%12));
                if(freq[evptr]!=f) {
                    if(transitionMode) {
                        if(freq[evptr]>0) {
                            snd->pitch(layout->getChan(iseg), ieventout[evptr],f,midinote, layout->getPitch(iseg));
                        } else {
                            ieventout[evptr]=ieventoutnext;
                            ieventoutnext++;
                            snd->noteOn(layout->getChan(iseg), ieventout[evptr], f, midinote, layout->getPitch(iseg), veldef);
                        }
                    } else {
                        if(freq[evptr]>0 && layout->getSegtype(isegb[evptr])==1) {
                            snd->pitch(layout->getChan(iseg), ieventout[evptr],f,midinote, layout->getPitch(iseg));
                        } else {
                            if(freq[evptr]>0) {
                                snd->noteOff(chan[evptr], ieventout[evptr], mnote[evptr]);
                            }
                            ieventout[evptr]=ieventoutnext;
                            ieventoutnext++;
                            snd->noteOn(layout->getChan(iseg), ieventout[evptr], f, midinote, layout->getPitch(iseg), veldef);
                        }
                    }
                    freq[evptr]=f;
                    chan[evptr]=layout->getChan(iseg);
                    mnote[evptr]=midinote;
                }
            } else if(layout->getSegtype(iseg)==1) {
                // for transition segments calculate the frequency value between neighbour segments

                // 1. make sure, to be in range to have two neighbours
                if(iseg>0 && iseg<layout->getNsegs()-1) {
                    // 2. make sure, two neighbours are notes
                    if(layout->getSegtype(iseg+1)==0 && layout->getSegtype(iseg-1)==0) {
                        // 3. calculate frequency

                        // 3b. calculate frequency difference
                        float fdiff=layout->getFreq(iseg+1)-layout->getFreq(iseg-1);
                        //float mndiff=layout->getValueInt(iseg+1)%12-layout->getValueInt(iseg-1)%12;
                        // 3c. calculate relative frequency
                        float frel=fdiff*xrel;
                        frel+=layout->getFreq(iseg-1);
                        //float hue=(float)(layout->getValueInt(iseg-1)%12)+(mndiff*(float)xrel);
                        //hue*=30;
                        //layout->setSegH(iseg, hue);    // store value for painter
                        //p->setHue(hue);

                        float pitchdiff=4096*(layout->getMidinote(iseg+1)-layout->getMidinote(iseg-1));
                        pitchdiff+=layout->getPitch(iseg+1);
                        pitchdiff-=layout->getPitch(iseg-1);
                        pitchdiff*=xrel;
                        float pitchednote=layout->getMidinote(iseg-1)*4096+layout->getPitch(iseg-1)+pitchdiff;
                        int midinote=round(pitchednote/4096);
                        int pitch=(pitchednote-midinote*4096)/2;

                        if(freq[evptr]>0) {
                            snd->pitch(layout->getChan(iseg), ieventout[evptr],frel,midinote,pitch);
                        } else {
                            ieventout[evptr]=ieventoutnext;
                            ieventoutnext++;
                            snd->noteOn(layout->getChan(iseg), ieventout[evptr],frel,midinote,pitch,veldef);
                        }
                        freq[evptr]=frel;
                    }
                }
            } else {
                // turn off note if moved out of note into functional field
                bool movedin=false;
                if(freq[evptr]>0) {
                    snd->noteOff(chan[evptr], ieventout[evptr], mnote[evptr]);
                    freq[evptr]=-1;
                    layout->decPressed(isegb[evptr]);
                    //qDebug() << "event " << evptr << " decpressed " << isegb[evptr] << " if moved from note- into control-field " << iseg;
                    movedin=true;
                }
                if(layout->getSegtype(iseg)==4) {
                    // x-slider
                    if(layout->getCtly(iseg)==-1) {
                        layout->setBasenote(xrelquant);
                        layout->updateLayout();
                    } else if(layout->getCtly(iseg)==-2) {
                        layout->setActProgmem(xrelquant+layout->getMidinote(iseg));
                        snd->pc(layout->getChannel(),layout->getSound());
                        rc1->transmitSoundParam();
                        //layout->updateLayout();
                    } else if(layout->getCtly(iseg)==-3) {
                        layout->setSound(xrelquant+layout->getMidinote(iseg));
                        snd->pc(layout->getChannel(),xrelquant+layout->getMidinote(iseg));
                        rc1->transmitSoundParam();
                        layout->updateLayout();
                    } else if(layout->getCtly(iseg)==-4) {
                        layout->setTopoct(xrelquant);
                        layout->updateLayout();
                    } else if(layout->getCtly(iseg)==-5) {
                        layout->setBaseoct(xrelquant);
                        layout->updateLayout();
                    } else if(layout->getCtly(iseg)==-6) {
                        if(xrelquant<5) {
                            snd->reset1(xrelquant,layout->getDisplayAddress().toLocal8Bit().data(),layout->getDisplayPort());
                            layout->setSenderType(xrelquant);
                        }
                    } else if(layout->getCtly(iseg)==-7) {
                        layout->setAllChan(xrelquant);
                        layout->setChannel(xrelquant);
                    } else if(layout->getCtly(iseg)==-8) {
                        snd->repeatOff=xrelquant;
                        layout->setErrorCorr(xrelquant);
                    } else if(layout->getCtly(iseg)==-9) {
                        layout->setRowsGen(xrelquant);
    //                } else {
    //                    snd->cc(0, 0, layout->getCtly(iseg)+100, xrelquant);
    //                    layout->setSoundParam(layout->getCtly(iseg),xrelquant);
                    }
                } else if(layout->getSegtype(iseg)==5) {
                    // y-slider
                    snd->cc(0, 0,layout->getCtly(iseg),yrelquant);
                    if(layout->getCtly(iseg)>=102) {
                        layout->setSoundParam(layout->getCtly(iseg)-102,yrelquant);
                    } else if(layout->getCtly(iseg)==-1) {
                        layout->initMidi2f(layout->getYrelq(iseg));
                        layout->updateLayout();
                    }
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
            yori[evptr]=-1;
            if(layout->getSegtype(iseg)<2) {
                layout->decPressed(isegb[evptr]);
            }

            /*
             * event handling for note fields with special function
             *
             *
             */
            if(layout->getSegtype(iseg)==0) {
                if(layout->getCtly(iseg)==-1) {
                    layout->setBasenote(layout->getCtlx(iseg));
                    layout->updateLayout();
                }
                if(layout->getCtly(iseg)==-2) {
                    layout->setBscale(layout->getCtlx(iseg),!layout->getBscale(layout->getCtlx(iseg)));
                    layout->updateLayout();
                }
            }

            /*
             * button action shall be on release
             *
             */
            if(layout->getSegtype(iseg)==2) {
                // push button
                if(layout->getCtly(iseg)==-1) {
                    // basenote button
                    layout->setBasenote(layout->getCtlx(iseg));
                    layout->updateLayout();
                } else if(layout->getCtly(iseg)==-2) {
                    // memory button, not in use
                    layout->setActProgmem(layout->getCtlx(iseg));
                    rc1->transmitSoundParam();
                } else if(layout->getCtly(iseg)==-3) {
                    // edit button
                    layout->toggleEdit();
                } else if(layout->getCtly(iseg)==-4) {
                    // layout switch button
                    layout->resetLayout(layout->getCtlx(iseg));
                    layout->updateLayout();
                } else if(layout->getCtly(iseg)==-5) {
    #ifdef RC1_IOS
                    rc1->startDialog();
    #else
                    // edit sender destination adress button
                    bool ok;
                    QString text = QInputDialog::getText(rc1, "Destination address",
                                                         "IP:", QLineEdit::Normal,
                                                         snd->getAddress(), &ok);
                    if (ok && !text.isEmpty()) {
                        QHostAddress testadr(text);
                        if(!testadr.isNull()) {
                            snd->setDestination(text.toLocal8Bit().data(),snd->getPort());
                            layout->setSegtext(iseg,snd->getAddress());
                            layout->setDisplayAddress(snd->getAddress());
                        }
                    }
    #endif
                } else if(layout->getCtly(iseg)==-6) {
    #ifdef RC1_IOS
                    rc1->startDialog();
    #else

                    bool ok;
                    QString p;
                    p.sprintf("%d",snd->getPort());
                    QInputDialog * d=new QInputDialog();
                    QString text = d->getText(0, "Destination address",
                     "Port:", QLineEdit::Normal, p, &ok);
                    if (ok && !text.isEmpty()) {
                        int convport=text.toInt();
                        if(convport>0 && convport<65535) {
                            char * a=new char[strlen(snd->getAddress())];
                            strcpy(a,snd->getAddress());
                            snd->setDestination(a,convport);
                            p.sprintf("%d",snd->getPort());
                            layout->setSegtext(iseg,p);
                            layout->setDisplayPort(snd->getPort());
                        }
                    }
                    d->close();
                    d->hide();
                    delete(d);
    #endif
                } else if(layout->getCtly(iseg)==-7) {
                    if(iseg<layout->nsegs_max-1) {
                        int newval=layout->getMidinote(iseg+1)-10;
                        if(newval<0) {
                            layout->setMidinote(iseg+1,layout->getCtlx(iseg));
                        } else {
                            layout->setMidinote(iseg+1,newval);
                        }
                        layout->setXrelq(iseg+1,-1);
                    }
                } else if(layout->getCtly(iseg)==-8) {
                    if(iseg>0) {
                        int newval=layout->getMidinote(iseg-1)+10;
                        if(newval>layout->getCtlx(iseg)) {
                            layout->setMidinote(iseg-1,0);
                        } else {
                            layout->setMidinote(iseg-1,newval);
                        }
                        layout->setXrelq(iseg-1,-1);
                    }
                } else if(layout->getCtly(iseg)==-9) {
                    QMessageBox msgBox;
                    msgBox.setText("The document has been modified.");
                    msgBox.setInformativeText("Do you want to save your changes?");
                    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                    msgBox.setDefaultButton(QMessageBox::Cancel);
                    int ret = msgBox.exec();
                    if(ret==QMessageBox::Ok) {
                        rc1->writeProgmem();
                    }
                }
            } else if(layout->getSegtype(iseg)==3) {
                // toggle button
                 //if( p->getState() == Qt::TouchPointPressed || movedin) {
                     //qDebug() << " seg type 3 " << iseg << " pressed " << layout->getPressed(iseg);
                     if(layout->getPressed(iseg)>0) {
                         layout->setPressed(iseg,0);
                         if(layout->getCtly(iseg)==-1) {
                             layout->setBscale(layout->getCtlx(iseg),false);
                             //qDebug() << " bscale off " << iseg;
                         } else if(layout->getCtly(iseg)==-2) {
                             layout->setTransMode(false);
                         }
                         layout->updateLayout();
                     } else {
                         layout->setPressed(iseg,1);
                         if(layout->getCtly(iseg)==-1) {
                             layout->setBscale(layout->getCtlx(iseg),true);
                         } else if(layout->getCtly(iseg)==-2) {
                             layout->setTransMode(true);
                         }
                         layout->updateLayout();
                     }
                 //}
            }

            if(layout->getSegtype(iseg)==0 || layout->getSegtype(iseg)==1 || layout->getSegtype(iseg)==3) {
                //qDebug() << "tp released " << evptr << " " <<  ieventout[evptr] << " " << mnote[evptr];
                snd->noteOff(chan[evptr],ieventout[evptr],mnote[evptr]);
                freq[evptr]=-1;
                isegb[evptr]=-1;
                mnote[evptr]=-1;
            }
            for(int i=evptr;i<evptr_stack_size;i++) {
                act[i]=act[i+1];
                ievent[i]=ievent[i+1];
                ieventout[i]=ieventout[i+1];
                ccval1[i]=ccval1[i+1];
                ccval2[i]=ccval2[i+1];
                freq[i]=freq[i+1];
                mnote[i]=mnote[i+1];
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
}

void EventHandlerRect::init()
{
    veldef = 1;
    layResize=false;
    layResizeDiff=0;
    layResizePnt=0;
    
    ieventoutnext=2000;
    
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
    yori=new float[ntp];
    yrelori=new float[ntp];
    freq=new float[ntp];
    chan=new int[ntp];
    mnote=new int[ntp];
    isegb=new int[ntp];
    
    evptr_stack=new int[ntp];
    evptr_stack_size=0;
    
    for(int i=0;i<ntp;i++) {
        ievent[i]=-1;
        ieventout[i]=0;
        act[i]=false;
        freq[i]=-1;
        chan[i]=-1;
        ccval1[i]=-1;
        ccval2[i]=-1;
        yori[i]=-1;
        yrelori[i]=0;
        isegb[i]=-1;
        evptr_stack[i]=-1;
    }
}
