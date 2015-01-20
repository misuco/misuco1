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
#include "../qqdialog.h"

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
    
    int evptr_stack_size;
    int * evptr_stack;

    float * freq;       // current frequency
    int * isegb;        // current segment index buffer
    int * mnote;        // current midi
    int * chan;         // current note channel

    float * ccval1;       // current controller1 value: xrel
    float * ccval2;       // current controller1 value: yrel

    float * yori;         // origin position for vertical fadder diff behavior
    float * yrelori;      // origin value for vertical fadder diff behavior

    float cccval1;     // common cc value, average of individual ccvals
    float cccval2;     // common cc value, average of individual ccvals

    int veldef;         // default velovity

    bool layResize;      // is layout resize going on?
    int layResizePnt;   // is layout resize going on?
    int layResizeDiff;   // how much has layout been resized?

    void init();
    float calcYrel(int y, int ysum, int height);
};

#endif // EVENTHANDLERRECT_H
