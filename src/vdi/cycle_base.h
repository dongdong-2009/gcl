#ifndef CYCLE_BASE_H
#define CYCLE_BASE_H


#include "protocol_global.h"

#include "vdi_declspec.h"

class GM_GCL_VDI_API CycleBase : public ICxTimerNotify
{
public:
    CycleBase();

    virtual ~CycleBase();

    inline int sendInterval() const { return _sendTimer->interval(); }

    inline void setSendInterval(int value) { _sendTimer->setInterval(value); }

    inline int sentTimes() const { return _sentTimes; }

    inline msepoch_t sentLastTime() const { return _sentLastTime; }

    void start() ;

    void stop() ;

    void toReport(std::vector<std::string> &sReports) const ;

protected:
    void timer_timeOut(const CxTimer* oTimer);

    virtual int cycleSendImpl() = 0;

private:
    CxTimer * _sendTimer;
    int _sentTimes;
    msepoch_t _sentLastTime;

};

#endif // CYCLE_BASE_H
