#ifndef PROGMEM_H
#define PROGMEM_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>

class ProgMem
{
public:
    ProgMem();
    // program memory
    
    static const uint bscale_max=11;
    static const uint soudnparam_max=10;
    static const uint progmem_max=11;
    
    struct prog {
        uint basenote;
        uint baseoct;
        uint topoct;
        bool bscale[bscale_max];
        uint soundParam[soudnparam_max];
    };

    prog progmem[progmem_max];

    void readProgmemXml(QString filename);
    void writeProgmemXml(QString filename);

private:
    QXmlStreamReader xmlr;
    QXmlStreamWriter xml;
};

#endif // PROGMEM_H
