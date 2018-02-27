#include "gcl_lua_db.h"
#include <vdi/gcl_data_server.h>
#include "gcl_lua_global.h"

using namespace std;

GclLuaDB * fn_oGclLuaDB()
{
    GclLua::registInitFunction(GclLuaDB::registerLua);
    static GclLuaDB m;
    return &m;
}

static GclLuaDB * f_oGclLuaDB = fn_oGclLuaDB();


static int gcs_db_execSql(lua_State *L)
{
    int n = lua_gettop(L);

    std::string sSql;
    std::string sField;
    bool bOk = true;
    if(n>=1)
    {
        bOk = LuaCommon::getValue(L, 1, sSql);
    }
    if(n>=2)
    {
        bOk = LuaCommon::getValue(L, 2, sField);
    }
    lua_pop(L,n);//清空

    if (! bOk) return FALSE;

    if(n==1)
    {
        //insert update delete
        if (sSql.size()>0)
        {
            CxDatabase * oDb = CxDatabaseManager::getDbByIndex(1);
            if (! oDb)
                return FALSE;

            msepoch_t dtNow = CxTime::currentMsepoch();

            int iResult = oDb->execSql(sSql);

            lua_pushinteger(L, iResult);

            cxDebug()<<"gcs_db_sql_exec : cost_time="<<CxTime::milliSecondDifferToNow(dtNow)<<";result="<<iResult;

            return iResult > 0;
        }
    }
    else if(n==2)
    {
        if (sSql.size()>0)
        {
            CxDatabase * oDb = CxDatabaseManager::getDbByIndex(1);
            if (! oDb)
                return FALSE;

            msepoch_t dtSql = CxTime::currentMsepoch();

            std::vector<std::vector<std::string> > rows;
            std::vector<std::string> fields;
            int iResult = oDb->loadSql(sSql,rows,&fields);

            msepoch_t dtLua = CxTime::currentMsepoch();

            //返回成功
            if(iResult>=0)
            {
                lua_newtable(L);

                LuaCommon::setTable(L,CS_Lua_Succed,1);
                LuaCommon::setTable(L,CS_Lua_Count,(int)rows.size());

                for(size_t i=0;i<rows.size();++i)
                {
                    std::vector<std::string>  rec = rows.at(i);

                    lua_newtable(L);
                    LuaCommon::setTable(L,CS_Lua_Count,(int)rec.size());

                    string sKey;
                    for(size_t j=0;j<rec.size();j++)
                    {
                        if(j<fields.size())
                        {
                            sKey = fields.at(j);
                        }else{
                            sKey = CxString::format("%d",j);
                        }
                        LuaCommon::setTable(L,sKey,rec.at(j));
                    }
                    lua_rawseti(L,-2,i+1);
                }
            }
            else
            {
                lua_newtable(L);
                LuaCommon::setTable(L,CS_Lua_Succed,0);
                LuaCommon::setTable(L,CS_Lua_Count,0);
            }

            cxDebug()<<"gcs_db_sql_load : sql_cost_time="<<dtLua-dtSql<<";lua_cost_time"<<CxTime::milliSecondDifferToNow(dtLua)<<";result="<<iResult;

            return iResult >= 0 ? TRUE : FALSE;
        }
        //cmx 20160823
        //return FALSE;
    }
    return FALSE;
}
//SQL RTDB 语句执行
static int gcs_db_execSql_rtdb(lua_State *L)
{
    int n = lua_gettop(L);

    std::string sSql;
    std::string sField;

    bool bOk = true;
    if(n>=1)
    {
        bOk = LuaCommon::getValue(L, 1, sSql);
    }
    if(n>=2)
    {
        bOk = LuaCommon::getValue(L, 2, sField);
    }
    lua_pop(L,n);//清空
    if (! bOk) return FALSE;

    if(n==1)
    {
        GclScript::outScriptPrompt()<<sSql;
//        msepoch_t tmStart = CxTime::currentMsepoch();
        CxDatabase * oDb = CxDatabase::getDefaultDb();
        bool bRet = oDb ? oDb->execSql(sSql) : false;

//        int spent = CxTime::currentMsepoch()-tmStart;
//        GclScript::outScriptPrompt()<<"execSql from " << tmStart << " spent time " << spent;

        lua_newtable(L);
        LuaCommon::setTable(L,CS_Lua_Succed,bRet);
        LuaCommon::setTable(L,CS_Lua_Count,0);
        return 0;
    }
    else if(n==2)
    {
//        msepoch_t tmStart = CxTime::currentMsepoch();
        std::vector<std::vector<std::string> > rows;
        CxDatabase * oDb = CxDatabase::getDefaultDb();
        int bRet = oDb ? oDb->loadSql(sSql,rows) : -1;

        //计算Field
        std::vector<std::string> vField = CxString::split(sField,',',false);

        //返回成功
        if(bRet>=0)
        {
            lua_newtable(L);

            LuaCommon::setTable(L,CS_Lua_Succed,1);
            LuaCommon::setTable(L,CS_Lua_Count,(int)rows.size());

            for(size_t i=0;i<rows.size();++i)
            {
                std::vector<std::string>  rec = rows.at(i);

                lua_newtable(L);
                LuaCommon::setTable(L,CS_Lua_Count,(int)rec.size());

                string sKey;
                for(size_t j=0;j<rec.size();j++)
                {
                    if(j<vField.size())
                    {
                        sKey = vField.at(j);
                    }else{
                        sKey = CxString::format("%d",j);
                    }
                    LuaCommon::setTable(L,sKey,rec.at(j));
                }
                lua_rawseti(L,-2,i+1);
            }
//            int spent = CxTime::currentMsepoch()-tmStart;
//            GclScript::outScriptPrompt()<<"execSql from " << tmStart << " spent time " << spent;
            return 1;
        }
        else
        {
            lua_newtable(L);
            LuaCommon::setTable(L,CS_Lua_Succed,0);
            LuaCommon::setTable(L,CS_Lua_Count,0);
            return 0;
        }
    }
    return 0;
}

//20150803
//params : sSql , sConnectSource , sParams
static int gcs_db_sql_exec(lua_State *L)
{

    int n = lua_gettop(L);

    std::string sSql;
    std::string sConnectSource;
    std::string sParams;

    bool bOk = true;
    if(n>=1)
    {
        bOk = LuaCommon::getValue(L, 1, sSql);
    }
    if(n>=2)
    {
        bOk = LuaCommon::getValue(L, 2, sConnectSource);
    }
    if(n>=3)
    {
        bOk = LuaCommon::getValue(L, 3, sParams);
    }
    lua_pop(L,n);//清空

    if (! bOk) return FALSE;

    if (sSql.size()>0)
    {
        CxDatabase * oDb = sConnectSource.size() > 0 ? CxDatabaseManager::findDb(sConnectSource) : CxDatabaseManager::getDefaultDb();
        if (! oDb)
            return FALSE;

        msepoch_t dtNow = CxTime::currentMsepoch();

        int iResult = oDb->execSql(sSql);

        lua_pushinteger(L, iResult);

        cxDebug()<<"gcs_db_sql_exec : cost_time="<<CxTime::milliSecondDifferToNow(dtNow)<<";result="<<iResult;

        return iResult > 0;
    }

    return FALSE;
}

//20150803
//params : sSql , sConnectSource , sParams
static int gcs_db_sql_list_exec(lua_State *L)
{

    int n = lua_gettop(L);

    std::vector<std::string> sqlList;
    std::string sConnectSource;
    std::string sParams;

    bool bOk = true;
    if(n>=1)
    {
        bOk = LuaCommon::getValue(L, 1, sqlList);
    }
    if(n>=2)
    {
        bOk = LuaCommon::getValue(L, 2, sConnectSource);
    }
    if(n>=3)
    {
        bOk = LuaCommon::getValue(L, 3, sParams);
    }
    lua_pop(L,n);//清空

    if (! bOk) return FALSE;

    if (sqlList.size()>0)
    {
        CxDatabase * oDb = sConnectSource.size() > 0 ? CxDatabaseManager::findDb(sConnectSource) : CxDatabaseManager::getDefaultDb();
        if (! oDb)
            return FALSE;

        msepoch_t dtNow = CxTime::currentMsepoch();

        int iResult = oDb->execSqlList(sqlList);

        lua_pushinteger(L, iResult);

        cxDebug()<<"gcs_db_sql_list_exec : cost_time="<<CxTime::milliSecondDifferToNow(dtNow)<<";result="<<iResult;

        return iResult > 0;
    }

    return FALSE;
}

//20150803
//params : sSql , sConnectSource, sParams
static int gcs_db_sql_load(lua_State *L)
{
    int n = lua_gettop(L);

    std::string sSql;
    std::string sConnectSource;
    std::string sParams;

    bool bOk = true;
    if(n>=1)
    {
        bOk = LuaCommon::getValue(L, 1, sSql);
    }
    if(n>=2)
    {
        bOk = LuaCommon::getValue(L, 2, sConnectSource);
    }
    if(n>=3)
    {
        bOk = LuaCommon::getValue(L, 3, sParams);
    }
    lua_pop(L,n);//清空

    if (! bOk) return FALSE;

    if (sSql.size()>0)
    {
        CxDatabase * oDb = sConnectSource.size() > 0 ? CxDatabaseManager::findDb(sConnectSource) : CxDatabaseManager::getDefaultDb();
        if (! oDb)
            return FALSE;

        msepoch_t dtSql = CxTime::currentMsepoch();

        std::vector<std::vector<std::string> > rows;
        std::vector<std::string> fields;
        int iResult = oDb->loadSql(sSql,rows,&fields);

        msepoch_t dtLua = CxTime::currentMsepoch();

        //返回成功
        if(iResult>=0)
        {
            lua_newtable(L);

            LuaCommon::setTable(L,CS_Lua_Succed,1);
            LuaCommon::setTable(L,CS_Lua_Count,(int)rows.size());

            for(size_t i=0;i<rows.size();++i)
            {
                std::vector<std::string>  rec = rows.at(i);

                lua_newtable(L);
                LuaCommon::setTable(L,CS_Lua_Count,(int)rec.size());

                string sKey;
                for(size_t j=0;j<rec.size();j++)
                {
                    if(j<fields.size())
                    {
                        sKey = fields.at(j);
                    }else{
                        sKey = CxString::format("%d",j);
                    }
                    LuaCommon::setTable(L,sKey,rec.at(j));
                }
                lua_rawseti(L,-2,i+1);
            }
        }
        else
        {
            lua_newtable(L);
            LuaCommon::setTable(L,CS_Lua_Succed,0);
            LuaCommon::setTable(L,CS_Lua_Count,0);
        }

        cxDebug()<<"gcs_db_sql_load : sql_cost_time="<<dtLua-dtSql<<";lua_cost_time"<<CxTime::milliSecondDifferToNow(dtLua)<<";result="<<iResult;

        return iResult >= 0 ? TRUE : FALSE;
    }

    return FALSE;
}

//20150803
//params : sTableName, columnNames, rows, sConnectSource
static int gcs_db_table_insert(lua_State *L)
{
    int n = lua_gettop(L);

    string sTableName;
    vector<string> sColumnNames;
    vector<vector<string> > sRows;
    string sConnectSource;

    bool bOk = true;
    if(n>=3)
    {
        bOk = LuaCommon::getValue(L, 1, sTableName);
        bOk = LuaCommon::getValue(L, 2, sColumnNames);
        bOk = LuaCommon::getValue(L, 3, sRows);
    }
    if(n>=4)
    {
        bOk = LuaCommon::getValue(L, 4, sConnectSource);
    }
    lua_pop(L,n);//清空

    if (! bOk) return FALSE;

    if (sTableName.size()>0 && sColumnNames.size()>0 && sRows.size()>0)
    {
        CxDatabase * oDb = sConnectSource.size() > 0 ? CxDatabaseManager::findDb(sConnectSource) : CxDatabaseManager::getDefaultDb();
        if (! oDb)
            return FALSE;

        msepoch_t dtSql = CxTime::currentMsepoch();

        int iResult = oDb->saveTable(sTableName, sColumnNames, sRows);

        cxDebug()<<"gcs_db_table_insert : sql_cost_time="<<CxTime::milliSecondDifferToNow(dtSql)<<";result="<<iResult;

        lua_pushinteger(L, iResult);

        return iResult > 0;
    }

    return FALSE;
}

static int gcs_reconnect_db(lua_State *L)
{
    int n = lua_gettop(L);

    int nConnectIndex;
    std::string sConnectSource;
    std::string sConnectType;

    bool bOk = true;
    if(n>=1)
    {
        bOk = LuaCommon::getValue(L, 1, nConnectIndex);
    }
    if(n>=2)
    {
        bOk = LuaCommon::getValue(L, 2, sConnectSource);
    }
    if(n>=3)
    {
        bOk = LuaCommon::getValue(L, 3, sConnectType);
    }
    lua_pop(L,n);//清空

    if (! bOk) return FALSE;

    if (sConnectSource.size()>0)
    {
        int iResult = 0;
        bool ret = CxDatabaseManager::closeDatabase(sConnectSource);
        if (ret == true)
        {
            CxDatabase *oDb = CxDatabaseManager::getDbByIndex(nConnectIndex);
            if (oDb)
            {
                map<string, string> sConnectParams = CxString::splitToMap("", CxGlobal::middleChar, CxGlobal::splitChar);
                iResult = oDb->openDatabase(sConnectSource, sConnectType, true, &sConnectParams);
            }
        }
        lua_pushinteger(L, iResult);
        msepoch_t dtNow = CxTime::currentMsepoch();
        cxDebug()<<"gcs_reconnect_db : cost_time="<<CxTime::milliSecondDifferToNow(dtNow)<<";result="<<iResult;
        return iResult > 0;
    }

    return FALSE;
}

void GclLuaDB::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "gcs_reconnect_db", gcs_reconnect_db);
    LuaCommon::setTable(L, "execSql", gcs_db_execSql);
    LuaCommon::setTable(L, "execSql_RTDB", gcs_db_execSql_rtdb);
    LuaCommon::setTable(L, "sql_exec", gcs_db_sql_exec);
    LuaCommon::setTable(L, "sql_list_exec", gcs_db_sql_list_exec);
    LuaCommon::setTable(L, "sql_load", gcs_db_sql_load);
    LuaCommon::setTable(L, "table_insert", gcs_db_table_insert);
    lua_setglobal(L, "gcs_db");    /* 'name' = table */
}

GclLuaDB::GclLuaDB()
{

}

