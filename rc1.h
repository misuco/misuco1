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
#ifndef VIEW_H
#define VIEW_H

#include <platform.h>

#ifdef NOGL
#include <QWidget>
#else
#include <QGLWidget>
#endif


#include <QPaintEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QTime>
#include <QNetworkAccessManager>

#include "storage.h"
#include "conf/layoutmodel.h"
#include "conf/scales.h"
#include "comm/isender.h"
#include "comm/sendermulti.h"
#include "event/ieventhandler.h"
#include "paint/ipaint.h"
#include "paint/ipointpaint.h"
#include "comm/libofqf/qosctypes.h"

#define RC1_SCALES_XML_URL "http://x21.ch/rc1/scales.xml"
#define RC1_ADS_URL "http://ads.misuco.org/go/?id="

class IPaint;
class IPointPaint;
class IEventHandler;

#ifdef NOGL
class RC1 : public QWidget, PathObject
#else
class RC1 : public QGLWidget, PathObject
#endif
{
    Q_OBJECT

public:
    explicit RC1(QWidget *parent = 0);
    ~RC1();

#ifdef RC1_IOS
    constexpr static const float freq_max = 20000.0f;
#else
    static const float freq_max = 20000.0f;
#endif

    Storage *getStorage() const;
    LayoutModel *getLayout() const;
    ISender *getSender() const;
    long getNow();
    int getFps();
    QTime *getFpsT();
    QImage *getBgImage();
    int getBlockerTimeLeft() {return blockerTimeLeft;}

    long getTtl() const;
    void setTtl(long value);

    void connectApp(QApplication * app);
    void transmitSoundParam();
    
public slots:
    void replyFinished(QNetworkReply * r);
    void appStateChange(Qt::ApplicationState state);

protected:
    /*  QGLWidget implementation */
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *);
    bool event(QEvent *event);

    /*  PathObject (Server) implementation */
    virtual void signalData( QString, QVariant, QHostAddress *, quint16 );

private:
    Storage * storage;
    LayoutModel * layout;
    SenderMulti * sender;
    IEventHandler * ehand;
    IPaint ** prepainters;
    IPointPaint ** pointpainters;
    IPaint ** postpainters;
    Scales scaletab;

    // painter switch
    bool * painterOn;

    QOscServer * oscin;

    //QHostAddress senderAddress;
    //int senderPort;
    //QList<QHostAddress> ignoreAddr;
    
    int chan; // midi chan for pc

    int nPrePainters;
    int nPointPainters;
    int nPostPainters;

    long now;
    long ttl;   // time to live for points

    // network config
    QNetworkAccessManager * netxs;
    QString storagePath;
    QString progmemFile;

    // mode
    bool midimode;

    // background image
    QImage bgImageOri;
    QImage bgImage;
    QString bgUrl;

    // blocker
    QImage bgBlockerOri;
    QImage bgBlocker;
    bool blockerOn;
    int blockerTimeout;
    int blockerTimeLeft;
    IPaint * blockerPainter;
    QString adid;
    bool downloadAd;

    // frame statistic
    int fps;
    int fcnt;
    QTime fpsT;
    bool secTimer;

    // emulated touch point id for mouse events
    int eventId;
    bool nomouse;
    
    // lists, required to manage tuio
    QList<quint32> tuioAlive;
    QList<quint16> tuioSources;

//    void resetStat();
    void setPPSmin();
    void fillWithScale();

};

#endif // VIEW_H
