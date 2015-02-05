#include "touchstat.h"

TouchStat::TouchStat()
{
    for(int i=0;i<1024;i++) {
        sums[i]=0;
    }
}

TouchStat::~TouchStat()
{

}

void TouchStat::newT(long t)
{
    int diff=t-prevT;
    if(diff>=1024) {
        sums[1023]++;
    } else {
        sums[diff]++;
    }
    prevT=t;
}

int TouchStat::getSum(int n)
{
    return sums[n];
}

