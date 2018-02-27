#ifndef GCL_LUA_MSG_H
#define GCL_LUA_MSG_H

class GclLuaMsg
{
public:
    static void registerLua();
public:
    GclLuaMsg();
    //业务处理
    static bool dealBuffer(const char *cmd, const char* pBuf, int len);

};

#endif // GCL_LUA_MSG_H
