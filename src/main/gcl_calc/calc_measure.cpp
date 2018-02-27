#include "calc_measure.h"

std::vector<ICalcManagerSubject*> * fn_oCalcManagerSubjects()
{
    static std::vector<ICalcManagerSubject*> m;
    return &m;
}

std::map<const CalcMeasure*, double> * fn_oValueChangedCalcOldValues()
{
    static std::map<const CalcMeasure*, double> m;
    return &m;
}


static int f_currentMaxCalcId = ci_minMeasureIdCalc;
static std::vector<ICalcManagerSubject*> * f_oCalcManagerSubjects = fn_oCalcManagerSubjects();
static std::map<const CalcMeasure*, double> * f_oValueChangedCalcOldValues = fn_oValueChangedCalcOldValues();
static int f_valueChangeCalcType = -1;
static int f_valueChangeSourceId = -1;
static int f_valueChangeTag = -1;
static bool f_valueChanging = false;


std::vector<CalcMeasure *> *CalcMeasure::measuresContainer()
{
    static std::vector<CalcMeasure *> m;
    return &m;
}

int CalcMeasure::currentMaxMeasureId()
{
    return f_currentMaxCalcId;
}

void CalcMeasure::setCurrentMaxMeasureId(int value)
{
    f_currentMaxCalcId = value;
}

void CalcMeasure::beginSetValues(int iCalcType, int iSourceId, int iTag)
{
    f_oValueChangedCalcOldValues->clear();
    f_valueChangeCalcType = iCalcType;
    f_valueChangeSourceId = iSourceId;
    f_valueChangeTag = iTag;
    f_valueChanging = true;
}

bool CalcMeasure::isSettingValues()
{
    return f_valueChanging;
}

void CalcMeasure::valuesChanged(CalcMeasure* o, double oldValue)
{
    (*f_oValueChangedCalcOldValues)[o] = oldValue;
}

void CalcMeasure::endSetValues()
{
    f_valueChanging = false;
    if (f_oValueChangedCalcOldValues->size()>0)
    {
        raiseValuesChanged();
    }
}

void CalcMeasure::raiseMeasuresPrepareChange(const std::map<const CalcMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iCalcType)
{
    for (size_t i = 0; i < f_oCalcManagerSubjects->size(); ++i)
    {
        ICalcManagerSubject* oSubject = f_oCalcManagerSubjects->at(i);
        oSubject->measures_prepareChange(measuresChangeType, iCalcType);
    }
}

void CalcMeasure::raiseMeasuresAfterLoad(const std::vector<const CalcMeasure*>& measures)
{
    for (size_t i = 0; i < f_oCalcManagerSubjects->size(); ++i)
    {
        ICalcManagerSubject * oSubject = f_oCalcManagerSubjects->at(i);
        oSubject->measures_afterLoad(measures);
    }
}

void CalcMeasure::raiseValuesChanged()
{
    for (size_t i = 0; i < f_oCalcManagerSubjects->size(); ++i)
    {
        ICalcManagerSubject* oSubject = f_oCalcManagerSubjects->at(i);
        oSubject->measures_valueChanged(*f_oValueChangedCalcOldValues, f_valueChangeCalcType, f_valueChangeSourceId, f_valueChangeTag);
    }
}


CalcMeasure::CalcMeasure()
{
    _calcType = CalcType_0;
    _publisher = new CalcPublisher();
}

CalcMeasure::~CalcMeasure()
{
    delete _publisher;
}

void CalcMeasure::toContext(CxISetSkv &context) const
{
    MeasureTemplate<double>::toContext(context);
    int iCalcTypeValue = _calcType;
    context.setValue(CS_EntryCalcType, iCalcTypeValue);

    context.setValue("calcType", m_nCalcType);
    context.setValue("calcExpression", m_szCalcExpression);
    context.setValue("saveCircle", m_nSaveCircle);
    context.setValue("warnType", m_nWarnType);
    context.setValue("runHigh", m_dRunHigh);
    context.setValue("runLow", m_dRunLow);
}

void CalcMeasure::fromContext(const CxIGetSkv &context)
{
    MeasureTemplate<double>::fromContext(context);
    int iCalcTypeValue = _calcType;
    iCalcTypeValue =      context.getValue(CS_EntryCalcType, iCalcTypeValue);
    _calcType = g_fn_calcTypeEnum(iCalcTypeValue);

    m_nCalcType = context.getValue("CalcType", m_nCalcType);
    m_szCalcExpression = context.getValue("CalcExpression", m_szCalcExpression);
    m_nSaveCircle = context.getValue("SaveCircle", m_nSaveCircle);
    m_nWarnType = context.getValue("WarnType", m_nWarnType);
    m_dRunHigh = context.getValue("RunHigh", m_dRunHigh);
    m_dRunLow = context.getValue("RunLow", m_dRunLow);
}

bool CalcMeasure::valueCanChange(const double & newValue)
{
    return _publisher->valueCanChange(this, newValue);
}

void CalcMeasure::valueChanging(const double & newValue)
{
    _publisher->valueChanging(this, newValue);
}

bool CalcMeasure::valueIsChange(const double &oldValue, const double &newValue)
{
//    if(fabs(newValue-oldValue)>0.01)
    if(fabs(newValue-oldValue)>0.01)
    {
       return true;
    }
    else
    {
        return false;
    }
}

void CalcMeasure::valueChanged(const double & oldValue, const double & newValue)
{
    _publisher->valueChanged(this, oldValue, newValue);
    if ( isSettingValues() )
        valuesChanged(this, oldValue);
}





//*CalcManager

void CalcManager::addObserver(ICalcManagerSubject *oObserver)
{
    fn_oCalcManagerSubjects()->push_back(oObserver);
}

void CalcManager::removeObserver(ICalcManagerSubject *oObserver)
{
    CxContainer::remove(* f_oCalcManagerSubjects, oObserver);
}

std::string CalcManager::describeDefaultValue(CxDescribeUnit *oDescribeUnit, const CxDescribeBase *oDescribe, int iRowIndex)
{
    if (oDescribe->name() == CS_EntryMeasureName)
    {
        CxDescribeEnum* oCalcTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryCalcType);
        if (oCalcTypeDescribe)
        {
            CalcTypeEnum eCalcType = (CalcTypeEnum)oCalcTypeDescribe->value();
            std::string sCalcTypeName = g_fn_calcTypeName(eCalcType);
            std::string sRowIndex = CxString::format("%06d",iRowIndex);
            return sCalcTypeName + sRowIndex;
        }
    }
    return "";
}

void CalcManager::describeSelf(CxDescribeUnit *oDescribeUnit)
{
    MeasureManagerTemplate<CalcMeasure, double>::describeSelf(oDescribeUnit);
    CxDescribeDouble* oMinDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryMinValue);
    oMinDescribe->initValue(CxValueType::minValue<short>());
    CxDescribeDouble* oMaxDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryMaxValue);
    oMaxDescribe->initValue(CxValueType::maxValue<short>());
    CxDescribeDouble* oDefaultDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryDefaultValue);
    oDefaultDescribe->initValue(0);
    CxDescribeEnum* oCalcTypeDescribe = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryCalcType);
    oCalcTypeDescribe->setIsReadOnly(true);
    oCalcTypeDescribe->setEnumCopeFromZero(ce_maxCalcType);
    oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
}
