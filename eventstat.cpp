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
#include "eventstat.h"

EventStat::EventStat()
{
    resetStat();
}

int EventStat::getOscnoteoffcount() const
{
    return oscnoteoffcount;
}

void EventStat::incOscnoteoffcount()
{
    oscnoteoffcount++;
}

int EventStat::getOscnoteoncount() const
{
    return oscnoteoncount;
}

void EventStat::incOscnoteoncount()
{
    oscnoteoncount++;
}

int EventStat::getOsccount() const
{
    return osccount;
}

void EventStat::incOsccount()
{
    osccount++;
}

int EventStat::getTransitioncount() const
{
    return transitioncount;
}

void EventStat::incTransitioncount()
{
    transitioncount++;
}

int EventStat::getTouchendcount() const
{
    return touchendcount;
}

void EventStat::incTouchendcount()
{
    touchendcount++;
}

int EventStat::getTouchmovecount() const
{
    return touchmovecount;
}

void EventStat::incTouchmovecount()
{
    touchmovecount++;
}

int EventStat::getTouchbegincount() const
{
    return touchsbegincount;
}

void EventStat::incTouchbegincount()
{
    touchsbegincount++;
}

int EventStat::getToucheventcount() const
{
    return toucheventcount;
}

void EventStat::incToucheventcount()
{
    toucheventcount++;
}

int EventStat::getAlreadyoncount() const
{
    return alreadyoncount;
}

void EventStat::incAlreadyoncount()
{
    alreadyoncount++;
}

void EventStat::resetStat()
{
    toucheventcount=0;
    touchsbegincount=0;
    touchmovecount=0;
    touchendcount=0;
    transitioncount=0;
    osccount=0;
    oscnoteoncount=0;
    oscnoteoffcount=0;
    alreadyoncount=0;
}
