#include "cycle_base.h"

CycleBase::CycleBase()
{
    _sendTimer = new CxTimer();
    _sendTimer->setTimerTimeOut(this);
}

CycleBase::~CycleBase()
{
    delete _sendTimer;
}

void CycleBase::start()
{
    if (_sendTimer->interval() >  0){
        _sendTimer->start();
    }
    _sentTimes = 0;
    _sentLastTime = 0;
}

void CycleBase::stop()
{
    _sendTimer->stop();
}

void CycleBase::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(std::string("Sent_Times=") + CxString::toString(_sentTimes));
    sReports.push_back(std::string("Sent_LastTime=") + CxTime::toString(_sentLastTime));
}

void CycleBase::timer_timeOut(const CxTimer *oTimer)
{
    if (oTimer == _sendTimer) {
        if ( cycleSendImpl() > -1 ) {
            _sentTimes ++;
            _sentLastTime = CxTime::currentSystemTime();
        }
    }
}

