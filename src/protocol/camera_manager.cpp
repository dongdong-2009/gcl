#include "camera_manager.h"


#include <vdi/terminal_manager.h>
#include "camera_protocol.h"


int CameraManager::postTakePhoto()
{
    CameraProtocol * oCameraProtocol = (CameraProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Camera);
    if (oCameraProtocol)
    {
        return oCameraProtocol->takePhoto();
    }
    return 0;
}

std::string CameraManager::waitTakePhoto(msepoch_t iTimeOut)
{
    msepoch_t dtNow = postTakePhoto();
    if (dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while (iG < iTimeOut)
        {
            iG = CxTime::currentSystemTime() - dtNow;
            if ( lastTimeUpdatePictruePath() - dtNow > 0 )
            {
                return lastPictruePath();
            }
            CxApplication::processEvents();
        }
    }
    return "";
}

long long CameraManager::lastTimeUpdatePictruePath()
{
    CameraProtocol * oCameraProtocol = (CameraProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Camera);
    if (oCameraProtocol)
    {
        return oCameraProtocol->lastTimeUpdatePictruePath();
    }
    return 0;
}

std::string CameraManager::lastPictruePath()
{
    CameraProtocol * oCameraProtocol = (CameraProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Camera);
    if (oCameraProtocol)
    {
        return oCameraProtocol->lastPictruePath();
    }
    return 0;
}
