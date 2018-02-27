#ifndef CALC_MEASURE_H
#define CALC_MEASURE_H


#include <vdi/measure_template.h>
#include <vdi/measure_manager_template.h>


class CalcMeasure;

class ICalcSubject : public MeasureSubjectTemplate<CalcMeasure, double>
{
public:

};

class CalcPublisher : public MeasurePublisherTemplate<CalcMeasure, double>
{
public:

};

class CalcMeasure : public MeasureTemplate<double>
{
public:
//*static measure
    static const std::string& TYPESIMPLENAME() { return CS_TypeSimpleNameCalc; }

    static MeasureTypeEnum MEASURETYPE() { return MeasureType_Calc; }

    static int minMeasureId() { return ci_minMeasureIdCalc; }

    static int maxMeasureId() { return ci_maxMeasureIdCalc; }

//*static measures
    static const std::vector<int>& measureSonTypes() { return ci_monsbTypeValues; }

    static std::vector<CalcMeasure*> * measuresContainer();

    static int currentMaxMeasureId();

    static void setCurrentMaxMeasureId(int value);

    static void beginSetValues(int iCalcType, int iSourceId, int iTag = 0);

    static bool isSettingValues();

    static void valuesChanged(CalcMeasure* o, double oldValue);

    static void endSetValues();

    static void raiseMeasuresPrepareChange(const std::map<const CalcMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iCalcMeasureType);

    static void raiseMeasuresAfterLoad(const std::vector<const CalcMeasure*>& measures);

    static void raiseValuesChanged();

//*Calc Measure Class
    CalcMeasure();
    ~CalcMeasure();

    inline const std::string& typeSimpleName() const { return CS_TypeSimpleNameCalc; }

    MeasureTypeEnum measureType() const { return MeasureType_Calc; }

    inline int measureSonType() const { return _calcType; }

    inline const CalcTypeEnum& calcType() const { return _calcType; }
    inline void setCalcType(const CalcTypeEnum& value) { _calcType = value; }

    inline const CalcPublisher* publisher() const { return _publisher; }

    inline void addObserver(ICalcSubject* oObserver) { _publisher->addObserver(oObserver); }

    inline void removeObserver(ICalcSubject* oObserver) { _publisher->removeObserver(oObserver); }

    void toContext(CxISetSkv& context) const;

    void fromContext(const CxIGetSkv& context);

    int getCalcType(){ return m_nCalcType; }
    int getWarnType(){ return m_nWarnType; }
    int getSaveCircle(){ return m_nSaveCircle; }

    double getRunLow(){ return m_dRunLow; }
    double getRunHigh(){ return m_dRunHigh; }

    std::string getExpression(){ return m_szCalcExpression; }

protected:
    bool valueCanChange(const double & newValue);

    void valueChanged(const double & oldValue, const double & newValue);

    void valueChanging(const double & newValue);

    bool valueIsChange(const double & oldValue, const double & newValue);
    //值大小
    int  sizeofValue(){return sizeof(double);}
    //to Buffer
    int toBufferValue(uchar *buffer,double& value)
    {
       *(double*)buffer = value;
       return sizeofValue();
    }
    //to Buffer
    int fromBufferValue(uchar *buffer,double& value)
    {
        value = *(double*)buffer;
        return sizeofValue();
    }

private:
    CalcTypeEnum _calcType;
    CalcPublisher* _publisher;

    int m_nCalcType;
    int m_nWarnType;
    int m_nSaveCircle;

    double m_dRunLow;
    double m_dRunHigh;

    std::string m_szCalcExpression;

};



class ICalcManagerSubject : public MeasureManagerSubjectTemplate<CalcMeasure, double>
{
public:
    friend class CalcMeasure;
};

class CalcManager : public MeasureManagerTemplate<CalcMeasure, double>
{
public:
    static void addObserver(ICalcManagerSubject * oObserver);

    static void removeObserver(ICalcManagerSubject * oObserver);

    static std::string describeDefaultValue(CxDescribeUnit* oDescribeUnit, const CxDescribeBase* oDescribe, int iRowIndex);

    static void describeSelf(CxDescribeUnit* oDescribeUnit);

};

#endif // !defined(CALC_MEASURE_H)
