#include "paintbgbitmap.h"

PaintBgBitmap::PaintBgBitmap()
{
}

void PaintBgBitmap::paint(RC1 *view, QPainter * pnt) {
    pnt->drawImage(0,0,*(view->getBgImage()),0,0,view->getLayout()->getWidth(),view->getLayout()->getHeight());
}

int PaintBgBitmap::getParamCount() {
    return 0;
}

float PaintBgBitmap::getParam(quint16 n) {
    return 0;
}

float PaintBgBitmap::getParamMin(quint16 n) {
    return 0;
}

float PaintBgBitmap::getParamMax(quint16 n) {
    return 0;
}

void PaintBgBitmap::setParam(quint16 n, float v) {

}
