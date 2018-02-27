#include "straw_measure.h"




std::vector<IStrawManagerSubject*> * fn_oStrawManagerSubjects()
{
    static std::vector<IStrawManagerSubject*> m;
    return &m;
}

std::map<const StrawMeasure*, std::string> * fn_oValueChangedStrawOldValues()
{
    static std::map<const StrawMeasure*, std::string> m;
    return &m;
}


static int f_currentMaxStrawId = ci_minMeasureIdStraw;
static std::vector<IStrawManagerSubject*> * f_oStrawManagerSubjects = fn_oStrawManagerSubjects();
static std::map<const StrawMeasure*, std::string> * f_oValueChangedStrawOldValues = fn_oValueChangedStrawOldValues();
static int f_valueChangeStrawType = -1;
static int f_valueChangeSourceId = -1;
static int f_valueChangeTag = -1;
static bool f_valueChanging = false;


std::vector<StrawMeasure *> *StrawMeasure::measuresContainer()
{
    static std::vector<StrawMeasure *> m;
    return &m;
}

int StrawMeasure::currentMaxMeasureId()
{
    return f_currentMaxStrawId;
}

void StrawMeasure::setCurrentMaxMeasureId(int value)
{
    f_currentMaxStrawId = value;
}

void StrawMeasure::beginSetValues(int iStrawType, int iSourceId, int iTag)
{
    f_oValueChangedStrawOldValues->clear();
    f_valueChangeStrawType = iStrawType;
    f_valueChangeSourceId = iSourceId;
    f_valueChangeTag = iTag;
    f_valueChanging = true;
}

bool StrawMeasure::isSettingValues()
{
    return f_valueChanging;
}

void StrawMeasure::valuesChanged(StrawMeasure* o, const std::string & oldValue)
{
    (*f_oValueChangedStrawOldValues)[o] = oldValue;
}

void StrawMeasure::endSetValues()
{
    f_valueChanging = false;
    if (f_oValueChangedStrawOldValues->size()>0)
    {
        raiseValuesChanged();
    }
}

void StrawMeasure::raiseMeasuresPrepareChange(const std::map<const StrawMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iStrawType)
{
    for (size_t i = 0; i < f_oStrawManagerSubjects->size(); ++i)
    {
        IStrawManagerSubject* oSubject = f_oStrawManagerSubjects->at(i);
        oSubject->measures_prepareChange(measuresChangeType, iStrawType);
    }
}

void StrawMeasure::raiseMeasuresAfterLoad(const std::vector<const StrawMeasure*>& measures)
{
    for (size_t i = 0; i < f_oStrawManagerSubjects->size(); ++i)
    {
        IStrawManagerSubject * oSubject = f_oStrawManagerSubjects->at(i);
        oSubject->measures_afterLoad(measures);
    }
}

void StrawMeasure::raiseValuesChanged()
{
    for (size_t i = 0; i < f_oStrawManagerSubjects->size(); ++i)
    {
        IStrawManagerSubject* oSubject = f_oStrawManagerSubjects->at(i);
        oSubject->measures_valueChanged(*f_oValueChangedStrawOldValues, f_valueChangeStrawType, f_valueChangeSourceId, f_valueChangeTag);
    }
}









StrawMeasure::StrawMeasure()
{
    _strawType = StrawType_0;
    _publisher = new StrawPublisher();
}

StrawMeasure::~StrawMeasure()
{
    delete _publisher;
}

void StrawMeasure::toContext(CxISetSkv &context) const
{
    MeasureTemplate<std::string>::toContext(context);
    int iStrawTypeValue = _strawType;
    context.setValue(CS_EntryStrawType, iStrawTypeValue);
}

void StrawMeasure::fromContext(const CxIGetSkv &context)
{
    MeasureTemplate<std::string>::fromContext(context);
    int iStrawTypeValue = _strawType;
    iStrawTypeValue     =      context.getValue(CS_EntryStrawType, iStrawTypeValue);
    _strawType = g_fn_strawTypeEnum(iStrawTypeValue);
}

bool StrawMeasure::valueCanChange(const std::string & newValue)
{
    return _publisher->valueCanChange(this, newValue);
}

void StrawMeasure::valueChanging(const std::string & newValue)
{
    _publisher->valueChanging(this, newValue);
}

bool StrawMeasure::valueIsChange(const std::string &oldValue, const std::string &newValue)
{
    return (oldValue==newValue)?false:true;
}

void StrawMeasure::valueChanged(const std::string & oldValue, const std::string & newValue)
{
    _publisher->valueChanged(this, oldValue, newValue);
    if ( isSettingValues() )
        valuesChanged(this, oldValue);
}




//*StrawManager

void StrawManager::addObserver(IStrawManagerSubject *oObserver)
{
    fn_oStrawManagerSubjects()->push_back(oObserver);
}

void StrawManager::removeObserver(IStrawManagerSubject *oObserver)
{
    CxContainer::remove(* f_oStrawManagerSubjects, oObserver);
}

std::string StrawManager::describeDefaultValue(CxDescribeUnit *oDescribeUnit, const CxDescribeBase *oDescribe, int iRowIndex)
{
    if (oDescribe->name() == CS_EntryMeasureName) {
        CxDescribeEnum* oStrawTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryStrawType);
        if (oStrawTypeDescribe)
        {
            StrawTypeEnum eStrawType = (StrawTypeEnum)oStrawTypeDescribe->value();
            std::string sStrawTypeName = g_fn_strawTypeName(eStrawType);
            std::string sRowIndex = CxString::format("%06d",iRowIndex);
            return sStrawTypeName + sRowIndex;
        }
    }
    return "";
}

void StrawManager::describeSelf(CxDescribeUnit *oDescribeUnit)
{
    MeasureManagerTemplate<StrawMeasure, std::string>::describeSelf(oDescribeUnit);
    CxDescribeString* oMinDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryMinValue);
    oMinDescribe->initValue("");
    CxDescribeString* oMaxDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryMaxValue);
    oMaxDescribe->initValue("");
    CxDescribeString* oDefaultDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryDefaultValue);
    oDefaultDescribe->initValue("");
    CxDescribeEnum* oStrawTypeDescribe = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryStrawType);
    oStrawTypeDescribe->setIsReadOnly(true);
    oStrawTypeDescribe->setEnumCopeFromZero(ce_maxStrawType);
    oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
}
