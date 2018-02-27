#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H


#include <ccxx/ccxx.h>


class CameraManager
{
public:
    //返回发送缓冲成功与否
    static int postTakePhoto();

    //阻塞型在线等待拍照到保存文件成功
    static std::string waitTakePhoto(msepoch_t iTimeOut = 5000LL);

    //返回最后获取到RFID号的时间
    static long long lastTimeUpdatePictruePath();

    //返回最后获取到的RFID号
    static std::string lastPictruePath();

};

#endif // CAMERA_MANAGER_H
