
#include "monsb_control_task.h"

static const int g_iRunTaskInterval = 300;
static const int g_iCheckTaskInterval = 1000;
static std::string cs_monsbControlTaskInfoTitle = "MonsbControlTask";

std::vector<MonsbControlTask *> * fn_tasksContainer()
{
    static std::vector<MonsbControlTask *> m;
    return &m;
}

MonsbControlTask::MonsbControlTask()
{
    _measure = NULL;
    _controlIndex = CxGlobal::npos;
    _currentRunMeasure = NULL;
}

MonsbControlTask::~MonsbControlTask()
{
}

std::string MonsbControlTask::taskDesc() const
{
    if (_measure)
        return _measure->measureName() + " : " + CxString::toString(_fromValue) + " - " + CxString::toString(_toValue);
    else
        return "NULL";
}

void MonsbControlTask::control(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    _measure = oMeasure;
    _fromValue = fromValue;
    _toValue = toValue;
    _controls = getControls(oMeasure, fromValue, toValue);
    _cancels = getCancels(oMeasure, fromValue, toValue);
    _controlIndex = CxGlobal::npos;
    measureCasesCountsInit(_controls);
    measureCasesCountsInit(_cancels);
    controlNext();
    if (_currentRunMeasure)
    {
        run();
    }
}

void MonsbControlTask::select(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    _measure = oMeasure;
    _fromValue = fromValue;
    _toValue = toValue;
    _controls = getControls(oMeasure, fromValue, toValue);
    _cancels = getCancels(oMeasure, fromValue, toValue);
    _controlIndex = CxGlobal::npos;
    measureCasesCountsInit(_controls);
    measureCasesCountsInit(_cancels);
    if (_currentRunMeasure)
    {
        run();
    }
}

void MonsbControlTask::runImpl()
{
    if (_currentRunMeasure)
    {
        //if has measure's cases and not finish
        if (needToControl(_currentRunMeasure, _currentRunFromValue, _currentRunToValue))
        {
            SendMonsbControlManager::sendControl(_currentRunMeasure, _currentRunFromValue, _currentRunToValue, sourceId());
        } else {
            controlNext();
            if (_currentRunMeasure) {
                //if has measure's cases and not finish
                if (needToControl(_currentRunMeasure, _currentRunFromValue, _currentRunToValue))
                {
                    SendMonsbControlManager::sendControl(_currentRunMeasure, _currentRunFromValue, _currentRunToValue, sourceId());
                } else {
                    controlNext();
                }
            }
            else
            {
                finish();
            }
        }
    } else {
        finish();
    }
}

void MonsbControlTask::cannelImpl()
{
    for (size_t i = 0; i < _cancels.size(); ++i) {
        const MonsbControl& cancel = _cancels[i];
        const MonsbMeasure* oMeasure = cancel.measure;
        int fromValue = cancel.fromValue;
        int toValue = cancel.toValue;
        SendMonsbControlManager::sendControl(oMeasure, fromValue, toValue, sourceId());
    }
}

void MonsbControlTask::toReport(std::vector<std::string> &sReports) const
{
    std::string sMeasureName = (_measure) ? _measure->measureName() : "";
    std::string sCurrentMeasureName = (_currentRunMeasure) ? _currentRunMeasure->measureName() : "";
    sReports.push_back(std::string("Measure_Name=" +              CxString::toString(sMeasureName)));
    sReports.push_back(std::string("FromValue=" +                 CxString::toString(_fromValue)));
    sReports.push_back(std::string("ToValue=" +                   CxString::toString(_toValue)));
    sReports.push_back(std::string("Current_run_measure_Name=" +  CxString::toString(sCurrentMeasureName)));
    sReports.push_back(std::string("Current_run_fromValue=" +     CxString::toString(_currentRunFromValue)));
    sReports.push_back(std::string("Current_run_toValue=" +       CxString::toString(_currentRunToValue)));
    sReports.push_back(std::string("Controls_Count=" +            CxString::toString(_controls.size())));
    sReports.push_back(std::string("Cancels_Count=" +             CxString::toString(_cancels.size())));
    sReports.push_back(std::string("Measure_CasesCounts_Count=" + CxString::toString(_measureCasesCounts.size())));
}

bool MonsbControlTask::needToControl(const MonsbMeasure *oMeasure, const int &fromValue, const int &toValue)
{sourceId();
    std::map<const MonsbMeasure*, int>::const_iterator it = _measureCasesCounts.find(oMeasure);
    if (it != _measureCasesCounts.end())
    {
        int iCasesCount = it->second;
        if (iCasesCount <= 0)
            return false;
    }
    //if finish
    return ( oMeasure && oMeasure->value() != toValue) ? true : false ;
}

void MonsbControlTask::controlNext()
{
    ++ _controlIndex;
    if (_controlIndex < _controls.size())
    {
        const MonsbControl& control = _controls[_controlIndex];
        _currentRunMeasure = control.measure;
        _currentRunFromValue = control.fromValue;
        _currentRunToValue = control.toValue;
    }
    else
    {
        _currentRunMeasure = NULL;
    }
}

void MonsbControlTask::measureCasesCountsInit(const std::vector<MonsbControl> &measureControls)
{
    for (size_t i = 0; i < measureControls.size(); ++i) {
        const MonsbControl& measureControl = measureControls.at(i);
        const MonsbMeasure* oMeasure = measureControl.measure;
        if (_measureCasesCounts.find(oMeasure) == _measureCasesCounts.end())
        {
            int iCasesCounts = SendMonsbControlManager::statisticseMeasureCasesCount(oMeasure, sourceId());
            _measureCasesCounts[oMeasure] = iCasesCounts;
        }
    }
}











//*MonsbControlTaskManager

const std::vector<MonsbControlTask *> * MonsbControlTaskManager::tasks()
{
    return fn_tasksContainer();
}

MonsbControlTask *MonsbControlTaskManager::task(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    for (size_t i = 0; i < oMeasureControlTasks->size(); ++i) {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if ( oTask->measure() == oMeasure && oTask->fromValue() == fromValue && oTask->toValue() == toValue ){
            return oTask;
        }
    }
    return NULL;
}

MonsbControlTask *MonsbControlTaskManager::task(const MonsbMeasure *oMeasure, int toValue)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    for (size_t i = 0; i < oMeasureControlTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if ( oTask->measure() == oMeasure && oTask->toValue() == toValue )
        {
            return oTask;
        }
    }
    return NULL;
}

void MonsbControlTaskManager::start()
{
    CxTimerManager::startTimer(timeCheckTask, g_iCheckTaskInterval);
    CxTimerManager::startTimer(timeDoTask, g_iRunTaskInterval);
}

void MonsbControlTaskManager::stop()
{
    CxTimerManager::stopTimer(timeCheckTask, g_iCheckTaskInterval);
    CxTimerManager::stopTimer(timeDoTask, g_iRunTaskInterval);
}

MonsbControlTask *MonsbControlTaskManager::receiveSelect(int iMeasureId, int fromValue, int toValue, int iSourceId, int iAddr)
{
    const MonsbMeasure* oMeasure = MonsbManager::measure(iMeasureId);
    if (oMeasure)
    {
        return receiveControl(oMeasure, fromValue, toValue, iSourceId,iAddr);
    }
    else
    {
        std::string sInfo = CxString::format("receiveControl measure id : From - To [%d : %s - %s], But the id is not exist .", iMeasureId, CxString::toString(fromValue).c_str(), CxString::toString(toValue).c_str());
        outInfo(sInfo);
        return NULL;
    }
}

MonsbControlTask *MonsbControlTaskManager::receiveSelect(const MonsbMeasure *oMeasure, int fromValue, int toValue, int iSourceId, int iAddr)
{
    std::string sInfo = CxString::format("receiveControl measure Name : From - To [%s : %s - %s]", oMeasure->measureName().c_str(), CxString::toString(fromValue).c_str(), CxString::toString(toValue).c_str());
    if (! oMeasure->isValidValue(fromValue))
    {
        sInfo += sInfo + ", But fromValue is invalid";
        outInfo(sInfo);
        return NULL;
    }
    if (! oMeasure->isValidValue(toValue))
    {
        outInfo(sInfo + ", But toValue is invalid");
        return NULL;
    }
    if (oMeasure->value() == toValue)
    {
        outInfo(sInfo +", But toValue is ok");
        return NULL;
    }
    MonsbControlTask* oTask = task(oMeasure, fromValue, toValue);
    if (oTask)
    {
        if ( oTask->taskState() == ScheduleTaskState_Suspend || oTask->taskState() == ScheduleTaskState_Running)
        {
            oTask->reset();
        }
        else
        {
            outInfo(sInfo + ", But MonsbControl task Had Exist , State : " + CxString::toString(oTask->taskState()));
        }
    }
    else
    {
        oTask = new MonsbControlTask();
        oTask->setOverInterval(5000);
        oTask->setSourceId(iSourceId);
        oTask->setId(iAddr);
        apendTask(oTask);
        oTask->control(oMeasure, fromValue, toValue);
        outInfo(sInfo + std::string(", Append and task Count : " + CxString::toString(fn_tasksContainer()->size())));
    }
    return oTask;
}

MonsbControlTask *MonsbControlTaskManager::receiveControl(int iMeasureId, int fromValue, int toValue, int iSourceId)
{
    const MonsbMeasure* oMeasure = MonsbManager::measure(iMeasureId);
    if (oMeasure)
    {
        return receiveControl(oMeasure, fromValue, toValue, iSourceId);
    }
    else
    {
        std::string sInfo = CxString::format("receiveControl measure id : From - To [%d : %s - %s], But the id is not exist .", iMeasureId, CxString::toString(fromValue).c_str(), CxString::toString(toValue).c_str());
        outInfo(sInfo);
        return NULL;
    }
}

MonsbControlTask *MonsbControlTaskManager::receiveControl(int iMeasureId, int fromValue, int toValue, int iSourceId, int iAddr)
{
    const MonsbMeasure* oMeasure = MonsbManager::measure(iMeasureId);
    if (oMeasure)
    {
        return receiveControl(oMeasure, fromValue, toValue, iSourceId,iAddr);
    }
    else
    {
        std::string sInfo = CxString::format("receiveControl measure id : From - To [%d : %s - %s], But the id is not exist .", iMeasureId, CxString::toString(fromValue).c_str(), CxString::toString(toValue).c_str());
        outInfo(sInfo);
        return NULL;
    }
}

MonsbControlTask *MonsbControlTaskManager::receiveControl(const MonsbMeasure *oMeasure, int fromValue, int toValue, int iSourceId, int iAddr)
{
    std::string sInfo = CxString::format("receiveControl measure Name : From - To [%s : %s - %s], But the id is not exist .", oMeasure->measureName().c_str(), CxString::toString(fromValue).c_str(), CxString::toString(toValue).c_str());
    if (! oMeasure->isValidValue(fromValue))
    {
        sInfo += sInfo + ", But fromValue is invalid";
        outInfo(sInfo);
        return NULL;
    }
    if (! oMeasure->isValidValue(toValue))
    {
        outInfo(sInfo + ", But toValue is invalid");
        return NULL;
    }
    if (oMeasure->value() == toValue)
    {
        outInfo(sInfo +", But toValue is ok");
        return NULL;
    }
    MonsbControlTask* oTask = task(oMeasure, fromValue, toValue);
    if (oTask) {
        if ( oTask->taskState() == ScheduleTaskState_Suspend || oTask->taskState() == ScheduleTaskState_Running)
        {
            oTask->reset();
        } else {
            outInfo(sInfo + std::string(", But MonsbControl task Had Exist , State : ") + CxString::toString(oTask->taskState()));
        }
    }
    else
    {
        oTask = new MonsbControlTask();
        oTask->setOverInterval(5000);
        oTask->setSourceId(iSourceId);
        oTask->setId(iAddr);
        apendTask(oTask);
        oTask->control(oMeasure, fromValue, toValue);
        outInfo(sInfo + std::string(", Append and task Count : ") + CxString::toString(fn_tasksContainer()->size()));
    }
    return oTask;
}

void MonsbControlTaskManager::cancelControls(int iSourceId)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    for (size_t i = 0; i < oMeasureControlTasks->size(); ++i) {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if (oTask && oTask->sourceId() == iSourceId) {
            oTask->cancel();
        }
    }
    outInfo(std::string("cancelControls by Source id : ") + CxString::toString(iSourceId));
}

bool MonsbControlTaskManager::cancelControls(int iSourceId, int iAddr)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    bool bRet = false;
    for (size_t i = 0; i < oMeasureControlTasks->size(); ++i) {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if (oTask && oTask->sourceId() == iSourceId && oTask->id()== iAddr)
        {
            oTask->cancel();
            bRet = true;
        }
    }
    outInfo(CxString::format("cancelControls by Source id : %d  Addr:%d",iSourceId,iAddr));
    return bRet;
}

void MonsbControlTaskManager::cancelControl(const MonsbMeasure *oMeasure, int iSourceId)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    for (size_t i = 0; i < oMeasureControlTasks->size(); ++i) {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if ( oTask->measure() == oMeasure ){
            oTask->cancel();
        }
    }
    outInfo(std::string("cancel control by Source id : ") + CxString::toString(iSourceId));
}

std::vector<std::string> MonsbControlTaskManager::reportTasks()
{
    std::vector<MonsbControlTask*> * oTasks = fn_tasksContainer();
    std::vector<std::string> sReports;
    sReports.push_back(CxString::format("*** Monsb Control Task Manager report [ count=%d ] ***", oTasks->size()));
    for (size_t i = 0; i < oTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oTasks->at(i);
        std::vector<std::string> sReport = oTask->reportSelf();
        sReports.insert(sReports.end(), sReport.begin(), sReport.end());
        sReports.push_back("");
    }
    return sReports;
}

int MonsbControlTaskManager::taskCount(ScheduleTaskStateEnum eState)
{
    std::vector<MonsbControlTask*> * oTasks = fn_tasksContainer();
    int iCount = 0;
    for (size_t i = 0; i < oTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oTasks->at(i);
        if (oTask->taskState() == eState)
            iCount ++;
    }
    return iCount;
}

MonsbControlTask * MonsbControlTaskManager::firstTask(ScheduleTaskStateEnum eState)
{
    std::vector<MonsbControlTask*> * oTasks = fn_tasksContainer();
    for (size_t i = 0; i < oTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oTasks->at(i);
        if (oTask->taskState() == eState)
        {
            return oTask;
        }
    }
    return NULL;
}

MonsbControlTask * MonsbControlTaskManager::firstTask()
{
    std::vector<MonsbControlTask*> * oTasks = fn_tasksContainer();
    if (oTasks->size()>0)
    {
        return oTasks->at(0);
    }
    return NULL;
}

MonsbControlTask * MonsbControlTaskManager::lastTask()
{
    std::vector<MonsbControlTask*> * oTasks = fn_tasksContainer();
    if (oTasks->size()>0)
    {
        return oTasks->at(oTasks->size()-1);
    }
    return NULL;
}

MonsbControlTask *MonsbControlTaskManager::firstTask(const MonsbMeasure *oMeasure)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    for (size_t i = 0; i < oMeasureControlTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if ( oTask->measure() == oMeasure )
        {
            return oTask;
        }
    }
    return NULL;
}

MonsbControlTask *MonsbControlTaskManager::lastTask(const MonsbMeasure *oMeasure)
{
    const std::vector<MonsbControlTask*>* oMeasureControlTasks = fn_tasksContainer();
    for (int i = oMeasureControlTasks->size()-1; i >= 0; --i)
    {
        MonsbControlTask* oTask = oMeasureControlTasks->at(i);
        if ( oTask->measure() == oMeasure )
        {
            return oTask;
        }
    }
    return NULL;
}


void MonsbControlTaskManager::apendTask(MonsbControlTask* oTask)
{
    fn_tasksContainer()->push_back(oTask);
    oTask->setFuncTaskStateChanged(&taskStateChanged);
}

void MonsbControlTaskManager::removeTask(MonsbControlTask* oTask)
{
    std::vector<MonsbControlTask*> * oTasks = fn_tasksContainer();
    oTask->setFuncTaskStateChanged(0);
    CxContainer::remove(*oTasks, oTask);
 }

void MonsbControlTaskManager::timeCheckTask(int iInterval)
{
    checkTask();
}

void MonsbControlTaskManager::timeDoTask(int iInterval)
{
    doTask();
}

void MonsbControlTaskManager::taskStateChanged(ScheduleTaskBase* oSender, ScheduleTaskStateEnum iOldState, ScheduleTaskStateEnum iNewState)
{
    //besttodo
}

void MonsbControlTaskManager::checkTask()
{
    std::vector<MonsbControlTask*>* oTasks = fn_tasksContainer();
    for (int i = oTasks->size()-1; i >= 0; --i)
    {
        MonsbControlTask* oTask = oTasks->at(i);
        if (oTask->taskState() == ScheduleTaskState_Finish || oTask->taskState() == ScheduleTaskState_Cancel || oTask->taskState() == ScheduleTaskState_Error)
        {
            removeTask(oTask);
            delete oTask;
        }
    }
}

void MonsbControlTaskManager::doTask()
{
    const std::vector<MonsbControlTask*>* oTasks = fn_tasksContainer();
    for (size_t i = 0; i < oTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oTasks->at(i);
        oTask->doIt();
    }
}

void MonsbControlTaskManager::outInfo(const std::string &sInfo)
{
    CxInterinfoOut::outInfo(sInfo, cs_monsbControlTaskInfoTitle, CxInterinfo::Type_Task);
}
