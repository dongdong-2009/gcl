#include "gcl_lua_base.h"
#include "gcl_lua_global.h"

using namespace std;

GclLuaBase::GclLuaBase()
{
}


string GclLuaBase::fromTable(lua_State *L, int idx)
{

    if(L==NULL || idx<1) return std::string();

    string sRet = std::string();
    int type = lua_type(L, idx);

    if (type == LUA_TTABLE)
    {
        lua_pushnil(L);
        string sKey;
        string sValue;
        map<string,string> map1;
        while( lua_next(L,idx) )
        {
            LuaCommon::getValue(L, -2, sKey);
            LuaCommon::getValue(L, -1, sValue);
            map1[sKey] = sValue;
            lua_pop(L,1); // 弹出 value，让 key 留在栈顶
        }
        sRet = CxString::join(map1,cs_middle_string,cs_split_string);
    }
    else if (type == LUA_TSTRING) {
        LuaCommon::getValue(L,idx, sRet);
    }
    else if (type == LUA_TNUMBER) {
        int num = 0;
        LuaCommon::getValue(L, idx, num);
        sRet = CxString::format("%d",num);
    }
    return sRet;
}

void GclLuaBase::mapToTable(lua_State *L, const std::map<string, string> &data)
{
    lua_newtable(L);
    int type = 0;
    int n=data.size();
    LuaCommon::setTable(L, CS_Lua_Type, type);
    LuaCommon::setTable(L, CS_Lua_Count, n);
    for (map<string, string>::const_iterator it = data.begin(); it != data.end(); ++it)
    {
        LuaCommon::setTable(L, it->first, it->second);
    }
}


void GclLuaBase::stringToTable(lua_State * L,const string &data)
{
    vector<string> v = CxString::split(data,"\r\n");

    if(v.size()==1) v = CxString::split(data,"\n");

    vector<map<string,string> > vv = CxString::splitToLines(v,'=',';');

    int type = 0;

    int n=vv.size();
    if(n>1)
    {
        map<string,string> map1 = vv.at(0);
        if(map1.size()>1)type=1;
    }

    lua_newtable(L);
    LuaCommon::setTable(L, CS_Lua_Type, type);
    LuaCommon::setTable(L, CS_Lua_Count, n);
    if(type == 1) //多记录
    {
        for(size_t i=0;i<vv.size();++i)
        {
            map<string,string> map2 = vv.at(i);
            mapToTable(L,map2);
            lua_rawseti(L,-2,i+1);
        }
    }
    else if(type == 0)//单记录
    {
        for(size_t i=0;i<vv.size();++i)
        {
            map<string,string> map2 = vv.at(i);
            for (map<string, string>::const_iterator it = map2.begin(); it != map2.end(); ++it)
            {
                LuaCommon::setTable(L, it->first, it->second);
            }
        }
    }
}
