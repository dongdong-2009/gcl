#ifndef MONSB_SHAREMEMORY_H
#define MONSB_SHAREMEMORY_H


#include "measure_sharememory.h"


class MonsbShareMemory : public MeasureShareMemory<int>
{
public:
    class MemoryManager : public MemoryManagerBase
    {
    public:

    protected:
        inline int getMeasureTypeImpl() {
            return 0x01000000;
        }

        int loadConfigImpl(MeasureShareMemory<int>::ConfigData * oConfigBegin, const MeasureShareMemory<int>::ConfigData * oConfigEnd);

        inline int getOriginalValueImpl(int iMeasureId = -1) {
            return -1;
        }

    };

    static MemoryManager * memoryManager();

};

typedef MonsbShareMemory::MemoryManager MonsbMemoryManager;
typedef MonsbShareMemory::ChangedData MonsbChangedData;
typedef MonsbShareMemory::ControlData MonsbControlData;
typedef MonsbShareMemory::MeasureData MonsbData;


#endif // MONSB_SHAREMEMORY_H
