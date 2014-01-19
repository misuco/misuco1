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
#include <QtWidgets>
#include <QTimer>
#include <QtGlobal>
#include "rc1.h"
#include "event/eventhandlerrect.h"
#include "comm/senderdebug.h"
#include "comm/senderoscpuredata.h"
#include "comm/sendersupercollider.h"
#include "comm/sendermobilesynth.h"
#include "paint/paintbgshapes.h"
#include "paint/pointpaintshape.h"
#include "paint/pointpaintsphere.h"
#include "paint/paintstat.h"


RC1::RC1(QWidget *parent) :
    QGLWidget(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents,true);
    //qDebug() << "View() size:" << width() << " " << height();
    eventId = 1;
    nomouse = true;
    ttl=2000;

    storage=new Storage();
    layout=new LayoutModel();
    sender=new SenderMobileSynth(this);
//    sender=new SenderSuperCollider(this);
//    sender=new SenderOscPuredata(this);
//    sender=new SenderDebug();
    ehand=new EventHandlerRect();
    evstat=new EventStat();

    layout->calcGeo(width(),height());

    nPrePainters=1;
    prepainters=new IPaint*[nPrePainters];
    prepainters[0]=new PaintBgShapes();

    nPointPainters=9;
    pointpainters=new IPointPaint*[nPointPainters];
    pointpainters[0]=new PointPaintSphere();
    pointpainters[1]=new PointPaintShape();
    pointpainters[2]=new PointPaintShape();
    pointpainters[3]=new PointPaintShape();
    pointpainters[4]=new PointPaintShape();
    pointpainters[5]=new PointPaintShape();
    pointpainters[6]=new PointPaintShape();
    pointpainters[7]=new PointPaintShape();
    pointpainters[8]=new PointPaintShape();

    nPostPainters=1;
    postpainters=new IPaint*[nPostPainters];
    postpainters[0]=new PaintStat();

    painterOn=new bool[nPrePainters+nPointPainters+nPostPainters];
    painterOn[0]=true;
    painterOn[1]=true;
    painterOn[2]=false;
    painterOn[3]=false;
    painterOn[4]=false;
    painterOn[5]=false;
    painterOn[6]=false;
    painterOn[7]=false;
    painterOn[8]=false;
    painterOn[9]=false;
    painterOn[10]=true;

//    setConfigSlideRC();
//    setConfigPdjam2013();
//    setConfigTransistions();

//    layout->setRaga(0,0);
    
//    layout->setFactoryLayout(1);
    
//    layout->setXY(9, 2);
//    layout->setScale(50, 18, 1, true);
    
//    setPPS0();
    
    setProg(0);

    /* OSC Server disabled for demo version
    oscin = new QOscServer(3333,this);
    oscin->registerPathObject(this);
    */
    
    
    resetStat();
    this->startTimer(0);

    fpsT.start();
    fps=50;
    fcnt=0;
    secTimer=true;
        
    // init test
    tpn=0;
    nTests=32;
    tpx=0;
    testMode=false;

    // setWindowState(Qt::WindowFullScreen);
}

void RC1::paintEvent(QPaintEvent *event)
{
    now=QDateTime::currentMSecsSinceEpoch();

    if(fpsT.elapsed()>1000) {
        fpsT.restart();
        fps=fcnt;
        fcnt=0;
        secTimer=true;
//        qDebug() << "fps: " << fps;
    }

    QPainter painter(this);
    int k=0;
    for(int i=0;i<nPrePainters;i++) {
        if(painterOn[k]) {
            prepainters[i]->paint(this,&painter);
        }
        k++;
    }
    for(int i=storage->getLen()-1;i>0;i--) {
        Point * p = storage->getPoint(i);
        if(p!=NULL) {
            int lifetime=now-p->getT();
            if(lifetime >0 && lifetime< ttl) {
                if(p->getX()>=0 && p->getY()>=0) {
                    for(int j=0;j<nPointPainters;j++) {
                        if(painterOn[j+k]) {
                            pointpainters[j]->paint(p,this,&painter);
                        }
                    }
                }
            }
        }
    }
    k+=nPointPainters;
    for(int i=0;i<nPostPainters;i++) {
        if(painterOn[k]) {
            postpainters[i]->paint(this,&painter);
        }
        k++;
    }
    fcnt++;
}

void RC1::resizeEvent(QResizeEvent *)
{
    //qDebug() << "resize event";
    layout->calcGeo(width(),height());
    for(int i=0;i<storage->getLen();i++) {
        storage->getPoint(i)->setWidth(width());
        storage->getPoint(i)->setHeight(height());
    }
}

void RC1::timerEvent(QTimerEvent *)
{
    
    if(secTimer) {
        secTimer=false;
        repaint();
//        qDebug() << "fps:" << fps;
    } else {
        repaint(0,100,width(),height()-100);
    }

    if(testMode) {
        tpy=height()/2;
        tpstep=width()/nTests;
        
        Point * p = storage->getPoint(0);
        storage->next();
        
        tpt=QDateTime::currentMSecsSinceEpoch();
        
        p->set(tpx,tpy,width(),height());
        p->setT(tpt);
        p->setTTL(2000);
        switch (tpn%4) {
            case 0:
                p->setState(Qt::TouchPointPressed);
                break;
                
            case 3:
                p->setState(Qt::TouchPointReleased);
                break;
                
            default:
                p->setState(Qt::TouchPointPressed);
                break;
        }
        p->setGid(0);
        ehand->processPoint(p, this);
        
        qDebug() << "fired test: " << tpx << " " << tpy << " " << tpt;
        
        tpx+=tpstep;
        tpn++;
        
        if(tpn>=nTests) {
            testMode=false;
        }
        
    }
    
    /*
     * total chaos
     *
    float p=(float)qrand()/(float)RAND_MAX;
    float q=(float)qrand()/(float)RAND_MAX;
    float r=(float)qrand()/(float)RAND_MAX;
    r*=nPointPainters;
    q*=pointpainters[(int)r]->getParamCount();
    pointpainters[(int)r]->setParam(q,p);
     */
}

bool RC1::event(QEvent *event)
{
    QList<QTouchEvent::TouchPoint> touchPoints;
    if( event->type()==QEvent::TouchEnd ||
            event->type()==QEvent::TouchUpdate ||
            event->type()==QEvent::TouchBegin ) {

        long t=QDateTime::currentMSecsSinceEpoch();
        
        nomouse=true;
        touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
        foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
            //            qDebug() << sEvent << ": x:" << touchPoint.pos().x() << " y:" << touchPoint.pos().y() << " t: " << t1.tv_sec << "." << t1.tv_usec;
            evstat->incToucheventcount();
            Point * p = storage->getPoint(0);
            p->set(touchPoint.pos().x(),touchPoint.pos().y(),this->width(),this->height());
            p->setT(t);
            p->setGid(touchPoint.id());
            p->setState(touchPoint.state());
            storage->next();
            ehand->processPoint(p,this);
        }
        return true;
    } else if( !nomouse && (
                    event->type()==QEvent::MouseMove ||
                    event->type()==QEvent::MouseButtonPress ||
                    event->type()==QEvent::MouseButtonRelease )) {

        long t=QDateTime::currentMSecsSinceEpoch();

        const QMouseEvent * meve = static_cast<QMouseEvent *>(event);

        Qt::TouchPointState state;

        if(event->type()==QEvent::MouseMove) {
            state=Qt::TouchPointMoved;
        } else if(event->type()==QEvent::MouseButtonPress) {
            state=Qt::TouchPointPressed;
            eventId++;
        } else if(event->type()==QEvent::MouseButtonRelease) {
            state=Qt::TouchPointReleased;
        }
        //        qDebug() << sEvent << ": x:" << meve->pos().x() << " y:" << meve->pos().y() << " t: " << t1.tv_sec << "." << t1.tv_usec;
        evstat->incToucheventcount();
        Point * p = storage->getPoint(0);
        p->set(meve->pos().x(),meve->pos().y(),this->width(),this->height());
        p->setT(t);
        p->setGid(eventId);
        p->setState(state);
        storage->next();
        ehand->processPoint(p,this);
        return true;
    }
    return QWidget::event(event);
}

void RC1::signalData(QString path, QVariant data, QHostAddress * host, quint16 port)
{
    // qDebug() << "got osc signal " << path << " data " << data << " source " << host->toString();
    int ignoreIndex=ignoreAddr.indexOf(*host);
    if(ignoreIndex==-1) {
        QList<QVariant> dl=data.toList();

        if(path=="/fs") {
            if(dl.size()==1) {
                if(dl.at(0).toInt()>0) {
                    setWindowState(Qt::WindowFullScreen);
                } else {
                    setWindowState(Qt::WindowNoState);
                }
            }
        }

        if(path=="/ign") {
            if(dl.size()==1) {
                ignoreAddr.append(QHostAddress(dl.at(0).toString()));
            }
        }

        if(path=="/lst") {
            if(dl.size()==1) {
                int i=ignoreAddr.indexOf(QHostAddress(dl.at(0).toString()));
                if(i>=0) {
                    ignoreAddr.removeAt(i);
                }
            }
        }

        if(path=="/dst") {
            if(dl.size()==2) {
                sender->setDestination(QHostAddress(dl.at(0).toString()),dl.at(1).toInt());
            }
        }

        if(path=="/ttl") {
            if(dl.size()==1) {
                ttl=dl.at(0).toInt();
            }
        }

        if(path=="/pnt") {
            if(dl.size()==2) {
                painterOn[dl.at(0).toInt()]=dl.at(1).toBool();
            }
        }

        if(path=="/pnt") {
            if(dl.size()==2) {
                painterOn[dl.at(0).toInt()]=dl.at(1).toBool();
            }
        }

        if(path=="/lxy") {
            if(dl.size()==2) {
                layout->setXY(dl.at(0).toInt(),dl.at(1).toInt());
            }
        }

        if(path=="/lsc") {
            if(dl.size()==3) {
                layout->setScale(dl.at(0).toInt(),dl.at(1).toInt(),dl.at(2).toInt(),false);
            }
        }

        if(path=="/ltx") {
            if(dl.size()==2) {
                layout->getSegText(dl.at(0).toInt())->clear();
                layout->getSegText(dl.at(0).toInt())->append(dl.at(1).toString());
            }
        }

        if(path=="/lxc") {
            if(dl.size()==1) {
                layout->setAllCtlx(dl.at(0).toInt());
            }
        }

        if(path=="/lyc") {
            if(dl.size()==1) {
                layout->setAllCtly(dl.at(0).toInt());
            }
        }

        if(path=="/tuio/2Dcur") {
            qDebug() << "got /tuio/2Dcur signal " << path << " data " << data << " source " << host->toString();
            if(dl.size()>0) {

                // find source host in ip source adress table
                qint16 sourceId=tuioSources.indexOf(host->toIPv4Address());
                // if not yet exists, add it
                if(sourceId==-1) {
                    tuioSources.append(host->toIPv4Address());
                    sourceId=tuioSources.size()-1;
                }
                sourceId++; // source Id=0 is for local events

                if(dl.at(0)=="set") {
                    QDateTime ct = QDateTime::currentDateTime();
                    long t=ct.toMSecsSinceEpoch();

                    quint32 sid=dl.at(1).toInt()%65536 + sourceId*65536;
                    // session id:
                    // bit0-15: sid according message
                    // bit16-31: sourceId according to index in ip list
                    quint16 xpos=dl.at(2).toFloat()*layout->getWidth();
                    quint16 ypos=dl.at(3).toFloat()*layout->getHeight();
                    Qt::TouchPointState touchType=Qt::TouchPointMoved;
                    if(!tuioAlive.contains(sid)) {
                        tuioAlive.append(sid);
                        touchType=Qt::TouchPointPressed;
                        //                   qDebug() << "new point with id " << sid;
                    }
                    Point * p = storage->getPoint(0);
                    p->set(xpos,ypos,this->layout->getWidth(),this->layout->getHeight());
                    p->setT(t);
                    p->setGid(sid);
                    p->setState(touchType);
                    storage->next();
                    ehand->processPoint(p,this);

                }
                if(dl.at(0)=="alive") {
                    for(int i=0;i<tuioAlive.size();i++) {
                        if(tuioAlive.at(i)/65536==sourceId) {
                            if(!dl.contains(tuioAlive.at(i)-sourceId*65536)) {
                                QDateTime ct = QDateTime::currentDateTime();
                                long t=ct.toMSecsSinceEpoch();
                                //                            qDebug() << "removing point with id " << tuioAlive.at(i);
                                Point * p = storage->getPoint(0);
                                p->set(0,0,this->layout->getWidth(),this->layout->getHeight());
                                p->setT(t);
                                p->setGid(tuioAlive.at(i));
                                p->setState(Qt::TouchPointReleased);
                                storage->next();
                                ehand->processPoint(p,this);
                                tuioAlive.removeAt(i);
                                i--;
                            }
                        }
                    }
                }
            }
        }
    }
}

void RC1::resetStat()
{

    fps=0;
}
void RC1::setConfigTransistions() {
    layout->setXY(15,1);
    layout->setScale(62,8,3,true);
}

void RC1::setConfigPdjam2013()
{
    painterOn[6]=false;
    for(int i=0;i<32;i++) {
        layout->getSegText(i)->clear();
    }
    layout->getSegText(0)->append("he");
    layout->getSegText(1)->append("jo");
    layout->getSegText(2)->append("hej");
    layout->getSegText(3)->append("oo");
    layout->getSegText(4)->append("ajo");
    layout->getSegText(5)->append("hej");
    layout->getSegText(6)->append("awe");
    layout->getSegText(7)->append("io");

    layout->getSegText(8)->append("he");
    layout->getSegText(9)->append("jo");
    layout->getSegText(10)->append("hej");
    layout->getSegText(11)->append("oo");
    layout->getSegText(12)->append("hej");
    layout->getSegText(13)->append("hej");
    layout->getSegText(14)->append("awe");
    layout->getSegText(15)->append("ioe");
    layout->getSegText(16)->append("ajo");
    layout->getSegText(17)->append("hej");
    layout->getSegText(18)->append("awe");
    layout->getSegText(19)->append("io");
    layout->getSegText(20)->append("ajo");
    layout->getSegText(21)->append("hej");
    layout->getSegText(22)->append("awe");
    layout->getSegText(23)->append("ioe");
    layout->getSegText(24)->append("ajo");
    layout->getSegText(25)->append("hej");
    layout->getSegText(26)->append("awe");
    layout->getSegText(27)->append("io");
    layout->getSegText(28)->append("ajo");
    layout->getSegText(29)->append("hej");
    layout->getSegText(30)->append("awe");
    layout->getSegText(31)->append("io");

    layout->setSegH(0,8*255/8);
    layout->setSegH(1,8*255/8);
    layout->setSegH(2,6*255/8);
    layout->setSegH(3,4*255/8);
    layout->setSegH(4,6*255/8);
    layout->setSegH(5,4*255/8);
    layout->setSegH(6,6*255/8);
    layout->setSegH(7,6*255/8);

    layout->setSegH(8,8*255/8);
    layout->setSegH(9,8*255/8);
    layout->setSegH(10,6*255/8);
    layout->setSegH(11,4*255/8);
    layout->setSegH(12,8*255/8);
    layout->setSegH(13,2*255/8);
    layout->setSegH(14,1*255/8);
    layout->setSegH(15,1*255/8);

    layout->setSegH(16,4*255/8);
    layout->setSegH(17,4*255/8);
    layout->setSegH(18,6*255/8);
    layout->setSegH(19,6*255/8);
    layout->setSegH(20,2*255/8);
    layout->setSegH(21,1*255/8);
    layout->setSegH(22,1*255/8);
    layout->setSegH(23,1*255/8);
    layout->setSegH(24,4*255/8);
    layout->setSegH(25,2*255/8);
    layout->setSegH(26,2*255/8);
    layout->setSegH(27,2*255/8);
    layout->setSegH(28,1*255/8);
    layout->setSegH(29,0*255/8);
    layout->setSegH(30,1*255/8);
    layout->setSegH(31,1*255/8);
}

void RC1::setConfigSlideRC()
{
    painterOn[6]=false;
    layout->setXY(3,1);
    for(int i=0;i<3;i++) {
        layout->getSegText(i)->clear();
    }
    layout->getSegText(0)->append("Go");
    layout->getSegText(1)->append("Previous");
    layout->getSegText(2)->append("Next");
    layout->setSegH(0,0);
    layout->setSegH(1,100);
    layout->setSegH(2,200);
}
void RC1::setPPSmin(int p) {
    ttl=500;
    int cornerrad=10;
    
    painterOn[0]=true;
    painterOn[1]=true;
    painterOn[2]=false;
    painterOn[3]=false;
    painterOn[4]=false;
    painterOn[5]=false;
    painterOn[6]=false;
    painterOn[7]=false;
    painterOn[8]=false;
    painterOn[9]=false;
    painterOn[10]=true;
    
    prepainters[0]->setParam(0, 210);       // sPenAct
    prepainters[0]->setParam(1, 0);       // lPenAct
    prepainters[0]->setParam(2, 0);         // sPenPsv
    prepainters[0]->setParam(3, 0);         // lPenPsv
    prepainters[0]->setParam(4, 220);       // sBrushAct
    prepainters[0]->setParam(5, 140);       // lBrushAct
    prepainters[0]->setParam(6, 180);       // sBrushPsv
    prepainters[0]->setParam(7, 60);        // lBrushPsv
    prepainters[0]->setParam(8, 1);         // colorMode
    prepainters[0]->setParam(9, 0);         // chue
    prepainters[0]->setParam(10, cornerrad);// cradx
    prepainters[0]->setParam(11, cornerrad);// crady
    prepainters[0]->setParam(12, 1);        // gradients
    prepainters[0]->setParam(13, 0);        // painttext
}

void RC1::setPPS(int p)
{
    int cornerrad=10;
    switch (p) {
        case 1:
            ttl=2000;
            painterOn[0]=true;
            painterOn[1]=false;
            painterOn[2]=true;
            painterOn[3]=false;
            painterOn[4]=false;
            painterOn[5]=false;
            painterOn[6]=false;
            painterOn[7]=false;
            painterOn[8]=false;
            painterOn[9]=false;
//            painterOn[10]=false;
            
            for(int i=0;i<pointpainters[1]->getParamCount();i++) {
                pointpainters[1]->setParam(i,0);
            }
            // init x/y
            pointpainters[1]->setParam(1,1);
            pointpainters[1]->setParam(11,1);
            
            // radius 5 constant
            pointpainters[1]->setParam(18,5);
            pointpainters[1]->setParam(27,5);
            
            // grow width
            pointpainters[1]->setParam(23,100);
            // grow height
            pointpainters[1]->setParam(32,100);
            
            // color brush constant
            pointpainters[1]->setParam(72,0);
            pointpainters[1]->setParam(77,255);  // change hue with time
            pointpainters[1]->setParam(81,127);
            pointpainters[1]->setParam(90,140);
            pointpainters[1]->setParam(99,250);
            
            // fade brush out
            pointpainters[1]->setParam(104,-250);
            
            // color pen constant
            pointpainters[1]->setParam(36,255);
            pointpainters[1]->setParam(45,255);
            pointpainters[1]->setParam(54,255);
            pointpainters[1]->setParam(63,0);   // pen alpha constant 0
            
            // fade pen out
            // pointpainters[1]->setParam(68,-255);
            
            // shape circle
            pointpainters[1]->setParam(117,0);
            
            prepainters[0]->setParam(0, 160);   // sPenAct
            prepainters[0]->setParam(1, 0);   // lPenAct
            prepainters[0]->setParam(2, 160);   // sPenPsv
            prepainters[0]->setParam(3, 0);    // lPenPsv
            prepainters[0]->setParam(4, 160);   // sBrushAct
            prepainters[0]->setParam(5, 200);   // lBrushAct
            prepainters[0]->setParam(6, 160);   // sBrushPsv
            prepainters[0]->setParam(7, 80);    // lBrushPsv
            prepainters[0]->setParam(8, 1);     // colorMode
            prepainters[0]->setParam(9, 0);     // chue
            prepainters[0]->setParam(10, cornerrad);   // cradx
            prepainters[0]->setParam(11, cornerrad);   // crady
            prepainters[0]->setParam(12, 0);    // gradients
            prepainters[0]->setParam(13, 0);    // painttext
            
            break;
            
        case 0:
            ttl=1000;
            painterOn[0]=true;
            painterOn[1]=false;
            painterOn[2]=true;
            painterOn[3]=false;
            painterOn[4]=false;
            painterOn[5]=false;
            painterOn[6]=false;
            painterOn[7]=false;
            painterOn[8]=false;
            painterOn[9]=false;
//            painterOn[10]=false;
            
            for(int i=0;i<pointpainters[1]->getParamCount();i++) {
                pointpainters[1]->setParam(i,0);
            }
            // init x/y
            pointpainters[1]->setParam(1,1);
            pointpainters[1]->setParam(11,1);
            
            // radius 5 constant
            pointpainters[1]->setParam(18,5);
            pointpainters[1]->setParam(27,5);
            
            // grow width
            pointpainters[1]->setParam(23,100);
            // grow height
            pointpainters[1]->setParam(32,100);
            
            // color brush constant
            pointpainters[1]->setParam(72,0);
            pointpainters[1]->setParam(77,255);
            pointpainters[1]->setParam(81,127);
            pointpainters[1]->setParam(90,140);
            pointpainters[1]->setParam(99,0);     // brush alpha constant 0
            
            // fade brush out
            // pointpainters[1]->setParam(104,-250);
            
            // color pen constant
            pointpainters[1]->setParam(36,255);
            pointpainters[1]->setParam(45,255);
            pointpainters[1]->setParam(54,255);
            pointpainters[1]->setParam(63,255);
            
            // fade pen out
            pointpainters[1]->setParam(68,-255);
            
            // shape circle
            pointpainters[1]->setParam(117,0);
            
            prepainters[0]->setParam(0, 0);     // sPenAct
            prepainters[0]->setParam(1, 200);   // lPenAct
            prepainters[0]->setParam(2, 0);     // sPenPsv
            prepainters[0]->setParam(3, 0);     // lPenPsv
            prepainters[0]->setParam(4, 0);     // sBrushAct
            prepainters[0]->setParam(5, 200);   // lBrushAct
            prepainters[0]->setParam(6, 0);     // sBrushPsv
            prepainters[0]->setParam(7, 80);    // lBrushPsv
            prepainters[0]->setParam(8, 0);     // colorMode
            prepainters[0]->setParam(9, 0);     // chue
            prepainters[0]->setParam(10, cornerrad);   // cradx
            prepainters[0]->setParam(11, cornerrad);   // crady
            prepainters[0]->setParam(12, 0);    // gradients
            prepainters[0]->setParam(13, 0);    // painttext
            break;
            
        case 2:
            ttl=1000;
            painterOn[0]=true;
            painterOn[1]=true;
            painterOn[2]=true;
            painterOn[3]=false;
            painterOn[4]=false;
            painterOn[5]=true;
            painterOn[6]=false;
            painterOn[7]=false;
            painterOn[8]=false;
            painterOn[9]=false;
//            painterOn[10]=false;
            
            for(int j=0;j<5;j++) {
                for(int i=0;i<pointpainters[1]->getParamCount();i++) {
                    pointpainters[j]->setParam(i,0);
                }
                // init x/y
                pointpainters[j]->setParam(1,1);
                pointpainters[j]->setParam(11,1);
                
                // radius 5 constant
                pointpainters[j]->setParam(18,5);
                pointpainters[j]->setParam(27,5);
                
                // radius by time
                pointpainters[j]->setParam(23,80);
                pointpainters[j]->setParam(32,50);
                
                // color pen constant
                pointpainters[j]->setParam(36,255);
                pointpainters[j]->setParam(45,255);
                pointpainters[j]->setParam(54,255);
                pointpainters[j]->setParam(63,255);
                // fade pen out
                pointpainters[j]->setParam(68,-255);
                
                // color brush constant
                pointpainters[j]->setParam(72,0);
                pointpainters[j]->setParam(81,0);
                pointpainters[j]->setParam(90,0);
                pointpainters[j]->setParam(99,250);
                // fade brush out
                pointpainters[j]->setParam(104,-250);
                
                // rotation constan 45
                pointpainters[j]->setParam(108,45);
                // rotate once per lt
                pointpainters[j]->setParam(113,360);
                
                // shape constan 1 (rect)
                pointpainters[j]->setParam(117,1);
            }
            
            // shape circle
            pointpainters[4]->setParam(117,0);
            // brush hue by time
            pointpainters[4]->setParam(77,255);
            // brush saturation constant
            pointpainters[4]->setParam(81,150);
            // brush light constant
            pointpainters[4]->setParam(90,120);
            
            // shape circle
            pointpainters[1]->setParam(117,0);
            // grow width
            pointpainters[1]->setParam(23,50);
            // grow height
            pointpainters[1]->setParam(32,100);
            
            
            prepainters[0]->setParam(0, 160);   // sPenAct
            prepainters[0]->setParam(1, 200);   // lPenAct
            prepainters[0]->setParam(2, 160);   // sPenPsv
            prepainters[0]->setParam(3, 50);    // lPenPsv
            prepainters[0]->setParam(4, 160);   // sBrushAct
            prepainters[0]->setParam(5, 200);   // lBrushAct
            prepainters[0]->setParam(6, 160);   // sBrushPsv
            prepainters[0]->setParam(7, 80);    // lBrushPsv
            prepainters[0]->setParam(8, 1);     // colorMode
            prepainters[0]->setParam(9, 0);     // chue
            prepainters[0]->setParam(10, cornerrad);   // cradx
            prepainters[0]->setParam(11, cornerrad);   // crady
            prepainters[0]->setParam(12, 1);    // gradients
            prepainters[0]->setParam(13, 0);    // painttext
            
            break;
            
        case 3:
            ttl=500;
            painterOn[0]=true;
            painterOn[1]=true;
            painterOn[2]=true;
            painterOn[3]=true;
            painterOn[4]=true;
            painterOn[5]=true;
            painterOn[6]=true;
            painterOn[7]=true;
            painterOn[8]=true;
            painterOn[9]=false;
//            painterOn[10]=false;
            
            for(int i=0;i<9;i++) {
                for(int j=0;j<pointpainters[i]->getParamCount();j++) {
                    pointpainters[i]->setParam(j,0);
                }
                // init x/y
                pointpainters[i]->setParam(1,1);
                pointpainters[i]->setParam(11,1);
                
                // radius 5 constant
                pointpainters[i]->setParam(18,5);
                pointpainters[i]->setParam(27,5);
                
                // color brush constant
                pointpainters[i]->setParam(72,0);
                pointpainters[i]->setParam(80,1);       // hue by context
                pointpainters[i]->setParam(81,127);
                pointpainters[i]->setParam(90,140);
                pointpainters[i]->setParam(99,255);     // brush alpha constant 255
                
                // color pen constant
                pointpainters[i]->setParam(36,0);
                pointpainters[i]->setParam(45,0);
                pointpainters[i]->setParam(54,0);
                pointpainters[i]->setParam(63,0);
                
                pointpainters[i]->setParam(113,720);    // rotate by lifetime
                pointpainters[i]->setParam(117,2);      // shape triangle
            }
            pointpainters[0]->setParam(6, 1);   // x*wt
            pointpainters[1]->setParam(6, -1);   // x*wt
            pointpainters[2]->setParam(16, 1);   // y*wt
            pointpainters[3]->setParam(16, -1);   // y*ht
            
            pointpainters[4]->setParam(6, 1);   // x*wt
            pointpainters[4]->setParam(16, -1);   // y*ht
            pointpainters[5]->setParam(6, 1);   // x*wt
            pointpainters[5]->setParam(16, 1);   // y*ht
            pointpainters[6]->setParam(6, -1);   // x*wt
            pointpainters[6]->setParam(16, -1);   // x*wt
            pointpainters[7]->setParam(6, -1);   // y*wt
            pointpainters[7]->setParam(16, 1);   // y*ht

            
            prepainters[0]->setParam(0, 160);   // sPenAct
            prepainters[0]->setParam(1, 200);   // lPenAct
            prepainters[0]->setParam(2, 160);   // sPenPsv
            prepainters[0]->setParam(3, 50);    // lPenPsv
            prepainters[0]->setParam(4, 160);   // sBrushAct
            prepainters[0]->setParam(5, 200);   // lBrushAct
            prepainters[0]->setParam(6, 160);   // sBrushPsv
            prepainters[0]->setParam(7, 80);    // lBrushPsv
            prepainters[0]->setParam(8, 1);     // colorMode
            prepainters[0]->setParam(9, 0);     // chue
            prepainters[0]->setParam(10, cornerrad);   // cradx
            prepainters[0]->setParam(11, cornerrad);   // crady
            prepainters[0]->setParam(12, 1);    // gradients
            prepainters[0]->setParam(13, 0);    // painttext
            break;
            
        default:
            break;
    }
}

Storage *RC1::getStorage() const
{
    return storage;
}

LayoutModel *RC1::getLayout() const
{
    return layout;
}

long RC1::getNow()
{
    return now;
}

int RC1::getFps()
{
    return fps;
}

QTime * RC1::getFpsT()
{
    return &fpsT;
}

long RC1::getTtl() const
{
    return ttl;
}

void RC1::setTtl(long value)
{
    ttl = value;
}

ISender * RC1::getSender() const
{
return sender;
}

EventStat *RC1::getEvstat() const
{
return evstat;
}

void RC1::setProg(int p)
{
    setPPSmin(p);
    layout->setFactoryProg(p);
    sender->pc(0, p);
}

