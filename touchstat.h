#ifndef TOUCHSTAT_H
#define TOUCHSTAT_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QString>

class TouchStat
{
public:
    TouchStat();
    void newT(long t);
    void newFps(long fps);
    int getLatSum(int n);
    int getFpsSum(int n);
    void readXml(QString filename);
    void writeXml(QString filename);
    void getStatParam(QString * statParam);

private:
    int latSums[256];
    int fpsSums[256];
    long prevT;
    QXmlStreamReader xmlr;
    QXmlStreamWriter xml;

};

#endif // TOUCHSTAT_H
