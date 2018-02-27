#ifndef GCL_LUA_XML_H
#define GCL_LUA_XML_H

class GclLuaXml
{
public:
    static void registerLua();

public:
    GclLuaXml();

    //XML业务处理
    static bool dealBuffer(const char *fc, const char* pBuf, int len);

    //XML业务处理
    static bool dealFile(const char *fc, const char *fullPath);

};

#endif // GCL_LUA_XML_H
