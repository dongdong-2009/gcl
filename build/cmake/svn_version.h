#ifndef GCL_VERSION_H
#define GCL_VERSION_H


//编译版本号
#define VERSION_BUILD  "75"
//源码版本范围
#define REVRANGE_BUILD  "82"
//编译时SVN服务器时间
#define SERVER_DATETIME_BUILD "2016/04/12 19:25:06"
//编译时本地时间
#define LOCAL_DATETIME_BUILD "2016/04/15 11:13:43"
//是否为混合版本（1：混合版本；0：非混合版本）
#define IS_MIXED 0
//本地源码是否存在修改（1：存在修改；0：不存在修改），若存在表示本地与SVN服务器源码不一致，此时应禁止发布
#define IS_MODS 1
//源码在服务器上的存放目录
#define SVN_URL "platform_cpp"


#endif	/* GCL_VERSION_H */
