/*
 
 Copyright (C) 2014 by Claudio Zopfi, Zurich, Suisse, z@x21.ch
 
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
