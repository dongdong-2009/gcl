#include "gcl_lua_shm.h"
#include <vdi/gcl_shm_server.h>
#include "gcl_lua_global.h"



using namespace std;



GclLuaShm * fn_oGclLuaShm()
{
    GclLua::registInitFunction(GclLuaShm::registerLua);
    static GclLuaShm m;
    return &m;
}

static GclLuaShm * f_oGclLuaShm = fn_oGclLuaShm();

//static string f_deal_message_lua_filePath;

#define GCL_LUA_PRAM_COUNT  5
//传入参数,MeasureID数组
//传出参数:ID,Type,q,lenth,value；#
static int shm_getValueByID(lua_State *L)
{
    //返回栈中元素的个数
    int n = lua_gettop(L);
    if (n < 1)
    {
        return 0;
    }

    std::vector<int> vec;
    vec.clear();

    for (int i = 1; i <= n; ++i)
    {
        int t = lua_type(L, i);

        switch(t)
        {
        case LUA_TNUMBER://数字形式
            vec.push_back(lua_tointeger(L,i));
            break;
        case LUA_TSTRING://字符串形式 0x01000000~0x0100300,0x0100400
        {
            string s = lua_tostring(L,1);

            std::vector<int> v = CxString::parseToInts(s);

            if (v.size()>0)CxContainer::append(vec,v);
        }
            break;
        case LUA_TTABLE://
            break;
        default:
            break;
        }
    }

    //清空
    lua_pop(L,n);//清空
    //    lua_settop(L,0); //清空


    gcl_stu_lua_value value;

    lua_newtable(L);     /* creates a table */

    for (size_t i = 0; i < vec.size(); ++i)
    {
        lua_newtable(L);

        GCLShareMemServer::getValueByID(vec.at(i),value);

        LuaCommon::setTable(L, CS_Lua_Measure_ID, value.id);
        LuaCommon::setTable(L, CS_Lua_Measure_Type, value.type);
        LuaCommon::setTable(L, CS_Lua_Measure_Q, value.q);
        LuaCommon::setTable(L, CS_Lua_Measure_T, value.t);
        LuaCommon::setTable(L, CS_Lua_Measure_Length, value.len);
        LuaCommon::setTable(L, CS_Lua_Measure_SRC, value.src);

        switch(value.type)
        {
        case GCL_VT_YX:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.iValue);
            break;
        case GCL_VT_YC:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.dValue);
            break;
        case GCL_VT_STR:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.sValue);
            break;
        default:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.iValue);
            break;
        }
        lua_rawseti(L, -2, i+1);
        //        stackDump(L);
    }

    n = lua_gettop(L);
    /* return the number of results */
    return 1;
}

//传入参数,url 数组
//传出参数:ID,Type,q,lenth,value；
static int shm_getValueByUrl(lua_State *L)
{
    int n = lua_gettop(L);

    std::vector<string> vec;
    vec.clear();

    int i;
    for ( i = 1; i <= n; i++)
    {
        if(lua_isstring(L, i))
        {
            vec.push_back(lua_tostring(L,i));
        }
    }
    //清空
    lua_pop(L,n);//清空

    gcl_stu_lua_value value;

    lua_newtable(L);     /* creates a table */

    for (size_t i = 0; i < vec.size(); ++i)
    {
        lua_newtable(L);

        GCLShareMemServer::getValueByUrl(vec.at(i),value);

        LuaCommon::setTable(L, CS_Lua_Measure_ID, value.id);
        LuaCommon::setTable(L, CS_Lua_Measure_Type, value.type);
        LuaCommon::setTable(L, CS_Lua_Measure_Q, value.q);
        LuaCommon::setTable(L, CS_Lua_Measure_T, value.t);
        LuaCommon::setTable(L, CS_Lua_Measure_Length, value.len);
        LuaCommon::setTable(L, CS_Lua_Measure_SRC, value.src);


        switch(value.type)
        {
        case GCL_VT_YX:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.iValue);
            break;
        case GCL_VT_YC:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.dValue);
            break;
        case GCL_VT_STR:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.sValue);
            break;
        default:
            LuaCommon::setTable(L, CS_Lua_Measure_V, value.iValue);
            break;
        }

        lua_rawseti(L, -2, i+1);
        //        stackDump(L);
    }

    n = lua_gettop(L);
    /* return the number of results */
    return 1;
}

//传入参数:ID,Type,q,lenth,value；
//传出参数,成功数目
static int shm_setValueByID(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1)
    {
        return 0;
    }

    lua_settop(L, 1);

    int t = lua_type(L, 1);
    int nCount = 0;

    if (t == LUA_TTABLE)
    {
        size_t iCount = lua_rawlen(L, 1);
        gcl_stu_lua_value oValue;
        bool bRet = false;
        for (size_t i = 1; i <= iCount; ++i)
        {
            t = lua_rawgeti(L, 1, i);

            if (t == LUA_TTABLE)
            {
                LuaCommon::getTable(L, CS_Lua_Measure_ID, oValue.id,2);
                LuaCommon::getTable(L, CS_Lua_Measure_Type, oValue.type,2);
                LuaCommon::getTable(L, CS_Lua_Measure_Q, oValue.q,2);
                LuaCommon::getTable(L, CS_Lua_Measure_T, oValue.t,2);
                LuaCommon::getTable(L, CS_Lua_Measure_Length, oValue.len,2);
                LuaCommon::getTable(L, CS_Lua_Measure_SRC, oValue.src,2);

                switch(oValue.type)
                {
                case GCL_VT_YX:
                    bRet = LuaCommon::getTable(L, CS_Lua_Measure_V, oValue.iValue,2);
                    break;
                case GCL_VT_YC:
                    bRet = LuaCommon::getTable(L, CS_Lua_Measure_V, oValue.dValue,2);
                    break;
                case GCL_VT_STR:
                    bRet = LuaCommon::getTable(L, CS_Lua_Measure_V, oValue.sValue,2);
                    break;
                default:
                    break;
                }
                if(bRet)
                {
                    GCLShareMemServer::setValueByID(oValue);
                    nCount++;
                }
            }
            lua_settop(L, 1);
        }
    }
    lua_pop(L,top);//清空
    /* return the number of results */
    lua_pushinteger(L,nCount);

    return 1;
}
//传入参数:url,Type,q,lenth,value；
//传出参数,成功数目
static int shm_setValueByUrl(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1)
    {
        return 0;
    }

    lua_settop(L, 1);

    int t = lua_type(L, 1);
    int nCount = 0;

    if (t == LUA_TTABLE)
    {
        size_t iCount = lua_rawlen(L, 1);
        gcl_stu_lua_value oValue;
        bool bRet = false;
        string sUrl="";

        for (size_t i = 1; i <= iCount; ++i)
        {
            t = lua_rawgeti(L, 1, i);

            if (t == LUA_TTABLE)
            {
                LuaCommon::getTable(L, CS_Lua_Measure_URL,  sUrl,2);
                LuaCommon::getTable(L, CS_Lua_Measure_Type, oValue.type,2);
                LuaCommon::getTable(L, CS_Lua_Measure_Q, oValue.q,2);
                LuaCommon::getTable(L, CS_Lua_Measure_T, oValue.t,2);
                LuaCommon::getTable(L, CS_Lua_Measure_Length, oValue.len,2);
                LuaCommon::getTable(L, CS_Lua_Measure_SRC, oValue.src,2);

                switch(oValue.type)
                {
                case GCL_VT_YX:
                    bRet = LuaCommon::getTable(L, CS_Lua_Measure_V, oValue.iValue,2);
                    break;
                case GCL_VT_YC:
                    bRet = LuaCommon::getTable(L, CS_Lua_Measure_V, oValue.dValue,2);
                    break;
                case GCL_VT_STR:
                    bRet = LuaCommon::getTable(L, CS_Lua_Measure_V, oValue.sValue,2);
                    break;
                default:
                    break;
                }
                if(bRet)
                {
                    GCLShareMemServer::setValueByUrl(sUrl,oValue);
                    nCount++;
                }
            }
            lua_settop(L, 1);
        }
    }
    lua_pop(L,top);//清空
    /* return the number of results */
    lua_pushinteger(L,nCount);

    return 1;
}




void GclLuaShm::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "getValueByID", shm_getValueByID);
    LuaCommon::setTable(L, "setValueByID", shm_setValueByID);
    LuaCommon::setTable(L, "getValueByUrl", shm_getValueByUrl);
    LuaCommon::setTable(L, "setValueByUrl", shm_setValueByUrl);
    lua_setglobal(L, "gcs_shm");    /* 'name' = table */
}

GclLuaShm::GclLuaShm()
{

}

int GclLuaShm::dealChangeData(std::vector<std::vector<string> > &rows)
{
    int iResult = -1;

    lua_State * L = GclLua::luaState();
    if(L==NULL) return false;
    //获取全局函数
    int iLuaType = lua_getglobal(L, "gcs_rtdb_change_data");
    if (iLuaType == LUA_TFUNCTION)
    {
        lua_newtable(L);
        for (size_t i=0;i<rows.size(); ++i)
        {
            lua_newtable(L);
            std::vector<string> row = rows.at(i);
            int pos = 0;
            if(row.size()>4)
            {
                LuaCommon::setTable(L, CS_Lua_Measure_ID, row.at(pos++));
                LuaCommon::setTable(L, CS_Lua_Measure_V, row.at(pos++));
                LuaCommon::setTable(L, CS_Lua_Measure_Q, row.at(pos++));
                LuaCommon::setTable(L, CS_Lua_Measure_T, row.at(pos++));
                LuaCommon::setTable(L, CS_Lua_Measure_SRC,row.at(pos++));
            }
            lua_rawseti(L, -2, i+1);
        }
        //函数调用
        if(LuaCommon::callLuaFunction(L,1,1,iResult))
        {
            return iResult;
        }
    }
    return iResult;
}
