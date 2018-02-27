#include "gcl_lua_xml.h"
#include "gcl_lua_global.h"
#include <vdi/protocol_global.h>


using namespace std;
const std::string CS_Lua_XML_Name = "name";
const std::string CS_Lua_XML_Attr = "attr";

void LuaXML_ParseNode (lua_State *L,TiXmlNode* pNode);


GclLuaXml * fn_oGclLuaXml()
{
    GclLua::registInitFunction(GclLuaXml::registerLua);
    static GclLuaXml m;
    return &m;
}

static GclLuaXml * f_oGclLuaXml = fn_oGclLuaXml();

void LuaXML_ParseNode (lua_State *L,TiXmlNode* pNode)
{
    if (!pNode) return;

    // resize stack if neccessary
    luaL_checkstack(L, 5, "LuaXML_ParseNode : recursion too deep");

    TiXmlElement* pElem = pNode->ToElement();
    if (pElem)
    {
        // element name
        LuaCommon::setTable(L,CS_Lua_XML_Name,string(pElem->Value()));
        // parse attributes
        TiXmlAttribute* pAttr = pElem->FirstAttribute();
        if (pAttr)
        {
            lua_pushstring(L,"attr");
            lua_newtable(L);
            for (;pAttr;pAttr = pAttr->Next())
            {
                LuaCommon::setTable(L,string(pAttr->Name()),string(pAttr->Value()));
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
        LuaCommon::setTable(L,CS_Lua_Count,iChildCount);
    }
}

static int gcs_xml_parse (lua_State *L)
{
//    const char* sFileName = luaL_checkstring(L,1);
    string sFileName; bool bOk = LuaCommon::getValue(L, 1, sFileName);
    if (! bOk || sFileName.empty())
        return FALSE;

    TiXmlDocument doc(sFileName);
    doc.LoadFile();
    lua_newtable(L);
    LuaXML_ParseNode(L,&doc);

    cxPrompt()<<CxTime::currentSepochString()<<"GCS_XmlFileParse"<<cxEndLine;

    return 1;
}

void GclLuaXml::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "parse", gcs_xml_parse);
//    LuaCommon::setTable(L, "save", gcs_db_execSql_rtdb);
    lua_setglobal(L, "gcs_xml");    /* 'name' = table */
}

GclLuaXml::GclLuaXml()
{
}

bool GclLuaXml::dealBuffer(const char *fc, const char *pBuf, int len)
{
    string sPath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , "Recv" );
    CxFileSystem::createDirMultiLevel(sPath);
    string sFileName =  CxFileSystem::mergeFilePath(sPath, CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0)+"_"+ string(fc) + ".xml");

    if(CxFile::save(sFileName,string(pBuf,len)))
    {
       return  dealFile(fc,sFileName.data());
    }
    return false;
}

bool GclLuaXml::dealFile(const char *fc, const char *fullPath)
{
//    int ret = 0;

    lua_State * L = GclLua::luaState();
    if(L==NULL) return false;
    //获取全局函数
    int iLuaType = lua_getglobal(L, "gcs_xml_dealReceiveFile");
    if (iLuaType == LUA_TFUNCTION)
    {
        //参数压栈
        lua_pushstring(L,fc);
        lua_pushstring(L,fullPath);
        //函数调用
        if(LuaCommon::callLuaFunction(L,2,1))
        {
            return true;
        }
    }
    return false;
}
