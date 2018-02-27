#ifndef CALC_EXEC_H
#define CALC_EXEC_H


#include <ccxx/ccxx.h>
#include <psm_struct.h>


class CalcExec
{
public:
    static void start();

    static void stop();

    //定时回调
    static void timerTimeout(int iInterval);

protected:
    static void CalcProcess();
    static void WarnProcess();
    static void MonsbWarnProc();
    static void YcaddWarnProc();
    static void CalcWarnProc();

};


#endif // CALC_EXEC_H
