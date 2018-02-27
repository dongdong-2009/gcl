#include "wfs_protocol.h"

#include <vdi/terminal_manager.h>

WfsProtocolFactory* WfsProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<WfsProtocolFactory>();


uchar WFSBufTitle [ 4 ] = { 0xA5 , 0x5A , 0xA5 , 0x5A };

const uchar CIWfsDefaultProtocolCode = 0x02;
const uchar CIWfsControlCode_Initiactive = 0x40;
const uchar CIWfsControlCode_Passivity = 0x80;
const uchar CIWfsControlCode_NACK = 0xC0;

const uchar CIWfsUserDataIndex = CIWfsFixTitleLength + 2;

//握手

const uchar CommandCode_Hand = 0x01;
const uchar FunCode_Hand_Time = 0x01;

//系统状态

const uchar CommandCode_Sys = 0x02;
const uchar FunCode_Sys_Query = 0x01;//请求系统状态
const uchar FunCode_Sys_Initiactive = 0x02;//主动上报系统状态
const uchar FunCode_Sys_Askfor = 0x03;//请求是否可以到XX工作状态
const uchar FunCode_Sys_Shutdown = 0x04;//请求关闭
const uchar FunCode_Sys_Translate = 0x11;//转发

//遥信

const uchar CommandCode_YX = 0x03;
const uchar FunCode_YX_All_Query = 0x01;
const uchar FunCode_YX_All_Initiactive = 0x02;
const uchar FunCode_YX_BianWei = 0x03;
const uchar FunCode_YX_DiXian_Initiactive = 0x04;

//遥测

const uchar CommandCode_YC = 0x04;
//const uchar FunCode_YC_All_Query = 0x01;
const uchar FunCode_YC_All_Initiactive = 0x01;
const uchar FunCode_YC_BianWei = 0x02;

//遥控许可，禁止

const uchar CommandCode_YK = 0x05;
const uchar FunCode_YK_Control = 0x01;
const uchar FunCode_YK_AllDisable = 0x02;

//时间

const uchar CommandCode_Time = 0x06;

//SOE

const uchar CommandCode_Soe = 0x07;

//操作票

const uchar CommandCode_Piao = 0x08;

//钥匙

const uchar CommandCode_Key = 0x09;

//Kbq

const uchar CommandCode_Kbq = 0x0A;

//远程打包

const uchar CommandCode_File= 0x0B;

//地线

const uchar CommandCode_DiXian = 0x0C;
const uchar FunCode_Dixian_Operation_Initiactive = 0x01;

//设备通讯方式

const uchar DeviceTransferType_Addr_First = 0x01;//首地址

const uchar DeviceTransferType_Addr_Info = 0x02;//信息前都带地址

const uchar DeviceTransferType_MonsbDiXian = 0x03;//地线对应关系

const uchar DeviceTransferType_Code_Info = 0x04;//信息前都带设备编号

//地址

const uchar BufBodyFirstAddr = 16;
const uchar RecBufBodyFirstAddr = 12;

//系统状态表
typedef struct tagSystemState
{
    ushort sourceStationType;
    ushort sourceStationId;
    uchar  sourceStationStateType;
    uchar  sourceStationStateId;
} SystemState;

static std::vector<SystemState> f_systemStates;

uchar WfsDeviceState(const int& value) {
    if ( value > 3 || value < 0)
        return 0xFF;
    return value;
}


WfsProtocol::WfsProtocol(){
    _yxSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, WfsProtocol>("YxSendTable", MonsbType_rtu_state);
    _ykSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, WfsProtocol>("YkSendTable", MonsbType_rtu_state);
    _ycSendTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, WfsProtocol>("YcSendTable", YcaddType_rtu_value);

    _yxRecTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, WfsProtocol>("YxReceiveTable", MonsbType_rtu_state);
    _ykRecTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, WfsProtocol>("YkReceiveTable", MonsbType_rtu_state);
    _ycRecTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, WfsProtocol>("YcReceiveTable", YcaddType_rtu_value);

    _kbqTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, WfsProtocol>("KbqTable", MonsbType_kbq_state);

    _sendMonsbCases = createAndRegisterCases<WfsProtocol, SendMonsbCases>(this);
    _sendYcaddCases = createAndRegisterCases<WfsProtocol, SendYcaddCases>(this);
    _sendMonsbControlCases = createAndRegisterCases<WfsProtocol, SendMonsbControlCases>(this);

    _sendMonsbCycle = createAndRegisterCycle<WfsProtocol, SendMonsbCycle>(this, MonsbType_rtu_state);
    _sendYcaddCycle = createAndRegisterCycle<WfsProtocol, SendYcaddCycle>(this, YcaddType_rtu_value);
    _sendKbqCycle = createAndRegisterCycle<WfsProtocol, SendMonsbCycle>(this, MonsbType_kbq_state);

    _heartJumpTimer = new CxTimer();
    _heartJumpTimer->setTimerTimeOut(this);

    _heartJumpInterval = 5000;
    _sendYxInterval = 2000;
    _sendYcInterval = 3000;
    _sendKbqInterval = 3000;

    _stationTypeSource = WfsTerminalType_TXServer;
    _stationTypeTarget = WfsTerminalType_FY2004;
    _stationNumSource = 0;
    _stationNumTarget = 1;
    _minCountValidYx = 0;
}

WfsProtocol::~WfsProtocol(){
    delete _heartJumpTimer;
}

void WfsProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CS_EntryWfsStationTypeSource, _stationTypeSource);
    context.setValue(CS_EntryWfsStationTypeTarget, _stationTypeTarget);
    context.setValue(CS_EntryWfsStationNumSource, _stationNumSource);
    context.setValue(CS_EntryWfsStationNumTarget, _stationNumTarget);
    context.setValue(CS_EntryWfsHeartJumpInterval, _heartJumpInterval);
    context.setValue(CS_EntryWfsYxSendInterval, _sendYxInterval);
    context.setValue(CS_EntryWfsYcSendInterval, _sendYcInterval);
    context.setValue(CS_EntryWfsMinCountValidYx, _minCountValidYx);
}

void WfsProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
    _stationTypeSource = (WfsTerminalTypeEnum)context.getValue(CS_EntryWfsStationTypeSource, _stationTypeSource);
    _stationTypeTarget = (WfsTerminalTypeEnum)context.getValue(CS_EntryWfsStationTypeTarget, _stationTypeTarget);
    _stationNumSource = context.getValue(CS_EntryWfsStationNumSource, _stationNumSource);
    _stationNumTarget = context.getValue(CS_EntryWfsStationNumTarget, _stationNumTarget);
    _heartJumpInterval = context.getValue(CS_EntryWfsHeartJumpInterval, _heartJumpInterval);
    _sendYxInterval = context.getValue(CS_EntryWfsYxSendInterval, _sendYxInterval);
    _sendYcInterval = context.getValue(CS_EntryWfsYcSendInterval, _sendYcInterval);
    _minCountValidYx = context.getValue(CS_EntryWfsMinCountValidYx, _minCountValidYx);

    _sendMonsbCycle->setSendInterval(_sendYxInterval);
    _sendYcaddCycle->setSendInterval(_sendYcInterval);
    _sendKbqCycle->setSendInterval(_sendKbqInterval);
}

void WfsProtocol::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(std::string("Station Type Source : " + CxString::toString(_stationTypeSource)));
    sReports.push_back(std::string("Station Type Target : " + CxString::toString(_stationTypeTarget)));
    sReports.push_back(std::string("Station No Source : " + CxString::toString(_stationNumSource )));
    sReports.push_back(std::string("Station No Target : " + CxString::toString(_stationNumTarget )));
    sReports.push_back(std::string("Heart Jump Interval : " + CxString::toString(_heartJumpInterval)));
    sReports.push_back(std::string("Yx Send Interval : " + CxString::toString(_sendYxInterval)));
    sReports.push_back(std::string("Yc Send Interval : " + CxString::toString(_sendYcInterval)));
    sReports.push_back(std::string("Kbq Send Interval : " + CxString::toString(_sendKbqInterval)));
    sReports.push_back(std::string("Valid State Min Count : " + CxString::toString(_minCountValidYx)));
}

void WfsProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
    CxDescribeEnum* oStationNoSourceDescribe          = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryWfsStationTypeSource   ); oStationNoSourceDescribe->initValue(_stationTypeSource  );  oStationNoSourceDescribe->setEnumCopeFromZero(CEMaxWfsTerminalType);
    CxDescribeEnum* oStationNoTargetDescribe          = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryWfsStationTypeTarget   ); oStationNoTargetDescribe->initValue(_stationTypeTarget  );  oStationNoTargetDescribe->setEnumCopeFromZero(CEMaxWfsTerminalType);
    CxDescribeInteger* oStationNumSourceDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsStationNumSource ); oStationNumSourceDescribe->initValue(_stationNumSource  );
    CxDescribeInteger* oStationNumTargetDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsStationNumTarget ); oStationNumTargetDescribe->initValue(_stationNumTarget  );
    CxDescribeInteger* oHeartJumpIntervalDescribe     = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsHeartJumpInterval); oHeartJumpIntervalDescribe->initValue(_heartJumpInterval);
    CxDescribeInteger* oYxSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsYxSendInterval);    oYxSendIntervalDescribe->initValue(_sendYxInterval);
    CxDescribeInteger* oYcSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsYcSendInterval);    oYcSendIntervalDescribe->initValue(_sendYcInterval);
    //    CxDescribeInteger* oKbqSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsKbqSendInterval);    oKbqSendIntervalDescribe->initValue(_sendKbqInterval);
    CxDescribeInteger* oValidStateMinCountDescribe    = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWfsMinCountValidYx);    oValidStateMinCountDescribe->initValue(_minCountValidYx);
}

void WfsProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    for (int i = 0; i < iLength; ++i) {
        switch ( _recState )
        {
        case
        // ---------------------头-----------------------------------------------------
        //报文头 $A5 $5A $A5 $5A
        0:
            if ( pData [ i ] == 0xA5 )
                _recState = 1;
            else
                _recState = 0;
            break;
        case 1:
            if ( pData [ i ] == 0x5A )
                _recState = 2;
            else
                _recState = 0;
            break;
        case 2:
            if ( pData [ i ] == 0xA5 )
                _recState = 3;
            else
                _recState = 0;
            break;
        case 3:
            if ( pData [ i ] == 0x5A )
                _recState = 4;
            else
                _recState = 0;
            break;

            //目标站类型

        case 4:
        {
            _recStationTypeTarget = pData [ i ];
            _recBuffer [ 4 ] = pData [ i ];
            _recState = 5;
        }
            break;

            //源站类型

        case 5:
        {
            _recStationTypeSource = pData [ i ];
            _recBuffer [ 5 ] = pData [ i ];
            _recState = 6;
        }
            break;

            //目标站号 两个字节

        case 6:
        {
            _recStationNumTarget = pData [ i ];
            _recBuffer [ 6 ] = pData [ i ];
            _recState = 7;
        }
            break;
        case 7:
        {
            _recStationNumTarget = ( pData [ i ] << 8 ) + _recStationNumTarget;
            _recBuffer [ 7 ] = pData [ i ];
            _recState = 8;
        }
            break;

            //源站号 两个字节

        case 8:
        {
            _recStationNumSource = pData [ i ];
            _recBuffer [ 8 ] = pData [ i ];
            _recState = 9;
        }
            break;
        case 9:
        {
            _recStationNumSource = ( pData [ i ] << 8 ) + _recStationNumSource;
            _recBuffer [ 9 ] = pData [ i ];
            _recState = 10;
        }
            break;

            //协议应用类别 目前写死 DefaultProtocolCode

        case 10:
        {
            _recProtocolCode = pData [ i ];
            _recBuffer [ 10 ] = pData [ i ];
            _recState = 11;
        }
            break;

            //控制字 ($40 主动发送帧；$80 应答帧；$81 应答ACK NAK帧)

        case 11:
        {
            _recControlCode = pData [ i ];
            _recBuffer [ 11 ] = pData [ i ];
            _recState = 12;
        }
            break;

            //数据长度 两个字节 （除报文头跟本身两个字节以外的所有数据长度）

        case 12:
        {
            _recDataLen = pData [ i ];
            _recBuffer [ 12 ] = pData [ i ];
            _recState = 13;
        }
            break;
        case 13:
        {
            _recDataLen = ( pData [ i ] << 8 ) + _recDataLen;
            //最小长为2  (命令码 功能码)
            if ( _recDataLen >= 2 && _recDataLen <= CIWfsFixMaxUserDataLength)
            {
                //最少固长为8  (数据长度 + 协议应用类别 + 控制字 + 站..)(2 + 1 + 1 + 6)
                _recBuffer [ 13 ] = pData [ i ];
                _recState = 14;
            } else {
                _recState = 0;
            }
        }
            break;

            // ---------------------身-----------------------------------------------------
            //命令码

        case 14:
        {
            _recCommonCode = pData [ i ];
            _recBuffer [ 14 ] = pData [ i ];
            _recState = 15;
        }
            break;

            //功能码

        case 15:
        {
            _recFunCode = pData [ i ];
            _recBuffer [ 15 ] = pData [ i ];
            _recState = 16;
            _recDataIndex = 16;
        }
            break;

            //数据正文

        case 16:
        {
            _recBuffer [ _recDataIndex ] = pData [ i ];
            _recDataIndex ++;
            if ( _recDataIndex > _recDataLen + CIWfsFixTitleLength )
            {
                //较验
                _recCrc = pData [ i ];
                _recState = 17;
            }
        }
            break;
        case 17:
        {
            _recState = 0;
            _recBuffer [ _recDataIndex ] = pData [ i ];
            _recCrc = ( pData [ i ] << 8 ) + _recCrc;
            DealData ( );
        }
            break;
        } //Case FMonstFlag of
    } //for i:=low(buf) to high(buf) do
}

std::vector<std::string> WfsProtocol::TranslateBuffer(const uchar *pData, int iLength)
{
    std::vector<std::string> sl;
    uchar  byRecStationTypeSource = pData[4];
    uchar  byRecStationTypeTarget = pData[5];
    ushort byRecStationNumTarget =  ( pData [ 7 ] << 8 ) + pData [ 6 ];
    ushort byRecStationNumSource =  ( pData [ 9 ] << 8 ) + pData [ 8 ];
    uchar  byRecProtocolCode = pData [ 10 ];
    uchar  byRecControlCode = pData [ 11 ];
    int    iRecDataLen = ( pData [ 13 ] << 8 ) + pData [ 12 ];
    uchar  byRecCommonCode = pData [ 14 ];
    uchar  byRecFunCode = pData [ 15 ];
    ushort wRecCrc = ( pData [ iRecDataLen + 15 ] << 8 ) + pData [ iRecDataLen + 14 ];

    sl.push_back(std::string("同步头：：" + CxString::toString(        CxString::toHexstring(&pData[0], 4))));
    sl.push_back(std::string("目标类型：" + CxString::toString(        byRecStationTypeSource)));
    sl.push_back(std::string("源类型：" + CxString::toString(          byRecStationTypeTarget)));
    sl.push_back(std::string("目标地址：" + CxString::toString(        byRecStationNumTarget)));
    sl.push_back(std::string("源地址：" + CxString::toString(          byRecStationNumSource)));
    sl.push_back(std::string("协议应用类别协议：" + CxString::toString( byRecProtocolCode)));
    sl.push_back(std::string("控制字：" + CxString::toString(          byRecControlCode)));
    sl.push_back(std::string("报文数据长度：" + CxString::toString(     iRecDataLen)));
    sl.push_back(std::string("命令码：" + CxString::toString(          byRecCommonCode)));
    sl.push_back(std::string("功能码：" + CxString::toString(          byRecFunCode)));
    sl.push_back(std::string("有效数据：" + CxString::toString(         CxString::toHexstring(&pData[16], iRecDataLen-2))));
    sl.push_back(std::string("数据校验：" + CxString::toString(         wRecCrc)));

    return sl;
}

void WfsProtocol::DealData()
{
    //FRecBuffer从头到身及检验的全部数据，不管正确与否
    ushort w = CxCrc::calcCRC16( & _recBuffer [ 4 ], _recDataLen + CIWfsFixTitleLength - 4 );
    if ( _recCrc == w )
    {
        switch ( _recCommonCode )
        {
        case
        //握手 心跳
        CommandCode_Hand:
        {
            DealHand ( );
        }
            break;

            //系统状态

        case CommandCode_Sys:
        {
            DealSys ( );
        }
            break;

            //遥信

        case CommandCode_YX:
        {
            DealYX ( );
        }
            break;

            //遥测

        case CommandCode_YC:
        {
            DealYC ( );
        }
            break;

            //遥控许可，禁止

        case CommandCode_YK:
        {
            DealYK ( );
        }
            break;

            //时间

        case CommandCode_Time:
        {
            DealTime ( );
        }
            break;

            //SOE

        case CommandCode_Soe:
        {
            DealSoe ( );
        }
            break;

            //操作票

        case CommandCode_Piao:
        {
            DealPiao ( );
        }
            break;

            //钥匙

        case CommandCode_Key:
        {
            DealKey ( );
        }
            break;

            //Kbq

        case CommandCode_Kbq:
        {
            DealKbq ( );
        }
            break;

            //文件传送

        case CommandCode_File:
        {
            DealFile ( );
        }
            break;

            //地线

        case CommandCode_DiXian:
        {
            DealDiXian ( );
        }
            break;
        }
    }
}

void WfsProtocol::DealHand()//未完成
{
    switch(_recControlCode){
    case CIWfsControlCode_Initiactive:
        ReplyAck();
        break;
    case CIWfsControlCode_Passivity:
        break;
    case CIWfsControlCode_NACK:
        break;
    }

}

void WfsProtocol::DealSys()
{
    if( ( _recControlCode == CIWfsControlCode_Initiactive ) && ( _recFunCode == FunCode_Sys_Initiactive) )
    {
        if (_recDataLen < 2 + 1)
            return;
        for (std::size_t i = 0; i < f_systemStates.size(); ++i)
        {
            SystemState & state = f_systemStates[i];
            if (state.sourceStationType == _stationTypeTarget && state.sourceStationId == _recStationNumTarget)
            {
                uchar byState = _recBuffer[CIWfsUserDataIndex + 0];
                if ( state.sourceStationStateId != byState )
                {
                    state.sourceStationStateId = byState;
                    TranslateSystemState();
                }
                return;
            }
        }
        SystemState state;
        state.sourceStationType = _stationTypeTarget;
        state.sourceStationId = _recStationNumTarget;
        state.sourceStationStateId = _recBuffer[CIWfsUserDataIndex + 0];
        f_systemStates.insert(f_systemStates.end(), state);
        TranslateSystemState();
    }
}

void WfsProtocol::DealYX()
{
    //query all yx
    if((_recControlCode == CIWfsControlCode_Initiactive) && (_recFunCode == FunCode_YX_All_Query))
    {
        sendAllMonsb(MonsbType_rtu_state);
        return;
    }
    ushort wMinRecDataLength = 2 + 3;

    if (_recDataLen < wMinRecDataLength)
        return;
    uchar byUserDataType = _recBuffer[CIWfsUserDataIndex + 0];
    ushort wCount = _recBuffer[CIWfsUserDataIndex + 1] + (_recBuffer[CIWfsUserDataIndex + 2] << 8);
    std::map<int, int> addressValues;
    int iProtocolId = protocolId();
    if (byUserDataType == DeviceTransferType_Addr_First) {
        ushort wUserDataLenth = 2 + 5 + wCount;
        if (_recDataLen < wUserDataLenth)
            return;
        int iFirstAddress = _recBuffer[CIWfsUserDataIndex + 3] + (_recBuffer[CIWfsUserDataIndex + 4] << 8);
        for (int i = 0; i < wCount; ++i) {
            int iAddress = iFirstAddress + i;
            int iState = _recBuffer[CIWfsUserDataIndex + 5 + i];
            addressValues[iAddress] = iState;
        }
    } else if (byUserDataType == DeviceTransferType_Addr_Info) {
        ushort wUserDataLenth = 2 + 3 + wCount * 3;
        if (_recDataLen < wUserDataLenth)
            return;
        for (int i = 0; i < wCount; ++i) {
            int iBufferIndex = CIWfsUserDataIndex + 3;
            iBufferIndex += 3 * i;
            int iAddress = _recBuffer[iBufferIndex + 0] + (_recBuffer[iBufferIndex + 1] << 8);;
            int iState = _recBuffer[iBufferIndex + 2];
            addressValues[iAddress] = iState;
        }
    } else if (byUserDataType == DeviceTransferType_MonsbDiXian) {

    }
    if (addressValues.size() > 0){
        _yxRecTable->receiveAddressValues(addressValues, iProtocolId, 0, isAutoCreateWorkerAtDeal());
    }
}

void WfsProtocol::DealYC()
{
    ushort wMinRecDataLength = 2 + 3;
    if (_recDataLen < wMinRecDataLength)
        return;
    uchar byUserDataType = _recBuffer[CIWfsUserDataIndex + 0];
    ushort wCount = _recBuffer[CIWfsUserDataIndex + 1] + (_recBuffer[CIWfsUserDataIndex + 2] << 8);
    std::map<int, double> addressValues;
    int iProtocolId = protocolId();
    if (byUserDataType == DeviceTransferType_Addr_First) {
        ushort wUserDataLenth = 2 + 6 + wCount * 4;
        if (_recDataLen < wUserDataLenth)
            return;
        int iFirstAddress = _recBuffer[CIWfsUserDataIndex + 3] + (_recBuffer[CIWfsUserDataIndex + 4] << 8);
        for (int i = 0; i < wCount; ++i) {
            int iAddress = iFirstAddress + i;
            float fValue;
            memcpy(&fValue , &_recBuffer[CIWfsUserDataIndex + 6 + i * 4], 4);
            addressValues[iAddress] = fValue;
        }
    } else if (byUserDataType == DeviceTransferType_Addr_Info) {
        ushort wUserDataLenth = 2 + 3 + wCount * 7;
        if (_recDataLen < wUserDataLenth)
            return;
        for (int i = 0; i < wCount; ++i) {
            int iStructIndex = CIWfsUserDataIndex + 3 + (7 * i);
            int iAddress = _recBuffer[iStructIndex + 0] + (_recBuffer[iStructIndex + 1] << 8);
            uchar byDataType = _recBuffer[iStructIndex + 2];
            if (byDataType == 1) {
                float fValue ;
                memcpy(&fValue , &_recBuffer[iStructIndex + 3], 4);
                addressValues[iAddress] = fValue;
            }
        }
    } else if (byUserDataType == DeviceTransferType_MonsbDiXian) {

    }
    if (addressValues.size() > 0){
        _ycRecTable->receiveAddressValues(addressValues, iProtocolId, 0, isAutoCreateWorkerAtDeal());
    }
}

void WfsProtocol::DealYK()
{
    int iProtocolId = protocolId();
    if((_recControlCode == CIWfsControlCode_Initiactive) && (_recFunCode == FunCode_YK_AllDisable))
    {
        MonsbControlTaskManager::cancelControls(iProtocolId);
        return;
    }
    ushort wMinRecDataLength = 2 + 3;
    if (_recDataLen < wMinRecDataLength)
        return;
    std::string sMeasureName;
    uchar byUserDataType = _recBuffer[CIWfsUserDataIndex + 0];
    ushort wCount = _recBuffer[CIWfsUserDataIndex + 1] + (_recBuffer[CIWfsUserDataIndex + 2] << 8);
    if (byUserDataType == DeviceTransferType_Addr_Info) {
        ushort wUserDataLenth = 2 + 3 + wCount * 4;
        if (_recDataLen < wUserDataLenth)
            return;
        for (int i = 0; i < wCount; ++i) {
            int iBufferIndex = CIWfsUserDataIndex + 3;
            iBufferIndex += 4 * i;
            int iAddress = _recBuffer[iBufferIndex + 0] + (_recBuffer[iBufferIndex + 1] << 8);;
            int iFromValue = _recBuffer[iBufferIndex + 2];
            int iToValue = _recBuffer[iBufferIndex + 3];
            MonsbControlTask* oTask = _ykRecTable->receiveControl(iAddress, iFromValue, iToValue, iProtocolId);
            if (oTask){
                oTask->setRunInterval(1000);
                if (oTask->measure()) {
                    sMeasureName = oTask->measure()->measureName();
                    outProtocolPrompt() << std::string("Receive YK measureName " + sMeasureName) << cxEndLine;
                }
            }
        }
    }
}

void WfsProtocol::DealTime()
{
}

void WfsProtocol::DealSoe()
{
}

void WfsProtocol::DealPiao()
{
}

void WfsProtocol::DealKey()
{
}

void WfsProtocol::DealKbq()
{
    //query all yx
    if((_recControlCode == CIWfsControlCode_Initiactive) && (_recFunCode == FunCode_YX_All_Query))
    {
        sendAllMonsb(MonsbType_kbq_state);
        return;
    }
    ushort wMinRecDataLength = 2 + 3;
    if (_recDataLen < wMinRecDataLength)
        return;
    uchar byUserDataType = _recBuffer[CIWfsUserDataIndex + 0];
    ushort wCount = _recBuffer[CIWfsUserDataIndex + 1] + (_recBuffer[CIWfsUserDataIndex + 2] << 8);
    std::map<int, int> addressValues;
    int iProtocolId = protocolId();
    if (byUserDataType == DeviceTransferType_Addr_First) {
        ushort wUserDataLenth = 2 + 5 + wCount;
        if (_recDataLen < CIWfsFixTitleLength + wUserDataLenth)
            return;
        int iFirstAddress = _recBuffer[CIWfsUserDataIndex + 3] + (_recBuffer[CIWfsUserDataIndex + 4] << 8);
        for (int i = 0; i < wCount; ++i) {
            int iAddress = iFirstAddress + i;
            int iState = _recBuffer[CIWfsUserDataIndex + 5 + i];
            addressValues[iAddress] = iState;
        }
    } else if (byUserDataType == DeviceTransferType_Addr_Info) {
        ushort wUserDataLenth = 2 + 3 + wCount * 3;
        if (_recDataLen + 14 < CIWfsFixTitleLength + wUserDataLenth)
            return;
        for (int i = 0; i < wCount; ++i) {
            int iBufferIndex = CIWfsUserDataIndex + 3;
            iBufferIndex += 3 * i;
            int iAddress = _recBuffer[iBufferIndex + 0] + (_recBuffer[iBufferIndex + 1] << 8);;
            int iState = _recBuffer[iBufferIndex + 2];
            addressValues[iAddress] = iState;
        }
    } else if (byUserDataType == DeviceTransferType_MonsbDiXian) {

    }
    if (addressValues.size() > 0){
        _yxRecTable->receiveAddressValues(addressValues, iProtocolId, 0, isAutoCreateWorkerAtDeal());
    }
}

void WfsProtocol::DealFile()
{
}

void WfsProtocol::DealDiXian()
{
}

int WfsProtocol::SendWfsBuffer(uchar byControlCode, uchar byCommandCode, uchar byFunCode, ushort wUserDataLenth)
{
    if ( wUserDataLenth < 2 || wUserDataLenth > CIWfsFixMaxUserDataLength)
        return -1;
    if ( wUserDataLenth > 3000 )
        outProtocolPrompt() << " - SendWfsBufferstd -> DataLength : " + CxString::toString(wUserDataLenth) << cxEndLine;
    memcpy(_sendBuffer, WFSBufTitle, 4);
    _sendBuffer[4] = _recStationTypeSource;//目标类型
    _sendBuffer[5] = _recStationTypeTarget;//源类型

    _sendBuffer[6] = (_recStationNumSource & 0xFF) ;//lo(target)
    _sendBuffer[7] = (_recStationNumSource >> 8);//hi(target)

    _sendBuffer[8] = (_recStationNumTarget & 0xFF);//lo(source)
    _sendBuffer[9] = (_recStationNumTarget >> 8);//hi(source)
    _sendBuffer[10] = CIWfsDefaultProtocolCode; //协议应用类型
    _sendBuffer[11] = byControlCode;//控制字
    _sendBuffer[12] = (wUserDataLenth & 0xFF);
    _sendBuffer[13] = (wUserDataLenth >> 8);
    _sendBuffer[14] = byCommandCode;//命令码
    _sendBuffer[15] = byFunCode;//功能码

    int iLenth = 14 + wUserDataLenth + 2;

    ushort w = CxCrc::calcCRC16(&_sendBuffer[4], wUserDataLenth+10);

    _sendBuffer[wUserDataLenth + 14] = (w & 0xFF);
    _sendBuffer[wUserDataLenth + 15] = (w >> 8);

    return writeData(_sendBuffer, iLenth);
}

int WfsProtocol::ReplyAck()
{
    ushort wUnitLength = 4;
    _sendBuffer[CIWfsUserDataIndex + 0] = 0x01;
    _sendBuffer[CIWfsUserDataIndex + 1] = 0x00;
    return SendWfsBuffer(CIWfsControlCode_NACK, _recCommonCode, _recFunCode, wUnitLength);
}

int WfsProtocol::ReplyNck(uchar Error)
{
    ushort wUnitLength = 4;
    _sendBuffer[CIWfsUserDataIndex + 0] = 0x01;
    _sendBuffer[CIWfsUserDataIndex + 1] = Error;
    return SendWfsBuffer(CIWfsControlCode_NACK, _recCommonCode, _recFunCode, wUnitLength);
}

int WfsProtocol::SendHeartJump()
{
    ushort wUnitLength = 2;
    return SendWfsBuffer(CIWfsControlCode_Initiactive, CommandCode_Hand, FunCode_Hand_Time, wUnitLength);
}

///*** *** *** *** *** *** *** *** *** *** *** *** *** ***
///*** *** *** *** sendMonsb *** *** *** *** begin ***
///*** *** *** *** *** *** *** *** *** *** *** *** *** ***
int WfsProtocol::sendMonsb(const MonsbMeasure *oMonsb, int value, SendMeasureArgvs argvs)
{
    if (! oMonsb) {
        return SendMeasureFail_Nomeasure;
    }
    MonsbAddressWorkerTable* oTable = _yxSendTable;
    MonsbAddressWorker *oWorker = oTable->workerByMeasure(oMonsb);
    if (! oWorker)
        return SendMeasureFail_Nomeasure;
    std::map<int, int> addressValues;
    addressValues[oWorker->address()] = value;
    return SendStates2(addressValues, FunCode_YX_BianWei);
}

int WfsProtocol::sendMonsbs(const std::map<const MonsbMeasure *, int> &monsbValues, MonsbTypeEnum eMonsbType, SendMeasureArgvs argvs)
{
    if (monsbValues.size() > 0 && eMonsbType == MonsbType_rtu_state){
        std::map<int, int> addressValues = _yxSendTable->addressValues(monsbValues);
        return SendStates2(addressValues, FunCode_YX_BianWei);
    }
    return SendMeasureFail_Nomeasure;
}

int WfsProtocol::sendAllMonsb(MonsbTypeEnum eMonsbType, int * value, SendMeasureArgvs argvs)
{
    if (eMonsbType != MonsbType_rtu_state) {
        return SendMeasureFail_Noprovide;
    }
    return SendAllMonsbRtu(value);
}

int WfsProtocol::SendStates(const std::map<int, int>& addressStates, WfsFunctionCodeYxEnum eFunctionType)
{
    return SendStates2(addressStates, eFunctionType);
}

int WfsProtocol::SendStates1(ushort byFirstAddress, const uchar* states, int iCount, uchar byFunctionCode)
{
    if (iCount <= 0) {
        return SendMeasureFail_Nomeasure;
    }
    uchar byUserDataType = 1;
    ushort wCount = iCount;
    ushort wUserDataLenth = 2 + 5 + wCount;
    if (wUserDataLenth > CIWfsFixMaxUserDataLength)
        return SendMeasureFail_Exscope;
    _sendBuffer[CIWfsUserDataIndex + 0] = byUserDataType;
    _sendBuffer[CIWfsUserDataIndex + 1] = (wCount & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 2] = ((wCount >> 8) & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 3] = (byFirstAddress & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 4] = ((byFirstAddress >> 8) & 0xFF);
    memcpy(&_sendBuffer[CIWfsUserDataIndex + 5], states, wCount);
    uchar byControlCode = CIWfsControlCode_Initiactive;
    if (byFunctionCode == FunCode_YX_All_Query)
        byControlCode = CIWfsControlCode_Passivity;
    return SendWfsBuffer(byControlCode, CommandCode_YX, byFunctionCode, wUserDataLenth);
}

int WfsProtocol::SendStates2(const std::map<int, int>& addressStates, uchar byFunctionCode)
{
    if (addressStates.size() <= 0){
        return SendMeasureFail_Nomeasure;
    }
    uchar byUserDataType = 2;
    ushort wCount = addressStates.size();
    ushort wUserDataLenth = 2 + 3 + wCount * 3;
    if (wUserDataLenth > (CIWfsFixMaxUserDataLength))
        return SendMeasureFail_Exscope;
    _sendBuffer[CIWfsUserDataIndex + 0] = byUserDataType;
    _sendBuffer[CIWfsUserDataIndex + 1] = (wCount & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 2] = ((wCount >> 8) & 0xFF);
    std::map<int, int>::const_iterator it = addressStates.begin();
    int iIndex = CIWfsUserDataIndex + 3;
    while (it != addressStates.end()) {
        ushort wAddress = it->first;
        int iState = it->second;
        uchar byState = WfsDeviceState(iState);
        _sendBuffer[iIndex + 0] = (wAddress & 0xFF);
        _sendBuffer[iIndex + 1] = ((wAddress >> 8) & 0xFF);
        _sendBuffer[iIndex + 2] = byState;
        iIndex += 3;
        ++it;
    }
    uchar byControlCode = CIWfsControlCode_Initiactive;
    if (byFunctionCode == FunCode_YX_All_Query)
        byControlCode = CIWfsControlCode_Passivity;
    return SendWfsBuffer(byControlCode, CommandCode_YX, byFunctionCode, wUserDataLenth);
}
///--- --- --- --- --- --- --- --- --- --- --- --- --- ---
///--- --- --- --- sendMonsb --- --- --- --- end ---
///--- --- --- --- --- --- --- --- --- --- --- --- --- ---



///*** *** *** *** *** *** *** *** *** *** *** *** *** ***
///*** *** *** *** sendYcadd *** *** *** *** begin ***
///*** *** *** *** *** *** *** *** *** *** *** *** *** ***
int WfsProtocol::sendYcadd(const YcaddMeasure *oYcadd, double value, SendMeasureArgvs argvs)
{
    if (! oYcadd) {
        return SendMeasureFail_Nomeasure;
    }
    YcaddAddressWorkerTable* oTable = _ycSendTable;
    YcaddAddressWorker *oWorker = oTable->workerByMeasure(oYcadd);
    if (! oWorker)
        return SendMeasureFail_Nomeasure;
    std::map<int, double> addressValues;
    addressValues[oWorker->address()] = value;
    return SendAnalog2(addressValues, FunCode_YC_BianWei);
}

int WfsProtocol::sendYcadds(const std::map<const YcaddMeasure *, double> &ycaddValues, YcaddTypeEnum eYcaddType, SendMeasureArgvs argvs)
{
    if (ycaddValues.size() > 0 && eYcaddType == YcaddType_rtu_value){
        std::map<int, double> addressValues = _ycSendTable->addressValues(ycaddValues);
        return SendAnalog2(addressValues, FunCode_YC_BianWei);
    }
    return SendMeasureFail_Nomeasure;
}

int WfsProtocol::sendAllYcadd(YcaddTypeEnum eYcaddType, double * value, SendMeasureArgvs argvs)
{
    if (eYcaddType != YcaddType_rtu_value) {
        return SendMeasureFail_Noprovide;
    }
    YcaddAddressWorkerTable * oWorkerTable = _ycSendTable;
    const std::vector<YcaddAddressWorker*>* oWorkers = oWorkerTable->workers();
    if (oWorkerTable->isContinueAddress()) {
        int iFirstAddress = oWorkerTable->minAddress();
        int iCount = oWorkerTable->addressCount();
        std::vector<float> values(iCount);
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            const YcaddAddressWorker* oWorker = oWorkers->at(i);
            int iIndex = oWorker->address() - iFirstAddress;
            double dValue;
            if (value)
                dValue = * value;
            else
                dValue = ( oWorker->measure() ) ? oWorker->measure()->value() : oWorker->value();
            float fValue = dValue;
            values[iIndex] = fValue;
        }
        int iResult = SendAnalog1(iFirstAddress, values.data(), iCount, FunCode_YC_All_Initiactive);
        return iResult;
    } else {
        std::map<int, double> addressValues = oWorkerTable->addressValues();
        return SendAnalog2(addressValues, FunCode_YC_All_Initiactive);
    }
}

int WfsProtocol::SendAnalogs(const std::map<int, double> &addressValues, WfsFunctionCodeYcEnum eFunctionType)
{
    return SendAnalog2(addressValues, eFunctionType);
}

int WfsProtocol::SendAnalog1(ushort byFirstAddress, const float* values, int iCount, uchar byFunctionCode)
{
    if (iCount <= 0) {
        return SendMeasureFail_Nomeasure;
    }
    uchar byUserDataType = 1;
    ushort wCount = iCount;
    ushort wUserDataLenth = 2 + 6 + wCount * 4;
    if (wUserDataLenth > (CIWfsFixMaxUserDataLength))
        return SendMeasureFail_Exscope;
    _sendBuffer[CIWfsUserDataIndex + 0] = byUserDataType;
    _sendBuffer[CIWfsUserDataIndex + 1] = (wCount & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 2] = ((wCount >> 8) & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 3] = (byFirstAddress & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 4] = ((byFirstAddress >> 8) & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 5] = 0x01;
    memcpy(&_sendBuffer[CIWfsUserDataIndex + 6], values, wCount * 4);
    uchar byControlCode = CIWfsControlCode_Initiactive;
    //    if (byFunctionCode == FunCode_YC_All_Query)
    //        byControlCode = CIWfsControlCode_Passivity;
    return SendWfsBuffer(byControlCode, CommandCode_YC, byFunctionCode, wUserDataLenth);
}

int WfsProtocol::SendAnalog2(const std::map<int, double>& addressValues, uchar byFunctionCode)
{
    if (addressValues.size() <= 0){
        return SendMeasureFail_Nomeasure;
    }
    uchar byUserDataType = 2;
    ushort wCount = addressValues.size();
    ushort wUserDataLenth = 2 + 3 + wCount * 6;
    if (wUserDataLenth > (CIWfsFixMaxUserDataLength))
        return SendMeasureFail_Exscope;
    _sendBuffer[CIWfsUserDataIndex + 0] = byUserDataType;
    _sendBuffer[CIWfsUserDataIndex + 1] = (wCount & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 2] = ((wCount >> 8) & 0xFF);
    std::map<int, double>::const_iterator it = addressValues.begin();
    int iIndex = CIWfsUserDataIndex + 3;
    while (it != addressValues.end()) {
        ushort wAddress = it->first;
        double dValue = it->second;
        float fValue = dValue;
        _sendBuffer[iIndex + 0] = (wAddress & 0xFF);
        _sendBuffer[iIndex + 1] = ((wAddress >> 8) & 0xFF);
        _sendBuffer[iIndex + 2] = 0x01;
        memcpy(&_sendBuffer[iIndex + 3], &fValue, 4);
        iIndex += 6;
        ++it;
    }
    uchar byControlCode = CIWfsControlCode_Initiactive;
    //    if (byFunctionCode == FunCode_YC_All_Query)
    //        byControlCode = CIWfsControlCode_Passivity;
    return SendWfsBuffer(byControlCode, CommandCode_YC, byFunctionCode, wUserDataLenth);
}
///--- --- --- --- --- --- --- --- --- --- --- --- --- ---
///--- --- --- --- sendYcadd --- --- --- --- end ---
///--- --- --- --- --- --- --- --- --- --- --- --- --- ---



///*** *** *** *** *** *** *** *** *** *** *** *** *** ***
///*** *** *** *** sendControl *** *** *** *** begin ***
///*** *** *** *** *** *** *** *** *** *** *** *** *** ***
int WfsProtocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs)
{
    MonsbAddressWorker *oWorker = _ykSendTable->workerByMeasure(oMonsb);
    if( oWorker )
    {
        int iAddress = oWorker->address();
        MonsbControlFromToState rFromToState;
        rFromToState.first = fromValue;
        rFromToState.second = toValue;
        std::map<int, MonsbControlFromToState> addressFromToStates;
        addressFromToStates[iAddress] = rFromToState;
        return sendControls(addressFromToStates);
    }
    else {
        return SendControlFail_Nomeasure;
    }
}

int WfsProtocol::sendControls(const std::vector<std::pair<const MonsbMeasure*, int> > & monsbsToValues, const SendControlArgvs & argvs)
{
    if(monsbsToValues.size() > 0)
    {
        std::map<int, MonsbControlFromToState> addressFromToStates;
        for (size_t i = 0; i < monsbsToValues.size(); ++i)
        {
            const std::pair<const MonsbMeasure*, int> & monsbsToValue = monsbsToValues.at(i);
            const MonsbMeasure* oMonsb = monsbsToValue.first;
            int iToValue = monsbsToValue.second;
            MonsbAddressWorker* oWorker = _ykSendTable->workerByMeasure(oMonsb);
            if (oWorker)
            {
                int iAddress = oWorker->address();
                MonsbControlFromToState rFromToState;
                rFromToState.first = oMonsb->value();
                rFromToState.second = iToValue;
                std::map<int, MonsbControlFromToState> addressFromToStates;
                addressFromToStates[iAddress] = rFromToState;
            }
        }
        return sendControls(addressFromToStates);
    }
    else
    {
        return SendControlFail_Nomeasure;
    }
}

int WfsProtocol::sendControls(const std::map<int, MonsbControlFromToState>& addressFromToStates)
{
    uchar byUserDataType = 2;
    ushort wCount = addressFromToStates.size();
    ushort wUserDataLenth = 2 + 3 + wCount * 4;
    if (wUserDataLenth > CIWfsFixMaxUserDataLength)
        return SendControlFail_Exscope;
    _sendBuffer[CIWfsUserDataIndex + 0] = byUserDataType;
    _sendBuffer[CIWfsUserDataIndex + 1] = (wCount & 0xFF);
    _sendBuffer[CIWfsUserDataIndex + 2] = ((wCount >> 8) & 0xFF);
    std::map<int, MonsbControlFromToState>::const_iterator it = addressFromToStates.begin();
    int iIndex = CIWfsUserDataIndex + 3;
    while (it != addressFromToStates.end()) {
        ushort wAddress = it->first;
        MonsbControlFromToState rFromToState = it->second;
        uchar byFromState = WfsDeviceState(rFromToState.first);
        uchar byToState = WfsDeviceState(rFromToState.second);
        _sendBuffer[iIndex + 0] = (wAddress & 0xFF);
        _sendBuffer[iIndex + 1] = ((wAddress >> 8) & 0xFF);
        _sendBuffer[iIndex + 2] = byFromState;
        _sendBuffer[iIndex + 3] = byToState;
        iIndex += 4;
        ++it;
    }
    return SendWfsBuffer(CIWfsControlCode_Initiactive, CommandCode_YK, FunCode_YK_Control, wUserDataLenth);
}

int WfsProtocol::sendAllControl(int toValue, const SendControlArgvs & argvs)
{
    uchar byToValue = WfsDeviceState( toValue );
    if ( byToValue == 0 ) {
        return SendAllDiscontrol();
    } else {
        MonsbAddressWorkerTable * oWorkerTable = _ykSendTable;
        const std::vector<MonsbAddressWorker*>* oWorkers = oWorkerTable->workers();
        std::map<int, MonsbControlFromToState> addressFromToStates;
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            const MonsbAddressWorker* oWorker = oWorkers->at(i);
            if (! oWorker->measure())
                continue;
            int iAddress = oWorker->address();
            MonsbControlFromToState rFromToState;
            rFromToState.first = oWorker->measure()->value();
            rFromToState.second = byToValue;
            std::map<int, MonsbControlFromToState> addressFromToStates;
            addressFromToStates[iAddress] = rFromToState;
        }
        return sendControls(addressFromToStates);
    }
}

int WfsProtocol::SendAllDiscontrol()
{
    ushort wUnitLength = 2;
    return SendWfsBuffer(CIWfsControlCode_Initiactive, CommandCode_YK, FunCode_YK_AllDisable, wUnitLength);
}

int WfsProtocol::SendAllMonsbRtu(int * value)
{
    if (_stationTypeTarget == WfsTerminalType_FY2004)
    {
        SendSystemState();
    }
    MonsbAddressWorkerTable * oWorkerTable = _yxSendTable;
    const std::vector<MonsbAddressWorker*>* oWorkers = oWorkerTable->workers();
    if (oWorkers->size() < 0)
    {
        outProtocolPrompt() << "[sendAllMonsb] Worker Count < 0 ! so no sendAllMonsb" << cxEndLine;
        return SendMeasureFail_Nomeasure;
    }
    if (oWorkerTable->validMeasureValueCount() < _minCountValidYx) {
        outProtocolPrompt() << "[sendAllMonsb] ValidStateMinCount is false! so no sendAllMonsb" << cxEndLine;
        return SendMeasureFail_Unknow;
    }
    if (oWorkerTable->isContinueAddress())
    {
        int iCountUnit = CIWfsFixMaxUserDataLength - 32;
        int iMinAddress = oWorkerTable->minAddress();
        int iMaxAddress = oWorkerTable->maxAddress();
        int iFirstAddress = iMinAddress;
        int iResult = 0;
        while (iFirstAddress <= iMaxAddress)
        {
            int iCount = iMaxAddress - iFirstAddress + 1;
            if (iCount > iCountUnit) iCount = iCountUnit;
            std::vector<uchar> states(iCount);
            int iWorkerIndex = iFirstAddress - iMinAddress;
            int iWorkerLast = iWorkerIndex + iCount;
            for (int i = iWorkerIndex; i < iWorkerLast; ++i)
            {
                const MonsbAddressWorker* oWorker = oWorkers->at(i);
                int iIndex = oWorker->address() - iFirstAddress;
                int iValue;
                if (value)
                    iValue = * value;
                else
                    iValue = ( oWorker->measure() ) ? oWorker->measure()->value() : oWorker->value();
                states[iIndex] = WfsDeviceState(iValue);
            }
            iResult += SendStates1(iFirstAddress, states.data(), iCount, FunCode_YX_All_Initiactive);
            iFirstAddress += iCount;
        }
        return iResult;
    }
    else
    {
        std::map<int, int> addressValues = oWorkerTable->addressValues();
        return SendStates2(addressValues, FunCode_YX_All_Initiactive);
    }
}

int WfsProtocol::SendSystemState()
{
    int iSendLength = 0;
    for (std::size_t i = 0; i < f_systemStates.size(); ++i)
    {
        SystemState & state = f_systemStates[i];
        _sendBuffer[CIWfsUserDataIndex + 0] = (state.sourceStationType & 0xFF);;
        _sendBuffer[CIWfsUserDataIndex + 1] = ((state.sourceStationType >> 8) & 0xFF);
        _sendBuffer[CIWfsUserDataIndex + 2] = (state.sourceStationId & 0xFF);;
        _sendBuffer[CIWfsUserDataIndex + 3] = ((state.sourceStationId >> 8) & 0xFF);
        _sendBuffer[CIWfsUserDataIndex + 4] = state.sourceStationStateType;
        _sendBuffer[CIWfsUserDataIndex + 5] = state.sourceStationStateId;
        iSendLength += SendWfsBuffer(CIWfsControlCode_Initiactive, CommandCode_Sys, FunCode_Sys_Translate, 2 + 6);
    }
    return iSendLength;
}

int WfsProtocol::TranslateSystemState()
{
    int iSendLength = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol->typeSimpleName() == CS_ProtocolTypeSimpleName_Wfs)
        {
            WfsProtocol* oWfsProtocol = (WfsProtocol*)oProtocol;
            iSendLength += oWfsProtocol->SendSystemState();
        }
    }
    return iSendLength;
}
///--- --- --- --- --- --- --- --- --- --- --- --- --- ---
///--- --- --- --- sendControl --- --- --- --- end ---
///--- --- --- --- --- --- --- --- --- --- --- --- --- ---



void WfsProtocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
    if (monsbOldValues.size() > 0 && eMonsbType == MonsbType_rtu_state) {
        std::map<int, int> addressValues = _yxSendTable->addressValues(monsbOldValues, true);
        SendStates2(addressValues, FunCode_YX_BianWei);
    }
}

void WfsProtocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
    if (ycaddOldValues.size() > 0 && eYcaddType == YcaddType_rtu_value){
        std::map<int, double> addressValues = _ycSendTable->addressValues(ycaddOldValues, true);
        SendAnalog2(addressValues, FunCode_YC_BianWei);
    }
}

void WfsProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    ProtocolBase::protocolTimerTimeOut(oTimer);
    if (oTimer == _heartJumpTimer)
    {
        SendHeartJump( );
    }
}

void WfsProtocol::protocolAfterStart()
{
    _recState = 0;
    _recDataIndex = 0;
    _recStationTypeSource = _stationTypeTarget;
    _recStationTypeTarget = _stationTypeSource;
    _recStationNumSource = _stationNumTarget;
    _recStationNumTarget = _stationNumSource;
    _recProtocolCode = 0;
    _recControlCode = 0;
    _recDataLen = 0;
    _recCommonCode = 0;
    _recFunCode = 0;
    _recCrc = 0;

    memcpy(_recBuffer, WFSBufTitle, 4);

    _heartJumpTimer->setInterval(_heartJumpInterval);
    _heartJumpTimer->start();
}

void WfsProtocol::protocolAfterStop()
{
    _heartJumpTimer->stop();
}

bool WfsProtocol::provideSendControl(int iMeasureSonType, const MonsbMeasure *oMonsb)
{
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

