#ifndef SCHEDULE_TASK_MANAGER_H
#define SCHEDULE_TASK_MANAGER_H

#include "schedule_task.h"

class ScheduleTaskManager : public ICxTimerNotify , public ScheduleTaskSubject
{
public:
    ScheduleTaskManager();

    virtual ~ScheduleTaskManager();

    const std::vector<ScheduleTaskBase*> & tasks() const;

    int taskCount(ScheduleTaskStateEnum eState) const;

    ScheduleTaskBase * firstTask() const;

    ScheduleTaskBase * firstTask(ScheduleTaskStateEnum eState) const;

    ScheduleTaskBase * lastTask() const;

    void start();

    void stop();

    void apendTask(ScheduleTaskBase* oTask);

    void removeTask(ScheduleTaskBase* oTask);

    std::vector<std::string> reportTasks() const;

protected:
    void timer_timeOut(const CxTimer* oTimer);

    void taskState_changed(ScheduleTaskBase* oSender, ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState);

    virtual bool checkTasksBefore() { return true; }

    virtual void checkTasksAfter() {}

    virtual bool doTasksBefore() { return true; }

    virtual void doTasksAfter() {}

    inline CxInterinfoOutStream outTaskPrompt(){
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Task); r.title("ScheduleTaskManager"); r << CxGlobal::promptString; return r;
    }

#if defined(GM_DEBUG)
    inline CxInterinfoOutStream outTaskDebug()  {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Task); r.title("ScheduleTaskManager"); r << CxGlobal::debugString; return r;
    }
#else
#undef outTaskDebug
    inline CxInterinfoNoOutStream outTaskDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outTaskDebug_MACRO while (false) outTaskDebug
#define outTaskDebug CX_NO_outTaskDebug_MACRO
#endif

private:
    void checkTask();

    void doTask();

private:
    std::vector<ScheduleTaskBase*> _tasks;
    CxTimer _timerCheck;
    CxTimer _timerDo;

};

#endif // SCHEDULE_TASK_MANAGER_H
