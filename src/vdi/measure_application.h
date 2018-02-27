#ifndef MEASURE_APPLICATION_H
#define MEASURE_APPLICATION_H


class MeasureApplication
{
public:
    static void start();

    static void stop();

    static void timerTimeout(int iInterval);

    //固定点(YX)的起始点 mid
    static int monsbIdBegin();

    //(YX)的数量
    static int monsbCount();

    //固定点(YC)的起始点 mid
    static int ycaddIdBegin();

    //(YC)的数量
    static int ycaddCount();

    static int strawIdBegin();

    static int strawCount();

private:
    //扫描共享内存的变量区
    static void scanShareMemory();

    static void setFixMonsbs();

    static void setFixStraws();

    static void setFixYcadds();

};

#endif // MEASURE_APPLICATION_H
