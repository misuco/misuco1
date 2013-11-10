#include <QVariantList>
#include "sendersupercollider.h"
#include "../comm/libofqf/qoscclient.h"

SenderSuperCollider::SenderSuperCollider(RC1 * rc1)
{
    oscout=new QOscClient(QHostAddress("255.255.255.255"),57110);
    oscout->setAddress(QHostAddress("255.255.255.255"),57110);
    this->rc1=rc1;
}

void SenderSuperCollider::cc(int c, int voiceId, int cc, double v1)
{

}

void SenderSuperCollider::pc(int c, int v1)
{

}

void SenderSuperCollider::setDestination(QHostAddress a, int p)
{
    oscout->setAddress(a,p);
}

void SenderSuperCollider::note(int c, int voiceId, double f, int vel)
{
    QVariantList v;
    qDebug() << "note vid:" << voiceId << " f " << f << " vel " << vel;
    if(vel>0) {
        v.append("c1a1");
        v.append(voiceId);
        v.append(0);
        v.append(0);
        sendOsc("/s_new",v);

        v.clear();
        v.append(voiceId);
        v.append("freq");
        v.append(f);
        sendOsc("/n_set",v);

        v.clear();
        v.append(voiceId);
        v.append("amp");
        v.append(vel);
        sendOsc("/n_set",v);

        onNoteCnt++;
        rc1->getEvstat()->incOscnoteoncount();
    } else {
        v.append(voiceId);
        v.append("amp");
        v.append(0);
        sendOsc("/n_set",v);
        onNoteCnt--;
        rc1->getEvstat()->incOscnoteoffcount();
    }
}

void SenderSuperCollider::pitch(int c, int voiceId, double f)
{
    QVariantList v;
    v.append(voiceId);
    v.append("freq");
    v.append(f);
    sendOsc("/n_set",v);
}

void SenderSuperCollider::sendOsc(QString path, QVariant list)
{
//    qDebug() << " sendOsc to " << path << " values " << list;
    oscout->sendData(path,list);
    rc1->getEvstat()->incOsccount();
}
