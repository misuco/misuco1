#include "paintblocker.h"

PaintBlocker::PaintBlocker()
{
}

void PaintBlocker::paint(RC1 *view, QPainter *pnt)
{
    QString text;
    text.sprintf("Please donnate or wait %d seconds to get rid of this blocker",view->getBlockerTimeLeft());
    pnt->drawText(1,1,view->getLayout()->getWidth(),view->getLayout()->getHeight(),Qt::AlignCenter,text);
}
