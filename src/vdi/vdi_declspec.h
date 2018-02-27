#ifndef VDI_DECLSPEC_H
#define VDI_DECLSPEC_H


#include <ccxx/cxglobal.h>


#if defined(GM_BUILD_GCL_VDI_LIB)
#define GM_GCL_VDI_API  GM_DECL_EXPORT
#elif defined(GM_BUILD_GCL_VDI_CODE)
#define GM_GCL_VDI_API
#else
#define GM_GCL_VDI_API  GM_DECL_IMPORT
// GM_BUILD_GCL_VDI_INCLUDE
#endif


#endif // VDI_DECLSPEC_H
