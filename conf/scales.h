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
