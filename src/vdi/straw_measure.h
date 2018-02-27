#ifndef STRAW_MEASURE_H
#define STRAW_MEASURE_H


#include "measure_template.h"
#include "measure_manager_template.h"


class StrawMeasure;

class IStrawSubject : public MeasureSubjectTemplate<StrawMeasure, std::string>
{
public:

};

class StrawPublisher : public MeasurePublisherTemplate<StrawMeasure, std::string>
{
public:

};

class GM_GCL_VDI_API StrawMeasure : public MeasureTemplate<std::string>
{
public:
//*static measure
    static const std::string& TYPESIMPLENAME() { return CS_TypeSimpleNameStraw; }

    static MeasureTypeEnum MEASURETYPE() { return MeasureType_Straw; }

    static const int& minMeasureId() { return ci_minMeasureIdStraw; }

    static const int& maxMeasureId() { return ci_maxMeasureIdStraw; }

//*static measures
    static const std::vector<int>& measureSonTypes() { return ci_strawTypeValues; }

    static std::vector<StrawMeasure*> * measuresContainer();

    static int currentMaxMeasureId();

    static void setCurrentMaxMeasureId(int value);

    static void beginSetValues(int iStrawType, int iSourceId, int iTag = 0);

    static bool isSettingValues();

    static void valuesChanged(StrawMeasure* o, const std::string & oldValue);

    static void endSetValues();

    static void raiseMeasuresPrepareChange(const std::map<const StrawMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iStrawMeasureType);

    static void raiseMeasuresAfterLoad(const std::vector<const StrawMeasure*>& measures);

    static void raiseValuesChanged();

//*Straw Measure Class
    StrawMeasure();
    ~StrawMeasure();

    inline const std::string& typeSimpleName() const { return CS_TypeSimpleNameStraw; }

    MeasureTypeEnum measureType() const { return MeasureType_Straw; }

    inline int measureSonType() const { return _strawType; }

    inline const StrawTypeEnum& strawType() const { return _strawType; }
    inline void setStrawType(const StrawTypeEnum& value) { _strawType = value; }

    inline const StrawPublisher* publisher() const { return _publisher; }

    inline void addObserver(IStrawSubject* oObserver) { _publisher->addObserver(oObserver); }

    inline void removeObserver(IStrawSubject* oObserver) { _publisher->removeObserver(oObserver); }

    void toContext(CxISetSkv& context) const;

    void fromContext(const CxIGetSkv& context);

protected:
    bool valueCanChange(const std::string & newValue);

    void valueChanged(const std::string & oldValue, const std::string & newValue);

    void valueChanging(const std::string & newValue);

    bool valueIsChange(const std::string & oldValue, const std::string & newValue);

    //值大小
    int  sizeofValue(){return ci_defaultLengthStrawMeasure;}
    //to Buffer
    int toBufferValue(uchar *buffer,std::string& value)
    {
       int len = value.length();
       if(len>sizeofValue())len = sizeofValue();
       value.copy((char*)buffer,len,0);
       return sizeofValue();
    }
    //to Buffer
    int fromBufferValue(uchar *buffer,std::string& value)
    {        
        value.clear();
        value.append((char*)buffer,sizeofValue());
        return sizeofValue();
    }

private:
    StrawTypeEnum _strawType;
    StrawPublisher* _publisher;

};



class IStrawManagerSubject : public MeasureManagerSubjectTemplate<StrawMeasure, std::string>
{
public:
    friend class StrawMeasure;
};

class GM_GCL_VDI_API StrawManager : public MeasureManagerTemplate<StrawMeasure, std::string>
{
public:
    static void addObserver(IStrawManagerSubject * oObserver);

    static void removeObserver(IStrawManagerSubject * oObserver);

    static std::string describeDefaultValue(CxDescribeUnit* oDescribeUnit, const CxDescribeBase* oDescribe, int iRowIndex);

    static void describeSelf(CxDescribeUnit* oDescribeUnit);

};

#endif // STRAW_MEASURE_H
