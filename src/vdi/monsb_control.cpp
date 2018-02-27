#include "monsb_control.h"



MonsbOperateManager * fn_oMonsbOperateManager()
{
    static MonsbOperateManager m;
    return &m;
}

std::vector<MonsbOperate*> * fn_monsbOperates()
{
    static std::vector<MonsbOperate*> m;
    return &m;
}

static std::vector<MonsbOperate*> * f_monsbOperates = fn_monsbOperates();

static std::vector<MonsbControl> f_map_null;


const MonsbOperate *MonsbOperateManager::measureOperate(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    return getMeasureOperate(oMeasure, fromValue, toValue);
}

const std::vector<MonsbControl> & MonsbOperateManager::controls(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    MonsbOperate* oMeasureOperate = getMeasureOperate(oMeasure, fromValue, toValue);
    return (oMeasureOperate) ? oMeasureOperate->controls() : f_map_null;
}

const std::vector<MonsbControl> & MonsbOperateManager::cancels(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    MonsbOperate* oMeasureOperate = getMeasureOperate(oMeasure, fromValue, toValue);
    return (oMeasureOperate) ? oMeasureOperate->cancels() : f_map_null;
}


void MonsbOperateManager::setControls(const MonsbMeasure* oMeasure, int fromValue, int toValue, const std::vector<MonsbControl>& controls)
{
    if (controls.size() <= 0)
        return;
    MonsbOperate* oMeasureOperate = getMeasureOperate(oMeasure, fromValue, toValue);
    if (! oMeasureOperate)
    {
        oMeasureOperate = createMeasureOperate(oMeasure, fromValue, toValue);
    }
    oMeasureOperate->setControls(controls);
}

void MonsbOperateManager::setCancels(const MonsbMeasure* oMeasure, int fromValue, int toValue, const std::vector<MonsbControl>& cancels)
{
    if (cancels.size() <= 0)
        return;
    MonsbOperate* oMeasureOperate = getMeasureOperate(oMeasure, fromValue, toValue);
    if (! oMeasureOperate)
    {
        oMeasureOperate = createMeasureOperate(oMeasure, fromValue, toValue);
    }
    oMeasureOperate->setCancels(cancels);
}

const std::vector<MonsbOperate*>* MonsbOperateManager::measureOperates()
{
    return f_monsbOperates;
}

std::vector<MonsbOperate*> MonsbOperateManager::measureOperates(int iMeasureSonType)
{
    std::vector<MonsbOperate*> measureOperates;
    const std::vector<MonsbOperate*>* oMeasureOperates = f_monsbOperates;
    for (size_t i = 0; i < oMeasureOperates->size(); ++i)
    {
        MonsbOperate* oMeasureOperate = oMeasureOperates->at(i);
        if (oMeasureOperate->measure()->measureSonType() == iMeasureSonType)
        {
            measureOperates.push_back(oMeasureOperate);
        }
    }
    return measureOperates;
}

//*保存成文件
void MonsbOperateManager::saveToFile(const std::string& sFilePath)
{
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath, false);
    if (oProfile)
    {
        std::vector<MonsbOperate*>* oMeasureOperates = f_monsbOperates;
        for (size_t i = 0; i < oMeasureOperates->size(); ++i)
        {
            MonsbOperate* oMeasureOperate = oMeasureOperates->at(i);
            oProfile->prepareSet(CxString::format("%06d",i));
            const MonsbMeasure* oMeasure = oMeasureOperate->measure();
            int fromValue = oMeasureOperate->fromValue();
            int toValue = oMeasureOperate->toValue();
            const std::vector<MonsbControl>& controls = oMeasureOperate->controls();
            const std::vector<MonsbControl>& cancels = oMeasureOperate->cancels();
            oProfile->setValue(CS_EntryMeasureId, oMeasure->measureId());
            oProfile->setValue(CS_EntryFromValue, fromValue);
            oProfile->setValue(CS_EntryToValue, toValue);
            oProfile->setValue(CS_EntryMeasureControls, measureControlsToString(controls));
            oProfile->setValue(CS_EntryMeasureCancels, measureControlsToString(cancels));
        }
        CxSkverManager::endUse(oProfile, true);
    }
}

//*从文件加载
void MonsbOperateManager::loadFromFile(const std::string& sFilePath)
{
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile)
    {
        std::vector<std::string> sSections = oProfile->sectionNames();
        for (size_t i = 0; i < sSections.size(); ++i)
        {
            oProfile->prepareGet(sSections.at(i));
            int iMeasureId = -1;
            int fromValue = 0;
            int toValue = 1;
            std::string sControls;
            std::string sCancels;
            iMeasureId = oProfile->getValue(CS_EntryMeasureId, iMeasureId);
            fromValue = oProfile->getValue(CS_EntryFromValue, fromValue);
            toValue = oProfile->getValue(CS_EntryToValue, toValue);
            sControls = oProfile->getValue(CS_EntryMeasureControls, sControls);
            sCancels = oProfile->getValue(CS_EntryMeasureCancels, sCancels);
            const MonsbMeasure* oMeasure = MeasureManagerTemplate<MonsbMeasure, int>::measure(iMeasureId);
            if (oMeasure)
            {
                std::vector<MonsbControl> controls = measureControlsFromString(sControls);
                std::vector<MonsbControl> cancels = measureControlsFromString(sCancels);
                setControls(oMeasure, fromValue, toValue, controls);
                setCancels(oMeasure, fromValue, toValue, cancels);
            }
        }
        CxSkverManager::endUse(oProfile);
    }
}

void MonsbOperateManager::initializeProcess()
{
    MonsbManager::addObserver(fn_oMonsbOperateManager());
}

void MonsbOperateManager::measures_prepareChange(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iMeasureSonType)
{
    initMeasureOperates(measuresChangeType, iMeasureSonType);
}

void MonsbOperateManager::measures_afterLoad(const std::vector<const MonsbMeasure*>& measures)
{
    initMeasureOperates(measures);
}

MonsbOperate* MonsbOperateManager::createMeasureOperate(const MonsbMeasure* oMeasure, int fromValue, int toValue)
{
    MonsbOperate* oMeasureOperate = new MonsbOperate();
    oMeasureOperate->setMeasure(oMeasure);
    oMeasureOperate->SetFromValue(fromValue);
    oMeasureOperate->setToValue(toValue);
    f_monsbOperates->push_back(oMeasureOperate);
    return oMeasureOperate;
}

//*MeasureControlTemplate （子项）单项操作转成文本
std::string MonsbOperateManager::measureControlsToString(const std::vector<MonsbControl>& controls)
{
    std::vector<std::string> sMeasureControls;
    for (size_t i = 0; i < controls.size(); ++i)
    {
        const MonsbControl& oMonsbControl = controls.at(i);
        std::string s = CxString::format("%d`%d`%d`%d`%d`%s",oMonsbControl.measure->measureId(), oMonsbControl.fromValue, oMonsbControl.toValue, oMonsbControl.runType, oMonsbControl.waitLong, oMonsbControl.extension.c_str());
        sMeasureControls.push_back(s);
    }
    return CxString::join(sMeasureControls, '|');
}

//*MeasureControlTemplate （子项）单项操作从文本转出
std::vector<MonsbControl> MonsbOperateManager::measureControlsFromString(const std::string& sEntry)
{
    std::vector<MonsbControl> measureControls;
    std::vector<std::string> sMeasureControls = CxString::split(sEntry, '|');
    for (size_t i = 0; i < sMeasureControls.size(); ++i)
    {
        std::string sMeasureControl = sMeasureControls.at(i);
        std::string sMeasureId = CxString::token(sMeasureControl, '`');
        std::string sFromValue = CxString::token(sMeasureControl, '`');
        std::string sToValue   = CxString::token(sMeasureControl, '`');
        std::string sRunType   = CxString::token(sMeasureControl, '`');
        std::string sWaitLong  = CxString::token(sMeasureControl, '`');
        std::string sExtension = sMeasureControl;
        int iMeasureId = CxString::fromString(sMeasureId, ci_int_minus_one);
        int tFromValue = CxString::fromString(sFromValue, ci_int_zero);
        int tToValue = CxString::fromString(sMeasureId, ci_int_zero);
        int iRunType = CxString::toInt32(sRunType);
        int iWaitLong= CxString::toInt32(sWaitLong);
        const MonsbMeasure* oMeasure = MeasureManagerTemplate<MonsbMeasure, int>::measure(iMeasureId);
        if (oMeasure)
        {
            MonsbControl measureControl(oMeasure, tFromValue, tToValue, iRunType, iWaitLong, sExtension);
            measureControls.push_back(measureControl);
        }
    }
    return measureControls;
}

//*Measure变化后初始操作列表（变化包括：measure的删除、新增）
void MonsbOperateManager::initMeasureOperates(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iMeasureSonType)
{
    std::vector<const MonsbMeasure*> measuresDelete = CxContainer::keys(measuresChangeType, MeasureChangeType_Delete);
    for(std::vector<MonsbOperate*>::iterator it = f_monsbOperates->begin(); it != f_monsbOperates->end();)
    {
        MonsbOperate * oAction = (* it);
        if (std::find(measuresDelete.begin(), measuresDelete.end(), oAction->measure()) != measuresDelete.end())
        {
            delete oAction;
            f_monsbOperates->erase(it);
            continue;
        }
        ++it;
    }

    std::vector<const MonsbMeasure*> measuresNew = CxContainer::keys(measuresChangeType, MeasureChangeType_New);
    for (size_t i = 0; i < measuresNew.size(); ++i)
    {
        const MonsbMeasure* oMeasure = measuresNew.at(i);
        initMeasureControl(oMeasure, *f_monsbOperates);
        InitMeasureCancel(oMeasure, *f_monsbOperates);
    }
}

void MonsbOperateManager::initMeasureOperates(const std::vector<const MonsbMeasure*>& measures)
{
    deleteAllControls();
    for (size_t i = 0; i < measures.size(); ++i)
    {
        const MonsbMeasure* oMeasure = measures.at(i);
        initMeasureControl(oMeasure, *f_monsbOperates);
        InitMeasureCancel(oMeasure, *f_monsbOperates);
    }
}

void MonsbOperateManager::deleteAllControls()
{
    GM_PLISTO_DELETEALL(f_monsbOperates, MonsbOperate)
}

MonsbOperate *MonsbOperateManager::getMeasureOperate(const MonsbMeasure *oMeasure, int fromValue, int toValue)
{
    std::vector<MonsbOperate*>* oMeasureOperates = f_monsbOperates;
    for (size_t i = 0; i < oMeasureOperates->size(); ++i)
    {
        MonsbOperate* oMeasureOperate = oMeasureOperates->at(i);
        if (oMeasureOperate->measure() == oMeasure && oMeasureOperate->fromValue() == fromValue && oMeasureOperate->toValue() == toValue)
        {
            return oMeasureOperate;
        }
    }
    return NULL;
}

void MonsbOperateManager::initMeasureControl(const MonsbMeasure *oMeasure, std::vector<MonsbOperate *> &measureOperates)
{
    //open
    MonsbOperate* oOpen = createMeasureOperate(oMeasure, 0, 1);
    ///default control
    std::vector<MonsbControl> openControls;
    MonsbControl openControl(oMeasure, 0, 1, 0, 0, "");
    openControls.push_back(openControl);
    oOpen->setControls(openControls);

    //close
    MonsbOperate* oClose = createMeasureOperate(oMeasure, 1, 0);
    ///default control
    std::vector<MonsbControl> closeControls;
    MonsbControl  closeControl(oMeasure, 1, 0, 0, 0, "");
    closeControls.push_back(closeControl);
    oClose->setControls(closeControls);
}

void MonsbOperateManager::InitMeasureCancel(const MonsbMeasure *oMeasure, std::vector<MonsbOperate *> &measureCancels)
{
}
