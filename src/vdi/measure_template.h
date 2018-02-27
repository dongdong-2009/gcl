#ifndef MEASURE_TEMPLATE_H
#define MEASURE_TEMPLATE_H

#include "measure_base.h"

#define GC_MEASURE_T_COUNT  6
#define GC_MEASURE_INT_COUNT 5
#define GC_MEASURE_TM_COUNT 2

template<typename T>
class MeasureTemplate : public MeasureBase
{
public:
    MeasureTemplate(){
        _minValue = CxValueType::minValue<T>();
        _maxValue = CxValueType::maxValue<T>();
        _originalValue = CxValueType::originalValue<T>();
        _defaultValue = _originalValue;
        _oldValue = _originalValue;
        _newValue = _originalValue;
        _value = _originalValue;
        _lastRefreshTime = 0;
        _lastRefreshSourceId = -1;
        _lastRefreshReasonId = -1;
        _lastChangeTime = 0;
        _lastChangeSourceId = -1;
        _lastChangeReasonId = -1;
        _outLineInterval = 0;
        _bChange = false;
        _quality = 0;
    }
    ~MeasureTemplate(){

    }

    void toContext(CxISetSkv& context) const{
        MeasureBase::toContext(context);
        context.setValue(CS_EntryMinValue, _minValue);
        context.setValue(CS_EntryMaxValue, _maxValue);
        context.setValue(CS_EntryDefaultValue, _defaultValue);
    }

    void fromContext(const CxIGetSkv& context){
        MeasureBase::fromContext(context);
        _minValue =      context.getValue(CS_EntryMinValue, _minValue);
        _maxValue =      context.getValue(CS_EntryMaxValue, _maxValue);
        _defaultValue =  context.getValue(CS_EntryDefaultValue, _defaultValue);
    }


    inline const T& originalValue() const { return _originalValue; }
    inline void setOriginalValue(const T& value) { _originalValue = value; }

    inline const T& maxValue() const { return _maxValue; }
    inline void setMaxValue(const T& value) { _maxValue = value; }

    inline const T& minValue() const { return _minValue; }
    inline void setMinValue(const T& value) { _minValue = value; }

    inline const T& defaultValue() const { return _defaultValue; }
    inline void setDefaultValue(const T& value) { _defaultValue = value; }

    inline const T& oldValue() const { return _oldValue; }
    inline void setOldValue(const T& value) { _oldValue = value; }

    inline const T& newValue() const { return _newValue; }
    inline void setNewValue(const T& value) { _newValue = value; }

    inline const T& value() const { return  _value; }
    bool setValue(const T& value, int iSourceId = -1, int iReason = -1)
    {
        _lastRefreshTime = CxTime::currentSystemTime();
        _lastRefreshSourceId = iSourceId;
        _lastRefreshReasonId = iReason;
        _bChange = false;
        if (valueIsChange(value,_newValue))
        {
            _newValue = value;
            if ( valueCanChange(value) )
            {
                valueChanging(value);
                _oldValue = _value;
                _value = value;
                _lastChangeTime = CxTime::currentSystemTime();
                _lastChangeSourceId = iSourceId;
                _lastChangeReasonId = iReason;
                _bChange = true;
                valueChanged(_oldValue, _value);
            }
        }
        return _bChange;
    }

    inline bool isValidValue() const { return isValidValue(_value); }

    inline bool isValidValue(T value) const { return (value >= _minValue) && (value <= _maxValue); }

    inline const msepoch_t& lastChangeTime() const { return _lastChangeTime; }

    inline const int& lastChangeSourceId() const { return _lastChangeSourceId; }

    inline const int& lastChangeReasonId() const { return _lastChangeReasonId; }

    inline const msepoch_t& lastRefreshTime() const { return _lastRefreshTime; }

    inline const int& lastRefreshSourceId() const { return _lastRefreshSourceId; }

    inline const int& lastRefreshReasonId() const { return _lastRefreshReasonId; }

    inline const int& outLineInterval() const { return _outLineInterval; }
    inline void setOutLineInterval(const int& value) { _outLineInterval = value; }

    inline bool isOnLine() const { return (_outLineInterval > 0 && CxTime::milliSecondDifferToNow(_lastRefreshTime) > _outLineInterval) ? false : true ; }

    inline const T& validValue() const { return isOnLine() ? _value : _defaultValue ; }

    //llb add 20130114
    inline const int &quality()const{return _quality;}
    inline void setQuality(int value){ _quality = value; }

    inline bool isChange(){return _bChange;}

    //从缓冲区获取数据
    int fromBuffer(uchar *buffer,int len)
    {
        if((buffer==NULL) || len<getSize()) return GC_MEASURE_ERROR;
        //简单信息
        int nPos = fromBufferS(buffer,len);
        if(nPos!=getSizeSHM()) return GC_MEASURE_ERROR;
        //扩展信息
//        T *pVal = (T*)(buffer+nPos);
//        _originalValue = *pVal++;
//        _maxValue      = *pVal++;
//        _minValue      = *pVal++;
//        _defaultValue  = *pVal++;
//        _oldValue      = *pVal++;
//        _newValue      = *pVal;
//        nPos += sizeof(T)*GC_MEASURE_T_COUNT;

        nPos += fromBufferValue(buffer+nPos,_originalValue);
        nPos += fromBufferValue(buffer+nPos,_maxValue);
        nPos += fromBufferValue(buffer+nPos,_minValue);
        nPos += fromBufferValue(buffer+nPos,_defaultValue);
        nPos += fromBufferValue(buffer+nPos,_oldValue);
        nPos += fromBufferValue(buffer+nPos,_newValue);

        int *pId =  (int*)(buffer+nPos);
        _lastRefreshSourceId = *pId++;
        _lastRefreshReasonId = *pId++;
        _lastChangeSourceId  = *pId++;
        _lastChangeReasonId  = *pId++;
        _outLineInterval     = *pId++;

        nPos += sizeof(int)*GC_MEASURE_INT_COUNT;
        if(nPos!=getSize())return GC_MEASURE_ERROR;
        else return nPos;
    }

    //深层拷贝
    int toBuffer(uchar *buffer,int len)
    {
        if((buffer==NULL) || len<getSize()) return GC_MEASURE_ERROR;
        //简单信息
        int nPos = toBufferS(buffer,len);
        if(nPos!=getSizeSHM()) return GC_MEASURE_ERROR;
        //扩展信息
//        T *pVal = (T*)(buffer+nPos);
//        *pVal++ = _originalValue;
//        *pVal++ = _maxValue;
//        *pVal++ = _minValue;
//        *pVal++ = _defaultValue;
//        *pVal++ = _oldValue;
//        *pVal   = _newValue;
//        nPos += sizeof(T)*GC_MEASURE_T_COUNT;

        nPos+=toBufferValue(buffer+nPos,_originalValue);
        nPos+=toBufferValue(buffer+nPos,_maxValue);
        nPos+=toBufferValue(buffer+nPos,_minValue);
        nPos+=toBufferValue(buffer+nPos,_defaultValue);
        nPos+=toBufferValue(buffer+nPos,_oldValue);
        nPos+=toBufferValue(buffer+nPos,_newValue);

        int *pId =  (int*)(buffer+nPos);
        *pId++ = _lastRefreshSourceId;
        *pId++ = _lastRefreshReasonId;
        *pId++ = _lastChangeSourceId;
        *pId++ = _lastChangeReasonId;
        *pId   = _outLineInterval;

        nPos += sizeof(int)*GC_MEASURE_INT_COUNT;
        if(nPos!=getSize())return GC_MEASURE_ERROR;
        else return nPos;
    }

    //全拷贝大小
    inline int getSize()
    {
        return (getSizeSHM()+sizeofValue()*GC_MEASURE_T_COUNT+sizeof(int)*GC_MEASURE_INT_COUNT);
    }

    //获取基础大小
    inline int getBaseSize()
    {
        return MeasureBase::size();
    }

    //简单拷贝 t q v
    int fromBufferS(uchar *buffer,int len)
    {
        if((buffer==NULL) || len<getSizeSHM()) return GC_MEASURE_ERROR;
        if(MeasureBase::fromBuffer(buffer,len)<1) return GC_MEASURE_ERROR;

        int nPos = MeasureBase::size();

        _lastRefreshTime =  *(msepoch_t*)(buffer+nPos);
        nPos += sizeof(msepoch_t);
        _lastChangeTime =  *(msepoch_t*)(buffer+nPos);
        nPos += sizeof(msepoch_t);
        _quality = *(int*)(buffer+nPos);
        nPos += sizeof(int);
//        _value = *(T*)(buffer+nPos);
//        nPos += sizeof(T);
        nPos += fromBufferValue(buffer+nPos,_value);
        return nPos;
    }

    //输出数据到缓冲区
    int toBufferS(uchar *buffer,int len)
    {
        if((buffer==NULL) || len<getSizeSHM()) return GC_MEASURE_ERROR;
        if(MeasureBase::toBuffer(buffer,len)<1) return GC_MEASURE_ERROR;

        int nPos = MeasureBase::size();
        *(msepoch_t*)(buffer+nPos) = _lastRefreshTime;
        nPos += sizeof(msepoch_t);
        *(msepoch_t*)(buffer+nPos) = _lastChangeTime;
        nPos += sizeof(msepoch_t);
        *(int*)(buffer+nPos) = _quality;
        nPos += sizeof(int);
//        *(T*)(buffer+nPos) = _value;
//        nPos += sizeof(T);
        nPos+=toBufferValue(buffer+nPos,_value);
        return nPos;
    }

    //时标 品质 值
    inline int getSizeSHM()
    {
        return (MeasureBase::size()+sizeof(msepoch_t)*GC_MEASURE_TM_COUNT+sizeof(int)+sizeofValue());
    }

protected:
    virtual void toReport(std::vector<std::string>& sReports) const
    {
        sReports.push_back(std::string("Value=" + CxString::toString(_value)));
        sReports.push_back(std::string("Quality=" + CxString::toString(_quality)));
        sReports.push_back(std::string("DateTime=" + CxTime::toString(_lastChangeTime)));
    }

    virtual bool valueCanChange(const T & newValue){ return true; }

    virtual void valueChanged(const T & oldValue, const T & newValue) = 0;

    virtual void valueChanging(const T& newValue) { }

    virtual bool valueIsChange(const T & oldValue, const T & newValue)= 0;

    //值定义大小
    virtual int  sizeofValue() = 0;

    //to Buffer
    virtual int toBufferValue(uchar *buffer,T& value) = 0;

    //to Buffer
    virtual int fromBufferValue(uchar *buffer,T& value) = 0;

protected:
    //基本信息
    msepoch_t _lastRefreshTime; //8
    msepoch_t _lastChangeTime;  //8
    int _quality; //品质
    T _value; //值
    //扩展信息
    T _originalValue;
    T _maxValue;
    T _minValue;
    T _defaultValue;
    T _oldValue;
    T _newValue;
    int _lastRefreshSourceId;
    int _lastRefreshReasonId;
    int _lastChangeSourceId;
    int _lastChangeReasonId;
    int _outLineInterval;
    bool _bChange;

};



template<class MeasureSon, typename T>
class MeasureSubjectTemplate
{
protected:
    virtual bool valueCanChange(const MeasureSon* oMeasure, const T& newValue){ return true; }

    virtual void valueChanging(const MeasureSon* oMeasure, const T& newValue){}

    virtual void valueChanged(const MeasureSon* oMeasure, const T& oldValue, const T& newValue){}

    template<class , typename> friend class MeasurePublisherTemplate;

};



template<class MeasureSon, typename T>
class MeasurePublisherTemplate
{
public:
    MeasurePublisherTemplate(){
        _subjects = new std::vector<MeasureSubjectTemplate<MeasureSon, T>*>();
    }

    virtual ~MeasurePublisherTemplate(){
        delete _subjects;
    }

    void addObserver(MeasureSubjectTemplate<MeasureSon, T>* oSubject)
    {
        _subjects->push_back(oSubject);
    }

    void removeObserver(MeasureSubjectTemplate<MeasureSon, T>* oSubject)
    {
        CxContainer::remove(*_subjects, oSubject);
    }

    virtual bool valueCanChange(const MeasureSon* oMeasure, const T& newValue)
    {
        bool bCan = true;
        for (size_t i = 0; i < _subjects->size(); ++i) {
            MeasureSubjectTemplate<MeasureSon, T>* oSubject = _subjects->at(i);
            bCan = oSubject->valueCanChange(oMeasure, newValue);
            if ( ! bCan )
            {
                return false;
            }
        }
        return bCan;
    }

    virtual void valueChanging(const MeasureSon* oMeasure, const T& newValue)
    {
        for (size_t i = 0; i < _subjects->size(); ++i) {
            MeasureSubjectTemplate<MeasureSon, T>* oSubject = _subjects->at(i);
            oSubject->valueChanging(oMeasure, newValue);
        }
    }

    virtual void valueChanged(const MeasureSon* oMeasure, const T& oldValue, const T& newValue){
        for (size_t i = 0; i < _subjects->size(); ++i) {
            MeasureSubjectTemplate<MeasureSon, T>* oSubject = _subjects->at(i);
            oSubject->valueChanged(oMeasure, oldValue, newValue);
        }
    }

private:
    std::vector<MeasureSubjectTemplate<MeasureSon, T>*> * _subjects;

};

#endif // !defined(MEASURE_TEMPLATE_H)
