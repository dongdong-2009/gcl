#include "schedule_task.h"



ScheduleTaskBase::ScheduleTaskBase()
{
    _funcTaskStateChanged = 0;
    _taskState = ScheduleTaskState_Suspend;
    _lastRunTime = CxValueType::minValue<msepoch_t>();
    _runInterval = 0;
    _createTime = CxValueType::minValue<msepoch_t>();
    _overTime = CxValueType::maxValue<msepoch_t>();
    _overTimes = CxValueType::maxValue<int>();
    _overInterval = CxValueType::maxValue<int>();
    _startTime = CxValueType::minValue<msepoch_t>();
    _finishTime = CxValueType::maxValue<msepoch_t>();
    _cancelTime = CxValueType::maxValue<msepoch_t>();
    _errorTime = CxValueType::maxValue<msepoch_t>();
    _runTimes = 0;
    _sourceId = -1;
    _subjects = new std::vector<ScheduleTaskSubject*>();
}

ScheduleTaskBase::~ScheduleTaskBase()
{
    delete _subjects;
}

void ScheduleTaskBase::run()
{
    ScheduleTaskStateEnum iOldState = _taskState;
    beforeRun();
    _taskState = ScheduleTaskState_Running;
    runImpl();
    if (_runTimes == 0) _startTime = CxTime::currentSystemTime();
    ++ _runTimes;
    _lastRunTime = CxTime::currentSystemTime();
    afterRun();
    raiseTaskStateChanged(iOldState, _taskState);
}

void ScheduleTaskBase::cancel()
{
    ScheduleTaskStateEnum iOldState = _taskState;
    beforeCancel();
    _taskState = ScheduleTaskState_Cancel;
    cannelImpl();
    _cancelTime = CxTime::currentSystemTime();
    afterCancel();
    raiseTaskStateChanged(iOldState, _taskState);
}

void ScheduleTaskBase::raiseError()
{
    ScheduleTaskStateEnum iOldState = _taskState;
    _taskState = ScheduleTaskState_Error;
    _errorTime = CxTime::currentSystemTime();
    raiseTaskStateChanged(iOldState, _taskState);
}

void ScheduleTaskBase::finish()
{
    ScheduleTaskStateEnum iOldState = _taskState;
    _taskState = ScheduleTaskState_Finish;
    _finishTime = CxTime::currentSystemTime();
    raiseTaskStateChanged(iOldState, _taskState);
}

void ScheduleTaskBase::suspend()
{
    ScheduleTaskStateEnum iOldState = _taskState;
    _taskState = ScheduleTaskState_Suspend;
    raiseTaskStateChanged(iOldState, _taskState);
}

void ScheduleTaskBase::reset()
{
    _startTime = CxTime::currentSystemTime();
}

void ScheduleTaskBase::addObserver(ScheduleTaskSubject *oSubject)
{
    _subjects->push_back(oSubject);
}

void ScheduleTaskBase::removeObserver(ScheduleTaskSubject *oSubject)
{
    CxContainer::remove(*_subjects, oSubject);
}

void ScheduleTaskBase::clearObserver()
{
    _subjects->clear();
}

void ScheduleTaskBase::raiseTaskStateChanged(ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState)
{
    if (iOldState != iNewState) {
        if (_funcTaskStateChanged)
        {
            _funcTaskStateChanged(this, iOldState, iNewState);
        }
        for (size_t i = 0; i < _subjects->size(); ++i)
        {
            ScheduleTaskSubject* oSubject = _subjects->at(i);
            oSubject->taskState_changed(this, iOldState, iNewState);
        }
    }
}

void ScheduleTaskBase::setFuncTaskStateChanged(fn_task_state_changed_t func)
{
    _funcTaskStateChanged = func;
}

bool ScheduleTaskBase::isOver() const
{
    //1) _overTime
    //2) _overTimes
    //3) _overInterval
    if (CxTime::milliSecondDifferToNow(_overTime) > 0)
        return true;
    if (_runTimes > _overTimes)
        return true;
    if (_lastRunTime != _startTime && _lastRunTime - _startTime > _overInterval)
        return true;
    return false;
}

void ScheduleTaskBase::doIt()
{
    if ( ! isOver() )
    {
        if (_taskState == ScheduleTaskState_Running && CxTime::milliSecondDifferToNow(_lastRunTime) > _runInterval )
        {
            run();
        }
    }
    else
    {
        cancel();
    }
}

std::vector<std::string> ScheduleTaskBase::reportSelf() const
{
    std::vector<std::string> sReports;
    sReports.push_back(std::string("Task_Desc=") +           taskDesc());
    sReports.push_back(std::string("Task_Subject Count=") +  CxString::toString(_subjects->size()));
    sReports.push_back(std::string("Task_State=") +          CxString::toString(_taskState));
    sReports.push_back(std::string("Last_Run Time=") +       CxString::toString(_lastRunTime));
    sReports.push_back(std::string("Run_Times=") +           CxString::toString(_runTimes));
    sReports.push_back(std::string("Run_Interval=") +        CxString::toString(_runInterval));
    sReports.push_back(std::string("Over_Time=") +           CxString::toString(_overTime));
    sReports.push_back(std::string("Over_Times=") +          CxString::toString(_overTimes));
    sReports.push_back(std::string("Over_Interval=") +       CxString::toString(_overInterval));
    sReports.push_back(std::string("Create_Time=") +         CxString::toString(_createTime));
    sReports.push_back(std::string("Start_Time=") +          CxString::toString(_startTime));
    sReports.push_back(std::string("Finish_Time=") +         CxString::toString(_finishTime));
    sReports.push_back(std::string("Stop_Time=") +           CxString::toString(_cancelTime));
    sReports.push_back(std::string("Error_Time=") +          CxString::toString(_errorTime));
    sReports.push_back(std::string("SourceId=") +            CxString::toString(_sourceId));
    toReport(sReports);
    return sReports;
}
