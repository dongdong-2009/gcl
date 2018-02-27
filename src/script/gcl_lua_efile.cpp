#include "gcl_lua_efile.h"

#include <ccxx/cxefile.h>

#include "gcl_lua_global.h"

#include <vdi/protocol_global.h>

//#include <common/gcl.h>

using namespace std;

const std::string CS_Lua_EFile_Class = "class";
const std::string CS_Lua_EFile_Instance = "instance";

//E file 数据单元加载
void LuaEE_Load(lua_State *L,EFileElement &obj, int index);
//map表
void LuaMap_Load(lua_State *L, std::map<std::string, std::string> &map1, int index);


GclLuaEFile * fn_oGclLuaEFile()
{
    GclLua::registInitFunction(GclLuaEFile::registerLua);
    static GclLuaEFile m;
    return &m;
}

static GclLuaEFile * f_oGclLuaEFile = fn_oGclLuaEFile();


//map表
void LuaMap_Load(lua_State *L,std::map<std::string, std::string> &map1,int index)
{
    lua_newtable(L);

    LuaCommon::setTable(L,CS_Lua_Count,(int)map1.size());

    for(std::map<std::string,std::string>::const_iterator it = map1.begin(); it != map1.end(); ++it)
    {
        std::string key = it->first;
        std::string value = it->second;

        LuaCommon::setTable(L,key,value);
    }
    lua_rawseti(L,-2,index);
}
//E file 数据单元加载
void LuaEE_Load(lua_State *L,EFileElement &obj,int index)
{
    std::vector<std::map<std::string, std::string> > vRecs = obj.getRec();

//    cxPrompt()<<vRecs.size();


    lua_newtable(L);

    std::string className,InstanceName;
    obj.getName(className,InstanceName);

    LuaCommon::setTable(L,CS_Lua_EFile_Class,className);
    LuaCommon::setTable(L,CS_Lua_EFile_Instance,InstanceName);
    LuaCommon::setTable(L,CS_Lua_Count,(int)vRecs.size());

    for(size_t i = 0;i<vRecs.size();++i)
    {
         LuaMap_Load(L,vRecs.at(i),i+1);
    }
    lua_rawseti(L,-2,index);
}


static int gcs_efile_parse(lua_State *L)
{
//    const char* sFile = luaL_checkstring(L,1);
    string sFile; bool bOk = LuaCommon::getValue(L, 1, sFile);
    if (! bOk || sFile.empty())
        return FALSE;

    std::map<std::string, std::string> declare;
    std::vector<EFileElement> vObj;

    lua_newtable(L);

    if(ExplainEfile::loadFromFile(sFile,declare,vObj)>0)
    {
         LuaCommon::setTable(L,CS_Lua_Succed,1);
         LuaCommon::setTable(L,CS_Lua_Count,(int)vObj.size()+1);

         //加载声明部分
         LuaMap_Load(L,declare,1);
         //加载数据部分
         EFileElement obj;
         for(size_t i =0;i<vObj.size();++i)
         {
             obj = vObj.at(i);
             LuaEE_Load(L,obj,i+1+1);
         }
    }
    else //失败
    {
        LuaCommon::setTable(L,CS_Lua_Succed,0);
        LuaCommon::setTable(L,CS_Lua_Count,0);
    }
    return 1;
}

static int gcs_efile_parse_string(lua_State *L)
{
//    const char* sFile = luaL_checkstring(L,1);
    string sFileData; bool bOk = LuaCommon::getValue(L, 1, sFileData);
    if (! bOk || sFileData.empty())
        return FALSE;

    std::map<std::string, std::string> declare;
    std::vector<EFileElement> vObj;

    lua_newtable(L);

    if(ExplainEfile::explain(sFileData,declare,vObj)>0)
    {
         LuaCommon::setTable(L,CS_Lua_Succed,1);
         LuaCommon::setTable(L,CS_Lua_Count,(int)vObj.size()+1);

         //加载声明部分
         LuaMap_Load(L,declare,1);
         //加载数据部分
         EFileElement obj;
         for(size_t i =0;i<vObj.size();++i)
         {
             obj = vObj.at(i);
             LuaEE_Load(L,obj,i+1+1);
         }
    }
    else //失败
    {
        LuaCommon::setTable(L,CS_Lua_Succed,0);
        LuaCommon::setTable(L,CS_Lua_Count,0);
    }
    return 1;
}

static int gcs_efile_from_string(lua_State *L)
{
//    const char* sBuffer = luaL_checkstring(L,1);
    string sBuffer; bool bOk = LuaCommon::getValue(L, 1, sBuffer);
    if (! bOk || sBuffer.empty())
        return FALSE;

    std::map<std::string, std::string> declare;
    std::vector<EFileElement> vObj;

    lua_newtable(L);

    if(ExplainEfile::loadFromString(sBuffer,declare,vObj)>0)
    {
         LuaCommon::setTable(L,CS_Lua_Succed,1);
         LuaCommon::setTable(L,CS_Lua_Count,(int)vObj.size()+1);

         //加载声明部分
         LuaMap_Load(L,declare,1);
         //加载数据部分
         EFileElement obj;
         for(size_t i =0;i<vObj.size();++i)
         {
             obj = vObj.at(i);
             LuaEE_Load(L,obj,i+1+1);
         }
    }
    else //失败
    {
        LuaCommon::setTable(L,CS_Lua_Succed,0);
        LuaCommon::setTable(L,CS_Lua_Count,0);
    }
    return 1;
}
//
static int gcs_efile_clear(lua_State *L)
{
    ExplainEfile::clear();
    return 1;
}

static int gcs_efile_createDeclare(lua_State *L)
{
    int iTop = lua_gettop(L);
    string sParam;
    if(iTop == 1)
    {
        LuaCommon::getValue(L, 1, sParam);
        std::map<string,string> map1 = CxString::splitToMap(sParam, '=', ' ');
        ExplainEfile::createDeclare(map1);
        return 1;
    }
    return 0;
}
static int gcs_efile_createHead(lua_State *L)
{
    int iTop = lua_gettop(L);

    if(iTop<1) return 0;
    string s = GclLuaBase::fromTable(L,1);
    ExplainEfile::createElement_row("Head","Head",s);
    return 1;
}
static int gcs_efile_createHead_v2(lua_State *L)
{
    int iTop = lua_gettop(L);
    if(iTop<1) return 0;
    string s = GclLuaBase::fromTable(L,1);
    ExplainEfile::createElement_row("Head","Head",s);
    return 1;
}

static int gcs_efile_createElement(lua_State *L)
{
    int iTop = lua_gettop(L);
    string sClassName,sInstanceName,sql,sField;

    if(iTop == 3)
    {
        LuaCommon::getValue(L, 1, sClassName);
        LuaCommon::getValue(L, 2, sInstanceName);
        LuaCommon::getValue(L, 3, sql);
        ExplainEfile::createElement_sql(sClassName,sInstanceName,sql);
        return 1;
    }
    else if(iTop == 4)
    {
        LuaCommon::getValue(L, 1, sClassName);
        LuaCommon::getValue(L, 2, sInstanceName);
        LuaCommon::getValue(L, 3, sql);
        LuaCommon::getValue(L, 4, sField);
        ExplainEfile::createElement_sql(sClassName,sInstanceName,sql,sField);
        return 1;
    }
    return 0;
}

static int gcs_efile_createElement_v2(lua_State *L)
{
    int iTop = lua_gettop(L);
    if(iTop < 3) return 0;

    string sClassName,sInstanceName,sValue;

    sClassName = GclLuaBase::fromTable(L,1);
    sInstanceName = GclLuaBase::fromTable(L,2);
    sValue = GclLuaBase::fromTable(L,3);
    ExplainEfile::createElement_row(sClassName,sInstanceName,sValue);
    return 1;
}

static int gcs_efile_save(lua_State *L)
{
    int iTop = lua_gettop(L);
    string sFile;

    if(iTop == 1)
    {
        LuaCommon::getValue(L, 1, sFile);
        ExplainEfile::toFile(sFile);
        return 1;
    }
    return 0;
}
static int gcs_efile_to_string(lua_State *L)
{
    string s=  ExplainEfile::toString();
    lua_newtable(L);
    int len = s.size();
    if(len>0)
    {
        LuaCommon::setTable(L,CS_Lua_Succed,1);
        LuaCommon::setTable(L,CS_Lua_Count,len);
        LuaCommon::setTable(L,CS_Lua_Value,s);
    }
    else //失败
    {
        LuaCommon::setTable(L,CS_Lua_Succed,0);
        LuaCommon::setTable(L,CS_Lua_Count,0);
    }
    return 1;
}



void GclLuaEFile::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "parse", gcs_efile_parse);
    //LuaCommon::setTable(L, "parse_file", gcs_efile_parse);
    LuaCommon::setTable(L, "parse_string", gcs_efile_parse_string);
    LuaCommon::setTable(L, "clear", gcs_efile_clear);
    LuaCommon::setTable(L, "createDeclare", gcs_efile_createDeclare);
    LuaCommon::setTable(L, "createHead", gcs_efile_createHead);
    LuaCommon::setTable(L, "createHead_v2", gcs_efile_createHead_v2);
    LuaCommon::setTable(L, "createElement", gcs_efile_createElement);
    LuaCommon::setTable(L, "createElement_v2", gcs_efile_createElement_v2);
    LuaCommon::setTable(L, "save", gcs_efile_save);
    LuaCommon::setTable(L, "toString", gcs_efile_to_string);
    LuaCommon::setTable(L, "fromString", gcs_efile_from_string);

    lua_setglobal(L, "gcs_efile");    /* 'name' = table */
}

GclLuaEFile::GclLuaEFile()
{

}

bool GclLuaEFile::dealBuffer(const char *fc, const char *pBuf, int len)
{
    string sPath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , "Recv" );
    CxFileSystem::createDirMultiLevel(sPath);
    string sFileName =  CxFileSystem::mergeFilePath(sPath, CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0)+"_"+ string(fc) + ".efl");

    if(CxFile::save(sFileName,string(pBuf,len)))
    {
       return  dealFile(fc,sFileName.data());
    }
    return false;
}

//lua : gcs_efile_dealReceiveFile(fc, fullpath) return
bool GclLuaEFile::dealFile(const char *fc, const char *fullPath)
{
//    int ret = 0;

//    string scriptName =  CxFileSystem::mergeFilePath(GclConfig::scriptPath(),"ics_e_file.lua");

//    cxPrompt()<<CxTime::currentSepochString()<<scriptName<<cxEndLine;
//    ret = luaL_dofile(L, scriptName.data());
//    if( ret != 0 )
//    {
//        string sErr = CxString::format("lua_pcall failed:[%s]",lua_tostring(L,-1));

//        cxPrompt()<<CxTime::currentSepochString()<<sErr<<cxEndLine;
//        return false;
//    }

    lua_State * L = GclLua::luaState();
    if(L==NULL) return false;

    //获取全局函数
    int iLuaType = lua_getglobal(L, "gcs_efile_dealReceiveFile");
    if (iLuaType == LUA_TFUNCTION)
    {
        //参数压栈
        lua_pushstring(L,fc);
        lua_pushstring(L,fullPath);

        if(LuaCommon::callLuaFunction(L,2,1))
        {
            return true;
        }
    }
    return false;
}
