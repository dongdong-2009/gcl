#include <ccxx/ccxx.h>

#include <gcl_lua.h>
#include "lunar.h"

using namespace std;

class Account {
    lua_Number m_balance;
public:
    static const char className[];
    static Lunar<Account>::RegType methods[];

    Account(lua_State *L)      { m_balance = luaL_checknumber(L, 1); }
    int deposit (lua_State *L) { m_balance += luaL_checknumber(L, 1); return 0; }
    int withdraw(lua_State *L) { m_balance -= luaL_checknumber(L, 1); return 0; }
    int balance (lua_State *L) { lua_pushnumber(L, m_balance); return 1; }
    ~Account() { printf("deleted Account (%p)\n", this); }
};

const char Account::className[] = "Account";

Lunar<Account>::RegType Account::methods[] = {
    LUNAR_DECLARE_METHOD(Account, deposit),
    LUNAR_DECLARE_METHOD(Account, withdraw),
    LUNAR_DECLARE_METHOD(Account, balance),
    {0,0}
};

static void stackDump (lua_State *L)
{
    int i;

    int top = lua_gettop(L);

    printf("\nlua_gettop - count=%d : \n", top);

    for (i = 1; i <= top; i++) {  /* repeat for each level */

        int t = lua_type(L, i);

        printf("%03d : typeid=%d   typename=%s   value=", i, t, lua_typename(L, i));

        switch (t) {

        case LUA_TSTRING:  /* strings */

            printf("`%s'", lua_tostring(L, i));

            break;

        case LUA_TBOOLEAN:  /* booleans */

            printf(lua_toboolean(L, i) ? "true" : "false");

            break;

        case LUA_TNUMBER:  /* numbers */

            printf("%g", lua_tonumber(L, i));

            break;

        default:  /* other values */

            printf("%s", lua_typename(L, t));

            break;

        }

        printf("  \n");  /* put a separator */
    }

    printf("\n");     /* end the listing */
}

static int fn_test_param_table (lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 1)
        printf("top empty");

    lua_settop(L, 1);
    int t = lua_type(L, 1);
    if (t == LUA_TTABLE)
    {
        size_t iCount = lua_rawlen(L, 1);
        for (size_t i = 1; i <= iCount; ++i)
        {
            t = lua_rawgeti(L, 1, i);
            if (t == LUA_TTABLE)
            {
                int q;
                long long t;
                int v;
                LuaCommon::getTable(L, "q", q, 2);
                LuaCommon::getTable(L, "t", t, 2);
                LuaCommon::getTable(L, "v", v, 2);
//                cout << "q=" << q << " t=" << t << " v=" << v << endl;
                printf("q=%d t=%lld v=%d", q, t, v);
            }
            lua_settop(L, 1);
        }
    }
}

static int my_ceil (lua_State *L) {
  if (lua_isinteger(L, 1))
    lua_settop(L, 1);  /* integer is its own ceil */
  else {
    lua_Number d = l_mathop(ceil)(luaL_checknumber(L, 1));
    lua_pushinteger(L, d);
  }
  return 1;
}


static int GCS_GetValueByID(lua_State *L)
{
    //返回栈中元素的个数
    int n = lua_gettop(L);
    std::vector<int> vec;

    int i;
    for ( i = 1; i <= n; i++)
    {
        if(lua_isinteger(L, i))
        {
            vec.push_back(lua_tointeger(L,i));
        }
    }

    //清空
    lua_pop(L,n);//清空
    //    lua_settop(L,0); //清空

    for (i = 0; i <vec.size(); i++)
    {

        lua_pushnumber(L,1);
        lua_pushnumber(L,2);
        lua_pushnumber(L,3);
        lua_pushnumber(L,4);

        lua_pushnumber(L,5);
    }
    n = lua_gettop(L);
//    cxDebug()<<"lua_gettop"<<n<<cxEndLine;

    /* return the number of results */
    return 5;
}



static int fn_l_test(lua_State *L)
{
    stackDump(L);
    return 0;
}

int main(int argc, char *argv[])
{
    cout << CxApplication::applicationDeployPath() << endl;
    cout << CxApplication::applicationTargetName() << endl;


    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

//    Lunar<Account>::Register(L);

    lua_newtable(L);     /* creates a table */
    LuaCommon::setTable(L, "r", (int)123);    /* table.r = ct->r */
    LuaCommon::setTable(L, "g", (int)133);  /* table.g = ct->g */
    LuaCommon::setTable(L, "b", (int)144);    /* table.b = ct->b */
    LuaCommon::setTable(L, "fn_test", fn_test_param_table);    /* table.b = ct->b */
    lua_setglobal(L, "color");    /* 'name' = table */

    clock_t dtNow = clock();

    luaL_dofile(L, "c:/002.lua");

    printf("dt1 : %d \n" ,clock() - dtNow);

    lua_close(L);

    return 0;

//    clock_t dtNow = clock();

    lua_settop(L, 0);

    lua_newtable(L);     /* creates a table */
//    lua_createtable(L, 1000 * 100 * 100, 1000 * 100);
    for (size_t i = 1; i < 1000 * 100; ++i)
    {
        lua_newtable(L);
        for (size_t j = 1; j < 100; ++j)
        {
            string sField = CxString::format("field%d", j);
            int iValue = (i * 100 + j);
            LuaCommon::setTable(L, sField, iValue);
        }
        lua_rawseti(L, -2, i);
//        stackDump(L);
    }
    lua_setglobal(L, "ary1");    /* 'name' = table */

    printf("dt1 : %d" ,clock() - dtNow);

    printf("dt2 : %d" ,clock() - dtNow);

    return 0;

    int iLuaType = lua_getglobal(L, "gcl_deal_received_message");
    if (iLuaType == LUA_TFUNCTION)
    {
        //lua function : function gcl_deal_received_message(iLocalTerminalId, iSourceTerminalId, sCommand, sParam)
        //参数压栈
        lua_pushinteger(L, 5);
        lua_pushstring(L, "aaaaa");
        //函数调用
        //3个参数 1个返回值
        if (lua_pcall(L,2,1,0) != 0)
            printf("error running function `f': %s",
             lua_tostring(L, -1));

        //获取返回结果
        int ret = lua_tonumber(L,-1);
        //清空
        lua_settop(L, 0);
//        lua_pop(L,lua_gettop(L));

        printf("%d", ret);     /* end the listing */
    }
//    printf("stackDump begin ...\n");     /* end the listing */
//    stackDump(L);
//    printf("stackDump end!");     /* end the listing */

    char str [80];
    printf ("Please enter any key to exit!");
    scanf ("%79s",str);


    lua_gc(L, LUA_GCCOLLECT, 0);  // collected garbage
    lua_close(L);
    return 0;
}


