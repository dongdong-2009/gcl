
#ifndef GCL_LPEG_GLOBAL_H
#define GCL_LPEG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCL_LPEG_LIBRARY)
#  define GCL_LPEGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GCL_LPEGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GCL_LPEG_GLOBAL_H
