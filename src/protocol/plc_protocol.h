#ifndef PLC_PROTOCOL_H
#define PLC_PROTOCOL_H


#include <vdi/protocol_base.h>
#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>


#include "plc_global.h"
#include "plc_fins_frame.h"


class PlcProtocol : public ProtocolBase
{
public:

    PlcProtocol();

    ~PlcProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Plc; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //receive
    //YX
    inline const MonsbAddressWorkerTable* YxTable() const            { return _yxTable            ; }
    //YC
    inline const YcaddAddressWorkerTable* YcTable() const            { return _ycTable            ; }

private:
    MonsbAddressWorkerTable* _yxTable;
    YcaddAddressWorkerTable* _ycTable;

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void CallUp();

    void DealData();

private:
    CxTimer* _timeCallUp;
    int _callUpInterval;
    uchar _pPlcbuffer[PLC_BUF_LEN];
    int _plcLen;
    CxTimer* _timeSend;
    uchar _sendBuff[PLC_BUF_LEN];
    uchar _plcStatus[PLC_BUF_LEN];

    std::map<int, std::string> _statusValues;
    std::map<int, long long> _statusTimes;

    bool _bWork;

    GCLFINSHead _recvHead;

    GCLFINSFrame _recvFrm;

    GCLFINSFrame _sendLast;

    GCLFINSFrame _sendFrm;

    std::queue<std::vector<char> > _sendBuffers;
    std::queue<std::vector<char> > _sentFrms;

    bool _bSend;
private:
    inline void push(const char * pData, int iLength) { _sendBuffers.push(std::vector<char>(pData, pData + iLength)); }

    void doSendOneToPlc();

    void login(); //登陆包

    void SendReq(ushort MSCmd,char* pBuf,int len);//发送请求

    int  FindHeader(uchar *pData, int iLength);//数据处理

    bool CheckFrame(uchar *pData, int iLength, int &nFrmLen);

    void dealDataResp();//响应数据处理

    void dealYX(uchar type, uint16 addr0, uint8 addr1, uint16 num, uchar *pBuf, uint16 len);

    void dealYC(uchar type, uint16 addr0, uint8 addr1, uint16 num, uchar *pBuf, uint16 len);

public:
    void CmdExecute(int iCmdCode, const std::string &sBuffer );

    void CallUp(bool bNow);

    void CmdQueryStatus(); //查询状态

    void CmdBackstatus(); //后台启动状态
    void CmdBackstatuszero(); //后台启动状态0
    void CmdHandmode(); //手动
    void CmdAutomode(); //自动
    void CmdDebugmode(); //调试
    void CmdXHome(); //X轴回原点
    void CmdYHome(); //Y轴回原点
    void CmdRHome(); //R轴回原点
    void CmdUHome(); //U轴回原点
    void CmdXHomezero(); //X轴回原点0
    void CmdYHomezero(); //Y轴回原点0
    void CmdRHomezero(); //R轴回原点0
    void CmdUHomezero(); //U轴回原点0
    void CmdUp(); //Y轴上
    void CmdDown(); //Y轴下
    void CmdLeft(); //X轴左
    void CmdRight(); //X轴右
    void CmdLeftrotate(); //左旋
    void CmdRightrotate(); //右旋
    void CmdUpzero(); //Y轴上0
    void CmdDownzero(); //Y轴下0
    void CmdLeftzero(); //X轴左0
    void CmdRightzero(); //X轴右0
    void CmdLeftrotatezero(); //左旋0
    void CmdRightrotatezero(); //右旋0
    void CmdOpendoor(); //安全门开
    void CmdClosedoor(); //安全门关
    void CmdOpendoorzero(); //安全门开 Zero
    void CmdClosedoorzero(); //安全门关 Zero

    void CmdUAtoB(); //手动换向A-->B
    void CmdUBtoA(); //手动换向B-->A
    void CmdUAtoBzero(); //手动换向A-->B Zero
    void CmdUBtoAzero(); //手动换向B-->A Zero
    void CmdForward(); //抓取位移前进
    void CmdBackward(); //抓取位移后退
    void CmdForwardzero(); //抓取位移前进0
    void CmdBackwardzero(); //抓取位移后退0

    void CmdReadRFIDok(); //存样读码OK
    void CmdReadRFIDokzero(); //存样读码OK Zero
    std::string CmdSavetoX(int x); //存样X轴接收坐标
    std::string CmdSavetoY(int y); //存样Y轴接收坐标
    std::string CmdSavetoU(int u); //存样U轴接收坐标
    void CmdGetfromX(int x); //取样X轴接收坐标
    void CmdGetfromY(int y); //取样Y轴接收坐标
    //0 : 表示当前机械手方向
    //1 : 表A面 GM_FaceA
    //2 : 表B面 GM_FaceB
    void CmdGetfromU(int u); //取样U轴接收坐标

    void CmdSendXYUFinish(); //坐标接收完成标志
    void CmdSendXYUFinishzero(); //坐标接收完成标志 Zero

    void CmdPrintNow(); //不打印按钮
    void CmdHandGetBottle();//手动取料
    void CmdHandSaveBottle(); //手动存料
    void CmdChangetoGetbottle(); //切换到取样界面
    void CmdChangetoSavebottle();//切换到存样界面
    void CmdPrintNowzero(); //不打印按钮 Zero
    void CmdHandGetBottlezero();//手动取料 Zero
    void CmdHandSaveBottlezero();//手动存料 Zero
    void CmdChangetoGetbottlezero();//切换到取样界面 Zero
    void CmdChangetoSavebottlezero();//切换到存样界面 Zero
    std::string convert(int value);
    int GetAreaCode(uchar *data, int length);

    void doPrint(bool flag);

    void setUpFrame(uint16 cmdCode, uint32 beginAddr, uint16 num, char *val, int len);

    char getYxCommand(const std::string &sCommandCode, bool * ok = NULL);

    bool getYxCommand(const std::string &sCommandCode, int &iValue);

    std::string getYcCommand(const std::string &sCommandCode, bool * ok = NULL);

    bool getYcCommand(const std::string &sCommandCode, double &dValue);
    bool getYcCommand(const std::string &sCommandCode, int &iValue);


    bool setYxCommand(const std::string &sCommandCode, char value);

    //bool setYcCommand(const std::string &sCommandCode, const std::string & sValue);

    bool setYcCommand(const std::string &sCommandCode, int value);

    bool setYcCommand(const std::string &sCommandCode, short value);
    //读所有遥信
    void readAllYX();
    //读所有遥测
    void readAllYC();

};

class PlcProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(PlcProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Plc;
    }

    ProtocolBase* createObject(){
        return new PlcProtocol();
    }
};

#endif // PLC_PROTOCOL_H
