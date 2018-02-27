#ifndef MONSB_MEASURE_H
#define MONSB_MEASURE_H


#include "measure_template.h"
#include "measure_manager_template.h"


class MonsbMeasure;

class IMonsbSubject : public MeasureSubjectTemplate<MonsbMeasure, int>
{
public:

};

class MonsbPublisher : public MeasurePublisherTemplate<MonsbMeasure, int>
{
public:

};


class GM_GCL_VDI_API MonsbMeasure : public MeasureTemplate<int>
{
public:
//*static measure
    static const std::string& TYPESIMPLENAME() { return CS_TypeSimpleNameMonsb; }

    static MeasureTypeEnum MEASURETYPE() { return MeasureType_Monsb; }

    static int minMeasureId() { return ci_minMeasureIdMonsb; }

    static int maxMeasureId() { return ci_maxMeasureIdMonsb; }

//*static measures
    static const std::vector<int>& measureSonTypes() { return ci_monsbTypeValues; }

    static std::vector<MonsbMeasure*> * measuresContainer();

    static void beginSetValues(int iMonsbType, int iSourceId, int iTag = 0);

    static bool isSettingValues();

    static void valuesChanged(MonsbMeasure* o, int oldValue);

    static void endSetValues();

    static void raiseMeasuresPrepareChange(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iMonsbMeasureType);

    static void raiseMeasuresAfterLoad(const std::vector<const MonsbMeasure*>& measures);

    static void raiseValuesChanged();

    static int currentMaxMeasureId();

    static void setCurrentMaxMeasureId(int value);

//*Monsb Measure Class
    MonsbMeasure();
    ~MonsbMeasure();

    inline const std::string& typeSimpleName() const { return CS_TypeSimpleNameMonsb; }

    MeasureTypeEnum measureType() const { return MeasureType_Monsb; }

    inline int measureSonType() const { return _monsbType; }

    inline const MonsbTypeEnum& monsbType() const { return _monsbType; }
    inline void setMonsbType(const MonsbTypeEnum& value) { _monsbType = value; }

    inline const MonsbPublisher* publisher() const { return _publisher; }

    inline void addObserver(IMonsbSubject* oObserver) { _publisher->addObserver(oObserver); }

    inline void removeObserver(IMonsbSubject* oObserver) { _publisher->removeObserver(oObserver); }

    inline int nextState() const
    {
        if (value() < maxValue())
        {
            return value() + 1;
        }
        else
        {
            return minValue();
        }
    }

    void toContext(CxISetSkv& context) const;

    void fromContext(const CxIGetSkv& context);

    int getSaveType(){ return m_nSaveType; }
    int getWarnType(){ return m_nWarnType; }

protected:
    bool valueCanChange(const int & newValue);

    void valueChanged(const int & oldValue, const int & newValue);

    void valueChanging(const int & newValue);

    bool valueIsChange(const int & oldValue, const int & newValue);

    //值大小
    int  sizeofValue(){return sizeof(int);}

    //to Buffer
    int toBufferValue(uchar *buffer,int& value)
    {
       *(int*)buffer = value;
       return sizeofValue();
    }

    //to Buffer
    int fromBufferValue(uchar *buffer,int& value)
    {
        value = *(int*)buffer;
        return sizeofValue();
    }

private:
    MonsbTypeEnum _monsbType;
    MonsbPublisher* _publisher;

    int m_nSaveType;
    int m_nWarnType;
};



class IMonsbManagerSubject : public MeasureManagerSubjectTemplate<MonsbMeasure, int>
{
public:
    friend class MonsbMeasure;
};


class GM_GCL_VDI_API MonsbManager : public MeasureManagerTemplate<MonsbMeasure, int>
{
public:
    static void addObserver(IMonsbManagerSubject * oObserver);

    static void removeObserver(IMonsbManagerSubject * oObserver);

    static std::string describeDefaultValue(CxDescribeUnit* oDescribeUnit, const CxDescribeBase* oDescribe, int iRowIndex);

    static void describeSelf(CxDescribeUnit* oDescribeUnit);

};

#endif // !defined(MONSB_MEASURE_H)
