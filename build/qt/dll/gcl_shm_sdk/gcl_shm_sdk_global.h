#ifndef GCL_SHM_SDK_GLOBAL_H
#define GCL_SHM_SDK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCL_SHM_SDK_LIBRARY)
#  define GCL_SHM_SDKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GCL_SHM_SDKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GCL_SHM_SDK_GLOBAL_H
