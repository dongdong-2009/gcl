#ifndef GCL_STRUCT_H
#define GCL_STRUCT_H

#include <ccxx/ccxx.h>
#include <common/gcl_macro.h>

typedef struct stu_lua_value
{
    int q;
    int id;
    int type;
    int len;
    long long t;
    int src;

    int iValue;
    double dValue;
    std::string sValue;

    stu_lua_value()
    {
        clear();
    }
    void clear()
    {
        q      =0;
        id     = 0;
        type   = 0;
        len    = 0;
        iValue = 0;
        sValue = "";
        src    = 0;
        t = 0;
    }
    std::string toString(std::string fmt="")
    {
        std::string v="";
        if(type==GCL_VT_YC)
        {
            if(fmt.length()>0)
                v = CxString::format(fmt.c_str(),dValue);
            else
                v = CxString::format("%.2f",dValue);
        }
        else if(type==GCL_VT_YX)
        {
            if(fmt.length()>0)
                v = CxString::format(fmt.c_str(),iValue);
            else
                v = CxString::format("%d",iValue);
        }
        else if(type==GCL_VT_STR)
        {
             v = sValue;
        }
        else v = "";
        return v;
    }




}gcl_stu_lua_value;


#endif // GCL_STRUCT_H
