#include "waveform.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

waveform::waveform()
{
    QFile wave(":/wav/waves/guitar1m.wav");
    qDebug() << "waveform opening ";
    if (wave.open(QIODevice::ReadOnly)) {
        int dimension = (wave.size()-44)/2;
        qDebug() << "waveform dimension " << dimension << " samples ";
        values = new float[dimension];
        QDataStream input(&wave);
        input.readRawData((char *)values,44);
        int val=0;
        int i=0;
        while (!input.atEnd()) {
            input.readRawData((char *)(&val),2);
            values[i]=(float)val/32768.0f-1;
            i++;
        }
        qDebug() << "waveform read " << i << " samples ";
        attack=1000;
        loop=80000;
        release=95000;
    }
}

float waveform::getValue(int i)
{
    return values[i];
}

int waveform::getAttack()
{
    return attack;
}

int waveform::getLoop()
{
    return loop;
}

int waveform::getRelease()
{
    return release;
}

