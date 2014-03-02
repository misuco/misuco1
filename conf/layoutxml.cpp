#include <QFile>
#include "layoutxml.h"
#include <QDebug>

layoutxml::layoutxml()
{
}

void layoutxml::readXml(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        /*
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
                             */
        return;
        qDebug("cannot read file");
    }
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        if (xmlr.name() == "misuco" && xmlr.attributes().value("version") == "1.0")
            readLayout();
        else
            xmlr.raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
    }
    file.close();
}

void layoutxml::writeXml(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    xml.setDevice(&file);
/*
 *
 *  int getNrows() const;
    int getNsegs() const;

    int getRowheightpx(int i) const;
    int getRowheightmax() {return rowheightmax;}
    int getRowheight(int i) {return rowheight[i];}
    int getNseg(int i) const;

    int getSegwidth(int i) const;
    int getSegwidthmax(int i) const;
    int getSegwidthpx(int i) const;
    double getNote(int i) const;
    int getMidiNote(int i) const;
    int getCtlx(int i) const;
    int getCtly(int i) const;
    int getChan(int i) const;
    int getPressed(int i) const;
    int getSegtype(int i) const;
    QString * getSegText(int i) const;
    int getSegH(int i) const;

 */
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
            att.sprintf("%lf",layout->getValue(i));
            xml.writeAttribute("value", att);
            att.sprintf("%d",layout->getValueInt(i));
            xml.writeAttribute("valueint", att);
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

void layoutxml::readLayout() {
    int row=0;
    int seg=0;
    int nrowseg=0;

    layout->rowheightmax=xmlr.attributes().value("rowheightmax").toString().toInt();
    while (xmlr.readNextStartElement()) {
//        qDebug() << "xmlr row name " << xmlr.name();
        if (xmlr.name() == "row") {
            if(row>0) {
                layout->nseg[row-1]=nrowseg;
                //qDebug() << "set nrowseg " << nrowseg << " for row " << row-1;
            }
            layout->segwidthmax[row]=xmlr.attributes().value("segwidthmax").toString().toInt();
            layout->rowheight[row]=xmlr.attributes().value("height").toString().toInt();
            row++;
            nrowseg=0;

            while(xmlr.readNextStartElement()) {
//                qDebug() << "xmlr seg name " << xmlr.name();
                if (xmlr.name() == "seg") {
                    layout->segwidth[seg]=xmlr.attributes().value("segwidth").toString().toInt();
                    layout->value[seg]=xmlr.attributes().value("value").toString().toDouble();
                    layout->valueint[seg]=xmlr.attributes().value("valueint").toString().toInt();
                    layout->ctlx[seg]=xmlr.attributes().value("ctlx").toString().toInt();
                    layout->ctly[seg]=xmlr.attributes().value("ctly").toString().toInt();
                    layout->segtype[seg]=xmlr.attributes().value("segtype").toString().toInt();
                    layout->pressed[seg]=xmlr.attributes().value("pressed").toString().toInt();
                    layout->chan[seg]=xmlr.attributes().value("chan").toString().toInt();
                    layout->segText[seg].clear();
                    layout->segText[seg].append(xmlr.attributes().value("segtext").toString());
                    layout->segH[seg]=xmlr.attributes().value("segh").toString().toInt();
//                    qDebug()<< "segH " << layout->segH[seg] << " for seg " << seg;
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
    layout->calcGeo(layout->width,layout->height);

//    qDebug() << xmlr.error();
//    qDebug() << xmlr.errorString();
//    qDebug() << "nrows " << row << " nsegs " << seg;
}
