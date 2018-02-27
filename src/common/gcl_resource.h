#ifndef GCL_RESOURCE_H
#define GCL_RESOURCE_H

#include <string>


const std::string CS_SectionShareMemory     = "ShareMemory";
const std::string CS_EntryMonsbMapName     = "MonsbMapName";
const std::string CS_EntryMonsbCount     = "MonsbCount";
const std::string CS_EntryYcaddMapName     = "YcaddMapName";
const std::string CS_EntryYcaddCount     = "YcaddCount";
const std::string CS_EntryStrawMapName     = "StrawMapName";
const std::string CS_EntryStrawCount     = "StrawCount";
const std::string CS_EntryCalcMapName     = "CalcMapName";
const std::string CS_EntryCalcCount     = "CalcCount";
const std::string CS_EntryFileInfoMapName     = "FileInfoMapName";
const std::string CS_EntryFileInfoCount     = "FileInfoCount";


const std::string CS_EntryApplicationPath     = "AppPath";
const std::string CS_EntryConfigPath          = "ConfigPath";
const std::string CS_EntryMeasurePath         = "MeasurePath";
const std::string CS_EntryDatabasePath        = "DatabasePath";
const std::string CS_EntryTempPath            = "TempPath";
const std::string CS_EntryLogPath             = "LogPath";
const std::string CS_EntryReportPath          = "ReportPath";
const std::string CS_EntryTerminalPath        = "TerminalPath";
const std::string CS_EntryDescribePath        = "DescribePath";
const std::string CS_EntryBusinessPath        = "BusinessPath";
const std::string CS_EntryMonsbMeasurePath    = "MonsbMeasurePath";
const std::string CS_EntryYcaddMeasurePath    = "YcaddMeasurePath";
const std::string CS_EntryStrawMeasurePath    = "StrawMeasurePath";
const std::string CS_EntryCalcMeasurePath    = "CalcMeasurePath";
const std::string CS_EntryMonsbOperatesPath   = "MonsbOperatesPath";
const std::string CS_EntryScriptPath          = "ScriptPath";
const std::string CS_EntryUiPath              = "UiPath";
const std::string CS_EntryDataPath              = "DataPath";
const std::string CS_EntryModelPath              = "ModelPath";

const std::string CS_DirNameConfig          = "config";
const std::string CS_DirNameDatabase        = "db";
const std::string CS_DirNameMeasure         = "measure";
const std::string CS_DirNameData            = "data";
const std::string CS_DirNameModel           = "model";
const std::string CS_DirNameTemp            = "temp";
const std::string CS_DirNameLog             = "log";
const std::string CS_DirNameReport          = "report";
const std::string CS_DirNameTerminal        = "terminal";
const std::string CS_DirNameBusiness        = "business";
const std::string CS_DirNameScript          = "script";
const std::string CS_DirNameUi              = "ui";
const std::string CS_DirNameDescribe        = "describe";
const std::string CS_FileNameMonsbMeasure   = "monsb.measure.txt";
const std::string CS_FileNameYcaddMeasure   = "ycadd.measure.txt";
const std::string CS_FileNameStrawMeasure   = "straw.measure.txt";
const std::string CS_FileNameCalcMeasure   = "calc.measure.txt";
const std::string CS_FileNameMonsbOperates  = "monsb.operates.txt";


const std::string CS_EntryIsAutoStart         = "IsAutoStart";
const std::string CS_EntryIsTesting           = "IsTesting";
const std::string CS_EntryIsWriteLog          = "IsWriteLog";
const std::string CS_EntryIsStartWatchdog     = "IsStartWatchdog";
const std::string CS_EntryIsLoadMonsbOperate  = "IsLoadMonsbOperate";
const std::string CS_EntryDefaultDbName       = "DefaultDbName";

const std::string CS_EntryChannelState        = "IsChanState";
const std::string CS_EntryChannelBaseAddr     = "ChannelBaseAddr";
const std::string CS_EntryMQIP                = "MQIP";
const std::string CS_EntryMQPort              = "MQPort";
const std::string CS_EntryByteInverse         = "ByteInverse";
//const std::string CS_EntryOutInterval         = "OutInterval";

const std::string CS_EntryDBType_ICS          = "DBType_ICS";
const std::string CS_EntryDSN_ICS             = "DSN_ICS";
const std::string CS_EntryUSER_ICS            = "User_ICS";
const std::string CS_EntryPW_ICS              = "PassWord_ICS";
const std::string CS_EntryShareMemSize         = "ShareMemSize";

const std::string CS_EntryMonsbType = "MonsbType";
const std::string CS_EntryYcaddType = "YcaddType";
const std::string CS_EntryStrawType = "StrawType";
const std::string CS_EntryCalcType = "CalcType";

const std::string CS_EntryWorkerAddress = "Address";
const std::string CS_EntryWorkerPort = "Port";
const std::string CS_EntryWorkerStationId = "StationId";
const std::string CS_EntryWorkerStationName = "StationName";


const std::string CS_TypeSimpleNameMeasure = "Measure";
const std::string CS_TypeSimpleNameMonsb = "Monsb";
const std::string CS_TypeSimpleNameYcadd = "Ycadd";
const std::string CS_TypeSimpleNameStraw = "Straw";
const std::string CS_TypeSimpleNameCalc = "Calc";


const std::string CS_EntryMeasureId    = "MeasureId";
const std::string CS_EntryContainerId  = "ContainerId";
const std::string CS_EntryContainerType= "ContainerType";
const std::string CS_EntryMeasureName  = "MeasureName";
const std::string CS_EntryMaxValue     = "MaxValue";
const std::string CS_EntryMinValue     = "MinValue";
const std::string CS_EntryDefaultValue = "DefaultValue";
const std::string CS_EntryValue        = "Value";
const std::string CS_EntryBianbi       = "Bianbi";
const std::string CS_EntryProperty     = "Property";
const std::string CS_EntryUrl          = "Url";
const std::string CS_EntryEqualStrategy= "EqualStrategy";
const std::string CS_EntryDeadValue    = "DeadValue";


const std::string CS_EntryWorkerName = "WorkerName";
const std::string CS_EntryWorkerCode = "WorkerCode";
const std::string CS_EntryExtension = "Extension";


const std::string CS_SectionTerminal = "Terminal";
const std::string CS_SectionBusiness = "Business";
const std::string CS_EntryTerminalId = "TerminalId";
const std::string CS_EntryProtocol = "Protocol";
const std::string CS_EntryBusiness = "Business";
const std::string CS_SectionConfigModel = "ConfigModel";
const std::string CS_SectionUiFiles = "UiFiles";

const std::string CS_SectionTerminals = "Terminals";
const std::string CS_EntryTerminalNameTypes = "NameTypes";

const std::string CS_SectionBusinesses = "Businesses";
const std::string CS_EntryBusinessNameTypes = "NameTypes";

const std::string CS_SectionConfigModels = "ConfigModels";
const std::string CS_EntryConfigModelNameTypes = "NameTypes";

const std::string CS_SectionConfigCatalogs = "ConfigCatalogs";
const std::string CS_EntryConfigCatalogNameTypes = "NameTypes";

const std::string CS_EntryModelName         = "ModelName";
const std::string CS_EntryModelTitle        = "ModelTitle";
const std::string CS_EntryModelType         = "ModelType";
const std::string CS_EntryDescribeFilePath  = "DescribeFilePath";
const std::string CS_EntryDataFilePath      = "DataFilePath";
const std::string CS_EntryDataSection       = "DataSection";
const std::string CS_EntryUiFilePath        = "UiFilePath";
const std::string CS_EntryCatalogName       = "CatalogName";
const std::string CS_EntryNewAttach         = "NewAttach";


const std::string CS_EntryFilePath        = "FilePath";
const std::string CS_EntrySection= "Section";
const std::string CS_EntryEntry        = "Entry";


const std::string CS_NewModeBusiness = "business";


const std::string CS_ProtocolTypeSimpleName_Wfs = "Wfs";
const std::string CS_ProtocolTypeSimpleName_Psm = "Psm";
const std::string CS_ProtocolTypeSimpleName_Cdt = "Cdt";
const std::string CS_ProtocolTypeSimpleName_Cvt = "Cvt";
const std::string CS_ProtocolTypeSimpleName_Kbq = "Kbq";
const std::string CS_ProtocolTypeSimpleName_Key = "Key";
const std::string CS_ProtocolTypeSimpleName_SHM = "ShareMemory";

//modbus
const std::string CS_ProtocolTypeSimpleName_Modbus_Common = "Modbus_Common";
const std::string CS_ProtocolTypeSimpleName_Modbus_TCP = "Modbus_TCP";
const std::string CS_ProtocolTypeSimpleName_Modbus_Master_TCP = "ModbusMaster_TCP";
const std::string CS_ProtocolTypeSimpleName_Modbus_Slave_TCP = "ModbusSlave_TCP";
const std::string CS_ProtocolTypeSimpleName_Modbus_RTU = "Modbus_RTU";
const std::string CS_ProtocolTypeSimpleName_Modbus_Master_RTU = "ModbusMaster_RTU";
const std::string CS_ProtocolTypeSimpleName_Modbus_Slave_RTU = "ModbusSlave_RTU";
//IEC104
const std::string CS_ProtocolTypeSimpleName_IEC104 = "IEC104";
const std::string CS_ProtocolTypeSimpleName_IEC104_Master = "IEC104Master";
const std::string CS_ProtocolTypeSimpleName_IEC104_Slave = "IEC104Slave";

const std::string CS_ProtocolTypeSimpleName_Dixian = "Dixian";
const std::string CS_ProtocolTypeSimpleName_Nanrui = "Nanrui";
const std::string CS_ProtocolTypeSimpleName_Utjz = "Utjz";
const std::string CS_ProtocolTypeSimpleName_CZP = "CZP";
const std::string CS_ProtocolTypeSimpleName_IEC61850 = "IEC61850";
const std::string CS_ProtocolTypeSimpleName_IEC61850Server = "IEC61850Server";
const std::string CS_ProtocolTypeSimpleName_IEC61850Client = "IEC61850Client";
const std::string CS_ProtocolTypeSimpleName_Transfer = "Transfer";
const std::string CS_ProtocolTypeSimpleName_Modbus = "Modbus";
const std::string CS_ProtocolTypeSimpleName_Modbus_Master = "Modbus_Master";
const std::string CS_ProtocolTypeSimpleName_Modbus_Slave = "Modbus_Slave";
const std::string CS_ProtocolTypeSimpleName_Asm570 = "ASM570";

const std::string CS_ProtocolTypeSimpleName_Camera = "Camera";
const std::string CS_ProtocolTypeSimpleName_Iccard = "Iccard";
const std::string CS_ProtocolTypeSimpleName_Plc = "Plc";
const std::string CS_ProtocolTypeSimpleName_Print = "Print";
const std::string CS_ProtocolTypeSimpleName_Rfid = "Rfid";

//ONE_CMD
const std::string CS_ProtocolTypeSimpleName_QueryInterval = "QueryInterval";
const std::string CS_ProtocolTypeSimpleName_SendBuf = "SendBuf";
const std::string CS_ProtocolTypeSimpleName_RecvBuf = "RecvBuf";
const std::string CS_ProtocolTypeSimpleName_RecvLen = "RecvLen";
const std::string CS_ProtocolTypeSimpleName_BeginIndex = "BeginIndex";
const std::string CS_ProtocolTypeSimpleName_CrcType = "CrcType";
const std::string CS_ProtocolTypeSimpleName_WStrType = "WStrType";
const std::string CS_ProtocolTypeSimpleName_RStrType = "RStrType";
const std::string CS_ProtocolTypeSimpleName_iTypeId = "iTypeId";



//单个查询命令
const std::string CS_ProtocolTypeSimpleName_ONE_CMD     = "ONE_CMD";
const std::string CS_ProtocolTypeSimpleName_SIS_VESTOR     = "SIS_VESTOR";
const std::string CS_ProtocolTypeSimpleName_SIS_eDNA   = "SIS_eDNA";
//亮华LED
const std::string CS_ProtocolTypeSimpleName_LH_LED     = "LH_LED";
//海康威视解码器
const std::string CS_ProtocolTypeSimpleName_HKVS       = "HKVS";
//海康威视解码器
const std::string CS_ProtocolTypeSimpleName_HKVS_DEC   = "HKVS_DEC";
//海康威视摄像头
const std::string CS_ProtocolTypeSimpleName_HKVS_IPC   = "HKVS_IPC";
//海康威视录像机
const std::string CS_ProtocolTypeSimpleName_HKVS_NVR   = "HKVS_NVR";
//气动检测终端控制器
const std::string CS_ProtocolTypeSimpleName_ASM582_V100 = "ASM582_V100";
//T_Matic 气动传输
const std::string CS_ProtocolTypeSimpleName_ASM580_V100 = "ASM580_V100";
//博瑞科技HDID 拼接控制器
const std::string CS_ProtocolTypeSimpleName_HDID_BV     = "HDID_BV";
//大华IPC
const std::string CS_ProtocolTypeSimpleName_DH_IPC   = "DH_IPC";
//大华录像机
const std::string CS_ProtocolTypeSimpleName_DH_NVR   = "DH_NVR";
//大华解码器
const std::string CS_ProtocolTypeSimpleName_DH_DEC   = "DH_DEC";
//深圳建业模型 沙盘模型
const std::string CS_ProtocolTypeSimpleName_CSMT_MODEL   = "CSMT_MODEL";
//钥匙管理机
const std::string CS_ProtocolTypeSimpleName_KEY_MANAGENMENT   = "KEY_MANAGENMENT";
//迈斯门禁
const std::string CS_ProtocolTypeSimpleName_ACS_MAX   = "ACS_MAX";
//中控门禁
const std::string CS_ProtocolTypeSimpleName_ACS_ZKT   = "ACS_ZKT";
//CMT_M1V2  CMT无线模块
const std::string CS_ProtocolTypeSimpleName_CMT_M1V2   = "CMT_M1V2";
const std::string CS_ProtocolTypeSimpleName_FOURPOS_CAL   = "FourPos_Cal";

const std::string CS_BusinessTypeSimpleName_Psm = "Psm";
const std::string CS_BusinessTypeSimpleName_Ruixiang = "Ruixiang";
const std::string CS_BusinessTypeSimpleName_Cics = "Cics";
const std::string CS_BusinessTypeSimpleName_Asm580 = "Asm580";
const std::string CS_BusinessTypeSimpleName_Zhiyang = "Zhiyang";
const std::string CS_BusinessTypeSimpleName_Unify = "Unify";
const std::string CS_BusinessTypeSimpleName_Asm570 = "Asm570";
const std::string CS_BusinessTypeSimpleName_Asm570Server = "Asm570Server";
const std::string CS_BusinessTypeSimpleName_Asm570Client = "Asm570Client";
const std::string CS_BusinessTypeSimpleName_Asm590 = "Asm590";

const std::string CS_BusinessTypeSimpleName_Ticket = "Ticket";

const std::string CS_ProtocolTypeSimpleName_TRANS     = "GCL_TRANS";

///////////////////////////////////////////////////////////////////////////////////////////////////////
//cases
const std::string CS_CasesTypeSimpleName_Cases  = "Cases";
const std::string CS_CasesTypeSimpleName_SendMessage = "SendMessage";
const std::string CS_CasesTypeSimpleName_SendFile = "SendFile";
const std::string CS_CasesTypeSimpleName_SendCustomData = "SendCustomData";

enum ProtocolRoleEnum
{
    ProtocolRole_0,
    ProtocolRole_server,
    ProtocolRole_communicator,
    ProtocolRole_wf_fy2004,
    ProtocolRole_wf_gk,
    ProtocolRole_wf_key
};
const ProtocolRoleEnum CE_MaxProtocolRole = ProtocolRole_wf_key;

class ProtocolStrings
{

public:
    template<class MeasureSon>
    static std::string SendMeasureTypeSimpleName();

    template<class MeasureSon>
    static std::string SendControlTypeSimpleName();

};
template<class MeasureSon> inline std::string ProtocolStrings::SendMeasureTypeSimpleName(){ return ""; }
template<class MeasureSon> inline std::string ProtocolStrings::SendControlTypeSimpleName(){ return ""; }


const std::string CS_TaskType_Control_Monsb = "Task_Control_Monsb";
const std::string CS_TaskType_Control_Ycadd = "Task_Control_Ycadd";
const std::string CS_TaskType_Piao_Monsb    = "Task_Piao_Monsb";
const std::string CS_TaskType_Piao_Ycadd    = "Task_Piao_Ycadd";
const std::string CS_TaskType_Bottle        = "Task_Bottle";


const std::string CS_EntryProtocolName = "ProtocolName";
const std::string CS_EntryProtocolId = "ProtocolId";
const std::string CS_EntryChannel = "Channel";
const std::string CS_EntryProtocolRole = "ProtocolRole";
const std::string CS_EntryProtocolTitle = "ProtocolTitle";

const std::string CS_EntryBusinessName = "BusinessName";

const std::string CS_EntryFromValue       = "FromValue";
const std::string CS_EntryToValue         = "ToValue";
const std::string CS_EntryMeasureControls = "MeasureControls";
const std::string CS_EntryMeasureCancels  = "MeasureCancels";

const std::string CS_Custom = "Custom";

const int CS_MQPortBase = 60000;

const std::string CS_EntryReqInterval     = "ReqInterval";
const std::string CS_EntrySendInterval    = "SendInterval";
const std::string CS_EntryUrlPath         = "UrlPath";

const std::string CS_EntryAddr            = "Addr";
const std::string CS_EntryReqYx           = "ReqYx";
const std::string CS_EntryReqYc           = "ReqYc";
const std::string CS_EntryNode            = "Node";

const std::string CS_EntryWellCome        = "WellCome";
const std::string CS_EntryCfgFile         = "CfgFile";
const std::string CS_EntryDisInfo         = "DisInfo";
const std::string CS_EntryLedTitle        = "LedTitle";
const std::string CS_EntryMaxColum        = "MaxColum";

const std::string CS_EntryUser            = "User";
const std::string CS_EntryPassWord        = "PassWord";

const std::string CS_EntryMode            = "Mode";
const std::string CS_EntryType            = "Type";
const std::string CS_EntryAddrMin         = "AddrMin";
const std::string CS_EntryAddrMax         = "AddrMax";
const std::string CS_EntryAddrName        = "AddrName";

const std::string CS_EntryRemoteIpAddress2 = "RemoteIpAddress2";
const std::string CS_EntryRemotePort2      = "RemotePort2";

const std::string CS_EntrySizeX            = "SizeX";
const std::string CS_EntrySizeY            = "SizeY";
const std::string CS_EntryResRatioX        = "ResRatioX";
const std::string CS_EntryResRatioY        = "ResRatioY";
const std::string CS_EntryInChanSum        = "InChanSum";
const std::string CS_EntryDefaultScence    = "DefaultScence";
//摄像机参数
const std::string CS_EntryLampOpen        = "LampOpen";
const std::string CS_EntryLampClose       = "LampClose";
const std::string CS_EntryPresetPos       = "PresetPos";
const std::string CS_EntryPresetMax       = "PresetMax";

const std::string CS_EntryALarmListenPort     = "ALarmListenPort";
const std::string CS_EntryALarmListenIPList   = "ALarmListenIPList";
//modbus参数
//地址类型
const std::string CS_EntryModbusAddrType = "AddrType";
//地址数上限
const std::string CS_EntryModbusAddrLimit = "AddrLimit";

const std::string CS_EntryModbusReadCoilStatusInterval = "ReadCoilStatusInterval";
const std::string CS_EntryModbusReadInputStatusInterval = "ReadInputStatusInterval";
const std::string CS_EntryModbusReadHoldingRegistersInterval = "ReadHoldingRegistersInterval";
const std::string CS_EntryModbusReadInputRegistersInterval = "ReadInputRegistersInterval";
//数据解析类型
const std::string CS_EntryModbusCoilStatusType = "CoilStatusType";
const std::string CS_EntryModbusInputStatusType = "InputStatusType";
const std::string CS_EntryModbusHoldingRegistersType = "HoldingRegistersType";
const std::string CS_EntryModbusInputRegistersType = "InputRegistersType";
//命令串
const std::string CS_EntryModbusCoilStatusCmd = "CoilStatusCmd";
const std::string CS_EntryModbusInputStatusCmd = "InputStatusCmd";
const std::string CS_EntryModbusHoldingRegistersCmd = "HoldingRegistersCmd";
const std::string CS_EntryModbusInputRegistersCmd = "InputRegistersCmd";
//命令模式
const std::string CS_EntryModbusCoilStatusCmdMode = "CoilStatusCmdMode";
const std::string CS_EntryModbusInputStatusCmdMode = "InputStatusCmdMode";
const std::string CS_EntryModbusHoldingRegistersCmdMode = "HoldingRegistersCmdMode";
const std::string CS_EntryModbusInputRegistersInCmdMode = "InputRegistersCmdMode";
//遥控功能码
const std::string CS_EntryModbusYkFunctionCode = "YkFunctionCode";


//迈斯门禁参数
const std::string CS_EntryACSClientsInfo = "ACSClientsInfo";
//本地配置端口及本地IP
const std::string CS_EntryACSConifgPort = "ACSConifgPort";
const std::string CS_EntryACSConifgIP = "ACSConifgIP";
const std::string CS_EntryACSStartYXID = "ACSStartYXID";
const std::string CS_EntryACSStartYWID = "ACSStartYWID";
const std::string CS_EntryACSStartYKID = "ACSStartYKID";

//四坐标
const std::string CS_EntryFourPos_Coord = "FourPosCoords";
const std::string CS_EntryFourPos_MeasureIdsIn = "MeasureIdsIn";
const std::string CS_EntryFourPos_MeasureIdsOut = "MeasureIdsOut";
//
const std::string CS_MsObject_TypeName_UserInfo = "UserInfo";
const std::string CS_MsObject_TypeName_RoleInfo = "RoleInfo";
const std::string CS_MsObject_TypeName_HuayanResult = "HuayanResult";
const std::string CS_MsObject_TypeName_ZhiyangInfo = "ZhiyangInfo";


const std::string CS_Entry_LocalStructTypies = "LocalStructTypies";
const std::string CS_Entry_MapingTypeNames = "MapingTypeNames";
const std::string CS_Entry_MapingFieldNames = "MapingFieldNames";
const std::string CS_Entry_LocalOperateTypies = "LocalOperateTypies";
const std::string CS_Entry_TransparentCommands = "TransparentCommands";

const std::string CS_Entry_DeviceCode = "DeviceCode";
const std::string CS_Entry_TerminalCode = "TerminalCode";
const std::string CS_Entry_RequestTypies = "RequestTypies";
const std::string CS_Entry_KeyTsValues = "KeyTsValues";
const std::string CS_Entry_TypeFileNames = "TypeFileNames";

const std::string CS_Entry_QueryInfo = "QueryInfo";

const std::string CS_Entry_WorkStateMeasureId = "WorkStateMeasureId";

const std::string CS_EntryControlMeasure = "measure";
const std::string CS_EntryControlValue = "value";
const std::string CS_EntryControlReasonId = "reason";
const std::string CS_EntryControlStrategyId = "strategy";
const std::string CS_EntryControlMethodId = "method";


const std::string CS_SectionMeasure = "Measure";
const std::string CS_EntryMonsbBegin = "MonsbBegin";
const std::string CS_EntryMonsbBeginFix = "MonsbBeginFix";
const std::string CS_EntryYcaddBegin = "YcaddBegin";
const std::string CS_EntryYcaddBeginFix = "YcaddBeginFix";
const std::string CS_EntryStrawBegin = "StrawBegin";
const std::string CS_EntryStrawBeginFix = "StrawBeginFix";
const std::string CS_EntryMeasure = "measure";
const std::string CS_EntryLocalName = "LocalName";
const std::string CS_EntryAliasName = "AliasName";
const std::string CS_EntryFullName = "FullName";

const std::string CS_EntryFC = "fc";
const std::string CS_EntryVT = "vt";

enum DataOperateType_Enum { DataOperateType_None = 0, DataOperateType_Turn, DataOperateType_Write, DataOperateType_Read, DataOperateType_Task };

//消息命令组成
const std::string CS_EntryMsgCmdName    = "command";
const std::string CS_EntryMsgCmdSession = "session";
const std::string CS_EntryMsgCmdFC      = "fc";
const std::string CS_EntryMsgCmdError   = "err";
const std::string CS_EntryMsgCmdMode    = "mode";
//消息参数组成
const std::string CS_EntryMsgPramValue = "value";
const std::string CS_EntryMsgPramSql   = "sql";
const std::string CS_EntryMsgPramInfo   = "info";
const std::string CS_EntryMsgPramDest = "dest"; //目标
//命令列表
const std::string CS_EntryCmdYK     = "send.yk.1";

const std::string CS_EntryCmdNotifySample = "notify.sample.1";
const std::string CS_EntryCmdPostSample = "post.sample.1";
const std::string CS_EntryCmdReqSample = "req.sample.1";
const std::string CS_EntryCmdRespSample = "resp.sample.1";

//命令列表
const std::string CS_EntryCmdSendFile     = "send.file.1";

//加密锁
const std::string CS_EntryVendorID     = "VendorID";
const std::string CS_EntryEncryPos     = "EncryPos";
const std::string CS_EntryEncryLen     = "EncryLen";
const std::string CS_EntryEncryID      = "EncryID";

#endif // GCL_RESOURCE_H
