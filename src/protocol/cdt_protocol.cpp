#include "cdt_protocol.h"


#include <vdi/monsb_control_task.h>


CdtProtocolFactory* CdtProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<CdtProtocolFactory>();

const uchar GCdtFrameTitle[6] = {0xEB, 0x90, 0xEB, 0x90, 0xEB, 0x90};

CdtProtocol::CdtProtocol(){
    _yxSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, CdtProtocol>("YxSendTable", MonsbType_rtu_state);
    _ykSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, CdtProtocol>("YkSendTable", MonsbType_rtu_state);
    _ycSendTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, CdtProtocol>("YcSendTable", YcaddType_rtu_value);

    _yxRecTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, CdtProtocol>("YxReceiveTable", MonsbType_rtu_state);
    _ykRecTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, CdtProtocol>("YkReceiveTable", MonsbType_rtu_state);
    _ycRecTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, CdtProtocol>("YcReceiveTable", YcaddType_rtu_value);

    _sendMonsbCases = createAndRegisterCases<CdtProtocol, SendMonsbCases>(this);
    _sendYcaddCases = createAndRegisterCases<CdtProtocol, SendYcaddCases>(this);
    _sendMonsbControlCases = createAndRegisterCases<CdtProtocol, SendMonsbControlCases>(this);

    _sendMonsbCycle = createAndRegisterCycle<CdtProtocol, SendMonsbCycle>(this, MonsbType_rtu_state);
    _sendYcaddCycle = createAndRegisterCycle<CdtProtocol, SendYcaddCycle>(this, YcaddType_rtu_value);
    _sendMonsbControlCycle = createAndRegisterCycle<CdtProtocol, SendMonsbControlCycle>(this, MonsbType_rtu_state);

    _recIndex = 0;
    _recState = 0;
    _recControlWord = 0;
    _recTypeFlag = 0;
    _recUnitLength = 0;

    _controlWord = 0x71;
    _typeFlagRYX = 0xF4;
    _typeFlagSYX = 0xA9;
    _typeFlagRYC = 0x61;
    _typeFlagSYC = 0x61;
    _typeFlagRYK = 0xF4;
    _typeFlagSYK = 0xF4;
    _typeFlagRSG = 0x26;
    _stationNoSource = 0;
    _stationNoTarget = 0;
    _funCodeRYXBegin = 0xF0;
    _funCodeSYXBegin = 0xF0;
    _funCodeRYKBegin = 0xF0;
    _funCodeSYKBegin = 0;

    _sendAllYxInterval = 2000;
    _sendAllYcInterval = 5000;
    _sendAllYkInterval = 3000;

    _validStateMinCount = 0;
    _controlTaskRunInterval = 1000;
}

CdtProtocol::~CdtProtocol()
{
}

int CdtProtocol::SendCdtBuffer(uchar byTypeFlag, uchar byUnitLength, uchar byFunCodeBegin)
{
    if (byUnitLength <= 0) return -1;
    memcpy(&_sendData[0], &GCdtFrameTitle[0], 6);
    _sendData[6] = _controlWord;
    _sendData[7] = byTypeFlag;
    _sendData[8] = byUnitLength;
    _sendData[9] = _stationNoSource;
    _sendData[10]= _stationNoTarget;
    _sendData[11]= CxCrc::calcCRC8(&_sendData[6], 5);
    int iLength = 12 + 6 * byUnitLength;
    for (int i = 0; i < byUnitLength; ++i) {
        int iFirstAddress = 12 + i * 6;
        _sendData[iFirstAddress] = byFunCodeBegin + i;
        uchar byCrc = CxCrc::calcCRC8(&_sendData[iFirstAddress], 5);
        _sendData[iFirstAddress+5] = byCrc;
    }
    return writeData(_sendData, iLength);
}

int CdtProtocol::sendAllMonsb(MonsbTypeEnum eMonsbType, int * value, SendMeasureArgvs argvs)
{
    if (eMonsbType != MonsbType_rtu_state ) {
        return SendMeasureFail_Noprovide;
    }
    MonsbAddressWorkerTable* oWorkerTable = _yxSendTable;
    const std::vector<MonsbAddressWorker*>* oWorkers = oWorkerTable->workers();
    if (oWorkers->size() < 0) {
        outProtocolPrompt() << "[sendAllMonsb] Worker Count < 0 ! so no sendAllMonsb" << cxEndLine;
        return SendMeasureFail_Nomeasure;
    }
    if (oWorkerTable->validMeasureValueCount() < _validStateMinCount) {
         outProtocolPrompt() << "[sendAllMonsb] ValidStateMinCount is false! so no sendAllMonsb"<< cxEndLine;
        return SendMeasureFail_Unknow;
    }
    int iMaxValidAddress = (0xFF - _funCodeSYXBegin) * 32;
    int iMaxAddress = 0;
    memset(&_sendData[12], 0x00, 6 * (iMaxValidAddress / 32 + 1));
    if (value) {
        iMaxAddress = oWorkerTable->maxAddress();
        if ( * value == 1) {
            memset(&_sendData[12], 0xFF, 6 * (iMaxValidAddress / 32 + 1));
        }
    } else {
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            MonsbAddressWorker* oWorker = oWorkers->at(i);
            if (! oWorker->measure()) continue;
            int iAddress = oWorker->address();
            if (iAddress <0 || iAddress > iMaxValidAddress) continue;
            if (iAddress > iMaxAddress) iMaxAddress = iAddress;
            int st = oWorker->measure()->value();
            if ( st == 1 ) {
                int m = 13 + 6 * ( iAddress / 32 );
                m = m + ( iAddress / 8 ) % 4;
                CxBuffer::setBitTrue(&_sendData[m], iAddress % 8);
            }
        }
    }
    if (iMaxAddress > 0) {
        int iUnitLength = iMaxAddress / 32 + 1;
        return SendCdtBuffer(_typeFlagSYX, iUnitLength, _funCodeSYXBegin);
    } else {
        return SendMeasureFail_Nomeasure;
    }
}

int CdtProtocol::sendAllYcadd(YcaddTypeEnum eYcaddType, double * value, SendMeasureArgvs argvs)
{
    if (eYcaddType != YcaddType_rtu_value) {
        return SendMeasureFail_Noprovide;
    }
    YcaddAddressWorkerTable* oWorkerTable = _ycSendTable;
    int iMaxValidAddress = 0xFF;
    int iMaxAddress = 0;
    memset(&_sendData[12], 0x00, 6 * (iMaxValidAddress + 1));
    ushort wMaxValue = 0x03FF;
    ushort wInvalidValue = 0x8000;
    ushort wValue = wInvalidValue;
    int iValue;
    const std::vector<YcaddAddressWorker*>* oWorkers = oWorkerTable->workers();
    for (size_t i = 0; i < oWorkers->size(); ++i) {
        YcaddAddressWorker* oWorker = oWorkers->at(i);
        if (! oWorker->measure()) continue;
        int iAddress = oWorker->address();
        if (iAddress <0 || iAddress > iMaxValidAddress) continue;
        if (iAddress > iMaxAddress) iMaxAddress = iAddress;
        if (value){
            iValue = floor(* value / oWorker->bianbi());
        } else {
            iValue = floor(oWorker->measure()->value() / oWorker->bianbi());
        }
        if (iValue > wMaxValue)
            iValue = wInvalidValue;
        if (iValue < 0) {
            wValue = abs(iValue);
            wValue = (0x800 - wValue) | 0x800;
        } else {
            wValue = iValue;
        }
//        int iDataType = (wValue >> 11) & 0x01;
//        if(iDataType == 1)
//        {
////            wValue = ((0x7FF ^ wValue) + 1) & 0x8FF;

//        }
        uchar byLo = wValue;
        uchar byHi = wValue >> 8;
        uchar m = iAddress / 2;
        uchar n = 0;
        if ( ( iAddress % 2 ) > 0 )
            n = 2;
        _sendData [ 12 + m * 6 + 1 + n ] = byLo;
        _sendData [ 12 + m * 6 + 1 + n + 1 ] = byHi;
    }
    if (iMaxAddress > 0) {
        int iUnitLength = iMaxAddress / 2 + 1;
        return SendCdtBuffer(_typeFlagSYC, iUnitLength, 0x00);
    } else  {
        return SendMeasureFail_Nomeasure;
    }
}

int CdtProtocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs)
{
    const MonsbAddressWorker* oWorker = _ykSendTable->workerByMeasure(oMonsb);
    if (oWorker){
        int iAddress = oWorker->address();
        uchar byFunCodeSYKBegin = _funCodeSYKBegin;
        int iMaxValidAddress = (0xFF - byFunCodeSYKBegin + 1) * 32;
        if (iAddress <0 || iAddress > iMaxValidAddress)
            return SendControlFail_Exscope;
        uchar byUnitLength = iAddress / 32 + 1;
        memset(&_sendData[12], 0x00, 6 * (byUnitLength));
//        if (toValue == 1) {
            int m = 13 + 6 * ( iAddress / 32 );
            m = m + ( iAddress / 8 ) % 4;
            CxBuffer::setBitTrue(&_sendData[m], iAddress % 8);
//        }
        _sendMonsbControlCycle->holdOnAllContron(_controlTaskRunInterval * 3);
        return SendCdtBuffer(_typeFlagSYK, byUnitLength, byFunCodeSYKBegin);
    } else {
        return SendControlFail_Nomeasure;
    }
}

int CdtProtocol::sendAllControl(int toValue, const SendControlArgvs & argvs)
{
//    if (toValue != 0)
//        return SendControlFail_Noprovide;
    const std::vector<MonsbControlTask*>* oTasks = MonsbControlTaskManager::tasks();
    for (size_t i = 0; i < oTasks->size(); ++i) {
        const MonsbControlTask* oTask = oTasks->at(i);
        if (_ykSendTable->workerByMeasure( oTask->measure() )) {
            return SendControlFail_Unknow;
        }
    }
    int iMaxAddress = _ykSendTable->maxAddress();
    uchar byFunCodeSYKBegin = _funCodeSYKBegin;
    int iMaxValidAddress = (0xFF - byFunCodeSYKBegin + 1) * 32;
    if (iMaxAddress <0 || iMaxAddress > iMaxValidAddress)
        return -1;
    int iUnitLength = iMaxAddress / 32 + 1;
    memset(&_sendData[12], 0x00, 6 * (iUnitLength));
    return SendCdtBuffer(_typeFlagSYK, iUnitLength, byFunCodeSYKBegin);
}

void CdtProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
}

void CdtProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CS_EntryCdtControlWord          , _controlWord             );
    context.setValue(CS_EntryCdtTypeFlagRYX          , _typeFlagRYX             );
    context.setValue(CS_EntryCdtTypeFlagSYX          , _typeFlagSYX             );
    context.setValue(CS_EntryCdtTypeFlagRYC          , _typeFlagRYC             );
    context.setValue(CS_EntryCdtTypeFlagSYC          , _typeFlagSYC             );
    context.setValue(CS_EntryCdtTypeFlagRYK          , _typeFlagRYK             );
    context.setValue(CS_EntryCdtTypeFlagSYK          , _typeFlagSYK             );
    context.setValue(CS_EntryCdtTypeFlagRSG          , _typeFlagRSG             );
    context.setValue(CS_EntryCdtStationNoSource      , _stationNoSource         );
    context.setValue(CS_EntryCdtStationNoTarget      , _stationNoTarget         );
    context.setValue(CS_EntryCdtFunCodeRYXBegin      , _funCodeRYXBegin         );
    context.setValue(CS_EntryCdtFunCodeSYXBegin      , _funCodeSYXBegin         );
    context.setValue(CS_EntryCdtFunCodeRYKBegin      , _funCodeRYKBegin         );
    context.setValue(CS_EntryCdtFunCodeSYKBegin      , _funCodeSYKBegin         );
    context.setValue(CS_EntryCdtTimeSendAllYxInterval, _sendAllYxInterval   );
    context.setValue(CS_EntryCdtTimeSendAllYcInterval, _sendAllYcInterval   );
    context.setValue(CS_EntryCdtTimeSendAllYkInterval, _sendAllYkInterval   );
    context.setValue(CS_EntryCdtValidStateMinCount,    _validStateMinCount   );
}

void CdtProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
    _controlWord              = context.getValue(CS_EntryCdtControlWord              , _controlWord            );
    _typeFlagRYX              = context.getValue(CS_EntryCdtTypeFlagRYX              , _typeFlagRYX            );
    _typeFlagSYX              = context.getValue(CS_EntryCdtTypeFlagSYX              , _typeFlagSYX            );
    _typeFlagRYC              = context.getValue(CS_EntryCdtTypeFlagRYC              , _typeFlagRYC            );
    _typeFlagSYC              = context.getValue(CS_EntryCdtTypeFlagSYC              , _typeFlagSYC            );
    _typeFlagRYK              = context.getValue(CS_EntryCdtTypeFlagRYK              , _typeFlagRYK            );
    _typeFlagSYK              = context.getValue(CS_EntryCdtTypeFlagSYK              , _typeFlagSYK            );
    _typeFlagRSG              = context.getValue(CS_EntryCdtTypeFlagRSG              , _typeFlagRSG            );
    _stationNoSource          = context.getValue(CS_EntryCdtStationNoSource          , _stationNoSource        );
    _stationNoTarget          = context.getValue(CS_EntryCdtStationNoTarget          , _stationNoTarget        );
    _funCodeRYXBegin          = context.getValue(CS_EntryCdtFunCodeRYXBegin          , _funCodeRYXBegin        );
    _funCodeSYXBegin          = context.getValue(CS_EntryCdtFunCodeSYXBegin          , _funCodeSYXBegin        );
    _funCodeRYKBegin          = context.getValue(CS_EntryCdtFunCodeRYKBegin          , _funCodeRYKBegin        );
    _funCodeSYKBegin          = context.getValue(CS_EntryCdtFunCodeSYKBegin          , _funCodeSYKBegin        );
    _sendAllYxInterval    = context.getValue(CS_EntryCdtTimeSendAllYxInterval    , _sendAllYxInterval  );
    _sendAllYcInterval    = context.getValue(CS_EntryCdtTimeSendAllYcInterval    , _sendAllYcInterval  );
    _sendAllYkInterval    = context.getValue(CS_EntryCdtTimeSendAllYkInterval    , _sendAllYkInterval  );
    _validStateMinCount   = context.getValue(CS_EntryCdtValidStateMinCount       , _validStateMinCount  );

    _sendMonsbCycle->setSendInterval(_sendAllYxInterval);
    _sendYcaddCycle->setSendInterval(_sendAllYcInterval);
    _sendMonsbControlCycle->setSendInterval(_sendAllYkInterval);
}

void CdtProtocol::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(std::string("control Word : "            ) + CxString::toString(_controlWord          ));
    sReports.push_back(std::string("Station No Source : "       ) + CxString::toString(_stationNoSource      ));
    sReports.push_back(std::string("Station No Target : "       ) + CxString::toString(_stationNoTarget      ));
    sReports.push_back(std::string("TypeFlag Receive YX : "     ) + CxString::toString(_typeFlagRYX          ));
    sReports.push_back(std::string("TypeFlag Receive YC : "     ) + CxString::toString(_typeFlagRYC          ));
    sReports.push_back(std::string("TypeFlag Receive YK : "     ) + CxString::toString(_typeFlagRYK          ));
    sReports.push_back(std::string("TypeFlag Receive SG : "     ) + CxString::toString(_typeFlagRSG          ));
    sReports.push_back(std::string("TypeFlag Send YX : "        ) + CxString::toString(_typeFlagSYX          ));
    sReports.push_back(std::string("TypeFlag Send YC : "        ) + CxString::toString(_typeFlagSYC          ));
    sReports.push_back(std::string("TypeFlag Send YK : "        ) + CxString::toString(_typeFlagSYK          ));
    sReports.push_back(std::string("Begin FunCode Receive YX : ") + CxString::toString(_funCodeRYXBegin      ));
    sReports.push_back(std::string("Begin FunCode Send YX : "   ) + CxString::toString(_funCodeSYXBegin      ));
    sReports.push_back(std::string("Begin FunCode Receive YK : ") + CxString::toString(_funCodeRYKBegin      ));
    sReports.push_back(std::string("Begin FunCode Send YK : "   ) + CxString::toString(_funCodeSYKBegin      ));
    sReports.push_back(std::string("Time Send AllYx Interval : ") + CxString::toString(_sendAllYxInterval));
    sReports.push_back(std::string("Time Send AllYc Interval : ") + CxString::toString(_sendAllYcInterval));
    sReports.push_back(std::string("Time Send AllYk Interval : ") + CxString::toString(_sendAllYkInterval));
    sReports.push_back(std::string("Valid State Min Count : "   ) + CxString::toString(_validStateMinCount));
}

void CdtProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
    oDescribeUnit->setFuncDescribeValuesCalc(describeValuesCalc);
    CxDescribeInteger* oControlWordDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtControlWord          ); oControlWordDescribe->initValue(_controlWord                    ); oControlWordDescribe->SetIsHexDisplay(true)    ; oControlWordDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagRYXDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagRYX          ); oTypeFlagRYXDescribe->initValue(_typeFlagRYX                    ); oTypeFlagRYXDescribe->SetIsHexDisplay(true)    ; oTypeFlagRYXDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagSYXDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagSYX          ); oTypeFlagSYXDescribe->initValue(_typeFlagSYX                    ); oTypeFlagSYXDescribe->SetIsHexDisplay(true)    ; oTypeFlagSYXDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagRYCDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagRYC          ); oTypeFlagRYCDescribe->initValue(_typeFlagRYC                    ); oTypeFlagRYCDescribe->SetIsHexDisplay(true)    ; oTypeFlagRYCDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagSYCDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagSYC          ); oTypeFlagSYCDescribe->initValue(_typeFlagSYC                    ); oTypeFlagSYCDescribe->SetIsHexDisplay(true)    ; oTypeFlagSYCDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagRYKDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagRYK          ); oTypeFlagRYKDescribe->initValue(_typeFlagRYK                    ); oTypeFlagRYKDescribe->SetIsHexDisplay(true)    ; oTypeFlagRYKDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagSYKDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagSYK          ); oTypeFlagSYKDescribe->initValue(_typeFlagSYK                    ); oTypeFlagSYKDescribe->SetIsHexDisplay(true)    ; oTypeFlagSYKDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oTypeFlagRSGDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTypeFlagRSG          ); oTypeFlagRSGDescribe->initValue(_typeFlagRSG                    ); oTypeFlagRSGDescribe->SetIsHexDisplay(true)    ; oTypeFlagRSGDescribe->SetRang(0, 255)    ;
    CxDescribeInteger* oStationNoSourceDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtStationNoSource      ); oStationNoSourceDescribe->initValue(_stationNoSource            );
    CxDescribeInteger* oStationNoTargetDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtStationNoTarget      ); oStationNoTargetDescribe->initValue(_stationNoTarget            );
    CxDescribeInteger* oFunCodeRYXBeginDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtFunCodeRYXBegin      ); oFunCodeRYXBeginDescribe->initValue(_funCodeRYXBegin            ); oFunCodeRYXBeginDescribe->SetIsHexDisplay(true); oFunCodeRYXBeginDescribe->SetRang(0, 255);
    CxDescribeInteger* oFunCodeSYXBeginDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtFunCodeSYXBegin      ); oFunCodeSYXBeginDescribe->initValue(_funCodeSYXBegin            ); oFunCodeSYXBeginDescribe->SetIsHexDisplay(true); oFunCodeSYXBeginDescribe->SetRang(0, 255);
    CxDescribeInteger* oFunCodeRYKBeginDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtFunCodeRYKBegin      ); oFunCodeRYKBeginDescribe->initValue(_funCodeRYKBegin            ); oFunCodeRYXBeginDescribe->SetIsHexDisplay(true); oFunCodeRYXBeginDescribe->SetRang(0, 255);
    CxDescribeInteger* oFunCodeSYKBeginDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtFunCodeSYKBegin      ); oFunCodeSYKBeginDescribe->initValue(_funCodeSYKBegin            ); oFunCodeSYXBeginDescribe->SetIsHexDisplay(true); oFunCodeSYXBeginDescribe->SetRang(0, 255);

    CxDescribeInteger* oTimeSendAllYxIntervalDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTimeSendAllYxInterval); oTimeSendAllYxIntervalDescribe->initValue(_sendAllYxInterval);
    CxDescribeInteger* oTimeSendAllYcIntervalDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTimeSendAllYcInterval); oTimeSendAllYcIntervalDescribe->initValue(_sendAllYcInterval);
    CxDescribeInteger* oTimeSendAllYkIntervalDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtTimeSendAllYkInterval); oTimeSendAllYkIntervalDescribe->initValue(_sendAllYkInterval);
    CxDescribeInteger* oValidStateMinCountDescribe    = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryCdtValidStateMinCount);    oValidStateMinCountDescribe->initValue(_validStateMinCount);
}

void CdtProtocol::describeValuesCalc(const CxDescribeUnit *oDescribeUnit)
{
    CxDescribeEnum*    oProtocolRoleDescribe          = oDescribeUnit->describeSon<CxDescribeEnum>   (CS_EntryProtocolRole            );
    CxDescribeInteger* oTypeFlagRYXDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagRYX          );
    CxDescribeInteger* oTypeFlagSYXDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagSYX          );
    CxDescribeInteger* oTypeFlagRYCDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagRYC          );
    CxDescribeInteger* oTypeFlagSYCDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagSYC          );
    CxDescribeInteger* oTypeFlagRYKDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagRYK          );
    CxDescribeInteger* oTypeFlagSYKDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagSYK          );
    CxDescribeInteger* oTypeFlagRSGDescribe           = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTypeFlagRSG          );
    CxDescribeInteger* oStationNoSourceDescribe       = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtStationNoSource      );
    CxDescribeInteger* oStationNoTargetDescribe       = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtStationNoTarget      );
    CxDescribeInteger* oFunCodeRYXBeginDescribe       = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtFunCodeRYXBegin      );
    CxDescribeInteger* oFunCodeSYXBeginDescribe       = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtFunCodeSYXBegin      );
    CxDescribeInteger* oFunCodeRYKBeginDescribe       = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtFunCodeRYKBegin      );
    CxDescribeInteger* oFunCodeSYKBeginDescribe       = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtFunCodeSYKBegin      );
    CxDescribeInteger* oTimeSendAllYxIntervalDescribe = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTimeSendAllYxInterval);
    CxDescribeInteger* oTimeSendAllYcIntervalDescribe = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTimeSendAllYcInterval);
    CxDescribeInteger* oTimeSendAllYkIntervalDescribe = oDescribeUnit->describeSon<CxDescribeInteger>(CS_EntryCdtTimeSendAllYkInterval);

    if (oTypeFlagRYCDescribe) oTypeFlagRYCDescribe->setValue(0x61);
    if (oTypeFlagSYCDescribe) oTypeFlagSYCDescribe->setValue(0x61);
    if (oTypeFlagRSGDescribe) oTypeFlagRSGDescribe->setValue(0x26);
    if (oStationNoSourceDescribe) oStationNoSourceDescribe->setValue(0);
    if (oStationNoTargetDescribe) oStationNoTargetDescribe->setValue(0);
    if (oFunCodeRYXBeginDescribe) oFunCodeRYXBeginDescribe->setValue(0xF0);
    if (oFunCodeSYXBeginDescribe) oFunCodeSYXBeginDescribe->setValue(0xF0);
    if (oFunCodeRYKBeginDescribe) oFunCodeRYKBeginDescribe->setValue(0xF0);
    if (oFunCodeSYKBeginDescribe) oFunCodeSYKBeginDescribe->setValue(0x0);
    if (oProtocolRoleDescribe) {
        if (oProtocolRoleDescribe->value() == ProtocolRole_wf_fy2004 || oProtocolRoleDescribe->value() == ProtocolRole_wf_gk) {
            if (oTypeFlagRYXDescribe) oTypeFlagRYXDescribe->setValue(0xA9);
            if (oTypeFlagSYXDescribe) oTypeFlagSYXDescribe->setValue(0xF4);
            if (oTypeFlagRYKDescribe) oTypeFlagRYKDescribe->setValue(0xF4);
            if (oTypeFlagSYKDescribe) oTypeFlagSYKDescribe->setValue(0x0);
            if (oTimeSendAllYxIntervalDescribe) oTimeSendAllYxIntervalDescribe->setValue(2000);
            if (oTimeSendAllYcIntervalDescribe) oTimeSendAllYcIntervalDescribe->setValue(5000);
            if (oTimeSendAllYkIntervalDescribe) oTimeSendAllYkIntervalDescribe->setValue(0);
        } else {
            if (oTypeFlagRYXDescribe) oTypeFlagRYXDescribe->setValue(0xF4);
            if (oTypeFlagSYXDescribe) oTypeFlagSYXDescribe->setValue(0xA9);
            if (oTypeFlagRYKDescribe) oTypeFlagRYKDescribe->setValue(0x0);
            if (oTypeFlagSYKDescribe) oTypeFlagSYKDescribe->setValue(0xF4);
            if (oTimeSendAllYxIntervalDescribe) oTimeSendAllYxIntervalDescribe->setValue(0);
            if (oTimeSendAllYcIntervalDescribe) oTimeSendAllYcIntervalDescribe->setValue(0);
            if (oTimeSendAllYkIntervalDescribe) oTimeSendAllYkIntervalDescribe->setValue(3000);
        }
    }
}

void CdtProtocol::protocolAfterStart()
{
}

void CdtProtocol::protocolAfterStop()
{
}

void CdtProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    for (int i = 0; i < iLength; ++i) {
        switch ( _recState )
        {
        case 0:
        {
            if ( pData [ i ] == 0xEB )
                _recState = 1;
        }
            break;
        case 1:
        {
            if ( pData [ i ] == 0x90 )
                _recState = 2;
            else
                _recState = 0;
        }
            break;
        case 2:
        {
            if ( pData [ i ] == 0xEB )
                _recState = 3;
            else
                _recState = 0;
        }
            break;
        case 3:
        {
            if ( pData [ i ] == 0x90 )
                _recState = 4;
            else
                _recState = 0;
        }
            break;
        case 4:
        {
            if ( pData [ i ] == 0xEB )
                _recState = 5;
            else
                _recState = 0;
        }
            break;
        case 5:
        {
            if ( pData [ i ] == 0x90 )
            {
                _recState = 6;
                _recIndex = 0;
            }
            else
                _recState = 0;
        }
            break;
        case 6:
        { //开始处理控制字
            _recData [ _recIndex ] = pData [ i ];
            _recIndex ++;
            if ( _recIndex == 6 ) {
                if ( CxCrc::calcCRC8( & _recData [ 0 ], 5 ) == _recData [ 5 ] ) {
                    _recControlWord = _recData [ 0 ];
                    _recTypeFlag = _recData [ 1 ];
                    _recUnitLength = _recData [ 2 ];
                    if ( _recUnitLength > 0 )
                    {
                        _recState = 7;
                        _recIndex = 0;
                    }
                    else
                    {
                        _recState = 0;
                    }
                }
                else {
                    _recState = 0;
                }
            }
        }
            break;
        case 7:
        { //处理信息字
            _recData [ _recIndex ] = pData [ i ];
            _recIndex ++;
            if ( _recIndex == _recUnitLength * 6 )
            {
                _recState = 0;
                DealData();
            }
        }
            break; //end 7
        } // end case
    }
}

void CdtProtocol::DealData()
{
    if (_recTypeFlag == _typeFlagRYC) {
        DealRYC();
    } else if (_recTypeFlag == _typeFlagRSG) {
        DealRSG();
    } else if (_recTypeFlag == _typeFlagRYX || _recTypeFlag == _typeFlagRYK) {
        if (_typeFlagRYX == _typeFlagRYK){
            if (_recData[0]>0){
                DealRYX();
            } else {
                DealRYKbyFunCode();
            }
        } else if (_recTypeFlag == _typeFlagRYX) {
            DealRYX();
        } else {
            DealRYK();
        }
    }
}

void CdtProtocol::DealRYX()
{
    std::map<int, int> addressValues;
    int iProtocolId = protocolId();
    for (int i = 0; i < _recUnitLength; ++i) {
        int iFirstDataIndex = 6 * i;
        if ( CxCrc::calcCRC8( & _recData [ iFirstDataIndex ], 5 ) == _recData [ iFirstDataIndex + 5 ] )
        {
            int iFirstAddrss = 8 * 4 * ( _recData [ iFirstDataIndex ] - _funCodeRYXBegin );
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 8; ++k){
                    int iAddress = iFirstAddrss + 8 * j + k;
                    int st = ( ( _recData[ iFirstDataIndex + 1 + j ] >> k ) & 0x01 );
                    addressValues[iAddress] = st;
                }
            }
        }
    }
    _yxRecTable->receiveAddressValues(addressValues, iProtocolId);
}

void CdtProtocol::DealRYC()
{
    std::map<int, double> addressValues;
    int iProtocolId = protocolId();
    for (int i = 0; i < _recUnitLength; ++i) {
        int iDataIndex = 6 * i;
        if ( CxCrc::calcCRC8( & _recData [ iDataIndex ], 5 ) == _recData [ iDataIndex + 5 ] )
        {
            int iAddress = _recData [ iDataIndex ] * 2;
            uchar byHi = _recData [ iDataIndex + 2 ];
            int w = ( ( byHi & 0x07 ) << 8 ) + _recData [ iDataIndex + 1];
            if ( ( byHi & 0x08 ) != 0 ){
                w = 0 - ((0x7FF ^ w) + 1);
            }
            addressValues[iAddress] = w;
            //            if (oWorker && oWorker->measure()){
            //                double v = oWorker->Bianbi() * w;
            //                ycaddValues[oWorker->measure()] = v;
            //            }

            iAddress += 1;
            byHi = _recData [ iDataIndex + 4 ];
            w = ( ( byHi & 0x07 ) << 8 ) + _recData [ iDataIndex + 3];
            if ( ( byHi & 0x08 ) != 0 )
            {
                w = 0 - ((0x7FF ^ w) + 1);
            }
            addressValues[iAddress] = w;

            //            if (oWorker && oWorker->measure()){
            //                double v = oWorker->Bianbi() * w;
            //                ycaddValues[oWorker->measure()] = v;
            //            }
        }
    }
    _ycRecTable->receiveAddressValues(addressValues, iProtocolId);
}

void CdtProtocol::DealRYK()
{
    DealRYKbyFunCode();
}

void CdtProtocol::DealRYKbyFunCode()
{
    bool bAllow = false;
    int iAddress = -1;
    std::string sMeasureName;
    if (_recData [ 0 ] != _funCodeRYKBegin) return;
    for (int i = 0; i < _recUnitLength; ++i) {
        int iFirstDataIndex = 6 * i;
        if ( CxCrc::calcCRC8( & _recData [ iFirstDataIndex ], 5 ) == _recData [ iFirstDataIndex + 5 ] )
        {
            int iFirstAddrss = 8 * 4 * ( _recData [ iFirstDataIndex ] - _funCodeRYKBegin );
            for (int j = 0; j < 4; ++j) {
                uchar byData = _recData[ iFirstDataIndex + 1 + j ];
                if (byData > 0) {
                    bAllow = true;
                    for (int k = 0; k < 8; ++k){
                        int st = ( ( byData >> k ) & 0x01 );
                        if (st > 0) {
                            iAddress = iFirstAddrss + 8 * j + k;
                        }
                    }
                }
            }
        }
    }
    if (bAllow){
        MonsbControlTask* oTask = _ykRecTable->receiveControl(iAddress, protocolId());
        if (oTask){
            oTask->setRunInterval(_controlTaskRunInterval);
            if (oTask->measure()) {
                sMeasureName = oTask->measure()->measureName();
            }
        }
    } else {
        MonsbControlTaskManager::cancelControls(protocolId());
    }
    outProtocolPrompt() << CxString::format("Receive YK control[%s] measureName[%s]", CxString::toString(bAllow).c_str(), CxString::toString(sMeasureName).c_str()) << cxEndLine;
}

void CdtProtocol::DealRSG()
{
}

void CdtProtocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
    //is not rtu
    if( eMonsbType != MonsbType_rtu_state ) {
        return;
    }
    //has time send, no send monsb change
    if (_sendAllYxInterval < 2001) {
        return;
    }
    //send monsb change
    std::map<const MonsbMeasure *, int>::const_iterator it = monsbOldValues.begin();
    while (it != monsbOldValues.end()) {
        const MonsbMeasure *oMonsb = it->first;
        if (_yxSendTable->workerByMeasure(oMonsb)) {
            if (_sendMonsbCycle->sentTimes() > 0) {
                sendAllMonsb(MonsbType_rtu_state);
            }
            return;
        }
        ++it;
    }
}

void CdtProtocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
    //is not rtu
    if( eYcaddType != YcaddType_rtu_value ) {
        return;
    }
    //has time send, no send ycadd change
    if (_sendAllYcInterval < 5001) {
        return;
    }
    //send ycadd change
    std::map<const YcaddMeasure *, double>::const_iterator it = ycaddOldValues.begin();
    while (it != ycaddOldValues.end()) {
        const YcaddMeasure *oYcadd = it->first;
        if (_ycSendTable->workerByMeasure(oYcadd)) {
            if (_sendYcaddCycle->sentTimes() > 0) {
                sendAllYcadd(YcaddType_rtu_value);
            }
            return;
        }
        ++it;
    }
}

bool CdtProtocol::provideSendControl(int iMeasureSonType, const MonsbMeasure *oMonsb)
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


