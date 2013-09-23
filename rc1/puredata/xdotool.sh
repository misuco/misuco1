#!/bin/sh
export DISPLAY=:0.0
if [ "$2" -eq "127" ]
then
if [ "$1" -eq "36" ]
then
    xdotool key F5
fi
if [ "$1" -eq "37" ]
then
    xdotool key Left
fi
if [ "$1" -eq "38" ]
then
    xdotool key Right 
fi
fi
