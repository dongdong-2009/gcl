#ifndef WFS_PROTOCOL_H
#define WFS_PROTOCOL_H


#include <vdi/protocol_base.h>
#include <vdi/send_monsb_cases.h>
#include <vdi/send_ycadd_cases.h>
#include <vdi/send_monsb_control_cases.h>
#include <vdi/send_monsb_cycle.h>
#include <vdi/send_ycadd_cycle.h>
#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>


const uchar CIWfsFixTitleLength = 14;

const ushort CIWfsFixMaxUserDataLength = 4096;

const ushort CIWfsFixMaxDataLength = CIWfsFixTitleLength + CIWfsFixMaxUserDataLength + 2;

enum WfsCommandCodeEnum {
    WfsCommandCode_Hand            = 0x01,
    WfsCommandCode_Sys             = 0x02,
    WfsCommandCode_Yx              = 0x03,
    WfsCommandCode_Yc              = 0x04,
    WfsCommandCode_Yk              = 0x05,
    WfsCommandCode_Time            = 0x06,
    WfsCommandCode_Soe             = 0x07,
    WfsCommandCode_Piao            = 0x08,
    WfsCommandCode_Key             = 0x09,
    WfsCommandCode_Kbq             = 0x0A,
    WfsCommandCode_File            = 0x0B,
    WfsCommandCode_Dixian          = 0x0C
};

enum WfsFunctionCodeYxEnum {
    WfsFunctionCodeYx_0            = 0x00,
    WfsFunctionCodeYx_1            = 0x01,
    WfsFunctionCodeYx_2            = 0x02,
    WfsFunctionCodeYx_3            = 0x03
};

enum WfsFunctionCodeYcEnum {
    WfsFunctionCodeYc_0            = 0x04,
    WfsFunctionCodeYc_1            = 0x01,
    WfsFunctionCodeYc_2            = 0x02,
    WfsFunctionCodeYc_3            = 0x03
};

//0x00表示服务器
//0xFF地址广播
//0x01通讯管理机
//0x02FY2004
//0x03五型工控机
//0x04钥匙
enum WfsTerminalTypeEnum {
    WfsTerminalType_Server         = 0x00,
    WfsTerminalType_TXServer       = 0x01,
    WfsTerminalType_FY2004         = 0x02,
    WfsTerminalType_GK             = 0x03,
    WfsTerminalType_Key            = 0x04,
    WfsTerminalType_Frontender     = 0x05
};

const WfsTerminalTypeEnum CEMaxWfsTerminalType = WfsTerminalType_Frontender;


const std::string CS_EntryWfsStationTypeSource = "WfsStationTypeSource";
const std::string CS_EntryWfsStationTypeTarget = "WfsStationTypeTarget";
const std::string CS_EntryWfsStationNumSource  = "WfsStationNumSource";
const std::string CS_EntryWfsStationNumTarget  = "WfsStationNumTarget";
const std::string CS_EntryWfsHeartJumpInterval = "WfsHeartJumpInterval";
const std::string CS_EntryWfsYxSendInterval    = "WfsYxSendInterval";
const std::string CS_EntryWfsYcSendInterval    = "WfsYcSendInterval";
const std::string CS_EntryWfsKbqSendInterval    = "WfsKbqSendInterval";
const std::string CS_EntryWfsMinCountValidYx   = "WfsMinCountValidYx";


class WfsProtocol : public ProtocolBase, public IProtocolSendMonsb, public IProtocolSendYcadd, public IProtocolSendMonsbControl
{
public:
    WfsProtocol();

    ~WfsProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Wfs; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //monsb send
    int sendMonsb(const MonsbMeasure* oMonsb, int value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendMonsbs(const std::map<const MonsbMeasure*, int>& monsbValues, MonsbTypeEnum eMonsbType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendAllMonsb(MonsbTypeEnum eMonsbType, int * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int SendStates(const std::map<int, int>& addressStates, WfsFunctionCodeYxEnum eFunctionType); //+++++

    //ycadd send
    int sendYcadd(const YcaddMeasure* oYcadd, double value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendYcadds(const std::map<const YcaddMeasure*, double>& ycaddsValue, YcaddTypeEnum eYcaddType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendAllYcadd(YcaddTypeEnum eYcaddType, double * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int SendAnalogs(const std::map<int, double>& addressValues, WfsFunctionCodeYcEnum eFunctionType); //+++++

    //control
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    int sendControls(const std::vector<std::pair<const MonsbMeasure*, int> > & monsbsToValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    int sendAllControl(int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    int sendControls(const std::map<int, MonsbControlFromToState>& addressFromToStates);

    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL);

    std::vector<std::string> TranslateBuffer(const uchar* pData, int iLength);

    int SendSystemState();

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
    //kbq send receive table
    MonsbAddressWorkerTable * _kbqTable;
    //send cases
    SendMonsbCases* _sendMonsbCases;
    SendYcaddCases* _sendYcaddCases;
    SendMonsbControlCases* _sendMonsbControlCases;
    //send cycle
    SendMonsbCycle* _sendMonsbCycle;
    SendYcaddCycle* _sendYcaddCycle;
    SendMonsbCycle* _sendKbqCycle;

    //头，控制字
    uchar _recBuffer[CIWfsFixMaxDataLength];
    uchar _sendBuffer[CIWfsFixMaxDataLength];

    int     _recState;
    int     _recDataIndex;
    uchar   _recStationTypeSource;
    uchar   _recStationTypeTarget;
    ushort  _recStationNumTarget;
    ushort  _recStationNumSource;
    uchar   _recProtocolCode;
    uchar   _recControlCode;
    ushort  _recDataLen;
    uchar   _recCommonCode;
    uchar   _recFunCode;
    ushort  _recCrc;

    //config
    WfsTerminalTypeEnum _stationTypeSource;
    WfsTerminalTypeEnum _stationTypeTarget;
    ushort _stationNumSource;
    ushort _stationNumTarget;

    int _minCountValidYx;

    CxTimer* _heartJumpTimer;
    int _heartJumpInterval;
    int _sendYxInterval;
    int _sendYcInterval;
    int _sendKbqInterval;

    void DealData();

    void DealHand();

    void DealSys();

    void DealYX();

    void DealYC();

    void DealYK();

    void DealTime();

    void DealSoe();

    void DealPiao();

    void DealKey();

    void DealKbq();

    void DealFile();

    void DealDiXian();

    int SendWfsBuffer(uchar byControlCode, uchar byFunCode, uchar byCommandCode, ushort wUnitLenth);

    int SendHeartJump();

    int ReplyAck();

    int ReplyNck(uchar Error);

    int SendStates1(ushort byFirstAddress, const uchar* states, int iCount, uchar byFunctionCode);

    int SendStates2(const std::map<int, int>& addressStates, uchar byFunctionCode);

    int SendAnalog1(ushort byFirstAddress, const float* values, int iCount, uchar byFunctionCode);

    int SendAnalog2(const std::map<int, double>& addressValues, uchar byFunctionCode);

    int SendAllDiscontrol();

    int SendAllMonsbRtu(int * value);

    int TranslateSystemState();

};

class WfsProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(WfsProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Wfs;
    }

    ProtocolBase* createObject(){
        return new WfsProtocol();
    }
};
#endif // !defined(WFS_PROTOCOL_H)
