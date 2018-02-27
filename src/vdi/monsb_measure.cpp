#include "monsb_measure.h"

/*
  don't delete the code, it's compile complete.

template<>
ObjectBatchPublisherTemplate<MonsbMeasure>* MeasureManagerTemplate<MonsbMeasure>::batchPublisher = NULL;

template<>
std::vector<MonsbMeasure*>* MeasureManagerTemplate<MonsbMeasure>::measures = NULL;

*/


std::vector<IMonsbManagerSubject*> * fn_oMonsbManagerSubjects()
{
    static std::vector<IMonsbManagerSubject*> m;
    return &m;
}

std::map<const MonsbMeasure*, int> * fn_oValueChangedMonsbOldValues()
{
    static std::map<const MonsbMeasure*, int> m;
    return &m;
}


static int f_currentMaxMonsbId = ci_minMeasureIdMonsb;
static std::vector<IMonsbManagerSubject*> * f_oMonsbManagerSubjects = fn_oMonsbManagerSubjects();
static std::map<const MonsbMeasure*, int> * f_oValueChangedMonsbOldValues = fn_oValueChangedMonsbOldValues();
static int f_valueChangeMonsbType = -1;
static int f_valueChangeSourceId = -1;
static int f_valueChangeTag = -1;
static bool f_valueChanging = false;


//*static measures

std::vector<MonsbMeasure *> *MonsbMeasure::measuresContainer()
{
    static std::vector<MonsbMeasure *> m;
    return &m;
}

int MonsbMeasure::currentMaxMeasureId()
{
    return f_currentMaxMonsbId;
}

void MonsbMeasure::setCurrentMaxMeasureId(int value)
{
    f_currentMaxMonsbId = value;
}

void MonsbMeasure::beginSetValues(int iMonsbType, int iSourceId, int iTag)
{
    f_oValueChangedMonsbOldValues->clear();
    f_valueChangeMonsbType = iMonsbType;
    f_valueChangeSourceId = iSourceId;
    f_valueChangeTag = iTag;
    f_valueChanging = true;
}

bool MonsbMeasure::isSettingValues()
{
    return f_valueChanging;
}

void MonsbMeasure::valuesChanged(MonsbMeasure* o, int oldValue)
{
    (*f_oValueChangedMonsbOldValues)[o] = oldValue;
}

void MonsbMeasure::endSetValues()
{
    f_valueChanging = false;
    if (f_oValueChangedMonsbOldValues->size()>0)
    {
        raiseValuesChanged();
    }
}

void MonsbMeasure::raiseMeasuresPrepareChange(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iMonsbType)
{
    for (size_t i = 0; i < f_oMonsbManagerSubjects->size(); ++i)
    {
        IMonsbManagerSubject* oSubject = f_oMonsbManagerSubjects->at(i);
        oSubject->measures_prepareChange(measuresChangeType, iMonsbType);
    }
}

void MonsbMeasure::raiseMeasuresAfterLoad(const std::vector<const MonsbMeasure*>& measures)
{
    for (size_t i = 0; i < f_oMonsbManagerSubjects->size(); ++i)
    {
        IMonsbManagerSubject * oSubject = f_oMonsbManagerSubjects->at(i);
        oSubject->measures_afterLoad(measures);
    }
}

void MonsbMeasure::raiseValuesChanged()
{
    for (size_t i = 0; i < f_oMonsbManagerSubjects->size(); ++i)
    {
        IMonsbManagerSubject* oSubject = f_oMonsbManagerSubjects->at(i);
        oSubject->measures_valueChanged(*f_oValueChangedMonsbOldValues, f_valueChangeMonsbType, f_valueChangeSourceId, f_valueChangeTag);
    }
}







//*MonsbMeasure

MonsbMeasure::MonsbMeasure()
{
    _monsbType = MonsbType_0;
    _publisher = new MonsbPublisher();
}

MonsbMeasure::~MonsbMeasure()
{
    delete _publisher;
}

void MonsbMeasure::toContext(CxISetSkv &context) const
{
    MeasureTemplate<int>::toContext(context);
    int iMonsbTypeValue = _monsbType;
    context.setValue(CS_EntryMonsbType, iMonsbTypeValue);

    context.setValue("saveType", m_nSaveType);
    context.setValue("warnType", m_nWarnType);
}

void MonsbMeasure::fromContext(const CxIGetSkv &context)
{
    MeasureTemplate<int>::fromContext(context);
    int iMonsbTypeValue = _monsbType;
    iMonsbTypeValue =      context.getValue(CS_EntryMonsbType, iMonsbTypeValue);
    _monsbType = g_fn_monsbTypeEnum(iMonsbTypeValue);

    m_nSaveType = context.getValue("saveType", m_nSaveType);
    m_nWarnType = context.getValue("warnType", m_nWarnType);
}

bool MonsbMeasure::valueCanChange(const int & newValue)
{
    return _publisher->valueCanChange(this, newValue);
}

void MonsbMeasure::valueChanging(const int & newValue)
{
    _publisher->valueChanging(this, newValue);
}

bool MonsbMeasure::valueIsChange(const int &oldValue, const int &newValue)
{
    return (oldValue==newValue)?false:true;
}

void MonsbMeasure::valueChanged(const int & oldValue, const int & newValue)
{
    _publisher->valueChanged(this, oldValue, newValue);
    if ( isSettingValues() )
        valuesChanged(this, oldValue);
}



//*MonsbManager

void MonsbManager::addObserver(IMonsbManagerSubject *oObserver)
{
    fn_oMonsbManagerSubjects()->push_back(oObserver);
}

void MonsbManager::removeObserver(IMonsbManagerSubject *oObserver)
{
    CxContainer::remove(* f_oMonsbManagerSubjects, oObserver);
}

std::string MonsbManager::describeDefaultValue(CxDescribeUnit *oDescribeUnit, const CxDescribeBase *oDescribe, int iRowIndex)
{
    if (oDescribe->name() == CS_EntryMeasureName) {
        CxDescribeEnum* oMonsbTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryMonsbType);
        if (oMonsbTypeDescribe)
        {
            MonsbTypeEnum eMonsbType = (MonsbTypeEnum)oMonsbTypeDescribe->value();
            std::string sMonsbTypeName = g_fn_monsbTypeName(eMonsbType);
            std::string sRowIndex = CxString::format("%06d",iRowIndex);
            return sMonsbTypeName + sRowIndex;
        }
    }
    return "";
}

void MonsbManager::describeSelf(CxDescribeUnit *oDescribeUnit)
{
    MeasureManagerTemplate<MonsbMeasure, int>::describeSelf(oDescribeUnit);
    CxDescribeInteger* oMinDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryMinValue);
    oMinDescribe->initValue(0);
    CxDescribeInteger* oMaxDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryMaxValue);
    oMaxDescribe->initValue(1);
    CxDescribeInteger* oDefaultDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryDefaultValue);
    oDefaultDescribe->initValue(-1);
    CxDescribeEnum* oMonsbTypeDescribe = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryMonsbType);
    oMonsbTypeDescribe->setIsReadOnly(true);
    oMonsbTypeDescribe->setEnumCopeFromZero(ce_maxMonsbType);
    oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
}
