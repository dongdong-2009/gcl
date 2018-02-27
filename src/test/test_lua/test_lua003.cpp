#include <ccxx/ccxx.h>

#include <lua5.3.0/lua.hpp>
//#include <lua5.1.5/lua.hpp>

using namespace std;


struct psm_packet_body_t
{
    int  frameType;
    int  sourceType;
    int  sourceAddress;
    int  targetType;
    int  targetAddress;
    //4(head) + 2 + 2 + 10 + 2
    int  controlWord;
    //  + 24 + 2(crc)
    int  command;
    int  reason;
    int  res;
    int  container;
    int  paramType;
    int  paramCount;
};



int main (void)
{
    char buff[256];
    int error;
    /* initialize Lua */
    //    L = lua_open();
    lua_State *L = luaL_newstate();
    /* load Lua libraries */
    luaL_openlibs(L);

//    lua_State *L = lua_open();  /* opens Lua */
//    luaopen_base(L);    /* opens the basic library */
//    luaopen_table(L);     /* opens the table library */
//    luaopen_io(L);    /* opens the I/O library */
//    luaopen_string(L);   /* opens the string lib. */
//    luaopen_math(L);    /* opens the math lib. */



//    while (fgets(buff, sizeof(buff), stdin) != NULL) {
//        error = luaL_loadbuffer(L, buff, strlen(buff),
//                                "line") || lua_pcall(L, 0, 0, 0);
//        if (error) {
//            fprintf(stderr, "%s", lua_tostring(L, -1));
//            lua_pop(L, 1);/* pop error message from the stack */
//        }
//    }

    lua_close(L);
    return 0;
}
