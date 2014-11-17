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
    
    static const int bscale_max=11;
    static const int soudnparam_max=10;
    static const int progmem_max=11;
    
    struct prog {
        int basenote;
        int baseoct;
        int topoct;
        bool bscale[bscale_max];
        int soundParam[soudnparam_max];
    };

    prog progmem[progmem_max];

    void readProgmemXml(QString filename);
    void writeProgmemXml(QString filename);

private:
    QXmlStreamReader xmlr;
    QXmlStreamWriter xml;
};

#endif // PROGMEM_H
