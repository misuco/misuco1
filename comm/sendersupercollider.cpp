#include <QVariantList>
#include "sendersupercollider.h"
#include "../comm/libofqf/qoscclient.h"

SenderSuperCollider::SenderSuperCollider(RC1 * rc1)
{
    oscout=new QOscClient(QHostAddress("255.255.255.255"),57110);
    oscout->setAddress(QHostAddress("255.255.255.255"),57110);
    this->rc1=rc1;
}

void SenderSuperCollider::cc(int, int, int, float)
{

}

void SenderSuperCollider::pc(int, int)
{

}

void SenderSuperCollider::setDestination(QHostAddress a, int p)
{
    oscout->setAddress(a,p);
}

void SenderSuperCollider::noteOn(int, int voiceId, float f, int, int, int vel)
{
    QVariantList v;
    //qDebug() << "noteOn vid:" << voiceId << " f " << f << " vel " << vel;
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
}

void SenderSuperCollider::pitch(int, int voiceId, float f, int, int)
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


void SenderSuperCollider::noteOff(int, int voiceId)
{
    QVariantList v;
    v.append(voiceId);
    v.append("amp");
    v.append(0);
    sendOsc("/n_set",v);
    onNoteCnt--;
}
