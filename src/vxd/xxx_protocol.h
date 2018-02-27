#ifndef HKVS_DEC_PROTOCOL_ICS_20140210_H
#define HKVS_DEC_PROTOCOL_ICS_20140210_H

#include <ccxx/ccxx.h>
#include "hkvsdec.h"
#include "protocol_global.h"
#include "protocol_base.h"
#include "monsb_address_worker.h"
#include "ycadd_address_worker.h"
#include "send_mcontrol_cases.h"


class HKVSDECProtocol : public ProtocolBase, public IProtocolSendMcontrol
{

public:

    HKVSDECProtocol();

    ~HKVSDECProtocol();

    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_LH_LED; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //control
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, SendControlArgvs argvs = SendControlArgvs::NullArgs());

    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL);

    //receive
    //YX
    inline const MonsbAddressWorkerTable* YxRecTable() const            { return _yxRecTable            ; }
    //YK
    inline const MonsbAddressWorkerTable* YkRecTable() const            { return _ykRecTable            ; }
    //YC
    inline const YcaddAddressWorkerTable* YcRecTable() const            { return _ycRecTable            ; }

    //send
    //YX
    inline const MonsbAddressWorkerTable* YxSendTable() const           { return _yxSendTable           ; }
    //YK
    inline const MonsbAddressWorkerTable* YkSendTable() const           { return _ykSendTable           ; }
    //YC
    inline const YcaddAddressWorkerTable* YcSendTable() const           { return _ycSendTable           ; }

    int sendYK(uchar funcode, ushort reason, ushort stationId, int address, bool gotoHe, bool isPerform = true);
protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void channelAfterOpen(const CxChannelBase* oChannel);

    void channelAfterClose(const CxChannelBase* oChannel);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

    void doTask(GCCommon::GCDataElement dt);

    void clock();
private:
    MonsbAddressWorkerTable* _yxRecTable;
    MonsbAddressWorkerTable* _ykRecTable;
    YcaddAddressWorkerTable* _ycRecTable;

    MonsbAddressWorkerTable* _yxSendTable;
    MonsbAddressWorkerTable* _ykSendTable;
    YcaddAddressWorkerTable* _ycSendTable;

    SendMcontrolCases* _sendMcontrol;

    CxTimer* _timeCallUp;

    //config
    int _callUpInterval;

    bool _started;
    bool _ykInitiative;

    //method
private:
    void callUp();

    void dealData();

    void resetCalcer();

    void dealYX();

    void dealYC();

    void dealYK();

    void dealYK(int stationId, int address, uchar ation);

    void dealCallUp(ushort wStationId);
private:
    bool _bRtShMStatus;
    HKVSDEC _hkvsDec;
    long _lUserID;
    bool _bLonIn;     
    int _nCurr;
    GIntVector _vJoinChan;
};


class HKVSDECProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(HKVSDECProtocolFactory)

public:
    GString FoctoryName(){
        return CS_ProtocolTypeSimpleName_HKVS_DEC;
    }

    ProtocolBase* CreateObject(){
        return new HKVSDECProtocol();
    }

};

#endif // HKVS_DEC_PROTOCOL_ICS_20140210_H
