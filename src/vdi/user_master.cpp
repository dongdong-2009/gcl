#include "user_master.h"

using namespace std;

map<int, int> f_remoteTerminalUserIds;

int UserMaster::login(int iRemoteTerminalId, const std::string &sIcard, const std::string &sUserName, const std::string &sPassword)
{
    string sSql;
    if (sIcard.size() > 0)
    {
        sSql = CxString::format("select user_id as counter from user where cardcode = '%s'", sIcard.c_str());
    }
    else
    {
        sSql = CxString::format("select user_id as counter from user where user_name = '%s' and password = '%s'", sUserName.c_str(), sPassword.c_str());
    }
    string sUserId = CxDatabase::defaultDb()->queryValue(sSql);
    if (sUserId.size() > 0)
    {
        int iUserId = CxString::toInt32(sUserId);
        f_remoteTerminalUserIds[iRemoteTerminalId] = iUserId;
        return iUserId;
    }
    return 0;
}

int UserMaster::userIdByTerminalId(int iRemoteTerminalId)
{
    return CxContainer::value(f_remoteTerminalUserIds, iRemoteTerminalId);
}
