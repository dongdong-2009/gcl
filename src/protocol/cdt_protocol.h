#ifndef CDT_PROTOCOL_H
#define CDT_PROTOCOL_H


#include <vdi/protocol_base.h>
#include <vdi/send_monsb_cases.h>
#include <vdi/send_ycadd_cases.h>
#include <vdi/send_monsb_control_cases.h>
#include <vdi/send_monsb_cycle.h>
#include <vdi/send_ycadd_cycle.h>
#include <vdi/send_monsb_control_cycle.h>
#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>


const std::string CS_EntryCdtControlWord              = "CdtControlWord"             ;
const std::string CS_EntryCdtTypeFlagRYX              = "CdtTypeFlagRYX"             ;
const std::string CS_EntryCdtTypeFlagSYX              = "CdtTypeFlagSYX"             ;
const std::string CS_EntryCdtTypeFlagRYC              = "CdtTypeFlagRYC"             ;
const std::string CS_EntryCdtTypeFlagSYC              = "CdtTypeFlagSYC"             ;
const std::string CS_EntryCdtTypeFlagRYK              = "CdtTypeFlagRYK"             ;
const std::string CS_EntryCdtTypeFlagSYK              = "CdtTypeFlagSYK"             ;
const std::string CS_EntryCdtTypeFlagRSG              = "CdtTypeFlagRSG"             ;
const std::string CS_EntryCdtStationNoSource          = "CdtStationNoSource"         ;
const std::string CS_EntryCdtStationNoTarget          = "CdtStationNoTarget"         ;
const std::string CS_EntryCdtFunCodeRYXBegin          = "CdtFunCodeRYXBegin"         ;
const std::string CS_EntryCdtFunCodeSYXBegin          = "CdtFunCodeSYXBegin"         ;
const std::string CS_EntryCdtFunCodeRYKBegin          = "CdtFunCodeRYKBegin"         ;
const std::string CS_EntryCdtFunCodeSYKBegin          = "CdtFunCodeSYKBegin"         ;
const std::string CS_EntryCdtTimeSendAllYxInterval    = "CdtTimeSendAllYxInterval"   ;
const std::string CS_EntryCdtTimeSendAllYcInterval    = "CdtTimeSendAllYcInterval"   ;
const std::string CS_EntryCdtTimeSendAllYkInterval    = "CdtTimeSendAllYkInterval"   ;
const std::string CS_EntryCdtValidStateMinCount       = "CdtValidStateMinCount"      ;

class CdtProtocol : public ProtocolBase, public IProtocolSendMonsb, public IProtocolSendYcadd, public IProtocolSendMonsbControl
{

public:

    CdtProtocol();

    ~CdtProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Cdt; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    static void describeValuesCalc(const CxDescribeUnit* oDescribeUnit);

    //monsb send
    int sendAllMonsb(MonsbTypeEnum eMonsbType, int * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    //ycadd send
    int sendAllYcadd(YcaddTypeEnum eYcaddType, double * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    //control send
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    int sendAllControl(int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL);

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

private:
    //send table
    MonsbAddressWorkerTable * _yxSendTable;
    MonsbAddressWorkerTable * _ykSendTable;
    YcaddAddressWorkerTable * _ycSendTable;
    //receive table
    MonsbAddressWorkerTable * _yxRecTable;
    MonsbAddressWorkerTable * _ykRecTable;
    YcaddAddressWorkerTable * _ycRecTable;
    //send cases
    SendMonsbCases          * _sendMonsbCases;
    SendYcaddCases          * _sendYcaddCases;
    SendMonsbControlCases       * _sendMonsbControlCases;
    //send cycle
    SendMonsbCycle          * _sendMonsbCycle;
    SendYcaddCycle          * _sendYcaddCycle;
    SendMonsbControlCycle       * _sendMonsbControlCycle;

    uchar _controlWord;
    uchar _typeFlagRYX;
    uchar _typeFlagSYX;
    uchar _typeFlagRYC;
    uchar _typeFlagSYC;
    uchar _typeFlagRYK;
    uchar _typeFlagSYK;
    uchar _typeFlagRSG;
    uchar _stationNoSource;
    uchar _stationNoTarget;
    uchar _funCodeRYXBegin;
    uchar _funCodeSYXBegin;
    uchar _funCodeRYKBegin;
    uchar _funCodeSYKBegin;

    //头，控制字
    int   _recIndex;
    int   _recState;
    uchar _recData[6+256*6];
    uchar _recControlWord;
    uchar _recTypeFlag;
    uchar _recUnitLength;

    uchar _sendData[12+256*6];

    //config
    int _sendAllYxInterval;
    int _sendAllYcInterval;
    int _sendAllYkInterval;

    int _validStateMinCount;

    int _controlTaskRunInterval;

    int SendCdtBuffer(uchar byTypeFlag, uchar byUnitLength, uchar byFunCodeBegin);

    void DealData();

    void DealRYX();

    void DealRYC();

    void DealRYK();

    void DealRYKbyFunCode();

    void DealRSG();

};

class CdtProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(CdtProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Cdt;
    }

    ProtocolBase* createObject(){
        return new CdtProtocol();
    }
};
#endif // !defined(CDT_PROTOCOL_H)
