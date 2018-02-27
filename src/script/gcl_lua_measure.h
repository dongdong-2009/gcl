#ifndef GCL_LUA_MEASURE_H
#define GCL_LUA_MEASURE_H

#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>

class GclLuaMeasure
{
public:
    static void registerLua();

public:
    GclLuaMeasure();
};

#endif // GCL_LUA_MEASURE_H
