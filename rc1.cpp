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
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>

#include "platform.h"
#include "conf/layoutxml.h"
#include "rc1.h"
#include "event/eventhandlerrect.h"
#include "comm/senderdebug.h"
#include "comm/senderoscpuredata.h"
#include "comm/sendersupercollider.h"
#include "comm/sendermobilesynth.h"
#include "comm/senderoscxy.h"
#include "paint/paintbgshapes.h"
#include "paint/paintbgbitmap.h"
#include "paint/pointpaintshape.h"
#include "paint/pointpaintsphere.h"
#include "paint/paintstat.h"
#include "paint/paintblocker.h"


RC1::RC1(QWidget *parent) :
    QGLWidget(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents,true);
    //qDebug() << "View() size:" << width() << " " << height();
    eventId = 1;
    nomouse = false;
    ttl=2000;

    blockerOn=false;
    blockerTimeout=0;
    blockerTimeLeft=blockerTimeout;
    blockerPainter=new PaintBlocker();

    storage=new Storage();
    layout=new LayoutModel();
//    sender = new SenderOscXY(this);
//    sender=new SenderSuperCollider(this);
//    sender=new SenderDebug();
    ehand=new EventHandlerRect();
    evstat=new EventStat();

#ifdef RC1_IOS
    sender=new SenderMobileSynth(this);

    QString curDir=QDir::currentPath();
    int found=curDir.lastIndexOf("/");
    storagePath=curDir.left(found+1);
    storagePath+="Documents";
    QDir dir;
    dir.mkdir(storagePath);
    qDebug() << "new path " << storagePath;
    storagePath+="/";

    midimode=false;
#else
    sender=new SenderOscPuredata(this);
    midimode=true;
    storagePath="./";
#endif
    storagePath=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    layout->calcGeo(width(),height());

    nPrePainters=1;
    prepainters=new IPaint*[nPrePainters];
    prepainters[0]=new PaintBgShapes();
//    prepainters[0]=new PaintBgBitmap();

    nPointPainters=0;
//    pointpainters=new IPointPaint*[nPointPainters];
//    pointpainters[0]=new PointPaintSphere();

    nPostPainters=0;
//    postpainters=new IPaint*[nPostPainters];
//    postpainters[0]=new PaintStat();

    painterOn=new bool[nPrePainters+nPointPainters+nPostPainters];
    painterOn[0]=true;
//    painterOn[1]=false;
//    painterOn[2]=false;

    oscin = new QOscServer(3333,this);
    oscin->registerPathObject(this);
    
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

    bgImageOri.load(storagePath+"init.jpg");
    bgImage=bgImageOri.scaled(width(),height());

    netxs = new QNetworkAccessManager(this);
    connect(netxs, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    //netxs->get(QNetworkRequest(QUrl(RC1_INIT_XML_URL)));
    //netxs->get(QNetworkRequest(QUrl(RC1_SCALES_XML_URL)));

    layoutxml lxml;
    lxml.setLayoutModel(layout);
    lxml.readXml(":/conf/misuco.xml");
    layout->updateLayout();

    actProgmen=0;
    readProgmemXml(storagePath+"prog.xml");
    setActProgmem(0);



    //setWindowState(Qt::WindowFullScreen);
}

RC1::~RC1()
{
    writeProgmemXml(storagePath+"prog.xml");
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

    if(blockerOn) {
        blockerPainter->paint(this,&painter);
    } else {
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
    }
    fcnt++;
}

void RC1::resizeEvent(QResizeEvent *)
{
    //qDebug() << "resize event";
    layout->calcGeo(width(),height());

    /*
    bgImage=bgImageOri.scaled(width(),height());
    for(int i=0;i<storage->getLen();i++) {
        storage->getPoint(i)->setWidth(width());
        storage->getPoint(i)->setHeight(height());
    }
    */
}

void RC1::timerEvent(QTimerEvent *)
{
    if(secTimer) {
        secTimer=false;
        if(blockerOn) {
            blockerTimeLeft--;
            if(blockerTimeLeft<=0) {
                blockerOn=false;
            }
        }
    }
    update();

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
    if(blockerOn) {
        if( event->type()==QEvent::MouseButtonPress ||
            event->type()==QEvent::TouchBegin ) {
            blockerOn=false;
        }
    } else {
        QList<QTouchEvent::TouchPoint> touchPoints;
        if( event->type()==QEvent::TouchEnd ||
            event->type()==QEvent::TouchUpdate ||
            event->type()==QEvent::TouchBegin ) {

            long t=QDateTime::currentMSecsSinceEpoch();

            // nomouse=true;
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
    }
    return QWidget::event(event);
}

void RC1::signalData(QString path, QVariant data, QHostAddress * host, quint16 port)
{
    // qDebug() << "got osc signal " << path << " data " << data << " source " << host->toString();
    int ignoreIndex=ignoreAddr.indexOf(*host);
    if(ignoreIndex==-1) {
        QList<QVariant> dl=data.toList();

        if(path=="/loadbg") {
            if(dl.size()==1) {
                QString loadurl=dl.at(0).toString();
                if(loadurl!="") {
                    netxs->get(QNetworkRequest(QUrl(loadurl)));
                }
            }
        }

        if(path=="/fs") {
            if(dl.size()==1) {
                if(dl.at(0).toInt()>0) {
                    setWindowState(Qt::WindowFullScreen);
                } else {
                    setWindowState(Qt::WindowNoState);
                }
            }
        }

        if(path=="/ignore") {
            if(dl.size()==1) {
                ignoreAddr.append(QHostAddress(dl.at(0).toString()));
            }
        }

        if(path=="/listen") {
            if(dl.size()==1) {
                int i=ignoreAddr.indexOf(QHostAddress(dl.at(0).toString()));
                if(i>=0) {
                    ignoreAddr.removeAt(i);
                }
            }
        }

        if(path=="/dest") {
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

/*
        if(path=="/lxy") {
            if(dl.size()==2) {
                //layout->setXY(dl.at(0).toInt(),dl.at(1).toInt());
            }
        }

        if(path=="/lsc") {
            if(dl.size()==3) {
                //layout->setScale(dl.at(0).toInt(),dl.at(1).toInt(),dl.at(2).toInt(),false);
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
*/
/*
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
        */
    }
}
bool RC1::getMidimode() const
{
    return midimode;
}

void RC1::setMidimode(bool value)
{
    midimode = value;
}


void RC1::setActProgmem(int n)
{
    // store current setup
    progmem[actProgmen].basenote=layout->getBasenote();
    progmem[actProgmen].baseoct=layout->getBaseoct();
    progmem[actProgmen].topoct=layout->getTopoct();
    progmem[actProgmen].sound=layout->getValueInt(26); // the sound segment
    progmem[actProgmen].waveform=layout->getValueInt(26); // the wave segment
    progmem[actProgmen].envelope=layout->getValueInt(27); // the envelope segment
    progmem[actProgmen].mod=layout->getValueInt(28); // the mod segment

    for(int i=0;i<11;i++) {
        progmem[actProgmen].bscale[i]=layout->getBscale(i);
    }
    // restore new setup
    actProgmen=n;
    layout->setBasenote(progmem[n].basenote);
    layout->setTopoct(progmem[n].topoct);
    layout->setBaseoct(progmem[n].baseoct);
    layout->setValueInt(26,progmem[n].waveform);
    layout->setValueInt(27,progmem[n].envelope);
    layout->setValueInt(28,progmem[n].mod);
    sender->pc(0,progmem[n].sound);
    for(int i=0;i<11;i++) {
        layout->setBscale(i,progmem[n].bscale[i]);
    }
    layout->updateLayout();
}

void RC1::resetStat()
{

    fps=0;
}
/*
void RC1::setConfigTransistions() {
    layout->setXY(15,1);
    layout->setScale(62,8,3,true);
}
*/

void RC1::setPPSmin(int p) {
    ttl=500;
    int cornerrad=10;
    
    painterOn[0]=true;
//    painterOn[1]=true;
//    painterOn[2]=true;

    
    prepainters[0]->setParam(0, 120);       // sPenAct
    prepainters[0]->setParam(1, 120);       // lPenAct
    prepainters[0]->setParam(2, 120);         // sPenPsv
    prepainters[0]->setParam(3, 120);         // lPenPsv
    prepainters[0]->setParam(4, 120);       // sBrushAct
    prepainters[0]->setParam(5, 120);       // lBrushAct
    prepainters[0]->setParam(6, 0);       // sBrushPsv
    prepainters[0]->setParam(7, 0);        // lBrushPsv
    prepainters[0]->setParam(8, 2);         // colorMode
    prepainters[0]->setParam(9, 0);         // chue
    prepainters[0]->setParam(10, cornerrad);// cradx
    prepainters[0]->setParam(11, cornerrad);// crady
    prepainters[0]->setParam(12, 1);        // gradients
    prepainters[0]->setParam(13, 0);        // painttext
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

QImage *RC1::getBgImage()
{
    return &bgImage;
}

long RC1::getTtl() const
{
    return ttl;
}

void RC1::setTtl(long value)
{
    ttl = value;
}

void RC1::replyFinished(QNetworkReply * r)
{
//    qDebug() << "received " << r->url();
    if(r->error()==QNetworkReply::NoError) {
        r->url();
        QByteArray data=r->readAll();
        if(r->url().toString()==RC1_INIT_XML_URL) {
            pendingConfigFile="init.xml";
        }
        if(r->url().toString()==RC1_SCALES_XML_URL) {
            pendingConfigFile="scales.xml";
        }
        QFile out(storagePath+pendingConfigFile);
        if(out.open(QIODevice::WriteOnly)) {
            out.write(data);
            out.close();
        }
        if(pendingConfigFile=="init.jpg") {
            bgImageOri.load(storagePath+"init.jpg");
            bgImage=bgImageOri.scaled(width(),height());
        }
        if(r->url().toString()==RC1_INIT_XML_URL) {
            qDebug() << "setup init.xml";
            layoutxml lxml;
            lxml.setLayoutModel(layout);
            lxml.readXml(storagePath+"init.xml");
            layout->updateLayout();
        }
        if(r->url().toString()==RC1_SCALES_XML_URL) {
            scaletab.readXml(storagePath+"scales.xml");
        }
        r->deleteLater();
    } else {
        qDebug() << "error reading background from www";
    }
}

ISender * RC1::getSender() const
{
return sender;
}

EventStat *RC1::getEvstat() const
{
return evstat;
}
/*
void RC1::setProg(int p)
{
    setPPSmin(p);
    layout->setFactoryProg(p);
    sender->pc(0, p);
}

*/

void RC1::readProgmemXml(QString filename)
{
    QXmlStreamReader xmlr;
    QFile file(filename);
    if(!file.exists()) {
        for(int i=0;i<NPROGMEM;i++) {
            progmem[i].basenote=i%5;
            progmem[i].baseoct=3;
            progmem[i].topoct=5;
            progmem[i].sound=i;
            progmem[i].waveform=i%4+1;
            progmem[i].envelope=i/3+1;
            progmem[i].mod=i%3+1;
            for(int j=0;j<11;j++) {
                progmem[i].bscale[j]=false;
            }
            progmem[i].bscale[i]=true;
            progmem[i].bscale[i%4]=true;
        }
        writeProgmemXml(filename);
    }
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
        qDebug() << "cannot read file " << filename;
    }
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        if (xmlr.name() == "misucoprogmem" && xmlr.attributes().value("version") == "1.0") {

            int row=0;

            while (xmlr.readNextStartElement() && row<NPROGMEM) {
                if (xmlr.name() == "prog") {
                    progmem[row].basenote=xmlr.attributes().value("basenote").toString().toInt();
                    progmem[row].baseoct=xmlr.attributes().value("baseoct").toString().toInt();
                    progmem[row].topoct=xmlr.attributes().value("topoct").toString().toInt();
                    progmem[row].bscale[0]=(bool)xmlr.attributes().value("bscale0").toString().toInt();
                    progmem[row].bscale[1]=(bool)xmlr.attributes().value("bscale1").toString().toInt();
                    progmem[row].bscale[2]=(bool)xmlr.attributes().value("bscale2").toString().toInt();
                    progmem[row].bscale[3]=(bool)xmlr.attributes().value("bscale3").toString().toInt();
                    progmem[row].bscale[4]=(bool)xmlr.attributes().value("bscale4").toString().toInt();
                    progmem[row].bscale[5]=(bool)xmlr.attributes().value("bscale5").toString().toInt();
                    progmem[row].bscale[6]=(bool)xmlr.attributes().value("bscale6").toString().toInt();
                    progmem[row].bscale[7]=(bool)xmlr.attributes().value("bscale7").toString().toInt();
                    progmem[row].bscale[8]=(bool)xmlr.attributes().value("bscale8").toString().toInt();
                    progmem[row].bscale[9]=(bool)xmlr.attributes().value("bscale9").toString().toInt();
                    progmem[row].bscale[10]=(bool)xmlr.attributes().value("bscale10").toString().toInt();
                    progmem[row].sound=(int)xmlr.attributes().value("sound").toString().toInt();
                    progmem[row].waveform=(int)xmlr.attributes().value("waveform").toString().toInt();
                    progmem[row].envelope=(int)xmlr.attributes().value("envelope").toString().toInt();
                    progmem[row].mod=(int)xmlr.attributes().value("mod").toString().toInt();
                    xmlr.skipCurrentElement();
                    row++;
                } else {
                    xmlr.skipCurrentElement();
                }
            }
        } else {
            xmlr.raiseError(QObject::tr("The file is not a MISUCO version 1.0 file."));
        }
    }
    file.close();
}

void RC1::writeProgmemXml(QString filename)
{
    QXmlStreamWriter xml;
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    xml.setDevice(&file);
    QString att;
    QString attname;

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE misuco>");
    xml.writeStartElement("misucoprogmem");
    xml.writeAttribute("version", "1.0");

    for (int row = 0; row < NPROGMEM; row++) {
        xml.writeStartElement("prog");

        att.sprintf("%d",progmem[row].basenote);
        xml.writeAttribute("basenote",att);

        att.sprintf("%d",progmem[row].baseoct);
        xml.writeAttribute("baseoct",att);

        att.sprintf("%d",progmem[row].topoct);
        xml.writeAttribute("topoct",att);

        att.sprintf("%d",progmem[row].sound);
        xml.writeAttribute("sound",att);
        
        att.sprintf("%d",progmem[row].waveform);
        xml.writeAttribute("waveform",att);
        
        att.sprintf("%d",progmem[row].envelope);
        xml.writeAttribute("envelope",att);
        
        att.sprintf("%d",progmem[row].mod);
        xml.writeAttribute("mod",att);
        
        for(int j=0;j<11;j++) {
            att.sprintf("%d",(int)progmem[row].bscale[j]);
            attname.sprintf("bscale%d",j);
            xml.writeAttribute(attname,att);
        }

        xml.writeEndElement();
    }
    xml.writeEndDocument();

    file.close();

}
