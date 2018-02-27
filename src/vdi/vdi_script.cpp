#include "vdi_script.h"

#include <gcl_lua.h>
#include <ccxx/cxefile.h>


using namespace std;


lua_State* f_oLuaState;

void LuaXML_ParseNode (lua_State *L,TiXmlNode* pNode);

#define GCL_LUA_PRAM_COUNT  5
//传入参数,MeasureID数组
//传出参数:ID,Type,q,lenth,value；
static int GCS_GetValueByID(lua_State *L)
{
    //返回栈中元素的个数
    int n = lua_gettop(L);
    std::vector<int> vec;
    vec.clear();

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

    gcl_stu_lua_value value;

    GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();

    for (i = 0; i <vec.size(); i++)
    {
        if(pSin==NULL)break;

        pSin->getValueByID(vec.at(i),value);

        lua_pushnumber(L,value.id);
        lua_pushnumber(L,value.type);
        lua_pushnumber(L,value.q);
        lua_pushnumber(L,value.len);

        switch(value.type)
        {
        case GCL_VT_YX:
            lua_pushnumber(L,value.iValue);

            break;
        case GCL_VT_YC:
            lua_pushnumber(L,value.dValue);
            break;
        case GCL_VT_STR:
            lua_pushstring(L,value.sValue.data());
            break;
        default:
            lua_pushnumber(L,value.iValue);
            break;
        }
    }
    n = lua_gettop(L);
//    cxDebug()<<"lua_gettop"<<n<<cxEndLine;

    /* return the number of results */
    return vec.size()*GCL_LUA_PRAM_COUNT;
}
//传入参数,url 数组
//传出参数:ID,Type,q,lenth,value；
static int GCS_GetValueByUrl(lua_State *L)
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
    GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();

    for (i = 0; i <vec.size(); i++)
    {
        if(pSin==NULL)break;
        pSin->getValueByUrl(vec.at(i),value);

        lua_pushnumber(L,value.id);
        lua_pushnumber(L,value.type);
        lua_pushnumber(L,value.q);
        lua_pushnumber(L,value.len);

        switch(value.type)
        {
        case GCL_VT_YX:
            lua_pushnumber(L,value.iValue);

            break;
        case GCL_VT_YC:
            lua_pushnumber(L,value.dValue);
            break;
        case GCL_VT_STR:
            lua_pushstring(L,value.sValue.data());
            break;
        default:
            lua_pushnumber(L,value.iValue);
            break;
        }
    }
    n = lua_gettop(L);
//    cxDebug()<<"lua_gettop"<<n<<cxEndLine;

    /* return the number of results */
    return vec.size()*GCL_LUA_PRAM_COUNT;
}

//传入参数:ID,Type,q,lenth,value；
//传出参数,成功数目
static int GCS_SetValueByID(lua_State *L)
{
    //返回栈中元素的个数
    int n = lua_gettop(L)/GCL_LUA_PRAM_COUNT;

    gcl_stu_lua_value value;

    int nCount =0;
    bool bRet = false;
    int pos = 0;
    for (int i = 1; i <= n; i++)
    {

        pos = (i-1)*GCL_LUA_PRAM_COUNT+1;

        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.id =  lua_tonumber(L,pos);
            bRet = true;
        }

        pos++;
        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.type =  lua_tonumber(L,pos);
            bRet = true;
        }
        pos++;
        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.q =  lua_tonumber(L,pos);
            bRet = true;
        }
        pos++;
        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.len =  lua_tonumber(L,pos);
            bRet = true;
        }
        pos++;

        bRet = false;
        switch(value.type)
        {
        case GCL_VT_YX:
            if(lua_isnumber(L,pos))
            {
                value.iValue = lua_tonumber(L,pos);
                bRet = true;
            }
            break;
        case GCL_VT_YC:
            if(lua_isnumber(L,pos))
            {
                value.dValue = lua_tonumber(L,pos);
                bRet = true;
            }
            break;
        case GCL_VT_STR:
            if(lua_isstring(L,pos))
            {
                value.sValue =lua_tostring(L,pos);
                bRet = true;
            }
            break;
        default:
            break;
        }
        if(bRet)
        {
            GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();

            if(pSin)
            {
                pSin->setValueByID(value);
                nCount++;
            }
        }
    }
    //清空
    lua_pop(L,n);//清空
    /* return the number of results */
    lua_pushinteger(L,nCount);

    return 1;
}
//传入参数:url,Type,q,lenth,value；
//传出参数,成功数目
static int GCS_SetValueByUrl(lua_State *L)
{
    //返回栈中元素的个数
    int n = lua_gettop(L)/GCL_LUA_PRAM_COUNT;

    gcl_stu_lua_value value;

    int nCount =0;
    bool bRet = false;

    string sUrl="";
    int pos = 0;
    for (int i = 1; i <= n; i++)
    {

        pos = (i-1)*GCL_LUA_PRAM_COUNT+1;

        bRet = false;
        if(lua_isstring(L,pos))
        {
            sUrl =  lua_tostring(L,pos);
            bRet = true;
        }

        pos++;
        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.type =  lua_tonumber(L,pos);
            bRet = true;
        }
        pos++;
        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.q =  lua_tonumber(L,pos);
            bRet = true;
        }
        pos++;
        bRet = false;
        if(lua_isnumber(L,pos))
        {
            value.len =  lua_tonumber(L,pos);
            bRet = true;
        }
        pos++;

        bRet = false;
        switch(value.type)
        {
        case GCL_VT_YX:
            if(lua_isnumber(L,pos))
            {
                value.iValue = lua_tonumber(L,pos);
                bRet = true;
            }
            break;
        case GCL_VT_YC:
            if(lua_isnumber(L,pos))
            {
                value.dValue = lua_tonumber(L,pos);
                bRet = true;
            }
            break;
        case GCL_VT_STR:
            if(lua_isstring(L,pos))
            {
                value.sValue =lua_tostring(L,pos);
                bRet = true;
            }
            break;
        default:
            break;
        }
        if(bRet)
        {
            GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();
            if(pSin)
            {
                pSin->setValueByUrl(sUrl,value);
                nCount++;
            }
        }
    }
    //清空
    lua_pop(L,n);//清空
    /* return the number of results */
    lua_pushinteger(L,nCount);

    return 1;
}
static int GCS_SendMessage(lua_State *L)
{
    int n = lua_gettop(L);

//    cxDebug()<<"YGCT_SendMessage lua_gettop"<<n<<cxEndLine;


    string sCommand="";
    string sPrameter="";
    bool bRet =false;

    if(n==1)
    {
        if(lua_isstring(L,1)>0)
        {
            sCommand = lua_tostring(L,1);
            bRet = true;
        }
    }

    if(n==2)
    {
        if(lua_isstring(L,1)>0)
        {
            sCommand = lua_tostring(L,1);
            bRet = true;
        }
        bRet = false;
        if(lua_isstring(L,2)>0)
        {
            sPrameter = lua_tostring(L,2);
            bRet = true;
        }
    }
    //清空
    lua_pop(L,lua_gettop(L));

    //发送消息
    if(bRet)
    {
        GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();
        if(pSin)
        {
            pSin->sendMeaaage(sCommand,sPrameter);
        }
    }
    //压栈
    lua_pushboolean(L,bRet);
    return 1;
}
static int GCS_XmlFileParse (lua_State *L)
{
    const char* sFileName = luaL_checkstring(L,1);
    TiXmlDocument doc(sFileName);
    doc.LoadFile();
    lua_newtable(L);
    LuaXML_ParseNode(L,&doc);

    cxPrompt()<<CxTime::currentSepochString()<<"GCS_XmlFileParse"<<cxEndLine;

    return 1;
}
//e File解析
static int GCS_EFileParse(lua_State *L)
{
    const char* sFile = luaL_checkstring(L,1);

    std::map<std::string, std::string> declare;
    std::vector<EFileElement> vObj;

    if(ExplainEfile::loadFromFile(sFile,declare,vObj)>0)
    {


    }
}

//文件保存
static int GCS_XmlFileSave (lua_State *L)
{
//    const char* sFileName = luaL_checkstring(L,1);
//    TiXmlDocument doc(sFileName);
//    doc.LoadFile();
//    lua_newtable(L);
//    LuaXML_ParseNode(L,&doc);

    return 0;
}
//SQL语句执行
static int GCS_SQLExecute(lua_State *L)
{
   int n = lua_gettop(L);

   std::string sSQL="";
   std::string sField="";

    cxDebug()<<"lua_gettop"<<n<<cxEndLine;

   if(n==1)
   {
       sSQL   = CxString::format("%s",luaL_checkstring(L,1));

       GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();
       if(pSin)
       {
           return (int)(pSin->executeSQL((char*)sSQL.data(),sSQL.length()));
       }
       return 0;
   }
   else if(n==2)
   {
       sSQL   = CxString::format("%s",luaL_checkstring(L,1));
       sField = CxString::format("%s",luaL_checkstring(L,2));

       lua_pop(L,n);//清空

       cxDebug()<<sSQL<<sField<<cxEndLine;

       //计算Field
       std::vector<std::string> vValue = CxString::split(sField,',',false);

       GCCommon::GCComObjVector vField;
       GCCommon::GCCommonObj obj;

       cxDebug()<<vValue.size()<<cxEndLine;

       int i=0;
       for(i=0;i<vValue.size();i++)
       {
           std::vector<std::string> vObj = CxString::split(vValue.at(i),'|',false);
           if(vObj.size()<2)continue;

           obj.clear();
           obj.setName(vObj.at(0));
           obj.setType(CxString::toUint32(vObj.at(1)));
           vField.push_back(obj);
       }

       std::vector<std::map<std::string,std::string> > vDataSet;
       vDataSet.clear();

       cxDebug()<<"vDataSet.clear()"<<cxEndLine;

       bool bRet = false;
       GCDataSvrSingleton *pSin = GCDataSvrSingleton::singleton();
       if(pSin)
       {
           bRet = pSin->executeSQL(vField,vDataSet,(char*)sSQL.data(),sSQL.length());
       }

       cxDebug()<<"pSin->executeSQL"<<bRet<<cxEndLine;

       //返回成功
       if(bRet)
       {
           cxDebug()<<"return:"<<vDataSet.size()<<cxEndLine;

           lua_newtable(L);

           lua_pushstring(L,"succed");
           lua_pushnumber(L,1);
           lua_settable(L,-3);

           lua_pushstring(L,"n");
           lua_pushnumber(L,vDataSet.size());
           lua_settable(L,-3);

           for(i=0;i<vDataSet.size();i++)
           {
               std::map<std::string,std::string> map1 = vDataSet.at(i);

               lua_newtable(L);
               lua_pushstring(L,"n");
               lua_pushnumber(L,vField.size());
               lua_settable(L,-3);

               for(int j=0;j<vField.size();j++)
               {
                   obj = vField.at(j);
                   std::string sVal = CxContainer::value(map1,obj.name());
                   sVal = CxString::trim(sVal,CxGlobal::spaceChar);

                   lua_pushstring(L,obj.name().data());
                   lua_pushstring(L,sVal.data());
                   lua_settable(L,-3);
               }

               lua_rawseti(L,-2,i+1);
           }
           return 1;
       }
       else
       {
           lua_newtable(L);

           lua_pushstring(L,"succed");
           lua_pushnumber(L,0);
           lua_settable(L,-3);

           lua_pushstring(L,"n");
           lua_pushnumber(L,0);
           lua_settable(L,-3);

           return 0;
       }

       cxDebug()<<"GCS_SQLExecute end"<<bRet<<cxEndLine;

   }
   return 0;
}
//SQL RTDB 语句执行
static int GCS_SQLExecute_RTDB(lua_State *L)
{
   int n = lua_gettop(L);

   std::string sSQL="";
   std::string sField="";

   cxDebug()<<"lua_gettop"<<n<<cxEndLine;

   if(n==1)
   {
       sSQL   = CxString::format("%s",luaL_checkstring(L,1));
       bool bRet = CxDatabase::defaultDb()->execSql(sSQL);

       lua_newtable(L);

       lua_pushstring(L,"succed");
       lua_pushnumber(L,bRet);
       lua_settable(L,-3);

       lua_pushstring(L,"n");
       lua_pushnumber(L,0);
       lua_settable(L,-3);

       return 0;
   }
   else if(n==2)
   {
       sSQL   = CxString::format("%s",luaL_checkstring(L,1));
       sField = CxString::format("%s",luaL_checkstring(L,2));

       lua_pop(L,n);//清空

       cxDebug()<<sSQL<<sField<<cxEndLine;

       std::vector<std::vector<std::string> > rows;
       bool bRet = CxDatabase::defaultDb()->loadSql(sSQL,rows);

       //计算Field
       std::vector<std::string> vField = CxString::split(sField,',',false);

       //返回成功
       if(bRet)
       {
           cxDebug()<<"return:"<<rows.size()<<cxEndLine;

           lua_newtable(L);

           lua_pushstring(L,"succed");
           lua_pushnumber(L,1);
           lua_settable(L,-3);

           lua_pushstring(L,"n");
           lua_pushnumber(L,rows.size());
           lua_settable(L,-3);

           for(size_t i=0;i<rows.size();++i)
           {
               std::vector<std::string>  rec = rows.at(i);


               lua_newtable(L);
               lua_pushstring(L,"n");
               lua_pushnumber(L,rec.size());
               lua_settable(L,-3);

               for(int j=0;j<rec.size();j++)
               {
                   if(j<vField.size())
                   {
                       lua_pushstring(L,vField.at(j).data());
                   }else{
                       lua_pushstring(L,CxString::format("%d",j).data());
                   }
                   lua_pushstring(L,rec.at(j).data());
                   lua_settable(L,-3);
               }
               lua_rawseti(L,-2,i+1);
           }
           return 1;
       }
       else
       {
           lua_newtable(L);

           lua_pushstring(L,"succed");
           lua_pushnumber(L,0);
           lua_settable(L,-3);

           lua_pushstring(L,"n");
           lua_pushnumber(L,0);
           lua_settable(L,-3);

           return 0;
       }

       cxDebug()<<"GCS_SQLExecute end"<<bRet<<cxEndLine;

   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LuaXML_ParseNode (lua_State *L,TiXmlNode* pNode)
{
    if (!pNode) return;

    // resize stack if neccessary
    luaL_checkstack(L, 5, "LuaXML_ParseNode : recursion too deep");

    TiXmlElement* pElem = pNode->ToElement();
    if (pElem)
    {
        // element name
        lua_pushstring(L,"name");
        lua_pushstring(L,pElem->Value());
        lua_settable(L,-3);

        // parse attributes
        TiXmlAttribute* pAttr = pElem->FirstAttribute();
        if (pAttr)
        {
            lua_pushstring(L,"attr");
            lua_newtable(L);
            for (;pAttr;pAttr = pAttr->Next())
            {
                lua_pushstring(L,pAttr->Name());
                lua_pushstring(L,pAttr->Value());
                lua_settable(L,-3);
            }
            lua_settable(L,-3);
        }
    }

    // children
    TiXmlNode *pChild = pNode->FirstChild();
    if (pChild)
    {
        int iChildCount = 0;
        for(;pChild;pChild = pChild->NextSibling())
        {
            switch (pChild->Type())
            {
            case TiXmlNode::TINYXML_DOCUMENT:
                break;
            case TiXmlNode::TINYXML_ELEMENT:
            {
                // normal element, parse recursive
                lua_newtable(L);
                LuaXML_ParseNode(L,pChild);
                lua_rawseti(L,-2,++iChildCount);
            }
                break;
            case TiXmlNode::TINYXML_COMMENT:
                break;
            case TiXmlNode::TINYXML_TEXT:
                // plaintext, push raw
                lua_pushstring(L,pChild->Value());
                lua_rawseti(L,-2,++iChildCount);
                break;
            case TiXmlNode::TINYXML_DECLARATION:
                break;
            case TiXmlNode::TINYXML_UNKNOWN:
                break;
            };
        }
        lua_pushstring(L,"n");
        lua_pushnumber(L,iChildCount);
        lua_settable(L,-3);
    }
}


void VdiScript::test()
{
    /* initialize Lua */
    //    L = lua_open();
    //    L = luaL_newstate();
    /* load Lua libraries */
    //    luaL_openlibs(L);
    //    /* register our function */
    //    lua_register(L, "average", average);
    //    /* run the script */
    //    luaL_dofile(L, "e15.lua");

    //    lua_getglobal(L,"avg");
    //    cxDebug()<<"avg is:"<<lua_tointeger(L,-1)<<cxEndLine;
    //    lua_pop(L,1);
    //    lua_getglobal(L,"sum");
    //    cxDebug()<<"sum is:"<<lua_tointeger(L,-1)<<cxEndLine;
    /* cleanup Lua */
    //    lua_close(L);
}
//初始化
bool VdiScript::init()
{
    /* initialize Lua */
    //    L = lua_open();
    f_oLuaState = luaL_newstate();
    /* load Lua libraries */
    luaL_openlibs(f_oLuaState);

    lua_register(f_oLuaState, "GCS_GetValueByID", GCS_GetValueByID);
    lua_register(f_oLuaState, "GCS_GetValueByUrl", GCS_GetValueByUrl);
    lua_register(f_oLuaState, "GCS_SetValueByID", GCS_SetValueByID);
    lua_register(f_oLuaState, "GCS_SetValueByUrl", GCS_SetValueByUrl);
    lua_register(f_oLuaState, "GCS_SendMessage", GCS_SendMessage);
    lua_register(f_oLuaState, "GCS_XmlFileParse", GCS_XmlFileParse);
    lua_register(f_oLuaState, "GCS_SQLExecute", GCS_SQLExecute);
    lua_register(f_oLuaState, "GCS_SQLExecute_RTDB", GCS_SQLExecute_RTDB);
    lua_register(f_oLuaState, "GCS_EFileParse", GCS_EFileParse);
    return true;
}
//反初始化
bool VdiScript::unInit()
{
    /* cleanup Lua */
    lua_close(f_oLuaState);
    return true;
}

bool VdiScript::regFunction(char *funName)
{
    return false;
}
//运行脚本本件
bool VdiScript::runScript(const char *scriptName)
{
    int ret = 0;
    //是初始化lua栈，和重新加载函数的作用建议luaL_loadfile后都调用一次
    ret = luaL_dofile(f_oLuaState, scriptName) ;
    if( ret != 0 )
    {
        string sErr = CxString::format("lua_pcall failed:[%s]",lua_tostring(f_oLuaState,-1));

        cxPrompt()<<CxTime::currentSepochString()<<sErr<<cxEndLine;
        return false;
    }
    return true;
}

bool VdiScript::dealXMLBuffer(const char *fc, const char *pBuf, int len)
{
    string sPath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , "Recv" );
    CxFileSystem::createDirMultiLevel(sPath);
    string sFileName =  CxFileSystem::mergeFilePath(sPath, CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0)+"_"+ string(fc) + ".xml");

    if(CxFile::save(sFileName,string(pBuf,len)))
    {
       return  dealXMLFile(fc,sFileName.data());
    }
    return false;
}
//xmlFile
bool VdiScript::dealXMLFile(const char* fc,const char* fullPath)
{
    int ret = 0;

    string scriptName =  CxFileSystem::mergeFilePath(GclConfig::scriptPath(),"ics_bi.lua");

    cxPrompt()<<CxTime::currentSepochString()<<scriptName<<cxEndLine;
    ret = luaL_dofile(f_oLuaState, scriptName.data()) ;
    if( ret != 0 )
    {
        string sErr = CxString::format("lua_pcall failed:[%s]",lua_tostring(f_oLuaState,-1));

        cxPrompt()<<CxTime::currentSepochString()<<sErr<<cxEndLine;
        return false;
    }
    //获取全局函数
    lua_getglobal(f_oLuaState, "g_DealBusiness");
    //参数压栈
    lua_pushstring(f_oLuaState,fc);
    lua_pushstring(f_oLuaState,fullPath);
    //函数调用
    lua_call(f_oLuaState,2,1); //2参数 1 返回值
    //获取返回结果
    ret = lua_tonumber(f_oLuaState,-1);
    //清空
    lua_pop(f_oLuaState,lua_gettop(f_oLuaState));

    if(ret>=0)
    {
        return true;
    }
    else return false;
}
//E File
bool VdiScript::dealEfile(const char *fc, const char *fullPath)
{
    int ret = 0;

    string scriptName =  CxFileSystem::mergeFilePath(GclConfig::scriptPath(),"ics_e_file.lua");

    cxPrompt()<<CxTime::currentSepochString()<<scriptName<<cxEndLine;
    ret = luaL_dofile(f_oLuaState, scriptName.data());
    if( ret != 0 )
    {
        string sErr = CxString::format("lua_pcall failed:[%s]",lua_tostring(f_oLuaState,-1));

        cxPrompt()<<CxTime::currentSepochString()<<sErr<<cxEndLine;
        return false;
    }
    //获取全局函数
    lua_getglobal(f_oLuaState, "g_DealEfile");
    //参数压栈
    lua_pushstring(f_oLuaState,fc);
    lua_pushstring(f_oLuaState,fullPath);
    //函数调用
    lua_call(f_oLuaState,2,1); //2参数 1 返回值
    //获取返回结果
    ret = lua_tonumber(f_oLuaState,-1);
    //清空
    lua_pop(f_oLuaState,lua_gettop(f_oLuaState));

    if(ret>=0)
    {
        return true;
    }
    else return false;
}
//消息处理
bool VdiScript::dealMsg(int iTerminalId, const char *cmd, int len, const char *pram)
{
    int ret = 0;

    string scriptName =  CxFileSystem::mergeFilePath(GclConfig::scriptPath(),"ics_msg.lua");

    cxPrompt()<<CxTime::currentSepochString()<<scriptName<<cxEndLine;
    ret = luaL_dofile(f_oLuaState, scriptName.data());
    if( ret != 0 )
    {
        string sErr = CxString::format("lua_pcall failed:[%s]",lua_tostring(f_oLuaState,-1));

        cxPrompt()<<CxTime::currentSepochString()<<sErr<<cxEndLine;
        return false;
    }
    //获取全局函数
    lua_getglobal(f_oLuaState, "g_DealMsg");
    //参数压栈
    lua_pushstring(f_oLuaState,cmd);
    lua_pushinteger(f_oLuaState,len);
    lua_pushstring(f_oLuaState,pram);
    //函数调用
    lua_call(f_oLuaState,3,1); //2参数 1 返回值
    //获取返回结果
    ret = lua_tonumber(f_oLuaState,-1);
    //清空
    lua_pop(f_oLuaState,lua_gettop(f_oLuaState));

    if(ret>=0)
    {
        return true;
    }
    else return false;
}
//////////////////////////////////////////////////////////////////////

