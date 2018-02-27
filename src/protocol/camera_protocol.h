#ifndef CAMERA_PROTOCOL_H
#define CAMERA_PROTOCOL_H


#include <vdi/protocol_base.h>


class CameraProtocol : public ProtocolBase
{

public:

    CameraProtocol();

    ~CameraProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Camera; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    int takePhoto();

    //返回最后获取到RFID号的时间
    inline long long lastTimeUpdatePictruePath() const { return _lastTimeUpdatePictruePath; }

    //返回最后获取到的RFID号
    inline std::string lastPictruePath() const { return _lastPictruePath; }

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

private:
    int initCamera();

    void DealData(const uchar *pData, int iLength);

    uchar istoASCs(uchar* desBuffer, uchar* srcBuffer,int len);

    int StrToEBCD( char *buf, uchar *ucBuffer, int BufLen );

    int SavePicture( uchar *pData );

private:
    int _cameraMode;
    std::string _beginaddr;
    std::string _endaddr;
    char _picture[512*1000];
    uchar desBuffer[512*1000];
    uint _nLength;
    int _iBegin;
    int _iEnd;
    bool _bPictrueHead;

    std::string _lastPictruePath;
    long long _lastTimeUpdatePictruePath;

};

class CameraProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(CameraProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Camera;
    }

    ProtocolBase* createObject(){
        return new CameraProtocol();
    }
};


#endif // CAMERA_PROTOCOL_H
