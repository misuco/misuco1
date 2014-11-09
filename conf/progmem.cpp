#include "progmem.h"

#include <QDebug>

ProgMem::ProgMem()
{
}

void ProgMem::readProgmemXml(QString filename)
{
    QFile file(filename);
    // default initial prog memory
    if(!file.exists()) {
        filename=":/conf/prog.xml";
        file.setFileName(filename);
    }
    qDebug() << "reading progmem " << filename;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //qDebug() << "cannot read file " << filename;
        return;
    }
    //qDebug() << "opened " << filename;
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        //qDebug() << "xmlr.name " << xmlr.name();
        if (xmlr.name() == "misucoprogmem" && xmlr.attributes().value("version") == "1.0") {
            int row=0;
            while (xmlr.readNextStartElement() && row<NPROGMEM) {
                //qDebug() << "row " << row;
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
                    for(int i=0;i<NSOUNDPARAM;i++) {
                        QString attrName;
                        attrName.sprintf("soundparam%d",i);
                        progmem[row].soundParam[i]=xmlr.attributes().value(attrName).toInt();
                    }
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

void ProgMem::writeProgmemXml(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
        //qDebug() << "cannot write file " << filename;
    }
    //qDebug() << "writing progmem file " << filename;

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

        for(int i=0;i<NSOUNDPARAM;i++) {
            att.sprintf("%d",progmem[row].soundParam[i]);
            QString attrName;
            attrName.sprintf("soundparam%d",i);
            xml.writeAttribute(attrName,att);
        }

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

