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
#ifndef EVENTSTAT_H
#define EVENTSTAT_H

class EventStat
{
public:
    EventStat();
    void resetStat();

    int getToucheventcount() const;
    void incToucheventcount();

    int getTouchbegincount() const;
    void incTouchbegincount();

    int getTouchmovecount() const;
    void incTouchmovecount();

    int getTouchendcount() const;
    void incTouchendcount();

    int getTransitioncount() const;
    void incTransitioncount();

    int getOsccount() const;
    void incOsccount();

    int getOscnoteoncount() const;
    void incOscnoteoncount();

    int getOscnoteoffcount() const;
    void incOscnoteoffcount();

    int getAlreadyoncount() const;
    void incAlreadyoncount();

private:
    int toucheventcount;
    int touchsbegincount;
    int touchmovecount;
    int touchendcount;
    int osccount;
    int oscnoteoncount;
    int oscnoteoffcount;

    int transitioncount;
    int alreadyoncount;
};

#endif // EVENTSTAT_H
