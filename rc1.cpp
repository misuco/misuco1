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
#include "rc1.h"
#include "event/eventhandlerrect.h"
#include "comm/senderdebug.h"
#include "comm/senderoscpuredata.h"
#include "comm/sendersupercollider.h"
#include "comm/sendermobilesynth.h"
#include "comm/senderoscxy.h"
#include "comm/sendermulti.h"
#include "paint/paintbgshapes.h"
#include "paint/paintbgbitmap.h"
#include "paint/pointpaintshape.h"
#include "paint/pointpaintsphere.h"
#include "paint/paintstat.h"
#include "paint/paintblocker.h"


RC1::RC1(QWidget *parent) :
    #ifdef NOGL
    QWidget(parent)
    #else
    QGLWidget(parent)
    #endif
{
    setAttribute(Qt::WA_AcceptTouchEvents,true);
    //qDebug() << "View() size:" << width() << " " << height();

    eventId = 1;
    nomouse = false;
    ttl=2000;

    blockerOn=true;
    blockerTimeout=30;
    blockerTimeLeft=blockerTimeout;
    blockerPainter=new PaintBlocker();
    downloadAd=false;

    storage=new Storage();
    layout=new LayoutModel();
    ehand=new EventHandlerRect();
    evstat=new EventStat();

    senderAddress=QHostAddress("255.255.255.255");
    senderPort=3150;
    sender=new SenderMulti(this);
    
    chan=0;

#ifdef RC1_LINUX
    storagePath=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    //  Android: /storage/emulated/0/Documents => not Persistent
    //  Linux: /home/c1/Documents => Persistent
    // iOS: not Persistent???? But document exchange folder
#elifdef RC1_IOS
    storagePath=QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    storagePath=QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    //  Android: /data/data/org.qtproject.example.rc1/files  => Persistent !!
    //  W8: C:/Users/c1/AppData/Local/rc1 => Persistent
    //  iOS: /var/mobile/Applications/ADDEBF69-B1C5-4E36-A8C2-789D717434C1/Documents => Persistent
    //  Linux: /home/c1/.local/share/rc1 => not Writable
#endif
    //qDebug() << "storage path: " << storagePath;
    progmemFile=storagePath+"/prog.xml";
    nPrePainters=2;
    prepainters=new IPaint*[nPrePainters];
    prepainters[0]=new PaintBgShapes();
    prepainters[1]=new PaintBgBitmap();

    nPointPainters=1;
    pointpainters=new IPointPaint*[nPointPainters];
    //pointpainters[0]=new PointPaintSphere();
    pointpainters[0]=new PointPaintShape();

    nPostPainters=0;
//    postpainters=new IPaint*[nPostPainters];
//    postpainters[0]=new PaintStat();

    painterOn=new bool[nPrePainters+nPointPainters+nPostPainters];
    painterOn[0]=true;
    painterOn[1]=false;
    painterOn[2]=false;

#ifdef RC1_PRO
    oscin = new QOscServer(3333,this);
    oscin->registerPathObject(this);
#endif
    
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

    QFile bgimg(storagePath+"/init.jpg");
    if(bgimg.exists()) {
        bgImageOri.load(bgimg.fileName());
        if(bgImageOri.width()==0) {
            bgImageOri.load(":/conf/misuco-logo.jpg");
        }
    } else {
        bgImageOri.load(":/conf/misuco-logo.jpg");
    }
    bgImage=bgImageOri.scaled(width(),height());

    adid=RC1_ADS_URL;
    QFile adidf(storagePath+"/adid.dat");
    if(adidf.exists()) {
        if(adidf.open(QIODevice::ReadOnly)) {
            QString adidfc=adidf.readAll();
            adid.append(adidfc);
        } else {
            adid.append("1");
        }
    } else {
        adid.append("1");
    }

    netxs = new QNetworkAccessManager(this);
    connect(netxs, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    layout->readProgmemXml(progmemFile);
    layout->resetLayout();
    layout->toggleEdit();
    layout->setActProgmem(0);
    layout->calcGeo(width(),height());
    layout->updateLayout();
    transmitSoundParam();
    
    /*
    QFontDatabase db;
    foreach (const QString &family, db.families()) {
         qDebug() << family;
    }
    */

    //setWindowState(Qt::WindowFullScreen);
}

RC1::~RC1()
{
    layout->writeProgmemXml(storagePath+"/prog.xml");
    //qDebug() << "progmem written";
}

void RC1::connectApp(QApplication * app) {
    connect(app, SIGNAL(applicationStateChanged(Qt::ApplicationState )),
            this, SLOT(appStateChange(Qt::ApplicationState)));

}

void RC1::paintEvent(QPaintEvent *)
{
    now=QDateTime::currentMSecsSinceEpoch();

    if(fpsT.elapsed()>1000) {
        fpsT.restart();
        fps=fcnt;
        fcnt=0;
        secTimer=true;
        //qDebug() << "fps: " << fps;
    }

    QPainter painter(this);
    //qDebug() << "painter window " << painter.window().width() << " " << painter.window().height() ;

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
    //qDebug() << "resize event " << width() << " " << height();
    int w=width();
    int h=height();
    if(height()>width()) {
        w=height();
        h=width();
    }
    layout->calcGeo(w,h);
    bgImage=bgImageOri.scaled(w,h);
    QString adurl;
    adurl.sprintf("http://ads.misuco.org/get/?w=%d&h=%d",w,h);
    if(!downloadAd) {
        downloadAd=true;
        netxs->get(QNetworkRequest(QUrl(adurl)));
    }
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
        
        //qDebug() << "fired test: " << tpx << " " << tpy << " " << tpt;
        
        tpx+=tpstep;
        tpn++;
        
        if(tpn>=nTests) {
            testMode=false;
        }
    }
}

bool RC1::event(QEvent *event)
{
    QList<QTouchEvent::TouchPoint> touchPoints;
    if(blockerOn) {
        if(adid!="") {
            if(event->type()==QEvent::TouchEnd ) {
                blockerOn=false;
                int closeArea=height()/5;
                touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
                foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
                    if(!(touchPoint.pos().x()>width()-closeArea && touchPoint.pos().y()<closeArea)) {
                        QDesktopServices::openUrl(QUrl(adid));
                    }
                }
            } else if(event->type()==QEvent::MouseButtonRelease)  {
                blockerOn=false;
                int closeArea=height()/5;
                const QMouseEvent * meve = static_cast<QMouseEvent *>(event);
                if(!(meve->pos().x()>width()-closeArea && meve->pos().y()<closeArea)) {
                    QDesktopServices::openUrl(QUrl(adid));
                }
            }
        }
    } else {
        if( event->type()==QEvent::TouchEnd ||
            event->type()==QEvent::TouchUpdate ||
            event->type()==QEvent::TouchBegin ) {

            long t=QDateTime::currentMSecsSinceEpoch();

            // nomouse=true;
            touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
            foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
                //qDebug() << " x:" << touchPoint.pos().x() << " y:" << touchPoint.pos().y() << " t: " << t ;
                evstat->incToucheventcount();
                Point * p = storage->getPoint(0);
                // ipad: x value 0 -> selects wrong segment -> fmax
                p->set(fmax(1,touchPoint.pos().x()),touchPoint.pos().y(),this->width(),this->height());
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

            state=Qt::TouchPointMoved;
            if(event->type()==QEvent::MouseMove) {
                state=Qt::TouchPointMoved;
            } else if(event->type()==QEvent::MouseButtonPress) {
                state=Qt::TouchPointPressed;
                eventId++;
            } else if(event->type()==QEvent::MouseButtonRelease) {
                state=Qt::TouchPointReleased;
            }
            //qDebug() << sEvent << ": x:" << meve->pos().x() << " y:" << meve->pos().y() << " t: " << t1.tv_sec << "." << t1.tv_usec;
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

void RC1::signalData(QString path, QVariant data, QHostAddress * host, quint16)
{
    //qDebug() << "got osc signal " << path << " data " << data << " source " << host->toString();
    int ignoreIndex=ignoreAddr.indexOf(*host);
    if(ignoreIndex==-1) {
        QList<QVariant> dl=data.toList();

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
                senderAddress=QHostAddress(dl.at(0).toString());
                senderPort=dl.at(1).toInt();
                sender->setDestination(senderAddress,senderPort);
            }
        }

        if(path=="/dim") {
            if(dl.size()<=32) {
                layout->setNrows(dl.size());
                int seg=0;
                for(int i=0;i<dl.size();i++) {
                    int nseg=dl.at(i).toInt();
                    if(nseg>32) {
                        nseg=32;
                    }
                    layout->setNseg(i,nseg);
                    layout->setRowheight(i,1);
                    layout->setSegwidthmax(i,nseg);
                    for(int j=0;j<nseg;j++) {
                        layout->setSegwidth(seg,1);
                        layout->setPressed(seg,0);
                        layout->setSegtype(seg,0);
                        seg++;
                    }
                }
                layout->setNsegs(seg);
                //qDebug() << "set nsegs " << seg;
                layout->setRowheightmax(dl.size());
                //layout->setScaleStartSeg(0);
                //layout->updateLayout();
                for(int doseg=0;doseg<=seg;) {
                    doseg=layout->generateScale(doseg);
                }
                layout->calcGeo();
            }
        }

        if(path=="/dimxy") {
            if(dl.size()==2) {
                int width=dl.at(0).toInt();
                int height=dl.at(1).toInt();
                //qDebug() << "dimxy " << width << " " << height;
                if(width<=32 && height<=32) {
                    layout->setNrows(height);
                    for(int i=0;i<height;i++) {
                        layout->setNseg(i,width);
                        layout->setRowheight(i,1);
                        layout->setSegwidthmax(i,width);
                    }
                    int nsegs=width*height;
                    //qDebug() << "total segs " << nsegs;
                    layout->setNsegs(nsegs);
                    layout->setRowheightmax(height);
                    for(int doseg=0;doseg<nsegs;) {
                        doseg=layout->generateScale(doseg);
                        //qDebug() << "do segs " << doseg;
                    }
                    layout->calcGeo();
                }
            }
        }

        if(path=="/freq") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    layout->setFreq(i,dl.at(i).toFloat());
                }
                //update();
            }
        }

        if(path=="/midinote") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    layout->setMidinote(i,dl.at(i).toInt());
                }
                //update();
            }
        }

        if(path=="/type") {
            //qDebug() << "osc get type size " << dl.size() << " nsegs " << layout->getNsegs();
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    int segtype=dl.at(i).toInt();
                    if(segtype==0 ||
                       (segtype==1 &&   // for segtype 1 both neighbors must be 0
                       i>0 && i+1<dl.size() &&
                       dl.at(i-1)==0 &&
                       dl.at(i+1)==0)) {
                        layout->setSegtype(i,segtype);
                    }
                }
                //qDebug() << "osc set type";
                //update();
            }
        }

        if(path=="/chan") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    int chan=dl.at(i).toInt();
                    if(chan>0) {
                        layout->setChan(i,chan);
                    }
                }
            }
        }

        if(path=="/ctlx") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    layout->setCtlx(i,dl.at(i).toInt());
                }
            }
        }

        if(path=="/ctly") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    layout->setCtly(i,dl.at(i).toInt());
                }
            }
        }

        if(path=="/width") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    layout->setSegwidth(i,dl.at(i).toInt());
                }
                layout->calcGeo();
            }
        }

        if(path=="/txt") {
            if(dl.size()<=layout->getNsegs()) {
                for(int i=0;i<dl.size();i++) {
                    layout->setSegtext(i,dl.at(i).toString());
                }
                layout->calcGeo();
            }
        }

        if(path=="/clrtxt") {
            if(dl.size()==1) {
                int seg=dl.at(0).toInt();
                if(seg<layout->getNsegs()) {
                    layout->setSegtext(seg,"");
                }
            }
        }
        if(path=="/cents") {
            if(dl.size()==2) {
                int seg=dl.at(0).toInt();
                if(seg>=0 && seg<=11) {
                    float cents=dl.at(1).toFloat();
                    if(cents>=0 && cents<=1200) {
                        layout->setMidi2fcent(seg,cents);
                        layout->updateLayout();
                    }
                }
            } else if(dl.size()==12) {
                for(int i=0;i<12;i++) {
                    float cents=dl.at(i).toFloat();
                    if(cents>=0 && cents<=1200) {
                        layout->setMidi2fcent(i,cents);
                    }
                }
                layout->updateLayout();
            }
        }

        if(path=="/reset") {
            layout->writeProgmemXml(storagePath+"/prog.xml");
            layout->resetLayout();
        }

        if(path=="/loadbg") {
            if(dl.size()==1) {
                QString loadurl=dl.at(0).toString();
                if(loadurl!="") {
                    netxs->get(QNetworkRequest(QUrl(loadurl)));
                }
            }
        }

        if(path=="/painter") {
            if(dl.size()==2) {
                int p=dl.at(0).toInt();
                if(p>=0 && p<=2) {
                    int val=dl.at(1).toInt();
                    if(val==0) {
                       painterOn[p]=false;
                    } else {
                        painterOn[p]=true;
                    }
                }
            }
        }


/*
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
        */

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
            //qDebug() << "got /tuio/2Dcur signal " << path << " data " << data << " source " << host->toString();
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

void RC1::resetStat()
{
    fps=0;
}

void RC1::setPPSmin() {
    ttl=500;
    int cornerrad=10;
    painterOn[0]=true;
    prepainters[0]->setParam(0, 120);        // sPenAct
    prepainters[0]->setParam(1, 120);        // lPenAct
    prepainters[0]->setParam(2, 120);        // sPenPsv
    prepainters[0]->setParam(3, 120);        // lPenPsv
    prepainters[0]->setParam(4, 120);        // sBrushAct
    prepainters[0]->setParam(5, 120);        // lBrushAct
    prepainters[0]->setParam(6, 0);          // sBrushPsv
    prepainters[0]->setParam(7, 0);          // lBrushPsv
    prepainters[0]->setParam(8, 2);          // colorMode
    prepainters[0]->setParam(9, 0);          // chue
    prepainters[0]->setParam(10, cornerrad); // cradx
    prepainters[0]->setParam(11, cornerrad); // crady
    prepainters[0]->setParam(12, 1);         // gradients
    prepainters[0]->setParam(13, 0);         // painttext
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
void RC1::appStateChange(Qt::ApplicationState state) {
    if(state==Qt::ApplicationActive) {
        delete(sender);
        sender=new SenderMulti(this);
        sender->setDestination(senderAddress,senderPort);
#ifdef RC1_PRO
        delete(oscin);
        oscin = new QOscServer(3333,this);
        oscin->registerPathObject(this);
#endif
    } else {
        layout->writeProgmemXml(progmemFile);
    }
}

void RC1::replyFinished(QNetworkReply * r)
{
    //qDebug() << "received " << r->url();
    if(r->error()==QNetworkReply::NoError) {
        QString pendingConfigFile;
        QByteArray data=r->readAll();
        if(r->url().toString()==RC1_SCALES_XML_URL) {
            pendingConfigFile="scales.xml";
        } else if(r->url().toString()==bgUrl) {
            pendingConfigFile="bg.jpg";
        } else {
            pendingConfigFile="init.jpg";
        }
        QFile out(storagePath+"/"+pendingConfigFile);
        if(out.open(QIODevice::WriteOnly)) {
            out.write(data);
            out.close();
        } else {
            //qDebug() << "cannot write " << storagePath << "/" << pendingConfigFile;
        }
        if(pendingConfigFile=="bg.jpg") {
            bgImageOri.load(storagePath+"/bg.jpg");
            bgImage=bgImageOri.scaled(width(),height());
        } else if(pendingConfigFile=="init.jpg") {
            QFile out(storagePath+"/adid.dat");
            if(out.open(QIODevice::WriteOnly)) {
                out.write(r->rawHeader("Adid"));
                out.close();
            }
        } else if(r->url().toString()==RC1_SCALES_XML_URL) {
            scaletab.readXml(storagePath+"/scales.xml");
        }
        r->deleteLater();
    } else {
        //qDebug() << "error reading background from www";
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

void RC1::transmitSoundParam()
{
    for(int i=0;i<NSOUNDPARAM;i++) {
         sender->cc(chan,0,i+102,layout->getSoundParam(i));
    }
}


