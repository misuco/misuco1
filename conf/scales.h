#ifndef SCALES_H
#define SCALES_H

#include <QXmlStreamReader>
#include <QString>

class Scales
{
public:
    Scales();
    void readXml(QString filename);
    QString getName(int n);
private:
    QXmlStreamReader xmlr;
    void readScales();
    QString scaleNames[2048];
};

#endif // SCALES_H
