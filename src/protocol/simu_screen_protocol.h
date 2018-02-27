#ifndef ICS_SimuScreen_PROTOCOL_20140415_H
#define ICS_SimuScreen_PROTOCOL_20140415_H


#include <vdi/protocol_common.h>
#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>


#define SYN_FLAG_TREND_LED  0xc3bd        /**!< 同步字                 */
#define SYN_HEAD_LED_BTN    0xbdc3     /**!< 数据包同步头   */

#define DIGIT_4     (4)
#define DIGIT_5     (5)


const std::string CS_ProtocolTypeSimpleName_SimuScreen = "SimuScreen";


//c3 bd ff ff db

//遥信：C3 BD ADD DATA1 DATA2 DATA3 DATA4 CRC8 00
//stationId (用Worker中StationId表示） : ADD
//byteIndex (用Worker中Address表示）   : 4个字节中第几个字节
//bitIndex  (用Worker中Port表示）      : 第几位

//遥测：BD C3 ADD MODE DATA1 DATA2 CRC8 00
//stationId (用Worker中StationId表示） : ADD
//mode (用Worker中Address表示）        : 模式
//dotIndex (用Worker中Port表示）       : 小数点位置


class SimuScreenProtocol : public ProtocolCommon
{
public:
    SimuScreenProtocol();

    ~SimuScreenProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_SimuScreen; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv & context);

    void describeSelf(CxDescribeUnit * oDescribeUnit) const;

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> & monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> & ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

    void checkProtocol();

private:
    int _ledType;
    CxTimer* _yxSendTimer;
    int _yxSendInterval;
    CxTimer* _ycSendTimer;
    int _ycSendInterval;
    uchar _sendBuff[GCL_LEN_DATA_EM];
//    GCVecSimuScreenYX _vObjYX;
//    GCVecSimuScreenYC _vObjYC;
    std::map<uint8,uint32> _sendYxAddValues;
    std::vector<uint32> _sendYxValues;

private:
    void doSendAll();

    void dealData();

private:
    void doInit();

    uint32 getCalVal(const MonsbAddressWorker * oWorker);

    void led_btn_set(uint8 adr, uint32 val);

    void trend_led_set(uint8 adr, uint8 mode, int32 dat, uint8 decimal_pos, uint8 led_type);

    bool get_board_color(uint8 adr, uint32 &color);

    void doSendAllYX();

    void doSendAllYC();

};


class SimuScreenProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(SimuScreenProtocolFactory)

public:
    std::string factoryName()
    {
        return CS_ProtocolTypeSimpleName_SimuScreen;
    }

    ProtocolBase* createObject()
    {
        return new SimuScreenProtocol();
    }

};

#endif // ICS_SimuScreen_PROTOCOL_20140415_H
