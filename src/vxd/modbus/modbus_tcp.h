#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "modbus_tcp_head.h"
#include "modbus_common.h"

class ModBus_TCP_Protocol : public ModBus_Common_Protocol
{

public:
    ModBus_TCP_Protocol();

    ~ModBus_TCP_Protocol();
    //协议类型
    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_TCP; }
private:
    //数据处理
    int  findHeader(uchar *pData,int iLength);
    //帧检查
    bool checkFrame(uchar *pData, int iLength,int &nFrmLen);
    //
    int setUpFrame(bool isResp = true, bool sendNow=true);
    //发送缓冲区
    bool sendBuffer();

    bool protolAfterVfy();

    void protolAfterSend();
private:
    friend class TCP_ModbusMasterProtocol;
    friend class TCP_ModbusSlaveProtocol;
    ModBusTcpHead  _headRev,_headSend,_headLast;
};


class TCP_ModbusMasterProtocol : public ModBus_TCP_Protocol
{
public:
    TCP_ModbusMasterProtocol();

    ~TCP_ModbusMasterProtocol();

    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Master_TCP; }

//    void DescribeSelf(CxDescribeUnit* oDescribeUnit) const;

};

class TCP_ModbusSlaveProtocol : public ModBus_TCP_Protocol
{
public:
    TCP_ModbusSlaveProtocol();

    ~TCP_ModbusSlaveProtocol();

    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Slave_TCP; }

};

class TCP_ModbusMasterProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(TCP_ModbusMasterProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_Master_TCP;
    }

    ProtocolBase* createObject(){
        return new TCP_ModbusMasterProtocol();
    }
};

class TCP_ModbusSlaveProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(TCP_ModbusSlaveProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_Slave_TCP;
    }

    ProtocolBase* createObject(){
        return new TCP_ModbusSlaveProtocol();
    }
};


class ModBus_TCP_ProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(ModBus_TCP_ProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_TCP;
    }

    ProtocolBase* createObject(){
        return new ModBus_TCP_Protocol();
    }
};

#endif // MODBUS_TCP_H
