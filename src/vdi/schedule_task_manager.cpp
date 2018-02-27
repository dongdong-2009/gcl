#include "schedule_task_manager.h"



ScheduleTaskManager::ScheduleTaskManager()
{
    _timerDo.init(this, 300);
    _timerCheck.init(this, 1000);
}

ScheduleTaskManager::~ScheduleTaskManager()
{
}

const std::vector<ScheduleTaskBase *> & ScheduleTaskManager::tasks() const
{
    return _tasks;
}

void ScheduleTaskManager::start()
{
    _timerCheck.start();
    _timerDo.start();
}

void ScheduleTaskManager::stop()
{
    _timerCheck.stop();
    _timerDo.stop();
}

std::vector<std::string> ScheduleTaskManager::reportTasks() const
{
    std::vector<std::string> sReports;
    sReports.push_back(CxString::format("*** Schedule Task Manager Report [ count=%d ] ***", _tasks.size()));
    for (size_t i = 0; i < _tasks.size(); ++i)
    {
        ScheduleTaskBase* oTask = _tasks.at(i);
        std::vector<std::string> sReport = oTask->reportSelf();
        sReports.insert(sReports.end(), sReport.begin(), sReport.end());
        sReports.push_back("");
    }
    return sReports;
}

int ScheduleTaskManager::taskCount(ScheduleTaskStateEnum eState) const
{
    int iCount = 0;
    for (size_t i = 0; i < _tasks.size(); ++i)
    {
        ScheduleTaskBase* oTask = _tasks.at(i);
        if (oTask->taskState() == eState)
        {
            iCount ++;
        }
    }
    return iCount;
}

ScheduleTaskBase *ScheduleTaskManager::firstTask(ScheduleTaskStateEnum eState) const
{
    for (size_t i = 0; i < _tasks.size(); ++i)
    {
        ScheduleTaskBase* oTask = _tasks.at(i);
        if (oTask->taskState() == eState)
        {
            return oTask;
        }
    }
    return NULL;
}

ScheduleTaskBase *ScheduleTaskManager::firstTask() const
{
    if (_tasks.size()>0)
    {
        return _tasks.at(0);
    }
    return NULL;
}

ScheduleTaskBase *ScheduleTaskManager::lastTask() const
{
    if (_tasks.size()>0)
    {
        return _tasks.at(_tasks.size()-1);
    }
    return NULL;
}

void ScheduleTaskManager::timer_timeOut(const CxTimer *oTimer)
{
    if (oTimer == &_timerCheck)
    {
        if (checkTasksBefore())
        {
            checkTask();
            checkTasksAfter();
        }
    }
    else if (oTimer == &_timerDo)
    {
        if (doTasksBefore())
        {
            doTask();
            doTasksAfter();
        }
    }

}

void ScheduleTaskManager::taskState_changed(ScheduleTaskBase *oSender, ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState)
{
    //besttodo
}

void ScheduleTaskManager::apendTask(ScheduleTaskBase *oTask)
{
    _tasks.push_back(oTask);
    oTask->addObserver(this);
}

void ScheduleTaskManager::removeTask(ScheduleTaskBase *oTask)
{
    oTask->removeObserver(this);
    CxContainer::remove(_tasks, oTask);
}

void ScheduleTaskManager::checkTask()
{
    for (int i = _tasks.size()-1; i >= 0; --i)
    {
        ScheduleTaskBase* oTask = _tasks.at(i);
        if (oTask->taskState() == ScheduleTaskState_Finish || oTask->taskState() == ScheduleTaskState_Cancel || oTask->taskState() == ScheduleTaskState_Error)
        {
            removeTask(oTask);
            delete oTask;
        }
    }
}

void ScheduleTaskManager::doTask()
{
    for (size_t i = 0; i < _tasks.size(); ++i)
    {
        ScheduleTaskBase* oTask = _tasks.at(i);
        oTask->doIt();
    }
}
