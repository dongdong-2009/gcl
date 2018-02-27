#ifndef WORKER_BASE_H
#define WORKER_BASE_H


#include "protocol_global.h"

#include "worker_strings.h"

#include "vdi_declspec.h"

//工作者
class GM_GCL_VDI_API WorkerBase
{

    GM_TYPEFULLNAME_BASE(WorkerStrings::Worker())

public:

    WorkerBase() {}

    virtual ~WorkerBase() {}

    void toContext(CxISetSkv& context) const;

    void fromContext(const CxIGetSkv& context);

    std::string reportSelf();

    inline const std::string& workerName() const { return _workerName; }
    //    inline void setName(const std::string& value) { _name = value; }

    inline const std::string& workerCode() const { return _workerCode; }
    inline void setWorkerCode(const std::string& value) { _workerCode= value; }

    inline const std::string& extension() const { return _extension; }
    inline void setExtension(const std::string& value) { _extension= value; }

protected:
    virtual void toReport(std::vector<std::string>& sReports) const {}

private:
    std::string _workerName;
    std::string _workerCode;
    std::string _extension;

};



class GM_GCL_VDI_API WorkerTableBase
{
public:
    WorkerTableBase() {}

    virtual ~WorkerTableBase() {}

    virtual std::string typeFullName() const = 0;

    virtual std::string typeSimpleName() const = 0;

    inline const std::string& tableName() const { return _tableName; }
    inline void setTableName(const std::string& sTableName) { _tableName = sTableName; }

    virtual const std::vector<WorkerBase*>* workerBases() const = 0;

    virtual void loadFromFile(const std::string& sFilePath) = 0;

    virtual void saveToFile(const std::string& sFilePath) const = 0;

    virtual void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    std::vector<std::string> reportSelf();

    virtual void refreshStatisticses() { }

    virtual void sortWorkers() {}

protected:
    virtual void toReport(std::vector<std::string>& sReports) const {}

private:
    std::string _tableName;

};



template<class WorkerSon>
class WorkerTableTemplate : public WorkerTableBase
{
public:
    WorkerTableTemplate(){
        _workerSons = new std::vector<WorkerSon*>();
    }
    ~WorkerTableTemplate(){
        DeleteAllWorker();
        delete _workerSons;
    }

    static std::string TYPEFULLNAME() {
        return WorkerSon::TYPEFULLNAME();
    }

    static std::string TYPESIMPLENAME() {
        return WorkerSon::TYPESIMPLENAME();
    }

    std::string typeFullName() const {
        return WorkerSon::TYPEFULLNAME();
    }

    std::string typeSimpleName() const {
        return WorkerSon::TYPESIMPLENAME();
    }

    const std::vector<WorkerBase*>* workerBases() const {
        return reinterpret_cast<std::vector<WorkerBase*>*>(_workerSons);
    }

    const std::vector<WorkerSon*>* workers() const {
        return _workerSons;
    }

    void loadFromFile(const std::string& sFilePath){
        std::vector<WorkerSon*> workerSons = CxSkverManager::loadObjects<WorkerSon>(sFilePath);
        DeleteAllWorker();
        appendWorkers(workerSons);
    }

    void saveToFile(const std::string& sFilePath) const {
        CxSkverManager::saveObjects(sFilePath, _workerSons);
    }

    WorkerSon* workerByName(const std::string& sName) const {
        const std::vector<WorkerSon*>* oWorkers = _workerSons;
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            WorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->workerName() == sName){
                return oWorker;
            }
        }
        return NULL;
    }

    WorkerSon* workerByCode(const std::string& sCode) const {
        const std::vector<WorkerSon*>* oWorkers = _workerSons;
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            WorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->workerCode() == sCode){
                return oWorker;
            }
        }
        return NULL;
    }

    WorkerSon* workerByExtension(const std::string& sExtension) const {
        const std::vector<WorkerSon*>* oWorkers = _workerSons;
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            WorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->extension() == sExtension){
                return oWorker;
            }
        }
        return NULL;
    }

protected:
    void appendWorkers(const std::vector<WorkerSon*>& workerSons)
    {
        _workerSons->insert(_workerSons->end(), workerSons.begin(), workerSons.end());
        refreshStatisticses();
    }

    void deleteWorkers(const std::vector<WorkerSon*>& workerSons)
    {
        for(typename std::vector<WorkerSon*>::iterator it = _workerSons->begin(); it != _workerSons->end();)
        {
            if (std::find(workerSons.begin(), workerSons.end(), * it) != workerSons.end())
            {
                delete (* it);
                _workerSons->erase(it);
                continue;
            }
            ++it;
        }

//        for (int i = _workerSons->size()-1; i >= 0; --i)
//        {
//            WorkerSon* oWorker = _workerSons->at(i);
//            if (workerSons.indexOf(oWorker) > -1)
//            {
//                _workerSons->removeAt(i);
//                delete oWorker;
//            }
//        }
        refreshStatisticses();
    }

    virtual void toReport(std::vector<std::string>& sReports)
    {
        sReports.push_back(std::string("Worker_typeFullName=") + (WorkerSon::TYPEFULLNAME()));
        const std::vector<WorkerBase*> * oWorkers = workerBases();
        sReports.push_back(CxString::format("Worker_Count=%d", oWorkers->size()));
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            WorkerBase* oWorker = oWorkers->at(i);
            sReports.push_back(oWorker->reportSelf());
        }
    }

    virtual void sortWorkers()
    {
        std::sort(_workerSons->begin(), _workerSons->end(), WorkerSon::lessThan);
    }

private:
    std::vector<WorkerSon*>* _workerSons;

    void DeleteAllWorker(){
        GM_PLISTO_DELETEALL(_workerSons, WorkerSon)
    }

};
#endif // !defined(WORKER_BASE_H)
