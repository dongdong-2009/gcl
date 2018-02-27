#ifndef STRAW_SHAREMEMORY_H
#define STRAW_SHAREMEMORY_H


#include "measure_sharememory.h"


#define ci_straw_length (128)


struct StrawValue
{
    char value[ci_straw_length];

    StrawValue()
    { memset(value, 0, ci_straw_length); }

    inline StrawValue(const char * pch)
    {
        CxStringC::set(value, ci_straw_length, pch);
//        size_t iSize = strlen(pch);
//        if (iSize < 128)
//        {
//            memcpy(value, pch, iSize);
//            value[iSize] = 0;
//        }
    }

    inline StrawValue &operator=(const char * pch)
    {
        CxStringC::set(value, ci_straw_length, pch);
//        size_t iSize = strlen(pch);
//        if (iSize < 128)
//        {
//            memcpy(value, pch, iSize);
//            value[iSize] = 0;
//        }
        return *this;
    }

    inline StrawValue(const std::string & s)
    {
//        CxStringC::set(value, 128, s.data(), s.size());
        size_t iSize = s.size();
        if (iSize < ci_straw_length)
        {
            memmove(value, s.data(), iSize);
            value[iSize] = 0;
        }
        else
        {
            memmove(value, s.data(), ci_straw_length-1);
            value[ci_straw_length-1] = 0;
        }
    }

    inline StrawValue &operator=(const std::string & s)
    {
        size_t iSize = s.size();
        if (iSize < ci_straw_length)
        {
            memmove(value, s.data(), iSize);
            value[iSize] = 0;
        }
        else
        {
            memmove(value, s.data(), ci_straw_length-1);
            value[ci_straw_length-1] = 0;
        }
        return *this;
    }



};

//*判断是否与 StrawValue 相等的
inline bool operator == (const StrawValue & sv1, const StrawValue & sv2) {
    size_t iSize = strlen(sv1.value);
    return (iSize == strlen(sv2.value)) && (memcmp(sv1.value, sv2.value, iSize) == 0);
}

inline bool operator == (const char * pch, const StrawValue & sv) {
    size_t iSize = strlen(pch);
    return (iSize == strlen(sv.value)) && (memcmp(pch, sv.value, iSize) == 0);
}

inline bool operator == (const StrawValue & sv, const char * pch) {
    size_t iSize = strlen(pch);
    return (iSize == strlen(sv.value)) && (memcmp(pch, sv.value, iSize) == 0);
}

inline bool operator == (const std::string & s, const StrawValue & sv) {
    size_t iSize = s.size();
    return (iSize == strlen(sv.value)) && (memcmp(s.data(), sv.value, iSize) == 0);
}

inline bool operator == (const StrawValue & sv, const std::string & s) {
    size_t iSize = s.size();
    return (iSize == strlen(sv.value)) && (memcmp(s.data(), sv.value, iSize) == 0);
}

//*判断是否与 StrawValue 不相等的
inline bool operator != (const StrawValue & sv1, const StrawValue & sv2) {
    return !(sv1 == sv2);
}

inline bool operator != (const char * pch, const StrawValue & sv) {
    return !(pch == sv);
}

inline bool operator != (const StrawValue & sv, const char * pch) {
    return !(sv == pch);
}

inline bool operator != (const std::string & s, const StrawValue & sv) {
    return !(s == sv);
}

inline bool operator != (const StrawValue & sv, const std::string & s) {
    return !(sv == s);
}


static StrawValue cs_strawvalue_empty;

class StrawShareMemory : public MeasureShareMemory<StrawValue>
{
public:
    class MemoryManager : public MemoryManagerBase
    {
    public:

    protected:
        inline int getMeasureTypeImpl() {
            return 0x03000000;
        }

        int loadConfigImpl(MeasureShareMemory<StrawValue>::ConfigData * oConfigBegin, const MeasureShareMemory<StrawValue>::ConfigData * oConfigEnd);

        inline StrawValue getOriginalValueImpl(int iMeasureId = -1) {
            return cs_strawvalue_empty;
        }

    };

    static MemoryManager * memoryManager();

};

typedef StrawShareMemory::MemoryManager StrawMemoryManager;
typedef StrawShareMemory::ChangedData StrawChangedData;
typedef StrawShareMemory::ControlData StrawControlData;
typedef StrawShareMemory::MeasureData StrawData;

#endif // STRAW_SHAREMEMORY_H
