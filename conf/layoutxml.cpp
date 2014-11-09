#include <QDebug>
#include "layoutmodel.h"

LayoutXml::LayoutXml()
{
}

void LayoutXml::readXml(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //qDebug("cannot read file");
        return;
    }
    //qDebug() << " opened " << filename;
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        //qDebug() << " xmlr.name() " << xmlr.name();
        if (xmlr.name() == "misuco" && xmlr.attributes().value("version") == "1.0")
            readLayout();
        else
            xmlr.raiseError(QObject::tr("The file is not a MISUCO version 1.0 file."));
    }
    file.close();
}

void LayoutXml::writeXml(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    xml.setDevice(&file);
    int i=0;
    QString att;

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE misuco>");
    xml.writeStartElement("misuco");
    xml.writeAttribute("version", "1.0");

    att.sprintf("%d",layout->getRowheightmax());
    xml.writeAttribute("rowheightmax", att);

    for (int row = 0; row < layout->getNrows(); row++) {
        xml.writeStartElement("row");
        att.sprintf("%d",layout->getRowheight(row));
        xml.writeAttribute("height",att);
        att.sprintf("%d",layout->getSegwidthmax(row));
        xml.writeAttribute("segwidthmax",att);
        for(int col=0;col<layout->getNseg(row); col++) {
            xml.writeStartElement("seg");
            att.sprintf("%d",layout->getSegwidth(i));
            xml.writeAttribute("segwidth", att);
            att.sprintf("%lf",layout->getFreq(i));
            xml.writeAttribute("freq", att);
            att.sprintf("%d",layout->getMidinote(i));
            xml.writeAttribute("midinote", att);
            att.sprintf("%d",layout->getCtlx(i));
            xml.writeAttribute("ctlx", att);
            att.sprintf("%d",layout->getCtly(i));
            xml.writeAttribute("ctly", att);
            att.sprintf("%d",layout->getChan(i));
            xml.writeAttribute("chan", att);
            att.sprintf("%d",layout->getPressed(i));
            xml.writeAttribute("pressed", att);
            att.sprintf("%d",layout->getSegtype(i));
            xml.writeAttribute("segtype", att);
            xml.writeAttribute("segtext", *layout->getSegText(i));
            att.sprintf("%d",layout->getSegH(i));
            xml.writeAttribute("segh", att);
            xml.writeEndElement();
            i++;
        }
        xml.writeEndElement();
    }
    xml.writeEndDocument();

    file.close();

}

void LayoutXml::readLayout() {
    int row=0;
    int seg=0;
    int nrowseg=0;

    layout->rowheightmax=xmlr.attributes().value("rowheightmax").toString().toInt();
    layout->scaleRow=xmlr.attributes().value("scalerow").toString().toInt();
    while (xmlr.readNextStartElement()) {
        //qDebug() << "xmlr row name " << xmlr.name();
        if (xmlr.name() == "row") {
            if(row>0) {
                layout->nseg[row-1]=nrowseg;
                //qDebug() << "set nrowseg " << nrowseg << " for row " << row-1;
            }
            layout->segwidthmax[row]=xmlr.attributes().value("segwidthmax").toString().toInt();
            layout->rowheight[row]=xmlr.attributes().value("height").toString().toInt();
            if(row==layout->scaleRow) {
                layout->scaleStartSeg=seg;
            }
            row++;
            nrowseg=0;

            while(xmlr.readNextStartElement()) {
                //qDebug() << "xmlr seg name " << xmlr.name();
                if (xmlr.name() == "seg") {
                    layout->segwidth[seg]=xmlr.attributes().value("segwidth").toString().toInt();
                    layout->freq[seg]=xmlr.attributes().value("freq").toString().toFloat();
                    layout->midinote[seg]=xmlr.attributes().value("midinote").toString().toInt();
                    layout->ctlx[seg]=xmlr.attributes().value("ctlx").toString().toInt();
                    layout->ctly[seg]=xmlr.attributes().value("ctly").toString().toInt();
                    layout->segtype[seg]=xmlr.attributes().value("segtype").toString().toInt();
                    layout->pressed[seg]=xmlr.attributes().value("pressed").toString().toInt();
                    layout->chan[seg]=xmlr.attributes().value("chan").toString().toInt();
                    layout->segText[seg].clear();
                    layout->segText[seg].append(xmlr.attributes().value("segtext").toString());
                    layout->segH[seg]=xmlr.attributes().value("segh").toString().toInt();
                    //qDebug()<< "segH " << layout->segH[seg] << " for seg " << seg;
                    seg++;
                    nrowseg++;
                    xmlr.skipCurrentElement();
                }
                else {
                    xmlr.skipCurrentElement();
                }
            }
        } else {
            xmlr.skipCurrentElement();
        }
    }
    layout->nseg[row-1]=nrowseg;
    layout->nrows=row;
    layout->nsegs=seg;
    layout->calcGeo(layout->widthPx,layout->heightPx);
    layout->updateLayout();

    //qDebug() << xmlr.error();
    //qDebug() << xmlr.errorString();
    //qDebug() << "nrows " << row << " nsegs " << seg;
}
