#ifndef MONSB_CONTROL_TASK_H
#define MONSB_CONTROL_TASK_H


#include "monsb_control.h"
#include "schedule_task.h"
#include "send_monsb_control_cases.h"

class MonsbControlTask : public ScheduleTaskBase
{
public:
    MonsbControlTask();
    ~MonsbControlTask();

    std::string taskDesc() const;

    inline const MonsbMeasure* measure() const { return _measure; }

    inline const int& fromValue() const { return _fromValue; }

    inline const int& toValue() { return _toValue; }

    inline const MonsbMeasure* currentRunMeasure() const { return _currentRunMeasure; }

    inline const int& currentRunFromValue() { return _currentRunFromValue; }

    inline const int& currentRunToValue() { return _currentRunToValue; }

    inline const size_t & controlIndex() const { return _controlIndex; }

    inline const std::vector<MonsbControl >& controls() const { return _controls; }

    inline const std::vector<MonsbControl >& cancels() const { return _cancels; }

    //执行
    void control(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    //选择
    void select(const MonsbMeasure* oMeasure, int fromValue, int toValue);

private:
    void runImpl();

    void cannelImpl();

    void toReport(std::vector<std::string>& sReports) const;

    bool needToControl(const MonsbMeasure* oMeasure, const int& fromValue, const int& toValue);

    //获得下一个
    void controlNext();

    void measureCasesCountsInit(const std::vector<MonsbControl >& measureControls);

    inline const std::vector<MonsbControl >& getControls(const MonsbMeasure* oMonsb, int fromValue, int toValue)
    {
        return MonsbOperateManager::controls(oMonsb, fromValue, toValue);
    }

    inline const std::vector<MonsbControl >& getCancels(const MonsbMeasure* oMonsb, int fromValue, int toValue)
    {
        return MonsbOperateManager::cancels(oMonsb, fromValue, toValue);
    }

private:
    const MonsbMeasure* _measure;
    int _fromValue;
    int _toValue;

    const MonsbMeasure* _currentRunMeasure;
    int _currentRunFromValue;
    int _currentRunToValue;

    size_t _controlIndex;

    std::vector<MonsbControl > _controls;
    std::vector<MonsbControl > _cancels;
    std::map<const MonsbMeasure*, int> _measureCasesCounts;
    //状态
    int _state;

    friend class MonsbControlTaskManager;

};



class MonsbControlTaskManager
{
public:
    static const std::vector<MonsbControlTask*>* tasks();

    static MonsbControlTask * task(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    static MonsbControlTask * task(const MonsbMeasure* oMeasure, int toValue);

    static MonsbControlTask * firstTask();

    static MonsbControlTask * firstTask(ScheduleTaskStateEnum eState);

    static MonsbControlTask* firstTask(const MonsbMeasure* oMeasure);

    static MonsbControlTask * lastTask();

    static MonsbControlTask* lastTask(const MonsbMeasure* oMeasure);

    static int taskCount(ScheduleTaskStateEnum eState);

    static std::vector<std::string> reportTasks();

    static void start();

    static void stop();

//*接收任务
    static MonsbControlTask* receiveSelect(int iMeasureId , int fromValue, int toValue, int iSourceId,int iAddr);

    static MonsbControlTask* receiveSelect(const MonsbMeasure* oMeasure, int fromValue, int toValue, int iSourceId,int iAddr=0);

    static MonsbControlTask* receiveControl(int iMeasureId , int fromValue, int toValue, int iSourceId);

    static MonsbControlTask* receiveControl(int iMeasureId , int fromValue, int toValue, int iSourceId,int iAddr);

    static MonsbControlTask* receiveControl(const MonsbMeasure* oMeasure, int fromValue, int toValue, int iSourceId,int iAddr=0);

//取消任务
    static void cancelControls(int iSourceId);

    static bool cancelControls(int iSourceId,int iAddr);

    static void cancelControl(const MonsbMeasure* oMeasure, int iSourceId);

private:
    static void apendTask(MonsbControlTask* oTask);

    static void removeTask(MonsbControlTask* oTask);

    static void timeCheckTask(int iInterval);

    static void timeDoTask(int iInterval);

    static void taskStateChanged(ScheduleTaskBase* oSender, ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState);

    static void checkTask();

    static void doTask();

    static void outInfo(const std::string & sInfo);

};


#endif // MONSB_CONTROL_TASK_H
