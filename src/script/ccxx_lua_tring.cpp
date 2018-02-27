#include "ccxx_lua_tring.h"

using namespace std;

CXLuaString * fn_oCXLuaString()
{
    GclLua::registInitFunction(CXLuaString::registerLua);
    static CXLuaString m;
    return &m;
}

static CXLuaString * f_oCXLuaString = fn_oCXLuaString();

static int fn_toDouble(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }
    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    double val = 0;

//    cxDebug()<<mode<<CxString::toHexstring(s.data(),s.length())<<cxEndLine;

    CxBuffer::fromBuf((uint8*)(s.c_str()),sizeof(double),val,mode);
    lua_pushnumber(L, val);
    return 1;
}

static int fn_toInt(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }
    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    int32 val = 0;

    CxBuffer::fromBuf((uint8*)(s.c_str()),sizeof(int32),val,mode);
    lua_pushnumber(L, val);
    return 1;
}
static int fn_toFloat(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    float val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str()),sizeof(float),val,mode);
    lua_pushnumber(L, val);
    return 1;
}
static int fn_toBit(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少1个参数
    {
        return 0;
    }
    string s = f_oCXLuaString->fromTable(L,1);
    int bit =  CxString::toInt32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    uint8* p = (uint8*)(s.c_str());
    int val = -1;
    if(p &&  bit>0 && bit<9 ) val = p[0]&(1<<(bit-1));
    lua_pushnumber(L, val);
    return 1;
}

static int fn_toBit_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 3) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);

    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int bit =  CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    uint8* p = (uint8*)(s.c_str());
    int val = -1;
    if(p && pos>0 && bit>0 && bit<9 ) val = p[pos-1]&(1<<(bit-1));
    lua_pushnumber(L, val);
    return 1;
}

static int fn_toShort(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    int16 val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str()),sizeof(int16),val,mode);
    lua_pushnumber(L, val);
    return 1;
}

static int fn_toWord(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    uint16 val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str()),sizeof(uint16),val,mode);
    lua_pushnumber(L, val);
    return 1;
}
static int fn_toDword(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toUint32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    uint32 val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str()),sizeof(uint32),val,mode);
    lua_pushnumber(L, val);
    return 1;
}

static int fn_string_to_uint(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toUint32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    uint32 val = CxString::toUint32(s);
    lua_pushnumber(L, val);
    return 1;
}

static int fn_string_to_int(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1) //最少1个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int mode =1;
    if(top>1) mode = CxString::toUint32(f_oCXLuaString->fromTable(L,2));

    lua_pop(L, top);//清空

    int val = CxString::toInt32(s);
    lua_pushnumber(L, val);
    return 1;
}




static int fn_toShort_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int mode =1;
    if(top>2) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    int16 val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str())+pos,sizeof(int16),val,mode);
    lua_pushnumber(L, val);
    return 1;
}

static int fn_toWord_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int mode =1;
    if(top>2) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    uint16 val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str())+pos,sizeof(uint16),val,mode);
    lua_pushnumber(L, val);
    return 1;
}
static int fn_toDword_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int mode =1;
    if(top>2) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    uint32 val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str())+pos,sizeof(uint32),val,mode);
    lua_pushnumber(L, val);
    return 1;
}


static int fn_toDouble_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }
    string s = f_oCXLuaString->fromTable(L,1);
    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int mode =1;
    if(top>2) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    double val = 0;

//    cxDebug()<<mode<<CxString::toHexstring(s.data(),s.length())<<cxEndLine;

    CxBuffer::fromBuf((uint8*)(s.c_str())+pos,sizeof(double),val,mode);
    lua_pushnumber(L, val);
    return 1;
}

static int fn_toInt_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }
    string s = f_oCXLuaString->fromTable(L,1);
    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int mode =1;
    if(top>2) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    int32 val = 0;

    CxBuffer::fromBuf((uint8*)(s.c_str())+pos,sizeof(int32),val,mode);
    lua_pushnumber(L, val);
    return 1;
}
static int fn_toFloat_v2(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    string s = f_oCXLuaString->fromTable(L,1);
    int pos = CxString::toInt32(f_oCXLuaString->fromTable(L,2));
    int mode =1;
    if(top>2) mode = CxString::toInt32(f_oCXLuaString->fromTable(L,3));

    lua_pop(L, top);//清空

    float val = 0;
    CxBuffer::fromBuf((uint8*)(s.c_str())+pos,sizeof(float),val,mode);
    lua_pushnumber(L, val);
    return 1;
}
void CXLuaString::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);

    LuaCommon::setTable(L, "toBit", fn_toBit);
    LuaCommon::setTable(L, "toShort", fn_toShort);
    LuaCommon::setTable(L, "toUshort", fn_toWord);
    LuaCommon::setTable(L, "toInt", fn_toInt);
    LuaCommon::setTable(L, "toUint", fn_toDword);
    LuaCommon::setTable(L, "toFloat", fn_toFloat);
    LuaCommon::setTable(L, "toDouble", fn_toDouble);
    LuaCommon::setTable(L, "toBit_v2", fn_toBit_v2);
    LuaCommon::setTable(L, "toShort_v2", fn_toShort_v2);
    LuaCommon::setTable(L, "toUshort_v2", fn_toWord_v2);
    LuaCommon::setTable(L, "toInt_v2", fn_toInt_v2);
    LuaCommon::setTable(L, "toUint_v2", fn_toDword_v2);
    LuaCommon::setTable(L, "toFloat_v2", fn_toFloat_v2);
    LuaCommon::setTable(L, "toDouble_v2", fn_toDouble_v2);

    LuaCommon::setTable(L, "string_to_int", fn_string_to_int);
    LuaCommon::setTable(L, "string_to_uint", fn_string_to_uint);

    lua_setglobal(L, "cx_string");    /* 'name' = table */
}

CXLuaString::CXLuaString()
{
}
