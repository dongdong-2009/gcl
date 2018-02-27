#ifndef MEASURE_MANAGER_TEMPLATE_H
#define MEASURE_MANAGER_TEMPLATE_H


#include "measure_base.h"


enum MeasureChangeTypeEnum
{
    MeasureChangeType_0,
    MeasureChangeType_New,
    MeasureChangeType_Delete,
    MeasureChangeType_Edit
};

template<class MeasureSon, typename T>
class MeasureManagerSubjectTemplate
{
protected:
    virtual void measures_prepareChange(const std::map<const MeasureSon*, MeasureChangeTypeEnum>& measuresChangeType, int iMeasureSonType) {}

    virtual void measures_afterLoad(const std::vector<const MeasureSon*>& measures) {}

    virtual void measures_valueChanged(const std::map<const MeasureSon*, T>& measureOldValues, int iMeasureSonType, int iSourceId, int iTag) {}

    template<class, typename> friend class MeasureManagerTemplate;
};


//提供 查找 过虑 统计 排序 等表的功能
template<class MeasureSon, typename T>
class MeasureManagerTemplate
{
public:
    //support : new load
    //support : update load
    static void loadFromFile(const std::string& sFilePath)
    {
        std::vector<MeasureSon*>* oMeasures = MeasureSon::measuresContainer();
        std::vector<MeasureSon*> measuresLoad = CxSkverManager::loadObjects<MeasureSon>(sFilePath);
        for (size_t i =0;i< measuresLoad.size(); ++i)
        {
            MeasureSon* o2 = measuresLoad.at(i);
            MeasureSon* o = measure(o2->measureId());
            if (o)
            {
                o->setMeasureName(o2->measureName());
                o->setContainerId(o2->containerId());
                o->setContainerType(o2->containerType());
                o->setOriginalValue(o2->originalValue());
                o->setMinValue(o2->minValue());
                o->setMaxValue(o2->maxValue());
                o->setDefaultValue(o2->defaultValue());
                o->setUrl(o2->url());
                o->setProperty(o2->property());
            }
            else
            {
                allocateMeasureId(o2);
                oMeasures->push_back(o2);
                measuresLoad[i] = NULL;
            }
        }
        sortMeasures(oMeasures);
        std::vector<const MeasureSon*> oMeasures2;
        for (size_t i = 0; i < oMeasures->size(); ++i)
        {
            oMeasures2.push_back(oMeasures->at(i));
        }
        MeasureSon::raiseMeasuresAfterLoad(oMeasures2);
        for (size_t i = 0; i < measuresLoad.size(); ++i)
        {
            MeasureSon* oMeasure = measuresLoad.at(i);
            if(oMeasure)delete oMeasure;
        }
    }

    static void loadFromAuto(int iMeasureIdBegin, int iMeasureCount)
    {
        std::vector<MeasureSon*>* oMeasures = MeasureSon::measuresContainer();
        for (size_t i = 0; i < iMeasureCount; ++i)
        {
            int iMid = iMeasureIdBegin + i;
            std::string sMeasureName = CxString::format("%s%06d", MeasureSon::TYPESIMPLENAME().c_str(), iMid);
            MeasureSon * oMeasure = new MeasureSon();
            oMeasure->setMeasureId(iMid);
            oMeasure->setMeasureName(sMeasureName);
            oMeasures->push_back(oMeasure);
        }
        sortMeasures(oMeasures);
        std::vector<const MeasureSon*> oMeasures2;
        for (size_t i = 0; i < oMeasures->size(); ++i)
        {
            oMeasures2.push_back(oMeasures->at(i));
        }
        MeasureSon::raiseMeasuresAfterLoad(oMeasures2);
    }

    static void loadFromFile(const std::string& sFilePath, int iMeasureSonType)
    {
        std::vector<MeasureSon*>* oMeasures = MeasureSon::measuresContainer();
        //load
        std::vector<MeasureSon*> measuresLoad = CxSkverManager::loadObjects<MeasureSon>(sFilePath);
        std::vector<MeasureSon*> measuresDelete = measures(iMeasureSonType);
        std::map<const MeasureSon*, MeasureChangeTypeEnum> measuresChangeType;
        //process
        for (size_t i = 0;i<measuresLoad.size();++i)
        {
            MeasureSon* o2 = measuresLoad.at(i);
            MeasureSon* o = measure(o2->measureId());
            if (o)
            {
                o->setMeasureName(o2->measureName());
                o->setContainerId(o2->containerId());
                o->setContainerType(o2->containerType());
                o->setOriginalValue(o2->originalValue());
                o->setMinValue(o2->minValue());
                o->setMaxValue(o2->maxValue());
                o->setDefaultValue(o2->defaultValue());
                o->setUrl(o2->url());
                o->setProperty(o2->property());

                CxContainer::remove(measuresDelete, o);
//                measuresDelete.removeAll(o);
                measuresChangeType[o] = MeasureChangeType_Edit;
            }
            else
            {
                allocateMeasureId(o2);
                oMeasures->push_back(o2);
                measuresLoad[i] = NULL;
//                measuresLoad.removeAt(i);
                measuresChangeType[o2] = MeasureChangeType_New;
            }
        }
        for (size_t i = 0; i < measuresDelete.size(); ++i)
        {
            MeasureSon* o3 = measuresDelete.at(i);
            measuresChangeType[o3] = MeasureChangeType_Delete;
        }
        //publish
        MeasureSon::raiseMeasuresPrepareChange(measuresChangeType, iMeasureSonType);
        for (size_t i = 0; i < measuresDelete.size(); ++i)
        {
            MeasureSon* o3 = measuresDelete.at(i);
            CxContainer::remove(*oMeasures,o3);
            delete o3;
        }
        sortMeasures(oMeasures);
        //end
        for (size_t i = 0; i < measuresLoad.size(); ++i)
        {
            MeasureSon* oMeasure = measuresLoad.at(i);
            delete oMeasure;
        }
    }

    static void saveToFile(const std::string& sFilePath)
    {
        CxSkverManager::saveObjects<MeasureSon>(sFilePath, MeasureSon::measuresContainer());
    }

    static const std::vector<MeasureSon*>* measures()
    {
        return MeasureSon::measuresContainer();
    }

    static void getMapUrl(std::map<std::string,int> &mapUrl)
    {
         std::vector<MeasureSon*>* o = MeasureSon::measuresContainer();
         if(o==NULL) return;

         std::string sUrl="";
         int nSize = o->size();
         for(int i=0;i<nSize;i++)
         {
             MeasureSon * obj = o->at(i);
             if(obj==NULL) continue;
             sUrl = obj->url();
             if(sUrl.length()<1) sUrl = obj->measureName();
             mapUrl[sUrl] = obj->measureId();
         }
    }

    static MeasureSon* measure(int iMeasureId)
    {
        GM_PLISTO_FIND(MeasureSon::measuresContainer(), MeasureSon, measureId(), iMeasureId)
    }

    static MeasureSon* measure(int iMeasureId, const MeasureSon* oExcludeMeasure)
    {
        std::vector<MeasureSon*>* oMeasures = MeasureSon::measuresContainer();
        for (size_t i = 0; i < oMeasures->size(); ++i){
            MeasureSon* o = oMeasures->at(i);
            if ( o->measureId() == iMeasureId && o != oExcludeMeasure ){
                return o;
            }
        }
        return NULL;
    }

    static std::vector<MeasureSon*> measures(const std::vector<int>& measureIds)
    {
        std::vector<MeasureSon*> measures;
        for (size_t i = 0; i < measureIds.size(); ++i)
        {
            MeasureSon* oMeasure = measure(measureIds.at(i));
            if (oMeasure)
                measures.push_back(oMeasure);
        }
        return measures;
    }

    static std::vector<MeasureSon*> measures(int iMeasureSonType)
    {
        std::vector<MeasureSon*> measures;
        std::vector<MeasureSon*>* oMeasures = MeasureSon::measuresContainer();
        for (size_t i = 0; i < oMeasures->size(); ++i)
        {
            MeasureSon* oMeasure = oMeasures->at(i);
            if (oMeasure->measureSonType() == iMeasureSonType)
                measures.push_back(oMeasure);
        }
        return measures;
    }

    static std::map<MeasureSon*, T> measureValues(const std::vector<int>& measureIds)
    {
        std::map<MeasureSon*, T> measureValues;
        for (size_t i = 0; i < measureIds.size(); ++i)
        {
            MeasureSon* oMeasure = measure(measureIds.at(i));
            if (oMeasure)
            {
                measureValues[oMeasure] = oMeasure->value();
            }
        }
        return measureValues;
    }

    static std::map<MeasureSon*, T> measureValues(const std::map<int, T>& measureIdValues)
    {
        std::map<MeasureSon*, T> measureValues;
        for (typename std::map<int, T>::const_iterator it = measureIdValues.begin(); it != measureIdValues.end(); ++it)
        {
            MeasureSon* oMeasure = measure(it->first);
            if (oMeasure)
            {
                measureValues[oMeasure] = it->second;
            }
        }
        return measureValues;
    }

    static std::vector<std::pair<const MeasureSon*, T> > measureValueList(const std::vector<int>& measureIds, const T& t)
    {
        std::vector<std::pair<const MeasureSon*, T> > measureValues;
        for (size_t i = 0; i < measureIds.size(); ++i)
        {
            const MeasureSon* oMeasure = measure(measureIds.at(i));
            if (oMeasure)
            {
                measureValues.push_back(std::pair<const MeasureSon*, T>(oMeasure, t));
            }
        }
        return measureValues;
    }

    static std::vector<std::pair<const MeasureSon*, T> > measureValueList(const std::vector<const MeasureSon*> & measures, const T& t)
    {
        std::vector<std::pair<const MeasureSon*, T> > measureValues;
        for (size_t i = 0; i < measures.size(); ++i)
        {
            const MeasureSon * oMeasure = measures.at(i);
            if (oMeasure)
            {
                measureValues.push_back(std::pair<const MeasureSon*, T>(oMeasure, t));
            }
        }
        return measureValues;
    }

    static void receiveValue(MeasureSon* oMeasure, T value, int iSourceId, int iReason = 0)
    {
        oMeasure->setValue(value, iSourceId, iReason);
    }

    static void receiveValue(int iMeasureId , T value, int iSourceId, int iReason = 0)
    {
        MeasureSon* oMeasure = measure(iMeasureId);
        if (oMeasure)
            receiveValue(oMeasure, value, iSourceId, iReason);
    }

    static void receiveValues(const std::map<MeasureSon*, T>& measureValues, int iMeasureSonType, int iSourceId, int iReason = 0)
    {
        MeasureSon* oMeasureSon = NULL;
        for (typename std::map<MeasureSon*, T>::const_iterator it = measureValues.begin(); it != measureValues.end(); ++it)
        {
            oMeasureSon = it->first;
            oMeasureSon->setValue(it->second, iSourceId, iReason);
        }
    }

    static std::vector<std::string> reportSelf()
    {
        std::vector<MeasureSon*>* oMeasures = MeasureSon::measuresContainer();
        std::vector<std::string> sReports;
        sReports.push_back("*** Measure Manager Template Report ***");
        sReports.push_back(std::string("TypeSimpleName=") + (MeasureSon::TYPESIMPLENAME()));
        sReports.push_back(CxString::format("Count=%d",oMeasures->size()));
        for (size_t i = 0; i < oMeasures->size(); ++i)
        {
            MeasureSon* oMeasure = oMeasures->at(i);
            sReports.push_back(oMeasure->reportSelf());
        }
        return sReports;
    }

    static void describeSelf(CxDescribeUnit* oDescribeUnit) {
        CxDescribeString* oMeasureNameDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryMeasureName);
        oMeasureNameDescribe->setRegularExpression(GclConfig::regstringName());
        oMeasureNameDescribe->setMinLength(1);
        oMeasureNameDescribe->setMaxLength(255);
        CxDescribeInteger* oMeasureIdDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryMeasureId);
        oMeasureIdDescribe->setIsReadOnly(true);
        oMeasureIdDescribe->setMinValue(MeasureSon::minMeasureId());
        oMeasureIdDescribe->setMaxValue(MeasureSon::maxMeasureId());
        CxDescribeString* oMeasureUrlDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryUrl);
        oMeasureUrlDescribe->isReadOnly();
        CxDescribeInteger* oContainerIdDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryContainerId);
        oContainerIdDescribe->initValue(ci_defaultMeasureContainerId);
        CxDescribeInteger* oContainerTypeDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryContainerType);
        oContainerTypeDescribe->initValue(ci_defaultMeasureContainerId);
        CxDescribeString* oMeasurePropertyDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryProperty);
        oMeasurePropertyDescribe->isReadOnly();
    }

private:
    static void allocateMeasureId(MeasureSon* oMeasure)
    {
        int iMaxMeasureIdCurrent = MeasureSon::currentMaxMeasureId();
        int iMeasureId = oMeasure->measureId();
        if (iMeasureId < MeasureSon::minMeasureId() || iMeasureId > MeasureSon::maxMeasureId()){
            ++iMaxMeasureIdCurrent;
            oMeasure->setMeasureId(iMaxMeasureIdCurrent);
        } else if ( iMeasureId > iMaxMeasureIdCurrent ){
            iMaxMeasureIdCurrent = iMeasureId;
        }
        MeasureSon::setCurrentMaxMeasureId(iMaxMeasureIdCurrent);
    }

    static void sortMeasures(std::vector<MeasureSon*>* oMeasures)
    {
        std::sort(oMeasures->begin(), oMeasures->end(), lessThanByMeasureId);
    }

    static bool lessThanByMeasureId(MeasureSon* one, MeasureSon* two) {
        return two->measureId() > one->measureId();
    }

};
#endif // !defined(MEASURE_MANAGER_TEMPLATE_H)
