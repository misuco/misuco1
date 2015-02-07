#include "touchstat.h"

TouchStat::TouchStat()
{
    for(int i=0;i<256;i++) {
        latSums[i]=0;
        fpsSums[i]=0;
    }
}

void TouchStat::newT(long t)
{
    int diff=t-prevT;
    if(diff>=1024) {
        latSums[1023]++;
    } else {
        latSums[diff]++;
    }
    prevT=t;
}

void TouchStat::newFps(long fps)
{
    if(fps>255) {
        fpsSums[255]++;
    } else {
        fpsSums[fps]++;
    }
}

int TouchStat::getFpsSum(int n)
{
    return fpsSums[n];
}

int TouchStat::getLatSum(int n)
{
    return latSums[n];
}

