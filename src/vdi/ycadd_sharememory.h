#ifndef YCADD_SHAREMEMORY_H
#define YCADD_SHAREMEMORY_H


#include "measure_sharememory.h"


class YcaddShareMemory : public MeasureShareMemory<double>
{
public:
    class MemoryManager : public MemoryManagerBase
    {
    public:

    protected:
        inline int getMeasureTypeImpl() {
            return 0x02000000;
        }

        int loadConfigImpl(MeasureShareMemory<double>::ConfigData * oConfigBegin, const MeasureShareMemory<double>::ConfigData * oConfigEnd);

        inline double getOriginalValueImpl(int iMeasureId = -1) {
            return -1;
        }

        inline bool equalValueImpl(const double & tOld, const double & tNew, const MeasureShareMemory<double>::ConfigData * oConfig) {
            if (oConfig->equalStrategyId == 2)
            {
                return fabs(tNew-tOld) < oConfig->value1;
            }
            else
            {
                if (tOld == tNew)
                {
                    return true;
                }
                else if (tOld != 0)
                {
//                    return fabs(tNew-tOld) < fabs(tOld*0.01);
                     return fabs(tNew-tOld) < 0.01;
                }
                else
                {
//                    return fabs(tNew) < fabs(tNew*0.01);
                    return fabs(tNew) < 0.01;
                }
            }
        }

    };

    static MemoryManager * memoryManager();

};

typedef YcaddShareMemory::MemoryManager YcaddMemoryManager;
typedef YcaddShareMemory::ChangedData YcaddChangedData;
typedef YcaddShareMemory::ControlData YcaddControlData;
typedef YcaddShareMemory::MeasureData YcaddData;


#endif // YCADD_SHAREMEMORY_H
