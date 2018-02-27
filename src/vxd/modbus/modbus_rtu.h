#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include "modbus_common.h"



class ModBus_RTU_Protocol : public ModBus_Common_Protocol
{

public:
    ModBus_RTU_Protocol();

    ~ModBus_RTU_Protocol();
    //协议类型
    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_RTU; }
public:
    //数据处理
    int  findHeader(uchar *pData,int iLength);
    //帧检查
    bool checkFrame(uchar *pData, int iLength,int &nFrmLen);
    //
    int setUpFrame(bool isResp = true, bool sendNow=true);
    //发送缓冲区
    bool sendBuffer();
    //再次校验
    bool protolAfterVfy(){return true;}

    void protolAfterSend(){}
private:
    friend class RTU_ModbusMasterProtocol;
    friend class RTU_ModbusSlaveProtocol;

};


class RTU_ModbusMasterProtocol : public ModBus_RTU_Protocol
{
public:
    RTU_ModbusMasterProtocol();

    ~RTU_ModbusMasterProtocol();

    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Master_RTU; }

//    void DescribeSelf(CxDescribeUnit* oDescribeUnit) const;

};

class RTU_ModbusSlaveProtocol : public ModBus_RTU_Protocol
{
public:
    RTU_ModbusSlaveProtocol();

    ~RTU_ModbusSlaveProtocol();

    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Slave_RTU; }

};

class RTU_ModbusMasterProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(RTU_ModbusMasterProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_Master_RTU;
    }

    ProtocolBase* createObject(){
        return new RTU_ModbusMasterProtocol();
    }
};

class RTU_ModbusSlaveProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(RTU_ModbusSlaveProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_Slave_RTU;
    }

    ProtocolBase* createObject(){
        return new RTU_ModbusSlaveProtocol();
    }
};


class ModBus_RTU_ProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(ModBus_RTU_ProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_RTU;
    }

    ProtocolBase* createObject(){
        return new ModBus_RTU_Protocol();
    }
};

#endif // MODBUS_RTU_H
