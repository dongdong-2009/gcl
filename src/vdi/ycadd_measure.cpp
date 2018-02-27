#include "ycadd_measure.h"


std::vector<IYcaddManagerSubject*> * fn_oYcaddManagerSubjects()
{
    static std::vector<IYcaddManagerSubject*> m;
    return &m;
}

std::map<const YcaddMeasure*, double> * fn_oValueChangedYcaddOldValues()
{
    static std::map<const YcaddMeasure*, double> m;
    return &m;
}


static int f_currentMaxYcaddId = ci_minMeasureIdYcadd;
static std::vector<IYcaddManagerSubject*> * f_oYcaddManagerSubjects = fn_oYcaddManagerSubjects();
static std::map<const YcaddMeasure*, double> * f_oValueChangedYcaddOldValues = fn_oValueChangedYcaddOldValues();
static int f_valueChangeYcaddType = -1;
static int f_valueChangeSourceId = -1;
static int f_valueChangeTag = -1;
static bool f_valueChanging = false;


std::vector<YcaddMeasure *> *YcaddMeasure::measuresContainer()
{
    static std::vector<YcaddMeasure *> m;
    return &m;
}

int YcaddMeasure::currentMaxMeasureId()
{
    return f_currentMaxYcaddId;
}

void YcaddMeasure::setCurrentMaxMeasureId(int value)
{
    f_currentMaxYcaddId = value;
}

void YcaddMeasure::beginSetValues(int iYcaddType, int iSourceId, int iTag)
{
    f_oValueChangedYcaddOldValues->clear();
    f_valueChangeYcaddType = iYcaddType;
    f_valueChangeSourceId = iSourceId;
    f_valueChangeTag = iTag;
    f_valueChanging = true;
}

bool YcaddMeasure::isSettingValues()
{
    return f_valueChanging;
}

void YcaddMeasure::valuesChanged(YcaddMeasure* o, double oldValue)
{
    (*f_oValueChangedYcaddOldValues)[o] = oldValue;
}

void YcaddMeasure::endSetValues()
{
    f_valueChanging = false;
    if (f_oValueChangedYcaddOldValues->size()>0)
    {
        raiseValuesChanged();
    }
}

void YcaddMeasure::raiseMeasuresPrepareChange(const std::map<const YcaddMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iYcaddType)
{
    for (size_t i = 0; i < f_oYcaddManagerSubjects->size(); ++i)
    {
        IYcaddManagerSubject* oSubject = f_oYcaddManagerSubjects->at(i);
        oSubject->measures_prepareChange(measuresChangeType, iYcaddType);
    }
}

void YcaddMeasure::raiseMeasuresAfterLoad(const std::vector<const YcaddMeasure*>& measures)
{
    for (size_t i = 0; i < f_oYcaddManagerSubjects->size(); ++i)
    {
        IYcaddManagerSubject * oSubject = f_oYcaddManagerSubjects->at(i);
        oSubject->measures_afterLoad(measures);
    }
}

void YcaddMeasure::raiseValuesChanged()
{
    for (size_t i = 0; i < f_oYcaddManagerSubjects->size(); ++i)
    {
        IYcaddManagerSubject* oSubject = f_oYcaddManagerSubjects->at(i);
        oSubject->measures_valueChanged(*f_oValueChangedYcaddOldValues, f_valueChangeYcaddType, f_valueChangeSourceId, f_valueChangeTag);
    }
}














YcaddMeasure::YcaddMeasure()
{
    _ycaddType = YcaddType_0;
    _publisher = new YcaddPublisher();
}

YcaddMeasure::~YcaddMeasure()
{
    delete _publisher;
}

void YcaddMeasure::toContext(CxISetSkv &context) const
{
    MeasureTemplate<double>::toContext(context);
    int iYcaddTypeValue = _ycaddType;
    context.setValue(CS_EntryYcaddType, iYcaddTypeValue);

    context.setValue("saveCircle", m_nSaveCircle);
    context.setValue("warnType", m_nWarnType);
    context.setValue("runHigh", m_dRunHigh);
    context.setValue("runLow", m_dRunLow);
}

void YcaddMeasure::fromContext(const CxIGetSkv &context)
{
    MeasureTemplate<double>::fromContext(context);
    int iYcaddTypeValue = _ycaddType;
    iYcaddTypeValue =      context.getValue(CS_EntryYcaddType, iYcaddTypeValue);
    _ycaddType = g_fn_ycaddTypeEnum(iYcaddTypeValue);

    m_nSaveCircle = context.getValue("saveCircle", m_nSaveCircle);
    m_nWarnType = context.getValue("warnType", m_nWarnType);
    m_dRunHigh = context.getValue("runHigh", m_dRunHigh);
    m_dRunLow = context.getValue("runLow", m_dRunLow);
}

bool YcaddMeasure::valueCanChange(const double & newValue)
{
    return _publisher->valueCanChange(this, newValue);
}

void YcaddMeasure::valueChanging(const double & newValue)
{
    _publisher->valueChanging(this, newValue);
}

bool YcaddMeasure::valueIsChange(const double &oldValue, const double &newValue)
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

void YcaddMeasure::valueChanged(const double & oldValue, const double & newValue)
{
    _publisher->valueChanged(this, oldValue, newValue);
    if ( isSettingValues() )
        valuesChanged(this, oldValue);
}





//*YcaddManager

void YcaddManager::addObserver(IYcaddManagerSubject *oObserver)
{
    fn_oYcaddManagerSubjects()->push_back(oObserver);
}

void YcaddManager::removeObserver(IYcaddManagerSubject *oObserver)
{
    CxContainer::remove(* f_oYcaddManagerSubjects, oObserver);
}

std::string YcaddManager::describeDefaultValue(CxDescribeUnit *oDescribeUnit, const CxDescribeBase *oDescribe, int iRowIndex)
{
    if (oDescribe->name() == CS_EntryMeasureName)
    {
        CxDescribeEnum* oYcaddTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryYcaddType);
        if (oYcaddTypeDescribe)
        {
            YcaddTypeEnum eYcaddType = (YcaddTypeEnum)oYcaddTypeDescribe->value();
            std::string sYcaddTypeName = g_fn_ycaddTypeName(eYcaddType);
            std::string sRowIndex = CxString::format("%06d",iRowIndex);
            return sYcaddTypeName + sRowIndex;
        }
    }
    return "";
}

void YcaddManager::describeSelf(CxDescribeUnit *oDescribeUnit)
{
    MeasureManagerTemplate<YcaddMeasure, double>::describeSelf(oDescribeUnit);
    CxDescribeDouble* oMinDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryMinValue);
    oMinDescribe->initValue(CxValueType::minValue<short>());
    CxDescribeDouble* oMaxDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryMaxValue);
    oMaxDescribe->initValue(CxValueType::maxValue<short>());
    CxDescribeDouble* oDefaultDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryDefaultValue);
    oDefaultDescribe->initValue(0);
    CxDescribeEnum* oYcaddTypeDescribe = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryYcaddType);
    oYcaddTypeDescribe->setIsReadOnly(true);
    oYcaddTypeDescribe->setEnumCopeFromZero(ce_maxYcaddType);
    oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
}
