#include "soundmem.h"
#include <QFile>

SoundMem::SoundMem()
{

}

SoundMem::~SoundMem()
{

}


void SoundMem::readSoundmemXml(QString filename)
{
    QFile file(filename);
    // default initial prog memory
    if(!file.exists()) {
        filename=":/conf/sound.xml";
        file.setFileName(filename);
    }
    //qDebug() << "reading progmem " << filename;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //qDebug() << "cannot read file " << filename;
        return;
    }
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        if (xmlr.name() == "misucosoundmem" && xmlr.attributes().value("version") == "1.0") {
            int row=0;
            while (xmlr.readNextStartElement() && row<soundmem_max) {
                //qDebug() << "row " << row;
                if (xmlr.name() == "sound") {
                    for(int j=0;j<soundparam_max;j++) {
                        QString attname;
                        attname.sprintf("p%d",j);
                        soundParam[row][j]=xmlr.attributes().value(attname).toString().toInt();
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

void SoundMem::writeSoundmemXml(QString filename)
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
    xml.writeStartElement("misucosoundmem");
    xml.writeAttribute("version", "1.0");

    for (int row = 0; row < soundmem_max; row++) {
        xml.writeStartElement("sound");

        for(int j=0;j<soundparam_max;j++) {
            att.sprintf("%d",soundParam[row][j]);
            attname.sprintf("p%d",j);
            xml.writeAttribute(attname,att);
        }

        xml.writeEndElement();
    }
    xml.writeEndDocument();
    file.close();
}
