#include "iec104_protocol.h"


#include <vdi/monsb_control_task.h>


IEC104MasterProtocolFactory* IEC104MasterProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<IEC104MasterProtocolFactory>();

//IEC104SlaveProtocolFactory* IEC104SlaveProtocolFactory::s_instance =
//        ProtocolFactoryManager::createAndRegister<IEC104SlaveProtocolFactory>();

IEC104Protocol::IEC104Protocol()
{
    _yxRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, IEC104Protocol>("YxReceiveTable", MonsbType_rtu_state);
    _ykRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, IEC104Protocol>("YkReceiveTable", MonsbType_rtu_state);
    _ycRecTable  = createAndRegisterWorkerTable<YcaddAddressWorkerTable, IEC104Protocol>("YcReceiveTable", YcaddType_rtu_value);

    _yxSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, IEC104Protocol>("YxSendTable", MonsbType_rtu_state);
    _ykSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, IEC104Protocol>("YkSendTable", MonsbType_rtu_state);
    _ycSendTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, IEC104Protocol>("YcSendTable", YcaddType_rtu_value);

    _sendMonsbControl= createAndRegisterCases<IEC104Protocol, SendMonsbControlCases>(this);

    _timeCallUp       = new CxTimer();
    _timeCallUp      ->setTimerTimeOut(this);

    _callUpInterval = 3000;

    _started = false;
    _ykInitiative = true;
}

IEC104Protocol::~IEC104Protocol()
{
    delete _timeCallUp;
}

void IEC104Protocol::toContext(CxISetSkv &context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CS_EntryIEC104CallUpInterval, _callUpInterval);
}

void IEC104Protocol::fromContext(const CxIGetSkv &context)
{
    ProtocolBase::fromContext(context);
    int iInterval = 0;
    iInterval = context.getValue(CS_EntryIEC104CallUpInterval ,iInterval);
    if (iInterval != 0 && iInterval > 2000) {
        _callUpInterval = iInterval;
    }
}

void IEC104Protocol::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(std::string("CallUpInterval : ") + CxString::toString(_callUpInterval));
}

void IEC104Protocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
}

int IEC104Protocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs)
{
    if (! _ykInitiative) return SendControlFail_Unknow;
    MonsbAddressWorker* oWorker = _ykSendTable->workerByMeasure(oMonsb);
    if (oWorker) {
        return SendYK(45 , 6, oWorker->stationId(), oWorker->address(), toValue == CIRtuState_He);
    } else {
        return SendControlFail_Nomeasure;
    }
}

int IEC104Protocol::sendAllControl(int toValue, const SendControlArgvs & argvs)
{
    return -1;
}

bool IEC104Protocol::provideSendControl(int iMeasureSonType, const MonsbMeasure *oMonsb)
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

void IEC104Protocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    if (oTimer == _timeCallUp) {
        if (! _started) {
            SendStart();
        } else {
            CallUp();
        }
    }
}

void IEC104Protocol::protocolReceivedData(const uchar *pData, int iLength)
{
    for (int i = 0; i < iLength; ++i) {
        switch ( FRecBufState )
        {
        case 0:
            if ( pData [ i ] == 0x68 )
            {
                FRecBufState ++;
            }
            break;
        case 1:
        {
            FRecBufLen = pData [ i ] + 2;
            if ( ( FRecBufLen >= 6 ) && ( FRecBufLen <= C104BUFLEN ) )
            {
                FRecBuf [ 0 ] = 0x68;
                FRecBuf [ FRecBufState ] = pData [ i ];
                FRecBufState ++;
                FRecBufIndex = FRecBufState;
            }
            else
                FRecBufState = 0;
        }
            break;
        case 2:
        {
            FRecBuf [ FRecBufIndex ] = pData [ i ];
            FRecBufIndex ++;
            if ( FRecBufIndex >= FRecBufLen )
            {
                DealData ( );
                FRecBufState = 0;
            }
        }
            break;
        }
    }
}

void IEC104Protocol::channelAfterOpen(const CxChannelBase *oChannel)
{
    _started = false;
}

void IEC104Protocol::channelAfterClose(const CxChannelBase *oChannel)
{
    _started = false;
}

void IEC104Protocol::CallUp()
{
    if (_stationIndex > _stationIdAll.size() - 1) {
        _stationIndex = 0;
    }

    if (_stationIndex < _stationIdAll.size())
    {
        int stationId = _stationIdAll.at(_stationIndex);
        CallUp(stationId);
        _stationIndex ++;
    }
}

void IEC104Protocol::CallUp(ushort stationId)
{
    SendCallUp ( 6 , stationId);
}

int IEC104Protocol::SendFrameI ( uchar* data, int length )
{
    if (! _started) return -1;
    int result = -1;
    if ( ( length < 6 ) || ( length > C104BUFLEN ) )
        return result;
    data [ 0 ] = 0x68;
    data [ 1 ] = length - 2; //
    data [ 2 ] = (FCalcerS * 2) ; //I格式　第0帧
    data [ 3 ] = (FCalcerS * 2) >> 8 ;
    data [ 4 ] = (FCalcerR * 2) ;
    data [ 5 ] = (FCalcerR * 2) >> 8 ;
    result = writeData ( data, length );
    if ( result > 0 )
    {
        FCalcerS = FCalcerS + 1;
        if ( FCalcerS > 0x7FFF )
            FCalcerS = 0;
    }
    return result;
}

int IEC104Protocol::SendCallUp ( ushort reason, ushort stationId, int address)
{
    uchar data [ 16 ];
    data [ 6 ] = 0x64;
    data [ 7 ] = 0x01; // SQ= 0 一个信息体
    data [ 8 ] = reason;
    data [ 9 ] = reason >> 8;
    data [ 10 ] = stationId;
    data [ 11 ] = stationId >> 8;
    data [ 12 ] = ( address );
    data [ 13 ] = ( address >> 8 );
    data [ 14 ] = ( address >> 16 );
    data [ 15 ] = 0x14;
    return SendFrameI ( data, 16 );
}

int IEC104Protocol::SendYK( uchar funcode, ushort reason, ushort stationId, int address, bool gotoHe, bool isPerform)
{
    uchar data[ 16 ];
    uchar action;
    if (funcode == 45) {
        action = (gotoHe) ? 1 : 0;
    } else {
        action = (gotoHe) ? 2 : 1;
    }
    if (! isPerform) action += 0x80;
    data [ 6 ]  = funcode;
    data [ 7 ]  = 0x01;
    data [ 8 ]  = ( reason );
    data [ 9 ]  = ( reason >> 8 );
    data [ 10 ] = ( stationId );
    data [ 11 ] = ( stationId >> 8 );
    data [ 12 ] = ( address );
    data [ 13 ] = ( ( address >> 8 ) );
    data [ 14 ] = ( ( address >> 16 ) );
    data [ 15 ] = action;
    return SendFrameI ( data , 16 );
}

void IEC104Protocol::DealData()
{

    uchar aType;
    uchar APDU;
    aType = FRecBuf [ 2 ] & 0x03;
    if ( aType == 0x00 || aType == 0x02 )  // I frame
    {
        DealFrameI ( );
    }
    else
        if ( aType == 0x01 )  // S frame 确认帧
        {
            //
        }
        else
            if ( aType == 0x03 )  // U frame
            {
                APDU = FRecBuf [ 2 ];
                switch ( APDU )
                {
                case
                //0100 0011 Tester ACT
                0x43:
                {
                    SendChannelCheckConfirmed ( );
                }
                    break;
                    //1000 0011 Tester CON

                case 0x83:
                {
                    //FChannelCheckReplyTime = now;
                }
                    break;
                    //0000 0111 start ACT

                case 0x07:
                {
                    _started = true;
                    SendStartConfirmed ( );
                    ResetCalcer ( );
                }
                    break;
                    //0000 1011 start CON

                case 0x0B:
                {
                    _started = true;
                    ResetCalcer ( );
                }
                    break;
                    //0001 0011 stop ACT

                case 0x13:
                {
                    _started = false;
                    SendStopConfirmed ( );
                    ResetCalcer ( );
                }
                    break;
                }
            }
}

void IEC104Protocol::ResetCalcer ( )
{
    FCalcerR = 0;
    FCalcerS = 0;
    FRecBufIndex = 0;
}

void IEC104Protocol::DealFrameI ( )
{
    //召唤原因 $06激活，$07确认，$0A结束
    //aReason: Byte;

    //FCalcerS取对方的 “接收收计数器”
    FCalcerS = ( FRecBuf [ 4 ] + (FRecBuf [ 5 ] << 8) ) / 2;
    FCalcerR = ( FRecBuf [ 2 ] + (FRecBuf [ 3 ] << 8) ) / 2;
    //FCalcerR取对方的 “发送计数器” 加1
    FCalcerR = FCalcerR + 1;
    if ( FCalcerR > 0x7FFF )
        FCalcerR = 0;
    FRecTypeFlag = FRecBuf[6]; //类型标识
    bool bNeedSure = true;
    switch ( FRecTypeFlag )
    {
    //遥信
    case 1: case 3: case 30: case 31:
    {
        DealYX ( );
    }
        break;

        //遥测
    case 9: case 11: case 13: case 34: case 35: case 36:
    {
        DealYC ( );
    }
        break;

        //遥控
    case 45: case 46:
    {
        DealYK ( );
    }
        break;

        //召唤
    case 100:
    {
        ushort aStationNo = ( FRecBuf [ 11 ] << 8 ) + FRecBuf [ 10 ];
        //发本站数据
        DealCallUp ( aStationNo );
        bNeedSure = false;
    }
        break;
    }
    if (bNeedSure) {
        SendSureFrameI ( );
    }
}

int IEC104Address(const uchar* pData)
{
    int iAddress = ( * ( ( int * ) pData ) ) & 0xFFFFFF;
    return iAddress;
}

void IEC104Protocol::DealYX ( )
{
    /*
    case 1: case 3: case 30: case 31:
    它表示信息体是顺序的，还是非顺序的，并表示信息体的个数，如信息体数目等于0，则表示没有信息体。
    bit    8   7   6   5   4   3   2   1
        SQ
    SQ＝0，表示由信息对象地址寻址的单个信息元素或综合信息元素。
    应用服务数据单元可以由一个或多个同类的信息对象所组成。
    SQ＝1，表示同类的信息元素序列（即同一种格式的测量值），
    由信息对象地址来寻址，信息对象地址是顺序信息元素的第一个信息元素地址，后续信息元素的地址是从这个地址起顺序加1。在顺序信息元素的情况下每个应用服务数据单元仅安排一种信息对象。
    bit 1～7表示信息对象的数目。
    FRecBuf[10..11] 公共地址
    FRecBuf[12..14] 信息体地址
    */
    uchar typeFlag = FRecBuf [ 6 ]; //类型标识
    uchar count = FRecBuf [ 7 ] & 0x7F; //可变结构限定词
    bool isContinueAddress = ( ( FRecBuf [ 7 ] & 0x80 ) == 0x80 ) ? true : false; //可变结构限定词
    ushort stationId = ( FRecBuf [ 11 ] << 8 ) + FRecBuf [ 10 ]; //公共地址
    if ( std::find(_yxRecTable->stationIds().begin(), _yxRecTable->stationIds().end(), stationId) == _yxRecTable->stationIds().end() )
    {
        cxDebug() << protocolName() << " - DealYX : Common address [" << stationId << " ] Is Not Exist";
        return;
    }
    std::map<int, int> addressValues;
    try
    {
        for ( int i = 0; i < count; i ++ )
        {
            int address = 0;
            int state = 0xFF;
            switch ( typeFlag )
            {
            case 1: //单点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + i * 4 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 4 ] ));
                    state = FRecBuf [ 15 + i * 4 ] & 0x01;
                }
                else
                {
                    if ( ( 15 + i ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    state = FRecBuf [ 15 + i ] & 0x01;
                }
            }
                break;
            case 3: //双点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + i * 4 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 4 ] ));
                    state = FRecBuf [ 15 + i * 4 ] & 0x03;
                    if ( state == 0 )
                        state = 0xFF;
                    else
                        if ( state == 1 )
                            state = 0;
                        else
                            if ( state == 2 )
                                state = 1;
                }
                else
                {
                    if ( ( 15 + i ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    state = FRecBuf [ 15 + i ] & 0x03;
                    if ( state == 0 )
                        state = 0xFF;
                    else
                        if ( state == 1 )
                            state = 0;
                        else
                            if ( state == 2 )
                                state = 1;
                }
            }
                break;
            case 30: //带时标的单点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + 10 * i ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 11 ] ));
                    state = FRecBuf [ 15 + 11 * i ] & 0x01;
                }
                else
                {
                    if ( ( 15 + i ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    state = FRecBuf [ 15 + i ] & 0x01;
                }
            }
                break;
            case 31: //带时标的双点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + i * 10 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 11 ] ));
                    state = FRecBuf [ 15 + 11 * i ] & 0x03;
                    if ( state == 0 )
                        state = 0xFF;
                    else
                        if ( state == 1 )
                            state = 0;
                        else
                            if ( state == 2 )
                                state = 1;
                }
                else
                {
                    if ( ( 15 + i ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    state = FRecBuf [ 15 + i ] & 0x03;
                    if ( state == 0 )
                        state = 0xFF;
                    else
                        if ( state == 1 )
                            state = 0;
                        else
                            if ( state == 2 )
                                state = 1;
                }
            }
                break;
            } //case
            if ( ( state != 0 ) && ( state != 1 ) )
            {
                state = 2;
            }
            addressValues[address] = state;
        }
    }
    catch(...)
    {
    }
    if (addressValues.size()>0){
        _yxRecTable->receiveAddressValues(stationId, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

ushort IEC104Value2(const uchar* pData)
{
    ushort value = * ( ( ushort * ) pData );
    //    if (pData[2] )
    return value;
}

float IEC104Value4(const uchar* pData)
{
    float value = * ( ( float * ) pData );
    //    if (pData[2] )
    return value;
}

void IEC104Protocol::DealYC ( )
{
    /*
    1 + 1 + 4 + +
    1 + 1 + 2 + 2 + +
    遥测规一化、标度化的数据结构
    ( 3 + [3] * N )        连续 不带时标
    ( [3 + 3] * N )        不连续 不带时标
    ( 3 + [3 + 7] * N )    连续 带时标
    ( [3 + 3 + 7] * N )    不连续 带时标

    遥测短浮点的数据结构
    ( 3 + [5] * N )        连续 不带时标
    ( [3 + 5] * N )        不连续 不带时标
    ( 3 + [5 + 7] * N )    连续 带时标
    ( [3 + 5 + 7] * N )    不连续 带时标
    */
    /*
    case 9: case 11: case 13: case 34: case 35: case 36:
    它表示信息体是顺序的，还是非顺序的，并表示信息体的个数，如信息体数目等于0，则表示没有信息体。
    bit    8   7   6   5   4   3   2   1
        SQ
    SQ＝0，表示由信息对象地址寻址的单个信息元素或综合信息元素。
    应用服务数据单元可以由一个或多个同类的信息对象所组成。
    SQ＝1，表示同类的信息元素序列（即同一种格式的测量值），
    由信息对象地址来寻址，信息对象地址是顺序信息元素的第一个信息元素地址，后续信息元素的地址是从这个地址起顺序加1。在顺序信息元素的情况下每个应用服务数据单元仅安排一种信息对象。
    bit 1～7表示信息对象的数目。
    FRecBuf[10..11] 公共地址
    FRecBuf[12..14] 信息体地址
    */
    uchar typeFlag = FRecBuf [ 6 ]; //类型标识
    uchar count = FRecBuf [ 7 ] & 0x7F; //可变结构限定词
    bool isContinueAddress = ( ( FRecBuf [ 7 ] & 0x80 ) == 0x80 ) ? true : false; //可变结构限定词
    ushort stationId = ( FRecBuf [ 11 ] << 8 ) + FRecBuf [ 10 ]; //公共地址

    if ( std::find(_ycRecTable->stationIds().begin(), _ycRecTable->stationIds().end(), stationId) == _ycRecTable->stationIds().end() )
    {
        cxDebug() << protocolName() << " - DealYC : Common address [" << stationId << " ] Is Not Exist";
        return;
    }
    std::map<int, double> addressValues;
    try
    {
        for ( int i = 0; i < count; i ++ )
        {
            int address = 0;
            double value = 0;
            switch ( typeFlag )
            {
            case 9: case 11: //规一化、标度化
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 12 + (i + 1) * 6 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 6 ] ));
                    value = IEC104Value2(& ( FRecBuf [ 15 + i * 6 ] ));
                }
                else
                {
                    if ( ( 15 + (i + 1) * 3 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    value = IEC104Value2(& ( FRecBuf [ 15 + i * 3 ] ));
                }
            }
                break;
            case 13: //短浮点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 12 + (i + 1) * 8 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 8 ] ));
                    value = IEC104Value4(& ( FRecBuf [ 15 + i * 8 ] ));
                }
                else
                {
                    if ( ( 15 + (i + 1) * 5 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    value = IEC104Value4(& ( FRecBuf [ 15 + i * 5 ] ));
                }
            }
                break;
            case 34: case 35://带时标的规一化、标度化
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 12 + (i + 1) * 13 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 13 ] ));
                    value = IEC104Value2(& ( FRecBuf [ 15 + i * 13 ] ));
                }
                else
                {
                    if ( ( 15 + (i + 1) * 10 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    value = IEC104Value2(& ( FRecBuf [ 15 + i * 10 ] ));
                }
            }
                break;
            case 36: //带时标的短浮点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 12 + (i + 1) * 15 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 15 ] ));
                    value = IEC104Value4(& ( FRecBuf [ 15 + i * 15 ] ));
                }
                else
                {
                    if ( ( 15 + (i + 1) * 12 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    value = IEC104Value4(& ( FRecBuf [ 15 + i * 12 ] ));
                }
            }
                break;
            } //case
            addressValues[address] = value;
        }
    }
    catch(...)
    {
    }
    if (addressValues.size()>0){
        _ycRecTable->receiveAddressValues(stationId, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

void IEC104Protocol::DealYK()
{
    if ( _ykInitiative ) {

    } else {
        uchar vsq = FRecBuf [ 7 ];
        int count = vsq & 0x7F;
        ushort stationId = FRecBuf [ 10 ] + ( FRecBuf [ 11 ] << 8 );
        //地址连续
        if ( ( vsq && 0x80 ) == 0x80 )
        {
            int beginAddress = IEC104Address(& ( FRecBuf [ 12 ] ) );
            for ( int i = 0; i < count; i ++ )
            {
                int address = beginAddress + i;
                uchar action = FRecBuf [ 15 + i ];
                DealYK ( stationId, address, action );
            }
        }
        //地址不连续
        else
        {
            if (count > 0x3C)
                return;
            for ( int i = 0; i < count; i ++ )
            {
                int address = IEC104Address(& ( FRecBuf [ 12 + i * 4 ] ) );
                uchar action = FRecBuf [ 15 + i * 4 ];
                DealYK ( stationId, address, action );
            }
        }
    }
}

void IEC104Protocol::DealYK ( int stationId, int address, uchar ation)
{
    bool gotoHe;
    if ( FRecTypeFlag == 45 )//单点遥控
    {
        if ( ation == 1 )
        {
            gotoHe = true;
        }
        else
            if ( ation == 0 )
            {
                gotoHe = false;
            }
            else
            {
                return;
            }
    }
    else//单点遥控
    {
        if ( ation == 2 )
        {
            gotoHe = true;
        }
        else
            if ( ation == 1 )
            {
                gotoHe = false;
            }
            else
            {
                return;
            }
    }
    int fromState;
    int toState;
    if (gotoHe) {
        fromState = CIRtuState_Fen;
        toState = CIRtuState_He;
    } else {
        fromState = CIRtuState_He;
        toState = CIRtuState_Fen;
    }
    uchar data [ 16 ];
    MonsbAddressWorker* oWorker = _ykRecTable->workerByStationIdAddress( stationId, address );
    if ( oWorker != NULL && oWorker->measure())
    {
        MonsbControlTask* oTask = MonsbControlTaskManager::task(oWorker->measure(), fromState, toState);
        if (oTask) {
            memcpy ( data, FRecBuf , 16 );
            data [ 0 ] = 0x68;
            data [ 1 ] = 0x0E;
            data [ 8 ] = 0x07;
            writeData ( data, sizeof /*Length*/ ( data ) );
            cxDebug() << protocolName() << " - DealYK : Common address [" << stationId << " ], address [" << address << "] Can control !";
        } else {
            cxDebug() << protocolName() << " - DealYK : Common address [" << stationId << " ], address [" << address << "] Can Not control !";
        }
    }
}

void IEC104Protocol::DealCallUp ( ushort wStationId )
{

}

void IEC104Protocol::SendSureFrameI ( )
{
    uchar aBuf [ 6/* range 0..5*/ ];
    aBuf [ 0 ] = 0x68;
    aBuf [ 1 ] = 0x04;
    aBuf [ 2 ] = 0x01; //00000001
    aBuf [ 3 ] = 0x00;
    aBuf [ 4 ] = ( FCalcerR * 2 );
    aBuf [ 5 ] = ( FCalcerR * 2 >> 8 );
    writeData ( aBuf, sizeof /*Length*/ ( aBuf ) );
}

void IEC104Protocol::SendStart()
{
    uchar buf [ 6 ];
    buf [ 0 ] = 0x68;
    buf [ 1 ] = 0x04;
    buf [ 2 ] = 0x07;
    buf [ 3 ] = 0x00;
    buf [ 4 ] = 0x00;
    buf [ 5 ] = 0x00;
    writeData ( buf, sizeof ( buf ) );
}

void IEC104Protocol::SendChannelCheckConfirmed ( )
{
    uchar buf [ 6/* range 0..5*/ ];
    buf [ 0 ] = 0x68;
    buf [ 1 ] = 0x04;
    buf [ 2 ] = 0x83; //10000011
    buf [ 3 ] = 0x00;
    buf [ 4 ] = 0x00;
    buf [ 5 ] = 0x00;
    writeData ( buf, sizeof /*Length*/ ( buf ) );
}

void IEC104Protocol::SendStartConfirmed ( )
{
    uchar buf [ 6/* range 0..5*/ ];
    buf [ 0 ] = 0x68;
    buf [ 1 ] = 0x04;
    buf [ 2 ] = 0x0B; //00001011
    buf [ 3 ] = 0x00;
    buf [ 4 ] = 0x00;
    buf [ 5 ] = 0x00;
    writeData ( buf, sizeof /*Length*/ ( buf ) );
}

void IEC104Protocol::SendStopConfirmed ( )
{
    uchar buf [ 6/* range 0..5*/ ];
    buf [ 0 ] = 0x68;
    buf [ 1 ] = 0x04;
    buf [ 2 ] = 0x23; //00100011
    buf [ 3 ] = 0x00;
    buf [ 4 ] = 0x00;
    buf [ 5 ] = 0x00;
    writeData ( buf, sizeof /*Length*/ ( buf ) );
}

void IEC104Protocol::protocolAfterStart()
{
    FRecBufState = 0;
    FRecBufLen = 0;
    FRecBufIndex = 0;
    FCalcerS = 0;
    FCalcerR = 0;
    FRecTypeFlag = 0;
    _started = false;

    //init stationIds
    std::vector<int> stationIdAll;
    stationIdAll.insert(stationIdAll.end(), _yxRecTable->stationIds().begin(), _yxRecTable->stationIds().end());
    stationIdAll.insert(stationIdAll.end(), _ykRecTable->stationIds().begin(), _ykRecTable->stationIds().end());
    stationIdAll.insert(stationIdAll.end(), _ycRecTable->stationIds().begin(), _ycRecTable->stationIds().end());
    _stationIdAll.clear();
    for (size_t i = 0; i < stationIdAll.size(); ++i)
    {
        int iStationId = stationIdAll.at(i);
        if (std::find(_stationIdAll.begin(), _stationIdAll.end(), iStationId) == _stationIdAll.end())
        {
            _stationIdAll.push_back(iStationId);
        }
    }

    _stationIndex = 0;
    _timeCallUp->setInterval(_callUpInterval);
    _timeCallUp->start();
}

void IEC104Protocol::protocolAfterStop()
{
    _timeCallUp->stop();
}

void IEC104Protocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
}

void IEC104Protocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
}

IEC104MasterProtocol::IEC104MasterProtocol()
{
    _ykInitiative = true;
}

IEC104MasterProtocol::~IEC104MasterProtocol()
{
}

void IEC104MasterProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    IEC104Protocol::describeSelf(oDescribeUnit);
    CxDescribeInteger* oCallUpIntervalDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryIEC104CallUpInterval);
    oCallUpIntervalDescribe->initValue(_callUpInterval);
    oCallUpIntervalDescribe->setMinValue(0);
}

IEC104SlaveProtocol::IEC104SlaveProtocol()
{
    _ykInitiative = false;
}

IEC104SlaveProtocol::~IEC104SlaveProtocol()
{
}
