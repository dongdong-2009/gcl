#ifndef SCHEDULE_TASK_H
#define SCHEDULE_TASK_H

#include "protocol_global.h"

class ScheduleTaskBase;

enum ScheduleTaskStateEnum
{
    ScheduleTaskState_Suspend,
    ScheduleTaskState_Running,
    ScheduleTaskState_Finish,
    ScheduleTaskState_Cancel,
    ScheduleTaskState_Error
};

typedef void (*fn_task_state_changed_t)(ScheduleTaskBase* oSender, ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState);


class ScheduleTaskSubject
{
protected:
    virtual void taskState_changed(ScheduleTaskBase* oSender, ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState) = 0;

    friend class ScheduleTaskBase;

};

class ScheduleTaskBase
{
public:
    ScheduleTaskBase();
    virtual ~ScheduleTaskBase();

    virtual std::string taskDesc() const = 0;

    void doIt();

    inline ScheduleTaskStateEnum taskState(){ return _taskState; }

    inline int runInterval(){ return _runInterval; }

    inline const msepoch_t& overTime(){ return _overTime; }
    inline void setOverTime(const msepoch_t& value){ _overTime = value; }

    inline int overInterval(){ return _overInterval; }
    inline void setOverInterval(int value){ _overInterval = value; }

    inline int overTimes(){ return _overTimes; }
    inline void setOverTimes(int value){ _overTimes = value; }

    inline int runTimes(){ return _runTimes; }

    inline const msepoch_t& createTime() const { return _createTime; }

    inline const msepoch_t& startTime() const { return _startTime; }

    inline const msepoch_t& finishTime() const { return _finishTime; }

    inline const msepoch_t& cancelTime() const { return _cancelTime; }

    inline const msepoch_t& errorTime() const { return _errorTime; }

    inline int sourceId() const { return _sourceId; }
    inline void setSourceId(const int& value) { _sourceId = value; }

    inline int id() const { return _id; }
    inline void setId(const int& value) { _id = value; }

    void addObserver(ScheduleTaskSubject* oSubject);

    void removeObserver(ScheduleTaskSubject* oSubject);

    void clearObserver();

    void setFuncTaskStateChanged(fn_task_state_changed_t func);

    bool isOver() const;

    std::vector<std::string> reportSelf() const;

    inline void setRunInterval(int value){ _runInterval = value; }

    void run();

    void finish();

    void cancel();

    void suspend();

    void raiseError();

    void reset();

protected:
    virtual void beforeRun() {}
    virtual void runImpl() = 0;
    virtual void afterRun() {}

    virtual void beforeCancel() {}
    virtual void cannelImpl() = 0;
    virtual void afterCancel() {}

    virtual void toReport(std::vector<std::string>& sReports) const {}

    void raiseTaskStateChanged(ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState);

private:
    std::vector<ScheduleTaskSubject*>* _subjects;
    fn_task_state_changed_t _funcTaskStateChanged;
    ScheduleTaskStateEnum _taskState;
    msepoch_t _lastRunTime;
    int _runInterval;
    msepoch_t _createTime;
    msepoch_t _overTime;
    int _overTimes;
    int _overInterval;
    msepoch_t _startTime;
    msepoch_t _finishTime;
    msepoch_t _cancelTime;
    msepoch_t _errorTime;
    int _runTimes;
    int _sourceId;
    int _id;

};


#endif // SCHEDULE_TASK_H
