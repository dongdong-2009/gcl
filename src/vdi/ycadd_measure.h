#ifndef YCADD_MEASURE_H
#define YCADD_MEASURE_H


#include "measure_template.h"
#include "measure_manager_template.h"


class YcaddMeasure;

class IYcaddSubject : public MeasureSubjectTemplate<YcaddMeasure, double>
{
public:

};

class YcaddPublisher : public MeasurePublisherTemplate<YcaddMeasure, double>
{
public:

};

class GM_GCL_VDI_API YcaddMeasure : public MeasureTemplate<double>
{
public:
//*static measure
    static const std::string& TYPESIMPLENAME() { return CS_TypeSimpleNameYcadd; }

    static MeasureTypeEnum MEASURETYPE() { return MeasureType_Ycadd; }

    static int minMeasureId() { return ci_minMeasureIdYcadd; }

    static int maxMeasureId() { return ci_maxMeasureIdYcadd; }

//*static measures
    static const std::vector<int>& measureSonTypes() { return ci_monsbTypeValues; }

    static std::vector<YcaddMeasure*> * measuresContainer();

    static int currentMaxMeasureId();

    static void setCurrentMaxMeasureId(int value);

    static void beginSetValues(int iYcaddType, int iSourceId, int iTag = 0);

    static bool isSettingValues();

    static void valuesChanged(YcaddMeasure* o, double oldValue);

    static void endSetValues();

    static void raiseMeasuresPrepareChange(const std::map<const YcaddMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iYcaddMeasureType);

    static void raiseMeasuresAfterLoad(const std::vector<const YcaddMeasure*>& measures);

    static void raiseValuesChanged();

//*Ycadd Measure Class
    YcaddMeasure();
    ~YcaddMeasure();

    inline const std::string& typeSimpleName() const { return CS_TypeSimpleNameYcadd; }

    MeasureTypeEnum measureType() const { return MeasureType_Ycadd; }

    inline int measureSonType() const { return _ycaddType; }

    inline const YcaddTypeEnum& ycaddType() const { return _ycaddType; }
    inline void setYcaddType(const YcaddTypeEnum& value) { _ycaddType = value; }

    inline const YcaddPublisher* publisher() const { return _publisher; }

    inline void addObserver(IYcaddSubject* oObserver) { _publisher->addObserver(oObserver); }

    inline void removeObserver(IYcaddSubject* oObserver) { _publisher->removeObserver(oObserver); }

    void toContext(CxISetSkv& context) const;

    void fromContext(const CxIGetSkv& context);

    int getWarnType(){ return m_nWarnType; }
    int getSaveCircle(){ return m_nSaveCircle; }

    double getRunLow(){ return m_dRunLow; }
    double getRunHigh(){ return m_dRunHigh; }

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
    YcaddTypeEnum _ycaddType;
    YcaddPublisher* _publisher;

    int m_nWarnType;
    int m_nSaveCircle;

    double m_dRunLow;
    double m_dRunHigh;
};



class IYcaddManagerSubject : public MeasureManagerSubjectTemplate<YcaddMeasure, double>
{
public:
    friend class YcaddMeasure;
};

class GM_GCL_VDI_API YcaddManager : public MeasureManagerTemplate<YcaddMeasure, double>
{
public:
    static void addObserver(IYcaddManagerSubject * oObserver);

    static void removeObserver(IYcaddManagerSubject * oObserver);

    static std::string describeDefaultValue(CxDescribeUnit* oDescribeUnit, const CxDescribeBase* oDescribe, int iRowIndex);

    static void describeSelf(CxDescribeUnit* oDescribeUnit);

};

#endif // !defined(YCADD_MEASURE_H)
