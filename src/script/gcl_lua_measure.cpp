#include "gcl_lua_measure.h"
#include "gcl_lua_global.h"
#include <common/gcl.h>
#include <vdi/gcl_measure_server.h>


using namespace std;



GclLuaMeasure * fn_oGclLuaMeasure()
{
    GclLua::registInitFunction(GclLuaMeasure::registerLua);
    static GclLuaMeasure m;
    return &m;
}

static GclLuaMeasure * f_oGclLuaMeasure = fn_oGclLuaMeasure();

//static string f_deal_message_lua_filePath;

#define GCL_LUA_PRAM_COUNT  5
//传入参数,MeasureID数组
//传出参数:ID,Type,q,lenth,value；#
static int meas_getValueByID(lua_State *L)
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

        GCLMeasureServer::getValueByID(vec.at(i),value);

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
static int meas_getValueByUrl(lua_State *L)
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

        GCLMeasureServer::getValueByUrl(vec.at(i),value);

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
static int meas_setValueByID(lua_State *L)
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
                    GCLMeasureServer::setValueByID(oValue);
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
static int meas_setValueByUrl(lua_State *L)
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
                    GCLMeasureServer::setValueByUrl(sUrl,oValue);
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

void GclLuaMeasure::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "getValueByID", meas_getValueByID);
    LuaCommon::setTable(L, "setValueByID", meas_setValueByID);
    LuaCommon::setTable(L, "getValueByUrl", meas_getValueByUrl);
    LuaCommon::setTable(L, "setValueByUrl", meas_setValueByUrl);
    lua_setglobal(L, "gcs_measure");    /* 'name' = table */
}

GclLuaMeasure::GclLuaMeasure()
{
}
