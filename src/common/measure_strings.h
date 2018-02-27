#ifndef MEASURE_STRINGS_H
#define MEASURE_STRINGS_H


#include <string>
#include <vector>


#define GM_MEASURE_NONE (0x00000000)
#define GM_MEASURE_MONSB (0x01000000)
#define GM_MEASURE_YCADD (0x02000000)
#define GM_MEASURE_STRAW (0x03000000)
#define GM_MEASURE_CALC (0x05000000)

#define GM_MEASURE_MONSB_END (0x02000000)
#define GM_MEASURE_YCADD_END (0x03000000)
#define GM_MEASURE_STRAW_END (0x04000000)
#define GM_MEASURE_CALC_END (0x06000000)

enum MeasureTypeEnum
{
    MeasureType_None = GM_MEASURE_NONE,
    MeasureType_Monsb = GM_MEASURE_MONSB,
    MeasureType_Ycadd = GM_MEASURE_YCADD,
    MeasureType_Straw = GM_MEASURE_STRAW,
    MeasureType_Calc = GM_MEASURE_CALC
};

const int ci_minMeasureIdMonsb = GM_MEASURE_MONSB;
const int ci_maxMeasureIdMonsb = (GM_MEASURE_YCADD - 1);

const int ci_minMeasureIdYcadd = GM_MEASURE_YCADD;
const int ci_maxMeasureIdYcadd = (GM_MEASURE_STRAW - 1);

const int ci_minMeasureIdStraw = GM_MEASURE_STRAW;
const int ci_maxMeasureIdStraw = (GM_MEASURE_STRAW_END - 1);

const int ci_minMeasureIdCalc = GM_MEASURE_CALC;
const int ci_maxMeasureIdCalc = (GM_MEASURE_CALC_END - 1);


const int ci_defaultMeasureContainerId = 0;
const int ci_defaultMeasureContainerType = 0;

const int ci_defaultLengthStrawMeasure = 128;


const std::string CS_MeasureTypeNames[] =
{
    "None",
    "Monsb",
    "Ycadd",
    "Straw",
    "Calc"
};

const std::string CS_MonsbTypeNames[] =
{
    "AllMonsb",
    "RtuState",
    "KeyState",
    "KbqState",
    "DixianState",
    "DixianLocation",
    "WfOperation"
};

const std::string CS_YcaddTypeNames[] =
{
    "AllYcadd",
    "RtuValue",
    "DixianPlace",
    "PdaPlace"
};

const std::string CS_StrawTypeNames[] =
{
    "AllStraw",
    "RtuText"
};

const std::string CS_CalcTypeNames[] =
{
    "AllCalc",
    "RtuValue",
    "DixianPlace",
    "PdaPlace"
};

inline int g_fn_getMeasureType(int iMeasureId) {
    if (iMeasureId >= MeasureType_Monsb && iMeasureId < MeasureType_Ycadd) {
        return MeasureType_Monsb;
    }
    else if (iMeasureId >= MeasureType_Ycadd && iMeasureId < MeasureType_Straw) {
        return MeasureType_Ycadd;
    }
    else if (iMeasureId >= MeasureType_Straw && iMeasureId < 0x04000000) {
        return MeasureType_Straw;
    }
    else if (iMeasureId >= MeasureType_Calc && iMeasureId < 0x06000000) {
        return MeasureType_Calc;
    }
    return MeasureType_None;
}

inline int g_fn_getMeasureType(const std::string & sMeasureTypeName) {
    if (sMeasureTypeName == CS_MeasureTypeNames[1]) {
        return MeasureType_Monsb;
    }
    else if (sMeasureTypeName == CS_MeasureTypeNames[2]) {
        return MeasureType_Ycadd;
    }
    else if (sMeasureTypeName == CS_MeasureTypeNames[3]) {
        return MeasureType_Straw;
    }
    else if (sMeasureTypeName == CS_MeasureTypeNames[4]) {
        return MeasureType_Calc;
    }
    return MeasureType_None;

}

inline const std::string & g_fn_getMeasureTypeName(int iMeasure) {
    if (iMeasure >= GM_MEASURE_MONSB && iMeasure < GM_MEASURE_MONSB_END) {
        return CS_MeasureTypeNames[1];
    }
    else if (iMeasure >= GM_MEASURE_YCADD && iMeasure < GM_MEASURE_YCADD_END) {
        return CS_MeasureTypeNames[2];
    }
    else if (iMeasure >= GM_MEASURE_STRAW && iMeasure < GM_MEASURE_STRAW_END) {
        return CS_MeasureTypeNames[3];
    }
    else if (iMeasure >= GM_MEASURE_CALC && iMeasure < GM_MEASURE_CALC_END) {
        return CS_MeasureTypeNames[4];
    }
    return CS_MeasureTypeNames[0];
}

inline int g_fn_getMinMeasureId(int iMeasureType) {
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        return ci_minMeasureIdMonsb;
        break;
    case MeasureType_Ycadd:
        return ci_minMeasureIdYcadd;
        break;
    case MeasureType_Straw:
        return ci_minMeasureIdStraw;
        break;
    case MeasureType_Calc:
        return ci_minMeasureIdCalc;
        break;
    default:
        return GM_MEASURE_MONSB;
        break;
    }
}


inline int g_fn_getMaxMeasureId(int iMeasureType) {
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        return ci_maxMeasureIdMonsb;
        break;
    case MeasureType_Ycadd:
        return ci_maxMeasureIdYcadd;
        break;
    case MeasureType_Straw:
        return ci_maxMeasureIdStraw;
        break;
    case MeasureType_Calc:
        return ci_maxMeasureIdCalc;
        break;
    default:
        return GM_MEASURE_MONSB;
        break;
    }
}

inline bool g_fn_isValidMeasureId(int iMeasureType, int iMeasureId) {
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        return iMeasureId >= ci_minMeasureIdMonsb && iMeasureId < GM_MEASURE_YCADD;
        break;
    case MeasureType_Ycadd:
        return iMeasureId >= ci_minMeasureIdYcadd && iMeasureId < GM_MEASURE_STRAW;;
        break;
    case MeasureType_Straw:
        return iMeasureId >= GM_MEASURE_STRAW && iMeasureId < GM_MEASURE_STRAW_END;;
        break;
    case MeasureType_Calc:
        return iMeasureId >= GM_MEASURE_CALC && iMeasureId < GM_MEASURE_CALC_END;;
        break;
    default:
        return false;
        break;
    }
}



///*** *** *** *** *** *** *** *** *** *** *** *** ***
///attention new monsb type to add every here begin
///*** *** *** *** *** *** *** *** *** *** *** *** ***
enum MonsbTypeEnum
{
    MonsbType_0,
    MonsbType_rtu_state,
    MonsbType_key_state,
    MonsbType_kbq_state,
    MonsbType_dixian_state,
    MonsbType_dixian_location,
    MonsbType_wf_operation
};

const int ce_maxMonsbType = MonsbType_wf_operation;

const int CIRtuState_He      = 0x01;
const int CIRtuState_Fen     = 0x00;
const int CIRtuState_Invalid = -1;

inline std::vector<int> g_fn_monsbTypeValues()
{
    std::vector<int> monsbTypeValues;
    for (int i = 0; i < ce_maxMonsbType; ++i)
    {
        monsbTypeValues.push_back(i);
    }
    return monsbTypeValues;
}

const std::vector<int> ci_monsbTypeValues = g_fn_monsbTypeValues();

inline const std::string& g_fn_monsbTypeName(MonsbTypeEnum eMonsbType) { return CS_MonsbTypeNames[eMonsbType]; }

inline MonsbTypeEnum g_fn_monsbTypeEnum(const std::string& sMonsbTypeName)
{
    for (size_t i = 0; i < sizeof(CS_MonsbTypeNames)/sizeof(CS_MonsbTypeNames[0]); ++i)
    {
        if (sMonsbTypeName == CS_MonsbTypeNames[i])
            return (MonsbTypeEnum)i;
    }
    return MonsbType_0;
}

inline MonsbTypeEnum g_fn_monsbTypeEnum(int iMonsbType)
{
    if (iMonsbType >= MonsbType_0 && iMonsbType <= ce_maxMonsbType)
    {
        return (MonsbTypeEnum)iMonsbType;
    }
    else
    {
        return MonsbType_0;
    }
}
///--- --- --- --- --- --- --- --- --- --- --- --- ---
///attention new monsb type end
///--- --- --- --- --- --- --- --- --- --- --- --- ---


///*** *** *** *** *** *** *** *** *** *** *** *** ***
///attention new ycadd type to add every here begin
///*** *** *** *** *** *** *** *** *** *** *** *** ***
enum YcaddTypeEnum {
    YcaddType_0,
    YcaddType_rtu_value,
    YcaddType_Dixian_Place,
    YcaddType_Pda_Place
};

//Warning：向YcaddTypeEnum增加表时，记得修改ce_maxYcaddType！
const YcaddTypeEnum ce_maxYcaddType = YcaddType_Pda_Place;

inline std::vector<int> g_fn_ycaddTypeValues()
{
    std::vector<int> YcaddTypeValues;
    for (int i = 0; i < ce_maxYcaddType; ++i)
    {
        YcaddTypeValues.push_back(i);
    }
    return YcaddTypeValues;
}

const std::vector<int> ci_ycaddTypeValues = g_fn_ycaddTypeValues();

inline const std::string& g_fn_ycaddTypeName(YcaddTypeEnum eYcaddType)
{
    return CS_YcaddTypeNames[eYcaddType];
}

inline YcaddTypeEnum g_fn_ycaddTypeEnum(const std::string& sYcaddTypeName)
{
    for (size_t i = 0; i < sizeof(CS_YcaddTypeNames)/sizeof(CS_YcaddTypeNames[0]); ++i)
    {
        if (sYcaddTypeName == CS_YcaddTypeNames[i])
            return (YcaddTypeEnum)i;
    }
    return YcaddType_0;
}

inline YcaddTypeEnum g_fn_ycaddTypeEnum(int iYcaddType)
{
    if (iYcaddType >= YcaddType_0 && iYcaddType <= ce_maxYcaddType)
        return (YcaddTypeEnum)iYcaddType;
    else
        return YcaddType_0;
}
///--- --- --- --- --- --- --- --- --- --- --- --- ---
///attention new ycadd type end
///--- --- --- --- --- --- --- --- --- --- --- --- ---


///*** *** *** *** *** *** *** *** *** *** *** *** ***
///attention new straw type to add every here begin
///*** *** *** *** *** *** *** *** *** *** *** *** ***
enum StrawTypeEnum {
    StrawType_0,
    StrawType_rtu_value
};

//Warning：向StrawTypeEnum增加表时，记得修改ce_maxStrawType！
const StrawTypeEnum ce_maxStrawType = StrawType_rtu_value;

inline std::vector<int> g_fn_strawTypeValues()
{
    std::vector<int> StrawTypeValues;
    for (int i = 0; i < ce_maxStrawType; ++i)
    {
        StrawTypeValues.push_back(i);
    }
    return StrawTypeValues;
}

const std::vector<int> ci_strawTypeValues = g_fn_strawTypeValues();

inline const std::string& g_fn_strawTypeName(StrawTypeEnum eStrawType)
{
    return CS_StrawTypeNames[eStrawType];
}

inline StrawTypeEnum g_fn_strawTypeEnum(const std::string& sStrawTypeName)
{
    for (size_t i = 0; i < sizeof(CS_StrawTypeNames)/sizeof(CS_StrawTypeNames[0]); ++i)
    {
        if (sStrawTypeName == CS_StrawTypeNames[i])
            return (StrawTypeEnum)i;
    }
    return StrawType_0;
}

inline StrawTypeEnum g_fn_strawTypeEnum(int iStrawType)
{
    if (iStrawType >= StrawType_0 && iStrawType <= ce_maxStrawType)
        return (StrawTypeEnum)iStrawType;
    else
        return StrawType_0;
}
///--- --- --- --- --- --- --- --- --- --- --- --- ---
///attention new straw type end
///--- --- --- --- --- --- --- --- --- --- --- --- ---


///*** *** *** *** *** *** *** *** *** *** *** *** ***
///attention new calc type to add every here begin
///*** *** *** *** *** *** *** *** *** *** *** *** ***
enum CalcTypeEnum {
    CalcType_0,
    CalcType_rtu_value,
    CalcType_Dixian_Place,
    CalcType_Pda_Place
};

//Warning：向CalcTypeEnum增加表时，记得修改ce_maxCalcType！
const CalcTypeEnum ce_maxCalcType = CalcType_Pda_Place;

inline std::vector<int> g_fn_calcTypeValues()
{
    std::vector<int> CalcTypeValues;
    for (int i = 0; i < ce_maxCalcType; ++i)
    {
        CalcTypeValues.push_back(i);
    }
    return CalcTypeValues;
}

const std::vector<int> ci_calcTypeValues = g_fn_calcTypeValues();

inline const std::string& g_fn_calcTypeName(CalcTypeEnum eCalcType)
{
    return CS_CalcTypeNames[eCalcType];
}

inline CalcTypeEnum g_fn_calcTypeEnum(const std::string& sCalcTypeName)
{
    for (size_t i = 0; i < sizeof(CS_CalcTypeNames)/sizeof(CS_CalcTypeNames[0]); ++i)
    {
        if (sCalcTypeName == CS_CalcTypeNames[i])
            return (CalcTypeEnum)i;
    }
    return CalcType_0;
}

inline CalcTypeEnum g_fn_calcTypeEnum(int iCalcType)
{
    if (iCalcType >= CalcType_0 && iCalcType <= ce_maxCalcType)
        return (CalcTypeEnum)iCalcType;
    else
        return CalcType_0;
}
///--- --- --- --- --- --- --- --- --- --- --- --- ---
///attention new calc type end
///--- --- --- --- --- --- --- --- --- --- --- --- ---

#endif // !defined(MEASURE_STRINGS_H)
