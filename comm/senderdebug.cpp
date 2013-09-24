/*

Copyright (C) 2013 by Claudio Zopfi, Zurich, Suisse, z@x21.ch

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "senderdebug.h"

#include <QDebug>

SenderDebug::SenderDebug()
{
}

void SenderDebug::cc(int c, int voiceId, int cc, double v1)
{
    qDebug() << "SenderDebug::cc(" << c << "," << voiceId << "," << cc << "," << v1 << ")";
}

void SenderDebug::pc(int c, int v1)
{
    qDebug() << "SenderDebug::pc(" << c << "," << v1 << ")";
}

void SenderDebug::note(int c, int voiceId, int v1, int v2)
{
    qDebug() << "SenderDebug::note(" << c << "," << voiceId << "," << v1 << "," << v2 << ")";
}
