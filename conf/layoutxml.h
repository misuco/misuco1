#ifndef LAYOUTXML_H
#define LAYOUTXML_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "layoutmodel.h"

class layoutxml
{
public:
    layoutxml();
    void setLayoutModel(LayoutModel* l) {layout=l;}
    void readXml();
    void writeXml();

private:
    QXmlStreamWriter xml;
    QXmlStreamReader xmlr;
    LayoutModel * layout;
    void readLayout();
    void writeLayout();
};

#endif // LAYOUTXML_H
