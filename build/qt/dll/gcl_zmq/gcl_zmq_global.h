#ifndef GCL_ZMQ_GLOBAL_H
#define GCL_ZMQ_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCL_ZMQ_LIBRARY)
#  define GCL_ZMQSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GCL_ZMQSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GCL_ZMQ_GLOBAL_H
