#include "iec104_protocol.h"


#include <vdi/monsb_control_task.h>


IEC104MasterProtocolFactory* IEC104MasterProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<IEC104MasterProtocolFactory>();

IEC104Protocol::IEC104Protocol()
{
    _fVal = 0.0f;
    _bVal = false;

    setConnect(true);
}

IEC104Protocol::~IEC104Protocol()
{
}


int IEC104Protocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs)
{
    if (! _ykInitiative) return SendControlFail_Unknow;
    MonsbAddressWorker* oWorker = _ykSendTable->workerByMeasure(oMonsb);
    if (oWorker) {
        return sendYK(45 , 6, oWorker->stationId(), oWorker->address(), toValue == CIRtuState_He);
    } else {
        return SendControlFail_Nomeasure;
    }
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
    ProtocolCommon::protocolTimerTimeOut(oTimer);

    if (oTimer == _timeCallUp)
    {
        setConnect(true);

        if (! _started)
        {
            sendStart();
        }
        else
        {
            callUp();
        }
    }
}

void IEC104Protocol::protocolReceivedData(const uchar *pData, int iLength)
{
//    outProtocolDebug() << "protocolReceivedData:" <<iLength<<cxEndLine;
    setConnect(true);

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
                dealData( );
                FRecBufState = 0;
            }
        }
            break;
        }
    }
}

void IEC104Protocol::callUp()
{
    if (_stationIndex > _stationIdAll.size() - 1) {
        _stationIndex = 0;
    }

    if (_stationIndex < _stationIdAll.size())
    {
        int stationId = _stationIdAll.at(_stationIndex);
        callUp(stationId);
        _stationIndex ++;
    }
}

void IEC104Protocol::callUp(ushort stationId)
{
    sendCallUp ( 6 , stationId);
}

int IEC104Protocol::sendFrameI ( uchar* data, int length )
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
//输出太快，出现丢帧情况，每次发送一帧，延时100ms
//    push(data, length);
//    return length;
}
int IEC104Protocol::sendFrameI2 ( uchar* data, int length )
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
//输出太快，出现丢帧情况，每次发送一帧，延时100ms
    push(data, length);
    return length;
}


int IEC104Protocol::sendCallUp ( ushort reason, ushort stationId, int address)
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
    return sendFrameI ( data, 16 );
}
//遥控
int IEC104Protocol::sendYK( uchar funcode, ushort reason, ushort stationId, int address, bool gotoHe, bool isPerform)
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
    return sendFrameI2 ( data , 16 );
}

//遥调
int IEC104Protocol::sendYT( uchar funcode, ushort reason, ushort stationId, int address, char *value )
{
    if(value==NULL) return -1;

    memset(_sendBuf,0,GCL_LEN_DT);

    int pos = 6;
    _sendBuf[pos++] = funcode;
    _sendBuf[pos++]  = 0x01;
    _sendBuf[pos++]  = ( reason );
    _sendBuf[pos++]  = ( reason >> 8 );
    _sendBuf[pos++] = ( stationId );
    _sendBuf[pos++] = ( stationId >> 8 );
    _sendBuf[pos++] = ( address );
    _sendBuf[pos++] = ( ( address >> 8 ) );
    _sendBuf[pos++] = ( ( address >> 16 ) );

    if(funcode == 48 || funcode == 49) //标度化值 归一化值
    {
        memcpy(_sendBuf+pos,value,sizeof(int16));
        pos+=sizeof(int16);
        _sendBuf[pos++] = 0x00;//QOS 执行 0x00   选择 0x80
        return sendFrameI2 ( _sendBuf , pos );
    }
    if(funcode == 50) //单精度浮点
    {
        memcpy(_sendBuf+pos,value,sizeof(float));
        pos+=sizeof(float);
        _sendBuf[pos++] = 0x00;//QOS 执行 0x00 选择 0x80
        return sendFrameI2 ( _sendBuf , pos );
    }
    return -1;

}

//发送字符串
int IEC104Protocol::sendSTR(uchar funcode, ushort reason, ushort stationId, int address,char *value,int len)
{
    if(value==NULL || len>240 || len<1) return -1;

    memset(_sendBuf,0,GCL_LEN_DT);
    int pos = 6;
    _sendBuf[pos++] = funcode;
    _sendBuf[pos++]  = 0x01;
    _sendBuf[pos++]  = ( reason );
    _sendBuf[pos++]  = ( reason >> 8 );
    _sendBuf[pos++] = ( stationId );
    _sendBuf[pos++] = ( stationId >> 8 );
    _sendBuf[pos++] = ( address );
    _sendBuf[pos++] = ( ( address >> 8 ) );
    _sendBuf[pos++] = ( ( address >> 16 ) );

    //长度
    _sendBuf[pos++] = len;
    memcpy(_sendBuf+pos,value,len);
    pos += len;
    _sendBuf[pos++] = 0x00;//QOS 执行 0x00 选择 0x80
    return sendFrameI2 ( _sendBuf , pos );
}

void IEC104Protocol::dealData()
{

    uchar aType;
    uchar APDU;
    aType = FRecBuf [ 2 ] & 0x03;
    if ( aType == 0x00 || aType == 0x02 )  // I frame
    {
        dealFrameI ( );
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
                    sendChannelCheckConfirmed ( );
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
                    sendStartConfirmed ( );
                    resetCalcer ( );
                }
                    break;
                    //0000 1011 start CON

                case 0x0B:
                {
                    _started = true;
                    resetCalcer ( );
                }
                    break;
                    //0001 0011 stop ACT

                case 0x13:
                {
                    _started = false;
                    sendStopConfirmed ( );
                    resetCalcer ( );
                }
                    break;
                }
            }
}

void IEC104Protocol::resetCalcer ( )
{
    FCalcerR = 0;
    FCalcerS = 0;
    FRecBufIndex = 0;
}

void IEC104Protocol::dealFrameI ( )
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
        dealYX ( );
    }
        break;

        //遥测
    case 9: case 11: case 13: case 15: case 34: case 35: case 36:
    {
        dealYC ( );
    }
        break;

        //遥控
    case 45: case 46:
    {
        dealYK ( );
    }
        break;
    case 41://字符串上送
    {
        dealYw();
    }
        break;

        //召唤
    case 100:
    {
        ushort aStationNo = ( FRecBuf [ 11 ] << 8 ) + FRecBuf [ 10 ];
        //发本站数据
        dealCallUp ( aStationNo );
        bNeedSure = false;
    }
        break;
    }
    if (bNeedSure) {
        sendSureFrameI ( );
    }
}

int IEC104Address(const uchar* pData)
{
    int iAddress = ( * ( ( int * ) pData ) ) & 0xFFFFFF;
    return iAddress;
}

void IEC104Protocol::dealYX ( )
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
        outProtocolDebug() << protocolName() << " - DealYX : Common address [" << stationId << " ] Is Not Exist";
        return;
    }
    std::map<int, int> addressValues;
    bool  bValid = false;
    int iSum = 0;
    try
    {
        for ( int i = 0; i < count; i ++ )
        {
            int address = 0;
            int state = 0xFF;
            bValid = false;
            switch ( typeFlag )
            {
            case 1: //单点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + i * 4 ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf [ 15 + i * 4 ]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 + i * 4 ] ));
                        state = FRecBuf [ 15 + i * 4 ] & 0x01;
                    }
                }
                else
                {
                    if ( ( 15 + i ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf [15 + i]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                        state = FRecBuf [ 15 + i ] & 0x01;
                    }
                }
            }
                break;
            case 3: //双点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + i * 4 ) >= FRecBufLen )
                        break;

                    if(!(FRecBuf [ 15 + i * 4 ]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 + i * 4 ] ));
                        state = FRecBuf [ 15 + i * 4 ] & 0x03;
                        if ( state == 0 )
                        {
                            state = 0xFF;
                        }
                        else
                        {
                            if ( state == 1 )
                            {
                                state = 0;
                            }
                            else
                            {
                                if ( state == 2 )state = 1;
                            }
                        }
                    }
                }
                else
                {
                    if ( ( 15 + i ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf[15+i]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                        state = FRecBuf [ 15 + i ] & 0x03;
                        if ( state == 0 )
                        {
                            state = 0xFF;
                        }
                        else
                        {
                            if(state == 1)
                                state = 0;
                            else
                            {
                                if(state == 2)state = 1;
                            }
                        }
                    }
                }
            }
                break;
            case 30: //带时标的单点
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 15 + 11 * i ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf[ 15 + 11 * i]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 + i * 11 ] ));
                        state = FRecBuf [ 15 + 11 * i ] & 0x01;
                    }
                }
                else
                {
                    if ( ( 15 + i ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf[ 15 + i]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                        state = FRecBuf [ 15 + i ] & 0x01;
                    }
                }
            }
                break;
            case 31: //带时标的双点
            {
                if (!isContinueAddress )
                {
                    if ( ( 15 + i * 11 ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf[ 15 + 11 * i]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 + i * 11 ] ));
                        state = FRecBuf [ 15 + 11 * i ] & 0x03;
                        if ( state == 0 )
                            state = 0xFF;
                        else
                        {
                            if ( state == 1 )
                                state = 0;
                            else
                            {
                                if ( state == 2 )
                                    state = 1;
                            }
                        }
                    }
                }
                else
                {
                    if ( ( 15 + i ) >= FRecBufLen )
                        break;
                    if(!(FRecBuf[ 15 + i]&0x80))//值有效
                    {
                        bValid = true;
                        address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                        state = FRecBuf [ 15 + i ] & 0x03;
                        if ( state == 0 )
                            state = 0xFF;
                        else
                        {
                            if ( state == 1 )
                                state = 0;
                            else
                            {
                                if ( state == 2 )
                                    state = 1;
                            }
                        }
                    }
                }
            }
                break;
            default:
                break;

            } //case end

            //判断值是否有效
            if(bValid)
            {
                if ( ( state != 0 ) && ( state != 1 ) )
                {
                    state = 2;
                }

                //需改状态
                int newState =0;
                if(state==0) newState = GCL_PT_YX_OFF;
                else if(state==1) newState = GCL_PT_YX_ON;
                else newState = GCL_PT_YX_NC;

                addressValues[address] = newState;
            }
        }
    }
    catch(...)
    {
    }
    if (addressValues.size()>0){
        _yxRecTable->receiveAddressValues(stationId, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }

//    outProtocolDebug()<<"writeYX sum:"<<iSum<<cxEndLine;
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
int IEC104Value4_int(const uchar* pData)
{
    int value = * ( ( int * ) pData );
    //    if (pData[2] )
    return value;
}
void IEC104Protocol::dealYC ( )
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
        outProtocolDebug() << protocolName() << " - DealYC : Common address [" << stationId << " ] Is Not Exist";
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
            case 15: //累加量
            {
                if ( ! isContinueAddress )
                {
                    if ( ( 12 + (i + 1) * 8 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 + i * 8 ] ));
                    value = IEC104Value4_int(& ( FRecBuf [ 15 + i * 8 ] ));
                }
                else
                {
                    if ( ( 15 + (i + 1) * 5 ) > C104BUFLEN - 1 )
                        break;
                    address = IEC104Address(& ( FRecBuf [ 12 ] )) + i;
                    value = IEC104Value4_int(& ( FRecBuf [ 15 + i * 5 ] ));
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
    if (addressValues.size()>0)
    {
        _ycRecTable->receiveAddressValues(stationId, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

void IEC104Protocol::dealYw()
{

    uchar vsq = FRecBuf [ 7 ];
    int count = vsq & 0x7F;
    ushort commAddr = FRecBuf [ 10 ] + ( FRecBuf [ 11 ] << 8 );
    //地址连续
    if ( ( vsq && 0x80 ) == 0x80 )
    {
        int beginAddress = IEC104Address(& ( FRecBuf [ 12 ] ) );
        int pos = 0;
        std::map<int, std::string> addressValues;
        for ( int i = 0; i < count; i ++ )
        {
            int infoAddr = beginAddress + i;
            uchar len = FRecBuf [ 15 + pos ];
            pos++;
            if(len>0)
            {
                if((16+pos+len)>C104BUFLEN)break;

                addressValues[infoAddr] = std::string((char*)(&FRecBuf[ 16 + pos ]),(int)len);

                pos += len;
            }
        }
        _ywRecTable->receiveAddressValues(commAddr, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
    //地址不连续
    else
    {
        if (count > 0x3C)
            return;
        int pos = 0;
        std::map<int, std::string> addressValues;
        for ( int i = 0; i < count; i ++ )
        {

            int infoAddr = IEC104Address(& ( FRecBuf [ 12 + pos ] ) );
            uchar len = FRecBuf [ 15 + pos ];

            if((16+pos+len)>C104BUFLEN)break;

            addressValues[infoAddr] = std::string((char*)(&FRecBuf[ 16 + pos ]),(int)len);

            pos += len;
        }
        _ywRecTable->receiveAddressValues(commAddr, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

void IEC104Protocol::dealYK()
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
                dealYK ( stationId, address, action );
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
                dealYK ( stationId, address, action );
            }
        }
    }
}

void IEC104Protocol::dealYK ( int stationId, int address, uchar ation)
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
            outProtocolDebug() << protocolName() << " - DealYK : Common address [" << stationId << " ], address [" << address << "] Can control !";
        } else {
            outProtocolDebug() << protocolName() << " - DealYK : Common address [" << stationId << " ], address [" << address << "] Can Not control !";
        }
    }
}

void IEC104Protocol::dealCallUp ( ushort wStationId )
{

}

void IEC104Protocol::sendSureFrameI ( )
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

void IEC104Protocol::sendStart()
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

void IEC104Protocol::sendChannelCheckConfirmed ( )
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

void IEC104Protocol::sendStartConfirmed ( )
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

void IEC104Protocol::sendStopConfirmed ( )
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
    ProtocolCommon::protocolAfterStart();

    FRecBufState = 0;
    FRecBufLen = 0;
    FRecBufIndex = 0;
    FCalcerS = 0;
    FCalcerR = 0;
    FRecTypeFlag = 0;

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
}

void IEC104Protocol::protocolAfterStop()
{
    ProtocolCommon::protocolAfterStop();
}

void IEC104Protocol::doTask(GCCommon::GCDataElement dt)
{
    outProtocolDebug()<<CxTime::currentSystemTimeString()<<protocolName()<<dt.toStringEx()<<cxEndLine;

    ProtocolCommon::doTask(dt);

    switch(dt.type())
    {
    case GCL_CMD_SYS_CLOCK://校时
        break;
    case GCL_CMD_SYS_YK: //遥控
    {
        int  addr = dt.pram()>>16;
        bool bOpen = (dt.pram()&0xffff)>0?true:false;
        if(sendYK(45 , 6, 1, addr, bOpen)>0)
        {
//             sendInfo(GCL_FC_ALM_NML,CxString::format("send YK Success.cmd:[45],addr:[%d],action:[%d]",addr,bOpen), 0);
        }
        else
        {
//             sendInfo(GCL_FC_ALM_EVT,CxString::format("send YK error.cmd:[45]addr:[%d],action:[%d]",addr,bOpen), 0);
        }
        break;
    }
    case GCL_CMD_SYS_YT://遥调 设定值
    {
        int  addr = dt.pram()>>16; //地址
        int  fc = (dt.pram()&0xffff);//命令字
        if(sendYT(fc,6,1,addr,dt.buffer()))//发送遥调
        {
//             sendInfo(GCL_FC_ALM_NML,CxString::format("send YT Success.cmd:[%d],addr:[%d]",fc,addr), 0);
        }
        else
        {
//             sendInfo(GCL_FC_ALM_EVT,CxString::format("send YT error.cmd:[%d],addr:[%d]",fc,addr), 0);
        }
    }
        break;
    case GCL_CMD_SYS_SC_W://字符串写
    {
        int  addr = dt.pram()>>16; //地址
        int  fc = (dt.pram()&0xffff);//命令字
        if(sendSTR(fc,6,1,addr,dt.buffer(),dt.dataLenth()))//发送写字符串
        {
//            sendInfo(GCL_FC_ALM_NML,CxString::format("Send STR Success.cmd:[%d],addr:[%d],value:[%s]",fc,addr,dt.buffer()), 0);
        }
        else
        {
//            sendInfo(GCL_FC_ALM_NML,CxString::format("Send STR faile.cmd:[%d],addr:[%d],value:[%s]",fc,addr,dt.buffer()), 0);
        }
    }
        break;
    default:
        break;
    }
}

//发送命令数据
int IEC104Protocol::sendCustomData(int iSourceId, int iCommandId, std::map<string, string> sParams, int iDataLength, const char *pData)
{
    string sKeyCode ="code";
    string sKeyExt ="ext";

    cxDebug()<<"IEC104Protocol::sendCustomData"<<iCommandId<<CxString::join(sParams,"=",";") <<cxEndLine;

    switch(iCommandId)
    {
    case GCL_CMD_SYS_CLOCK://校时
        break;
    case GCL_CMD_SYS_YK: //遥控
    {
        string sCode;
        sCode = CxContainer::value(sParams,sKeyCode,sCode);
        uint32 code = CxString::toUint32(sCode);

        if(code==0)break;

        int  addr = code>>16;
        bool bOpen = (code&0xffff)>0?true:false;


        if(sendYK(45 , 6, 1, addr, bOpen)>0)
        {
            cxDebug()<<"sendYK OK"<<addr<<bOpen<<cxEndLine;
//             sendInfo(GCL_FC_ALM_NML,CxString::format("send YK Success.cmd:[45],addr:[%d],action:[%d]",addr,bOpen), 0);
        }
        else
        {
//             sendInfo(GCL_FC_ALM_EVT,CxString::format("send YK error.cmd:[45]addr:[%d],action:[%d]",addr,bOpen), 0);
        }
        break;
    }
    case GCL_CMD_SYS_YT://遥调 设定值
    {
        string sCode;
        sCode = CxContainer::value(sParams,sKeyCode,sCode);
        uint32 code = CxString::toUint32(sCode);
        if(code==0)break;

        int  addr = code>>16; //地址
        int  fc = code&0xffff;//命令字

        string sExt;
        sExt = CxContainer::value(sParams,sKeyExt,sExt);

        std::vector<char> v = CxString::hexToData(sExt);

        if(sendYT(fc,6,1,addr,v.data()))//发送遥调
        {
            cxDebug()<<"sendYT OK"<<addr<<fc<<sExt<<cxEndLine;
//             sendInfo(GCL_FC_ALM_NML,CxString::format("send YT Success.cmd:[%d],addr:[%d]",fc,addr), 0);
        }
        else
        {
            cxDebug()<<"sendYT fail"<<cxEndLine;
//             sendInfo(GCL_FC_ALM_EVT,CxString::format("send YT error.cmd:[%d],addr:[%d]",fc,addr), 0);
        }
    }
        break;
    case GCL_CMD_SYS_SC_W://字符串写
    {

        string sCode="";
        sCode = CxContainer::value(sParams,sKeyCode,sCode);
        uint32 code = CxString::toUint32(sCode);
        if(code==0)break;

        int  addr =code>>16; //地址
        int  fc = code&0xffff;//命令字

        string sExt="";
        sExt = CxContainer::value(sParams,sKeyExt,sExt);

        std::vector<char> v = CxString::hexToData(sExt);

        if(sendSTR(fc,6,1,addr,v.data(),v.size()))//发送写字符串
        {
//            sendInfo(GCL_FC_ALM_NML,CxString::format("Send STR Success.cmd:[%d],addr:[%d],value:[%s]",fc,addr,dt.buffer()), 0);
        }
        else
        {
//            sendInfo(GCL_FC_ALM_NML,CxString::format("Send STR faile.cmd:[%d],addr:[%d],value:[%s]",fc,addr,dt.buffer()), 0);
        }
    }
        break;
    default:
        break;
    }
    return true;
}

void IEC104Protocol::clock()
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
    CxDescribeInteger* oCallUpIntervalDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryReqInterval);
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
