#ifndef GCL_LUA_SHM_H
#define GCL_LUA_SHM_H
#include  <ccxx/ccxx.h>
class GclLuaShm
{
public:
    static void registerLua();

public:
    GclLuaShm();
    static int dealChangeData(std::vector<std::vector<std::string> > &rows);
};

#endif // GCL_LUA_SHM_H
