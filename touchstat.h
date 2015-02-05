#ifndef TOUCHSTAT_H
#define TOUCHSTAT_H


class TouchStat
{
public:
    TouchStat();
    ~TouchStat();
    void newT(long t);
    int getSum(int n);

private:
    int sums[1024];
    long prevT;

};

#endif // TOUCHSTAT_H
