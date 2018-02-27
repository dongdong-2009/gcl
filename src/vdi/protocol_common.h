#ifndef PROTOCOL_COMMON_H
#define PROTOCOL_COMMON_H

#include <ccxx/ccxx.h>
#include "protocol_global.h"
#include "protocol_base.h"
#include "monsb_address_worker.h"
#include "ycadd_address_worker.h"
#include "straw_address_worker.h"
#include "send_monsb_control_cases.h"
#include "send_custom_data_cases.h"

#include "send_ycadd_cases.h"
#include "send_monsb_cases.h"
#include "send_ycadd_cycle.h"
#include "send_monsb_cycle.h"


class GM_GCL_VDI_API ProtocolCommon: public ProtocolBase, public IProtocolSendMonsbControl, public IProtocolSendCustomData, public IProtocolSendMonsb, public IProtocolSendYcadd
{
public:
    ProtocolCommon();

    ~ProtocolCommon();

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //control
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs()) {
        if (! _ykInitiative) return SendControlFail_Unknow;
        return -1;
    }

    int sendAllControl(int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs()) {
        return  -1;
    }

    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL) {
        if (iMeasureSonType == MonsbType_rtu_state) {
            if (oMonsb) {
                return _ykSendTable->workerByMeasure(oMonsb);
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    //异步发送
    int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData) {
        return  -1;
    }
     //同步发送
    int sendCustomData_syn(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData,std::map<std::string, std::string> &mapData)
    {
     return -1;
    }


    //monsb send
    int sendMonsb(const MonsbMeasure* oMonsb, int value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return  -1;
    }

    int sendMonsbs(const std::map<const MonsbMeasure*, int>& monsbValues, MonsbTypeEnum eMonsbType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()){
        return  -1;
    }

    int sendAllMonsb(MonsbTypeEnum eMonsbType, int * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return  -1;
    }

    //ycadd send
    int sendYcadd(const YcaddMeasure* oYcadd, double value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return  -1;
    }

    int sendYcadds(const std::map<const YcaddMeasure*, double>& ycaddsValue, YcaddTypeEnum eYcaddType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return  -1;
    }

    int sendAllYcadd(YcaddTypeEnum eYcaddType, double * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return  -1;
    }

    //receive
    //YX
    inline const MonsbAddressWorkerTable* YxRecTable() const            { return _yxRecTable            ; }
    //YK
    inline const MonsbAddressWorkerTable* YkRecTable() const            { return _ykRecTable            ; }
    //YC
    inline const YcaddAddressWorkerTable* YcRecTable() const            { return _ycRecTable            ; }
    //YSTRAW
    inline const StrawAddressWorkerTable* YbRecTable() const            { return _ywRecTable            ; }

    //send
    //YX
    inline const MonsbAddressWorkerTable* YxSendTable() const           { return _yxSendTable           ; }
    //YK
    inline const MonsbAddressWorkerTable* YkSendTable() const           { return _ykSendTable           ; }
    //YC
    inline const YcaddAddressWorkerTable* YcSendTable() const           { return _ycSendTable           ; }
    //YSTRAW
    inline const StrawAddressWorkerTable* YbSendTable() const           { return _ywSendTable           ; }
    //STRAW
protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void channelAfterOpen(const CxChannelBase* oChannel);

    void channelAfterClose(const CxChannelBase* oChannel);

    void protocolAfterStart();

    void protocolAfterStop();

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> & monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> & ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

    void doTask(GCCommon::GCDataElement dt);

protected:
    MonsbAddressWorkerTable* _yxRecTable;
    MonsbAddressWorkerTable* _ykRecTable;
    YcaddAddressWorkerTable* _ycRecTable;
    StrawAddressWorkerTable* _ywRecTable;

    MonsbAddressWorkerTable* _yxSendTable;
    MonsbAddressWorkerTable* _ykSendTable;
    YcaddAddressWorkerTable* _ycSendTable;
    StrawAddressWorkerTable* _ywSendTable;

    //send cycle
    SendMonsbCycle* _sendMonsbCycle;
    SendYcaddCycle* _sendYcaddCycle;
    //召唤周期
    CxTimer* _timeCallUp;
    int _callUpInterval;

    bool _started;
    bool _ykInitiative;

    std::string _prompt;
    bool    _bInit;

protected:
//    virtual void callUp()=0;

    void dealData();

    void dealYX();

    void dealYC();

    void dealYK();

    void regMeasureToTRDB();
    //注册遥信表
    void regMonsbTable(MonsbAddressWorkerTable *pTable,int type);
    //注册遥测表
    void regYcaddTable(YcaddAddressWorkerTable *pTable,int type);
    //注册字符串表
    void regStrawTable(StrawAddressWorkerTable *pTable,int type);

};

#endif // PROTOCOL_COMMON_H
