
#ifndef GCL_LFS_GLOBAL_H
#define GCL_LFS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCL_LFS_LIBRARY)
#  define GCL_LFSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GCL_LFSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GCL_LFS_GLOBAL_H
