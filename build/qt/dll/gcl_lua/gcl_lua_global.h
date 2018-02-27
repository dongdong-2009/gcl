#ifndef GCL_LUA_GLOBAL_H
#define GCL_LUA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCL_LUA_LIBRARY)
#  define GCL_LUASHARED_EXPORT Q_DECL_EXPORT
#else
#  define GCL_LUASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GCL_LUA_GLOBAL_H
