#ifndef SOUNDMEM_H
#define SOUNDMEM_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class SoundMem
{
public:
    static const int soundparam_max=10;
    static const int soundmem_max=110;

    SoundMem();
    ~SoundMem();

    int soundParam[soundmem_max][soundparam_max];

    void readSoundmemXml(QString filename);
    void writeSoundmemXml(QString filename);

private:
    QXmlStreamReader xmlr;
    QXmlStreamWriter xml;

};

#endif // SOUNDMEM_H
