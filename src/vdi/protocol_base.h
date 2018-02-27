#ifndef PROTOCOL_BASE_H
#define PROTOCOL_BASE_H

#include "protocol_global.h"
#include "monsb_measure.h"
#include "ycadd_measure.h"
#include "straw_measure.h"
#include "cases_base.h"
#include "cycle_base.h"
#include "worker_base.h"

#include <ccxx/cxdescribe.h>
#include <common/gccommon.h>


class ProtocolBase;


class ProtocolFactoryBase : public CxFactoryTemplate<ProtocolBase>
{

};


class GM_GCL_VDI_API ProtocolBase : public ICxTimerNotify, public CxIChannelSubject, public IMonsbManagerSubject, public IYcaddManagerSubject, public IStrawManagerSubject
{
public:
    static std::vector<CxFactoryTemplate<ProtocolBase> *> * factoriesContainer();

    static void factoriesCreateAndRegister();

public:
    ProtocolBase();

    virtual ~ProtocolBase();

    virtual const std::string& typeSimpleName() const = 0;

    virtual void toContext(CxISetSkv& context) const;

    virtual void fromContext(const CxIGetSkv& context);

    virtual void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    std::vector<std::string> reportSelf() const;

    inline const std::string& protocolName() const { return _protocolName; }
    inline void setProtocolName(const std::string& value) { _protocolName = value; }

    inline int protocolId() const { return _protocolId; }
    inline void setProtocolId(int value) { _protocolId = value; }

    inline int terminalId() const {return _terminalId;}
    inline void setTerminalId(int value) { _terminalId = value; }



    inline CxInterinfoOutStream outProtocolPrompt() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Protocol); r.source(_protocolId); r << CxGlobal::promptString << typeSimpleName() << " Protocol : "; return r;
    }

#if defined(GM_DEBUG)
    inline CxInterinfoOutStream outProtocolDebug() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Protocol); r.source(_protocolId); r << CxGlobal::debugString << typeSimpleName() << " Protocol : "; return r;
    }
#else
#undef outProtocolDebug
    inline CxInterinfoNoOutStream outProtocolDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outProtocolDebug_MACRO while (false) outProtocolDebug
#define outProtocolDebug CX_NO_outProtocolDebug_MACRO
#endif


    inline const CxChannelBase* channel() const { return _channel; }
    void setChannel(CxChannelBase* oChannel);

    void channelFromContext(const CxIGetSkv& context);

    void start();

    void stop();

    inline bool isStart() const { return _isStart; }

    inline const std::vector<CasesBase*>* caseses() const { return _caseses; }

    CasesBase* cases(const std::string& sTypeSimpleName) const;

    //获取案例模型
    template<class CasesSonClass>
    CasesSonClass* casesSon() const
    {
        for (size_t i = 0; i < _caseses->size(); ++i)
        {
            CasesBase* o = _caseses->at(i);
            if ( o->typeSimpleName() == CasesSonClass::TYPESIMPLENAME() )
            {
                return reinterpret_cast<CasesSonClass*>(o);
            }
        }
        return NULL;
    }

    inline const std::vector<WorkerTableBase*>* workerTables() const { return _workerTables; }

    WorkerTableBase* workerTable(const std::string& sTableName) const;

    template<class WorkerTableSonClass>
    WorkerTableSonClass* workerTableSon(const std::string& sTableName) const {
        for (size_t i = 0; i < _workerTables->size(); ++i){
            WorkerTableBase* o = _workerTables->at(i);
            if (o->tableName() == sTableName && o->typeSimpleName() == WorkerTableSonClass::TYPESIMPLENAME() ){
                return reinterpret_cast<WorkerTableSonClass*>(o);
            }
        }
        return NULL;
    }

    void loadTable(const std::string& sTableName, const std::string& sFilePath);

    void saveTable(const std::string& sTableName, const std::string& sFilePath) const;

    virtual void describeWorkerTable(CxDescribeUnit* oDescribeUnit, WorkerTableBase* oWorkerTable) const;

    inline bool isAutoCreateWorkerAtDeal() { return _isAutoCreateWorkerAtDeal; }

    inline void setIsAutoCreateWorkerAtDeal(bool value) { _isAutoCreateWorkerAtDeal = value; }

    inline ProtocolRoleEnum protocolRole() const { return _protocolRole; }

    inline void setProtocolRole(ProtocolRoleEnum value) { _protocolRole = value; }

    inline const msepoch_t & receivedProtocolDataTime() const { return _receivedProtocolDataTime; }

    inline bool isOnline() const { return CxTime::milliSecondDifferToNow(_receivedProtocolDataTime) < _outLineInterval; }

    inline const std::string& infoTitle() const { return _infoTitle; }

    inline void setInfoTitle(const std::string& value) { _infoTitle = value;}

    inline const std::string& protocolTitle()const{return _protocolTitle;}

    inline void setProtocolTitle(const std::string& value) { _protocolTitle = value;}

protected:
    virtual void toReport(std::vector<std::string>& sReports) const {}

    //ICxTimerTimeOut
    void timer_timeOut(const CxTimer* oTimer);

    //IChannelSubject
    bool channel_canChangeConnect(const CxChannelBase* oChannel, bool bOldConnect, bool bNewConnect) { return true; }

    void channel_connectChanged(const CxChannelBase* oChannel) { if (oChannel->connected()) { channelAfterOpen(oChannel); } else { channelAfterClose(oChannel); } }

    void channel_beforeDelete(const CxChannelBase *oChannel);

    void channel_receivedData(const uchar* pData, int iLength, void * oSource) { protocolReceivedData(pData, iLength); }

    //IMonsbManagerSubject
    void measures_prepareChange(const std::map<const MonsbMeasure*, MeasureChangeTypeEnum>& monsbChangeTypes, int iMeasureSonType);

    void measures_afterLoad(const std::vector<const MonsbMeasure*>& monsbs) {}

    void measures_valueChanged(const std::map<const MonsbMeasure*, int>& monsbOldValues, int iMeasureSonType, int iSourceId, int iTag);

    //IYcaddManagerSubject
    void measures_prepareChange(const std::map<const YcaddMeasure*, MeasureChangeTypeEnum>& ycaddChangeTypes, int iMeasureSonType);

    void measures_afterLoad(const std::vector<const YcaddMeasure*>& ycadds) {}

    void measures_valueChanged(const std::map<const YcaddMeasure*, double>& ycaddOldValues, int iMeasureSonType, int iSourceId, int iTag);

    //IStrawManagerSubject
    void measures_prepareChange(const std::map<const StrawMeasure*, MeasureChangeTypeEnum>& strawChangeTypes, int iMeasureSonType);

    void measures_afterLoad(const std::vector<const StrawMeasure*>& straws) {}

    void measures_valueChanged(const std::map<const StrawMeasure*, std::string>& strawOldValues, int iMeasureSonType, int iSourceId, int iTag);

    virtual void protocolTimerTimeOut(const CxTimer* oTimer) {}

    virtual void channelAfterOpen(const CxChannelBase* oChannel) {}

    virtual void channelAfterClose(const CxChannelBase* oChannel) {}

    virtual void protocolAfterStart() {}

    virtual void protocolAfterStop() {}

    virtual void protocolReceivedData(const uchar* pData, int iLength) {}

    virtual void monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag) {}

    virtual void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag) {}

    virtual void strawsValueChanged(const std::map<const StrawMeasure *, std::string> &strawOldValues, StrawTypeEnum eStrawType, int iSource, int iTag) {}

    virtual void checkProtocol() {}

    virtual void protocolOutLine() {}

    //任务
    virtual void doTask(GCCommon::GCDataElement dt) {}
    //clock
    virtual void clock() {}


    template<class ProtocolSonClass, class CasesSonClass>
    CasesSonClass* createAndRegisterCases(ProtocolSonClass* oProtocol)
    {
        CasesSonClass* oCases = new CasesSonClass();
        oCases->setProtocol(oProtocol);
        _caseses->push_back(oCases);
        return oCases;
    }

    template<class ProtocolSonClass, class CycleSonClass>
    CycleSonClass* createAndRegisterCycle(ProtocolSonClass* oProtocol, int iMeasureSonType = -1)
    {
        CycleSonClass* oCycle = new CycleSonClass();
        oCycle->setProtocol(oProtocol);
        oCycle->setMeasureSonType(iMeasureSonType);
        _cycles->push_back(oCycle);
        return oCycle;
    }

    template<class WorkerTableSonClass>
    WorkerTableSonClass* createAndRegisterWorkerTable(const std::string& sTableName)
    {
        WorkerTableSonClass* oWorkerTable = new WorkerTableSonClass();
        oWorkerTable->setTableName(sTableName);
        _workerTables->push_back(oWorkerTable);
        return oWorkerTable;
    }

    template<class WorkerTableSonClass, class ProtocolSonClass>
    WorkerTableSonClass* createAndRegisterWorkerTable(const std::string& sTableName, int iMeasureSonType)
    {
        WorkerTableSonClass* oWorkerTable = createAndRegisterWorkerTable<WorkerTableSonClass>(sTableName);
        oWorkerTable->setMeasureSonType(iMeasureSonType);
        return oWorkerTable;
    }

    void unLoadChannel();

    int writeData(const char* pData, int iLength);

    int writeData(const uchar* pData, int iLength);

    int writeBytes(const std::vector<uchar>& bytes);

    int writeText(const std::string& sText);

    void receivedProtocolDataNow() { _receivedProtocolDataTime = CxTime::currentSystemTime(); }

    void setConnect(bool bState)
    {
        _bConnect = bState;
        //刷新状态
        if(GclConfig::getChannelBaseAddr()>0)
        {

        }
    }
    void setLastTime(msepoch_t tmLast){_lastTime = tmLast;}
    //数据压榨
    void push(const uchar *pData, int iLength);
    void push(const char *pData, int iLength);
    //发送周期
    void setSendInterval(uint32 nVal){_outInterval = nVal;}
    uint32 getSendInterval(){return _outInterval;}
    void startSendTm(uint32 nVal)
    {
        _outInterval = nVal;
        if(_sendTm!=NULL)
        {
            _sendTm->stop();
            _sendTm->setInterval(_outInterval);
            _sendTm->start();
        }
    }
protected:
    int _terminalId;//终端ID
private:
    int _protocolId;
    std::string _protocolName;
    CxChannelBase* _channel;
    std::vector<CasesBase*>* _caseses;
    std::vector<CycleBase*>* _cycles;
    std::vector<WorkerTableBase*>* _workerTables;
    bool _isStart;
    bool _isAutoCreateWorkerAtDeal;
    ProtocolRoleEnum _protocolRole;
    msepoch_t _receivedProtocolDataTime;
    int _outLineInterval;
    std::string _infoTitle;
    std::string _protocolTitle;
protected:
    CxTimer* _sendTm;
    CxTimer* _checkProtocolTm;
    CxTimer* _workTm;

    bool _bConnect;
    msepoch_t _lastTime;
    int _outInterval;
    int _workInterval;
    std::string _urlPath;
protected:
    GCCommon::GCDataEmQueue  _qDataEm;
    GCCommon::GCDataElement  _dtIn;
    GCCommon::GCDataElement  _dtOut;

protected:
//    GCMsgQueue               _qMsg; //消息队列
    int                      _mqPort;
    std::string                  _mqIp;

protected:
    char*                     _pBuffer;
    int                       _bufLen;
    //接收缓冲区
    CxSafeBuffer  _revBuffer;
    int           _failTimes;

protected:
    //注册
    void regToRTDB();
    //重置
    void reset(int maxTimes=10);

public:
    //发送消息
    void sendMsg(uint32 cmd,uint32 fc,uint32 len=0,char *pBuf=NULL);
    //发送信息
    void sendInfo(int level, std::string sInfo, int type=0);
//llb add 20161125 增加终端信息监控
    void measureFromContext(const CxIGetSkv& context);
protected:
    int _iMonsbIdBegin;
    int _iMonsbCount;
    int _iMonsbBeginFix;
    int _iYcaddIdBegin;
    int _iYcaddCount;
    int _iYcaddBeginFix;
    int _iStrawIdBegin;
    int _iStrawCount;
    int _iStrawBeginFix;
protected:

    void reportTerminalStatus();
    void setFixMonsbs();
    void setFixStraws();
    void setFixYcadds();
};


class ProtocolFactoryManager : public CxFactoryManagerTemplate<ProtocolBase>
{
};



#endif // !defined(PROTOCOL_BASE_H)
