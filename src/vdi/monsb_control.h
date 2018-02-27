#ifndef MONSBCONTROL_H
#define MONSBCONTROL_H


#include "monsb_measure.h"


///MeasureControl
struct MonsbControl
{
    MonsbControl(const MonsbMeasure* t1, const int& t2, const int& t3) :
        measure(t1), fromValue(t2), toValue(t3), runType(0), waitLong(0), extension("")
    {}

    MonsbControl(const MonsbMeasure* t1, const int& t2, const int& t3, int iRunType, int iWaitLong, const std::string & sExtension) :
        measure(t1), fromValue(t2), toValue(t3), runType(iRunType), waitLong(iWaitLong), extension(sExtension)
    {}

    MonsbControl &operator=(const MonsbControl &other)
    {
        measure = other.measure; toValue = other.toValue; runType=other.runType; waitLong=other.waitLong; extension=other.extension; return *this;
    }

    MonsbControl(const MonsbControl &other) :
        measure(other.measure), toValue(other.toValue), runType(other.runType), waitLong(other.waitLong), extension(other.extension)
    {}

    const MonsbMeasure* measure;
    int fromValue;
    int toValue;
    int runType; //运行方式：检查、发控制
    int waitLong;
    std::string extension;

};

inline bool operator==(const MonsbControl &p1, const MonsbControl &p2)
{ return p1.measure == p2.measure && p1.fromValue == p2.fromValue && p1.toValue == p2.toValue && p1.runType == p2.runType; }

inline bool operator!=(const MonsbControl &p1, const MonsbControl &p2)
{ return !(p1 == p2); }

inline bool operator<(const MonsbControl &p1, const MonsbControl &p2)
{ return p1.measure < p2.measure || (!(p2.measure < p1.measure) && p1.toValue < p2.toValue); }

inline bool operator>(const MonsbControl &p1, const MonsbControl &p2)
{ return p2 < p1; }

inline bool operator<=(const MonsbControl &p1, const MonsbControl &p2)
{ return !(p2 < p1); }

inline bool operator>=(const MonsbControl &p1, const MonsbControl &p2)
{ return !(p1 < p2); }



///measureOperate
class MonsbOperate
{
public:
    inline const MonsbMeasure* measure() const { return _measure; }
    inline void setMeasure(const MonsbMeasure* value) { _measure = value; }

    inline const int& fromValue() const { return _fromValue; }
    inline void SetFromValue(const int& value) { _fromValue = value; }

    inline const int& toValue() const { return _toValue; }
    inline void setToValue(const int& value) { _toValue = value; }

    inline const std::vector<MonsbControl>& controls() const { return _controls; }
    inline void setControls(const std::vector<MonsbControl>& value) { _controls = value; }

    inline const std::vector<MonsbControl>& cancels() const { return _cancels; }
    inline void setCancels(const std::vector<MonsbControl>& value) { _cancels = value; }

private:
    const MonsbMeasure* _measure;
    int _fromValue;
    int _toValue;
    std::vector<MonsbControl> _controls;
    std::vector<MonsbControl> _cancels;

};


class MonsbOperateManager : public IMonsbManagerSubject
{
public:
    //*保存成文件
    static void saveToFile(const std::string& sFilePath);

    //*从文件加载
    static void loadFromFile(const std::string& sFilePath);

    static const std::vector<MonsbOperate*>* measureOperates();

    static std::vector<MonsbOperate*> measureOperates(int iMeasureSonType);

    static void initializeProcess();

    static const MonsbOperate* measureOperate(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    static const std::vector<MonsbControl>& controls(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    static void setControls(const MonsbMeasure* oMeasure, int fromValue, int toValue, const std::vector<MonsbControl>& controls);

    static const std::vector<MonsbControl>& cancels(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    static void setCancels(const MonsbMeasure* oMeasure, int fromValue, int toValue, const std::vector<MonsbControl>& cancels);

private:
    static MonsbOperateManager* createInstance();

    static void deleteAllControls();

    static MonsbOperate* getMeasureOperate(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    static MonsbOperate* createMeasureOperate(const MonsbMeasure* oMeasure, int fromValue, int toValue);

    //*MeasureControlTemplate （子项）单项操作转成文本
    static std::string measureControlsToString(const std::vector<MonsbControl>& controls);

    //*MeasureControlTemplate （子项）单项操作从文本转出
    static std::vector<MonsbControl> measureControlsFromString(const std::string& sEntry);

    //*Measure变化后初始操作列表（变化包括：measure的删除、新增）
    static void initMeasureOperates(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iMeasureSonType);

    static void initMeasureOperates(const std::vector<const MonsbMeasure*>& measures);

    //*初始化遥信量的操作
    static void initMeasureControl(const MonsbMeasure* oMeasure, std::vector<MonsbOperate*>& measureOperates);

    static void InitMeasureCancel(const MonsbMeasure* oMeasure, std::vector<MonsbOperate*>& measureCancels);

protected:
    void measures_prepareChange(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& measuresChangeType, int iMeasureSonType);

    void measures_afterLoad(const std::vector<const MonsbMeasure*>& measures);

};


#endif // MONSBCONTROL_H
