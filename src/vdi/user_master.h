#ifndef USER_MASTER_H
#define USER_MASTER_H

#include "protocol_global.h"

class UserMaster
{
public:
    static int login(int iRemoteTerminalId, const std::string & sIcard, const std::string & sUserName, const std::string & sPassword);

    static int userIdByTerminalId(int iRemoteTerminalId);

};

#endif // USER_MASTER_H
