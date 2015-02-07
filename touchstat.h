#ifndef TOUCHSTAT_H
#define TOUCHSTAT_H


class TouchStat
{
public:
    TouchStat();
    void newT(long t);
    void newFps(long fps);
    int getLatSum(int n);
    int getFpsSum(int n);

private:
    int latSums[1024];
    int fpsSums[1024];
    long prevT;

};

#endif // TOUCHSTAT_H
