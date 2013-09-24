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
#ifndef EVENTHANDLERRECT_H
#define EVENTHANDLERRECT_H

#include "ieventhandler.h"

class EventHandlerRect : public IEventHandler
{
public:
    EventHandlerRect();
    void processPoint(Point * p, RC1 * rc1);

private:
    int ntp;            // number of simultanously possible touchpoints

    bool * act;         // is touched?
    int * ievent;       // current event Index
    int * ieventout;    // event id sent (one input event can lead in several output events)
    int ieventoutnext;  // id of next output event

    int * note;         // current note
    int * isegb;        // current segment index buffer
    int * chan;         // current note channel

    double * ccval1;    // current controller1 value: xrel
    double * ccval2;    // current controller1 value: yrel

    double cccval1;     // common cc value, average of individual ccvals
    double cccval2;     // common cc value, average of individual ccvals
    bool useCCCVal;
    int cccvalAvg;      // number of individual ccvals for average calc

    int veldef;         // default velovity

    void init();
};

#endif // EVENTHANDLERRECT_H
