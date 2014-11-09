#ifndef PROGMEM_H
#define PROGMEM_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>

#define NSOUNDPARAM 10
#define NPROGMEM 11

class ProgMem
{
public:
    ProgMem();
    // program memory
    struct prog {
        int basenote;
        int baseoct;
        int topoct;
        bool bscale[11];
        int soundParam[NSOUNDPARAM];
    };

    prog progmem[NPROGMEM];

    void readProgmemXml(QString filename);
    void writeProgmemXml(QString filename);

private:
    QXmlStreamReader xmlr;
    QXmlStreamWriter xml;
};

#endif // PROGMEM_H
