/*

   ––– ––– ––– ––– ––– –––
  | m | i | s | u | c | o |
   ––– ––– ––– ––– ––– –––
   microtonal
             surface
                    controller

http://misuco.org

Copyright (C) 2008-2014 by Claudio Zopfi, Zurich, Suisse, z@x21.ch

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

#include "platform.h"
#include <QApplication>
#include "rc1.h"

#if defined(Q_OS_IOS)
extern "C" int qtmn(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    QApplication a(argc, argv);
    
    RC1 * w = new RC1();
    w->connectApp(&a);
    w->show();
    /*
    RC1 w;
    w.connectApp(&a);
    w.show();
    */
    
    return a.exec();
}
