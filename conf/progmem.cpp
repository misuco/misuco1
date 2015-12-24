/*
 
 Copyright (C) 2014 by Claudio Zopfi, Zurich, Suisse, z@x21.ch
 
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

#include "progmem.h"

#include <QDebug>

ProgMem::ProgMem()
{
    adr="255.255.255.255";
    port=3150;
    channel=1;
    errCorr=0;
    senderType=0;
}

void ProgMem::readProgmemXml(QString filename)
{
    QFile file(filename);
    // default initial prog memory
    if(!file.exists()) {
        filename=":/conf/prog.xml";
        file.setFileName(filename);
    }
    //qDebug() << "reading progmem " << filename;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //qDebug() << "cannot read file " << filename;
        return;
    }
    //qDebug() << "opened " << filename;
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        //qDebug() << "xmlr.name " << xmlr.name();
        if (xmlr.name() == "misucoprogmem" && xmlr.attributes().value("version") == "1.0") {
            adr=xmlr.attributes().value("adr").toString();
            port=xmlr.attributes().value("port").toInt();
            channel=xmlr.attributes().value("channel").toInt();
            senderType=xmlr.attributes().value("senderType").toInt();
            errCorr=xmlr.attributes().value("errCorr").toInt();
            int row=0;
            while (xmlr.readNextStartElement() && row<progmem_max) {
                //qDebug() << "row " << row;
                if (xmlr.name() == "prog") {
                    if(xmlr.attributes().hasAttribute("rows")) {
                        progmem[row].rows=xmlr.attributes().value("rows").toString().toInt();
                    } else {
                        progmem[row].rows=0; // default value
                    }
                    progmem[row].basenote=xmlr.attributes().value("basenote").toString().toInt();
                    progmem[row].baseoct=xmlr.attributes().value("baseoct").toString().toInt();
                    progmem[row].topoct=xmlr.attributes().value("topoct").toString().toInt();

                    for(int j=0;j<bscale_max;j++) {
                        QString attname;
                        attname.sprintf("bscale%d",j);
                        progmem[row].bscale[j]=(bool)xmlr.attributes().value(attname).toString().toInt();
                    }
                    progmem[row].sound=row;
                    xmlr.skipCurrentElement();
                    row++;
                } else {
                    xmlr.skipCurrentElement();
                }
            }
        } else if (xmlr.name() == "misucoprogmem" && xmlr.attributes().value("version") == "1.1") {
            adr=xmlr.attributes().value("adr").toString();
            port=xmlr.attributes().value("port").toInt();
            channel=xmlr.attributes().value("channel").toInt();
            senderType=xmlr.attributes().value("senderType").toInt();
            errCorr=xmlr.attributes().value("errCorr").toInt();
            //qDebug() << "read adr " << adr << " port " << port << " senderType " << senderType << " channel " << channel;
            int row=0;
            while (xmlr.readNextStartElement() && row<progmem_max) {
                //qDebug() << "row " << row;
                if (xmlr.name() == "prog") {
                    if(xmlr.attributes().hasAttribute("rows")) {
                        progmem[row].rows=xmlr.attributes().value("rows").toString().toInt();
                    } else {
                        progmem[row].rows=0; // default value
                    }
                    progmem[row].basenote=xmlr.attributes().value("basenote").toString().toInt();
                    progmem[row].baseoct=xmlr.attributes().value("baseoct").toString().toInt();
                    progmem[row].topoct=xmlr.attributes().value("topoct").toString().toInt();
                    progmem[row].seq=row;
                    for(int j=0;j<bscale_max;j++) {
                        QString attname;
                        attname.sprintf("s%d",j);
                        progmem[row].bscale[j]=(bool)xmlr.attributes().value(attname).toString().toInt();
                    }
                    progmem[row].sound=xmlr.attributes().value("sound").toInt();
                    xmlr.skipCurrentElement();
                    row++;
                } else {
                    xmlr.skipCurrentElement();
                }
            }
        } else if (xmlr.name() == "misucoprogmem" && xmlr.attributes().value("version") == "1.2") {
            adr=xmlr.attributes().value("adr").toString();
            port=xmlr.attributes().value("port").toInt();
            channel=xmlr.attributes().value("channel").toInt();
            senderType=xmlr.attributes().value("senderType").toInt();
            errCorr=xmlr.attributes().value("errCorr").toInt();
            //qDebug() << "read adr " << adr << " port " << port << " senderType " << senderType << " channel " << channel;
            int row=0;
            while (xmlr.readNextStartElement() && row<progmem_max) {
                //qDebug() << "row " << row;
                if (xmlr.name() == "prog") {
                    if(xmlr.attributes().hasAttribute("rows")) {
                        progmem[row].rows=xmlr.attributes().value("rows").toString().toInt();
                    } else {
                        progmem[row].rows=0; // default value
                    }
                    progmem[row].basenote=xmlr.attributes().value("basenote").toString().toInt();
                    progmem[row].baseoct=xmlr.attributes().value("baseoct").toString().toInt();
                    progmem[row].topoct=xmlr.attributes().value("topoct").toString().toInt();
                    progmem[row].seq=xmlr.attributes().value("seq").toString().toInt();
                    for(int j=0;j<bscale_max;j++) {
                        QString attname;
                        attname.sprintf("s%d",j);
                        progmem[row].bscale[j]=(bool)xmlr.attributes().value(attname).toString().toInt();
                    }
                    progmem[row].sound=xmlr.attributes().value("sound").toInt();
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
        qDebug() << "cannot write file " << filename;
        return;
    }
    //qDebug() << "writing progmem file " << filename;

    xml.setDevice(&file);
    QString att;
    QString attname;

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE misuco>");
    xml.writeStartElement("misucoprogmem");
    xml.writeAttribute("version", "1.2");
    xml.writeAttribute("adr",adr);
    att.sprintf("%d",port);
    xml.writeAttribute("port",att);
    //qDebug() << "wrote port " << att;
    att.sprintf("%d",senderType);
    xml.writeAttribute("senderType",att);
    //qDebug() << "wrote senderType " << att;
    att.sprintf("%d",errCorr);
    xml.writeAttribute("errCorr",att);
    //qDebug() << "wrote errCorr " << att;
    att.sprintf("%d",channel);
    xml.writeAttribute("channel",att);
    //qDebug() << "wrote channel " << att;
    //qDebug() << "wrote adr " << adr << " port " << port << " senderType " << senderType << " channel " << channel;

    for (int row = 0; row < progmem_max; row++) {
        xml.writeStartElement("prog");

        att.sprintf("%d",progmem[row].basenote);
        xml.writeAttribute("basenote",att);

        att.sprintf("%d",progmem[row].baseoct);
        xml.writeAttribute("baseoct",att);

        att.sprintf("%d",progmem[row].topoct);
        xml.writeAttribute("topoct",att);

        att.sprintf("%d",progmem[row].rows);
        xml.writeAttribute("rows",att);

        att.sprintf("%d",progmem[row].sound);
        xml.writeAttribute("sound",att);

        att.sprintf("%d",progmem[row].seq);
        xml.writeAttribute("seq",att);

        for(int j=0;j<bscale_max;j++) {
            att.sprintf("%d",(int)progmem[row].bscale[j]);
            attname.sprintf("s%d",j);
            xml.writeAttribute(attname,att);
        }

        xml.writeEndElement();
    }
    xml.writeEndDocument();
    file.close();
}

