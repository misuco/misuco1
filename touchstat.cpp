#include "touchstat.h"

#include <QDebug>
#include <QFile>

TouchStat::TouchStat()
{
    for(int i=0;i<256;i++) {
        latSums[i]=0;
        fpsSums[i]=0;
    }
}

void TouchStat::newT(long t)
{
    int diff=t-prevT;
    if(diff>=0) {
        if(diff>=255) {
            latSums[255]++;
        } else {
            latSums[diff]++;
        }
    }
    prevT=t;
    evcnt++;
}

void TouchStat::newFps(long fps)
{
    if(fps>=255) {
        fpsSums[255]++;
    } else {
        fpsSums[fps]++;
    }
    ltcnt++;
}

int TouchStat::getFpsSum(int n)
{
    return fpsSums[n];
}

int TouchStat::getLatSum(int n)
{
    return latSums[n];
}

void TouchStat::readXml(QString filename)
{
    QFile file(filename);
    for(int i=0;i<256;i++) {
        latSums[i]=0;
        fpsSums[i]=0;
    }
    int total=0;
    // default initial prog memory
    if(file.exists()) {
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "cannot read file " << filename;
            return;
        }
        xmlr.setDevice(&file);
        if (xmlr.readNextStartElement()) {
            if (xmlr.name() == "touchstat" && xmlr.attributes().value("version") == "1.03") {
                if(xmlr.attributes().hasAttribute("lt")) {
                    ltcnt=xmlr.attributes().value("lt").toLong();
                    //qDebug() << "load ltcnt " << ltcnt;
                }
                if(xmlr.attributes().hasAttribute("ev")) {
                    evcnt=xmlr.attributes().value("ev").toLong();
                    //qDebug() << "load evcnt " << evcnt;
                }
                //int row=0;
                while (xmlr.readNextStartElement() ) {
                    //qDebug() << "row " << row++;
                    if (xmlr.name() == "t") {
                        if(xmlr.attributes().hasAttribute("n")) {
                            int val=xmlr.attributes().value("n").toString().toInt();
                            latSums[xmlr.attributes().value("t").toString().toInt()]=val;
                            total+=val;
                        }
                    } else if (xmlr.name() == "f") {
                        if(xmlr.attributes().hasAttribute("n")) {
                            int val=xmlr.attributes().value("n").toString().toInt();
                            fpsSums[xmlr.attributes().value("f").toString().toInt()]=val;
                            total+=val;
                        }
                    }
                    xmlr.skipCurrentElement();
                }
            } else {
                qDebug() << "The file is not a MISUCO version 1.03 touchstat file.";
            }
            // reset statistic after 32000, to prevent overflow
            if(total>32000) {
                qDebug() << "reset statistic.";
                for(int i=0;i<256;i++) {
                    latSums[i]=0;
                    fpsSums[i]=0;
                }
            }
        }
        file.close();
    }
}

void TouchStat::writeXml(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "cannot write file " << filename;
        return;
    }
    xml.setDevice(&file);
    QString att;

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE misuco>");
    xml.writeStartElement("touchstat");
    xml.writeAttribute("version", "1.03");
    att.sprintf("%ld",ltcnt);
    //qDebug() << "writing ltcnt " << att;
    xml.writeAttribute("lt", att);
    att.sprintf("%ld",evcnt);
    //qDebug() << "writing evcnt " << att;
    xml.writeAttribute("ev", att);

    for (int row = 0; row < 255; row++) {
        if(latSums[row]>0) {
            xml.writeStartElement("t");
            att.sprintf("%d",row);
            xml.writeAttribute("t",att);
            att.sprintf("%d",latSums[row]);
            xml.writeAttribute("n",att);
            xml.writeEndElement();
        }
    }

    for (int row = 0; row < 255; row++) {
        if(fpsSums[row]>0) {
            xml.writeStartElement("f");
            att.sprintf("%d",row);
            xml.writeAttribute("f",att);
            att.sprintf("%d",fpsSums[row]);
            xml.writeAttribute("n",att);
            xml.writeEndElement();
        }
    }
    xml.writeEndDocument();
    file.close();
}

void TouchStat::getStatParam(QString *statParam)
{
    quint16 cs=0;
    QString spr;
    int fpsMax=0;
    int latMax=0;
    for(int i=0;i<256;i++) {
        if(fpsSums[i]>fpsMax) {
            fpsMax=fpsSums[i];
        }
        if(latSums[i]>latMax) {
            latMax=latSums[i];
        }
    }
    latMax/=10;
    latMax++;   // allways min 1
    fpsMax/=10;
    fpsMax++;   // allways min 1
    for(int i=0;i<256;i++) {
        if(fpsSums[i]>fpsMax) {
            spr.sprintf("&f%02x=%04x",i,fpsSums[i]);
            statParam->append(spr);
            cs+=fpsSums[i]*0x0301;
        }
        if(latSums[i]>latMax) {
            spr.sprintf("&l%02x=%04x",i,latSums[i]);
            statParam->append(spr);
            cs+=latSums[i]*0x1100;
        }
    }
    if(cs!=0) {
        cs+=0x7999;
    }
    spr.sprintf("&cs=%04x",cs);
    statParam->append(spr);
    spr.sprintf("&lt=%016x",ltcnt);
    statParam->append(spr);
    spr.sprintf("&ev=%016x",evcnt);
    statParam->append(spr);
    //qDebug() << "statparam " << *statParam;
}
