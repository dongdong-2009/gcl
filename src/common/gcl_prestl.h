#ifndef GCL_PRESTL
#define GCL_PRESTL

#include <ccxx/cxglobal.h>

typedef msepoch_t msepoch;
typedef std::string   GString;

typedef std::vector<GString>     GStringVector;
typedef std::vector<char>       GCharVector;
typedef std::vector<byte>       GByteVector;
typedef std::vector<uchar>      GUcharVector;
typedef std::vector<short>      GShortVector;
typedef std::vector<ushort>     GUShortVector;
typedef std::vector<int>        GIntVector;
typedef std::vector<uint>       GUintVector;
typedef std::vector<int64>      GInt64Vector;
typedef std::vector<uint64>     GUnit64Vector;
typedef std::vector<msepoch>    GMsepochVector;
typedef std::vector<float>      GFloatVector;
typedef std::vector<double>     GDoubleVector;


typedef std::list<GString>       GStringList;
typedef std::list<char>         GCharList;
typedef std::list<byte>         GByteList;
typedef std::list<uchar>        GUcharList;
typedef std::list<short>        GShortList;
typedef std::list<ushort>       GUShortList;
typedef std::list<int>          GIntList;
typedef std::list<uint>         GUintList;
typedef std::list<int64>        GInt64List;
typedef std::list<uint64>       GUnit64List;
typedef std::list<msepoch>      GMsepochList;
typedef std::list<float>        GFloatList;
typedef std::list<double>       GDoubleList;


typedef std::map<double, GString>         GDoubleStringMap;
typedef std::pair<double, GString>        GDoubleStringPair;
typedef std::map<double, char>           GDoubleCharMap;
typedef std::pair<double, char>          GDoubleCharPair;
typedef std::map<double, byte>           GDoubleByteMap;
typedef std::pair<double, byte>          GDoubleBytePair;
typedef std::map<double, uchar>          GDoubleUcharMap;
typedef std::pair<double, uchar>         GDoubleUcharPair;
typedef std::map<double, short>          GDoubleShortMap;
typedef std::pair<double, short>         GDoubleShortPair;
typedef std::map<double, ushort>         GDoubleUshortMap;
typedef std::pair<double, ushort>        GDoubleUshortPair;
typedef std::map<double, int>            GDoubleIntMap;
typedef std::pair<double, int>           GDoubleIntPair;
typedef std::map<double, uint>           GDoubleUintMap;
typedef std::pair<double, uint>          GDoubleUintPair;
typedef std::map<double, int64>          GDoubleInt64Map;
typedef std::pair<double, int64>         GDoubleInt64Pair;
typedef std::map<double, uint64>         GDoubleUint64Map;
typedef std::pair<double, uint64>        GDoubleUint64Pair;
typedef std::map<double, msepoch>        GDoubleMsepochMap;
typedef std::pair<double, msepoch>       GDoubleMsepochPair;
typedef std::map<double, float>          GDoubleFloatMap;
typedef std::pair<double, float>         GDoubleFloatPair;
typedef std::map<double, double>         GDoubleDoubleMap;
typedef std::pair<double, double>        GDoubleDoublePair;


typedef std::map<float, GString>         GFloatStringMap;
typedef std::pair<float, GString>        GFloatStringPair;
typedef std::map<float, char>           GFloatCharMap;
typedef std::pair<float, char>          GFloatCharPair;
typedef std::map<float, byte>           GFloatByteMap;
typedef std::pair<float, byte>          GFloatBytePair;
typedef std::map<float, uchar>          GFloatUcharMap;
typedef std::pair<float, uchar>         GFloatUcharPair;
typedef std::map<float, short>          GFloatShortMap;
typedef std::pair<float, short>         GFloatShortPair;
typedef std::map<float, ushort>         GFloatUshortMap;
typedef std::pair<float, ushort>        GFloatUshortPair;
typedef std::map<float, int>            GFloatIntMap;
typedef std::pair<float, int>           GFloatIntPair;
typedef std::map<float, uint>           GFloatUintMap;
typedef std::pair<float, uint>          GFloatUintPair;
typedef std::map<float, int64>          GFloatInt64Map;
typedef std::pair<float, int64>         GFloatInt64Pair;
typedef std::map<float, uint64>         GFloatUint64Map;
typedef std::pair<float, uint64>        GFloatUint64Pair;
typedef std::map<float, msepoch>        GFloatMsepochMap;
typedef std::pair<float, msepoch>       GFloatMsepochPair;
typedef std::map<float, float>          GFloatFloatMap;
typedef std::pair<float, float>         GFloatFloatPair;
typedef std::map<float, double>         GFloatDoubleMap;
typedef std::pair<float, double>        GFloatDoublePair;


typedef std::map<msepoch, GString>         GMsepochStringMap;
typedef std::pair<msepoch, GString>        GMsepochStringPair;
typedef std::map<msepoch, char>           GMsepochCharMap;
typedef std::pair<msepoch, char>          GMsepochCharPair;
typedef std::map<msepoch, byte>           GMsepochByteMap;
typedef std::pair<msepoch, byte>          GMsepochBytePair;
typedef std::map<msepoch, uchar>          GMsepochUcharMap;
typedef std::pair<msepoch, uchar>         GMsepochUcharPair;
typedef std::map<msepoch, short>          GMsepochShortMap;
typedef std::pair<msepoch, short>         GMsepochShortPair;
typedef std::map<msepoch, ushort>         GMsepochUshortMap;
typedef std::pair<msepoch, ushort>        GMsepochUshortPair;
typedef std::map<msepoch, int>            GMsepochIntMap;
typedef std::pair<msepoch, int>           GMsepochIntPair;
typedef std::map<msepoch, uint>           GMsepochUintMap;
typedef std::pair<msepoch, uint>          GMsepochUintPair;
typedef std::map<msepoch, int64>          GMsepochInt64Map;
typedef std::pair<msepoch, int64>         GMsepochInt64Pair;
typedef std::map<msepoch, uint64>         GMsepochUint64Map;
typedef std::pair<msepoch, uint64>        GMsepochUint64Pair;
typedef std::map<msepoch, msepoch>        GMsepochMsepochMap;
typedef std::pair<msepoch, msepoch>       GMsepochMsepochPair;
typedef std::map<msepoch, float>          GMsepochFloatMap;
typedef std::pair<msepoch, float>         GMsepochFloatPair;
typedef std::map<msepoch, double>         GMsepochDoubleMap;
typedef std::pair<msepoch, double>        GMsepochDoublePair;


typedef std::map<uint64, GString>         GUint64StringMap;
typedef std::pair<uint64, GString>        GUint64StringPair;
typedef std::map<uint64, char>           GUint64CharMap;
typedef std::pair<uint64, char>          GUint64CharPair;
typedef std::map<uint64, byte>           GUint64ByteMap;
typedef std::pair<uint64, byte>          GUint64BytePair;
typedef std::map<uint64, uchar>          GUint64UcharMap;
typedef std::pair<uint64, uchar>         GUint64UcharPair;
typedef std::map<uint64, short>          GUint64ShortMap;
typedef std::pair<uint64, short>         GUint64ShortPair;
typedef std::map<uint64, ushort>         GUint64UshortMap;
typedef std::pair<uint64, ushort>        GUint64UshortPair;
typedef std::map<uint64, int>            GUint64IntMap;
typedef std::pair<uint64, int>           GUint64IntPair;
typedef std::map<uint64, uint>           GUint64UintMap;
typedef std::pair<uint64, uint>          GUint64UintPair;
typedef std::map<uint64, int64>          GUint64Int64Map;
typedef std::pair<uint64, int64>         GUint64Int64Pair;
typedef std::map<uint64, uint64>         GUint64Uint64Map;
typedef std::pair<uint64, uint64>        GUint64Uint64Pair;
typedef std::map<uint64, msepoch>        GUint64MsepochMap;
typedef std::pair<uint64, msepoch>       GUint64MsepochPair;
typedef std::map<uint64, float>          GUint64FloatMap;
typedef std::pair<uint64, float>         GUint64FloatPair;
typedef std::map<uint64, double>         GUint64DoubleMap;
typedef std::pair<uint64, double>        GUint64DoublePair;


typedef std::map<int64, GString>         GInt64StringMap;
typedef std::pair<int64, GString>        GInt64StringPair;
typedef std::map<int64, char>           GInt64CharMap;
typedef std::pair<int64, char>          GInt64CharPair;
typedef std::map<int64, byte>           GInt64ByteMap;
typedef std::pair<int64, byte>          GInt64BytePair;
typedef std::map<int64, uchar>          GInt64UcharMap;
typedef std::pair<int64, uchar>         GInt64UcharPair;
typedef std::map<int64, short>          GInt64ShortMap;
typedef std::pair<int64, short>         GInt64ShortPair;
typedef std::map<int64, ushort>         GInt64UshortMap;
typedef std::pair<int64, ushort>        GInt64UshortPair;
typedef std::map<int64, int>            GInt64IntMap;
typedef std::pair<int64, int>           GInt64IntPair;
typedef std::map<int64, uint>           GInt64UintMap;
typedef std::pair<int64, uint>          GInt64UintPair;
typedef std::map<int64, int64>          GInt64Int64Map;
typedef std::pair<int64, int64>         GInt64Int64Pair;
typedef std::map<int64, uint64>         GInt64Uint64Map;
typedef std::pair<int64, uint64>        GInt64Uint64Pair;
typedef std::map<int64, msepoch>        GInt64MsepochMap;
typedef std::pair<int64, msepoch>       GInt64MsepochPair;
typedef std::map<int64, float>          GInt64FloatMap;
typedef std::pair<int64, float>         GInt64FloatPair;
typedef std::map<int64, double>         GInt64DoubleMap;
typedef std::pair<int64, double>        GInt64DoublePair;


typedef std::map<uint, GString>         GUintStringMap;
typedef std::pair<uint, GString>        GUintStringPair;
typedef std::map<uint, char>           GUintCharMap;
typedef std::pair<uint, char>          GUintCharPair;
typedef std::map<uint, byte>           GUintByteMap;
typedef std::pair<uint, byte>          GUintBytePair;
typedef std::map<uint, uchar>          GUintUcharMap;
typedef std::pair<uint, uchar>         GUintUcharPair;
typedef std::map<uint, short>          GUintShortMap;
typedef std::pair<uint, short>         GUintShortPair;
typedef std::map<uint, ushort>         GUintUshortMap;
typedef std::pair<uint, ushort>        GUintUshortPair;
typedef std::map<uint, int>            GUintIntMap;
typedef std::pair<uint, int>           GUintIntPair;
typedef std::map<uint, uint>           GUintUintMap;
typedef std::pair<uint, uint>          GUintUintPair;
typedef std::map<uint, int64>          GUintInt64Map;
typedef std::pair<uint, int64>         GUintInt64Pair;
typedef std::map<uint, uint64>         GUintUint64Map;
typedef std::pair<uint, uint64>        GUintUint64Pair;
typedef std::map<uint, msepoch>        GUintMsepochMap;
typedef std::pair<uint, msepoch>       GUintMsepochPair;
typedef std::map<uint, float>          GUintFloatMap;
typedef std::pair<uint, float>         GUintFloatPair;
typedef std::map<uint, double>         GUintDoubleMap;
typedef std::pair<uint, double>        GUintDoublePair;


typedef std::map<int, GString>         GIntStringMap;
typedef std::pair<int, GString>        GIntStringPair;
typedef std::map<int, char>           GIntCharMap;
typedef std::pair<int, char>          GIntCharPair;
typedef std::map<int, byte>           GIntByteMap;
typedef std::pair<int, byte>          GIntBytePair;
typedef std::map<int, uchar>          GIntUcharMap;
typedef std::pair<int, uchar>         GIntUcharPair;
typedef std::map<int, short>          GIntShortMap;
typedef std::pair<int, short>         GIntShortPair;
typedef std::map<int, ushort>         GIntUshortMap;
typedef std::pair<int, ushort>        GIntUshortPair;
typedef std::map<int, int>            GIntIntMap;
typedef std::pair<int, int>           GIntIntPair;
typedef std::map<int, uint>           GIntUintMap;
typedef std::pair<int, uint>          GIntUintPair;
typedef std::map<int, int64>          GIntInt64Map;
typedef std::pair<int, int64>         GIntInt64Pair;
typedef std::map<int, uint64>         GIntUint64Map;
typedef std::pair<int, uint64>        GIntUint64Pair;
typedef std::map<int, msepoch>        GIntMsepochMap;
typedef std::pair<int, msepoch>       GIntMsepochPair;
typedef std::map<int, float>          GIntFloatMap;
typedef std::pair<int, float>         GIntFloatPair;
typedef std::map<int, double>         GIntDoubleMap;
typedef std::pair<int, double>        GIntDoublePair;


typedef std::map<ushort, GString>         GUshortStringMap;
typedef std::pair<ushort, GString>        GUshortStringPair;
typedef std::map<ushort, char>           GUshortCharMap;
typedef std::pair<ushort, char>          GUshortCharPair;
typedef std::map<ushort, byte>           GUshortByteMap;
typedef std::pair<ushort, byte>          GUshortBytePair;
typedef std::map<ushort, uchar>          GUshortUcharMap;
typedef std::pair<ushort, uchar>         GUshortUcharPair;
typedef std::map<ushort, short>          GUshortShortMap;
typedef std::pair<ushort, short>         GUshortShortPair;
typedef std::map<ushort, ushort>         GUshortUshortMap;
typedef std::pair<ushort, ushort>        GUshortUshortPair;
typedef std::map<ushort, int>            GUshortIntMap;
typedef std::pair<ushort, int>           GUshortIntPair;
typedef std::map<ushort, uint>           GUshortUintMap;
typedef std::pair<ushort, uint>          GUshortUintPair;
typedef std::map<ushort, int64>          GUshortInt64Map;
typedef std::pair<ushort, int64>         GUshortInt64Pair;
typedef std::map<ushort, uint64>         GUshortUint64Map;
typedef std::pair<ushort, uint64>        GUshortUint64Pair;
typedef std::map<ushort, msepoch>        GUshortMsepochMap;
typedef std::pair<ushort, msepoch>       GUshortMsepochPair;
typedef std::map<ushort, float>          GUshortFloatMap;
typedef std::pair<ushort, float>         GUshortFloatPair;
typedef std::map<ushort, double>         GUshortDoubleMap;
typedef std::pair<ushort, double>        GUshortDoublePair;


typedef std::map<short, GString>         GShortStringMap;
typedef std::pair<short, GString>        GShortStringPair;
typedef std::map<short, char>           GShortCharMap;
typedef std::pair<short, char>          GShortCharPair;
typedef std::map<short, byte>           GShortByteMap;
typedef std::pair<short, byte>          GShortBytePair;
typedef std::map<short, uchar>          GShortUcharMap;
typedef std::pair<short, uchar>         GShortUcharPair;
typedef std::map<short, short>          GShortShortMap;
typedef std::pair<short, short>         GShortShortPair;
typedef std::map<short, ushort>         GShortUshortMap;
typedef std::pair<short, ushort>        GShortUshortPair;
typedef std::map<short, int>            GShortIntMap;
typedef std::pair<short, int>           GShortIntPair;
typedef std::map<short, uint>           GShortUintMap;
typedef std::pair<short, uint>          GShortUintPair;
typedef std::map<short, int64>          GShortInt64Map;
typedef std::pair<short, int64>         GShortInt64Pair;
typedef std::map<short, uint64>         GShortUint64Map;
typedef std::pair<short, uint64>        GShortUint64Pair;
typedef std::map<short, msepoch>        GShortMsepochMap;
typedef std::pair<short, msepoch>       GShortMsepochPair;
typedef std::map<short, float>          GShortFloatMap;
typedef std::pair<short, float>         GShortFloatPair;
typedef std::map<short, double>         GShortDoubleMap;
typedef std::pair<short, double>        GShortDoublePair;


typedef std::map<uchar, GString>         GUcharStringMap;
typedef std::pair<uchar, GString>        GUcharStringPair;
typedef std::map<uchar, char>           GUcharCharMap;
typedef std::pair<uchar, char>          GUcharCharPair;
typedef std::map<uchar, byte>           GUcharByteMap;
typedef std::pair<uchar, byte>          GUcharBytePair;
typedef std::map<uchar, uchar>          GUcharUcharMap;
typedef std::pair<uchar, uchar>         GUcharUcharPair;
typedef std::map<uchar, short>          GUcharShortMap;
typedef std::pair<uchar, short>         GUcharShortPair;
typedef std::map<uchar, ushort>         GUcharUshortMap;
typedef std::pair<uchar, ushort>        GUcharUshortPair;
typedef std::map<uchar, int>            GUcharIntMap;
typedef std::pair<uchar, int>           GUcharIntPair;
typedef std::map<uchar, uint>           GUcharUintMap;
typedef std::pair<uchar, uint>          GUcharUintPair;
typedef std::map<uchar, int64>          GUcharInt64Map;
typedef std::pair<uchar, int64>         GUcharInt64Pair;
typedef std::map<uchar, uint64>         GUcharUint64Map;
typedef std::pair<uchar, uint64>        GUcharUint64Pair;
typedef std::map<uchar, msepoch>        GUcharMsepochMap;
typedef std::pair<uchar, msepoch>       GUcharMsepochPair;
typedef std::map<uchar, float>          GUcharFloatMap;
typedef std::pair<uchar, float>         GUcharFloatPair;
typedef std::map<uchar, double>         GUcharDoubleMap;
typedef std::pair<uchar, double>        GUcharDoublePair;


typedef std::map<byte, GString>         GByteStringMap;
typedef std::pair<byte, GString>        GByteStringPair;
typedef std::map<byte, char>           GByteCharMap;
typedef std::pair<byte, char>          GByteCharPair;
typedef std::map<byte, byte>           GByteByteMap;
typedef std::pair<byte, byte>          GByteBytePair;
typedef std::map<byte, uchar>          GByteUcharMap;
typedef std::pair<byte, uchar>         GByteUcharPair;
typedef std::map<byte, short>          GByteShortMap;
typedef std::pair<byte, short>         GByteShortPair;
typedef std::map<byte, ushort>         GByteUshortMap;
typedef std::pair<byte, ushort>        GByteUshortPair;
typedef std::map<byte, int>            GByteIntMap;
typedef std::pair<byte, int>           GByteIntPair;
typedef std::map<byte, uint>           GByteUintMap;
typedef std::pair<byte, uint>          GByteUintPair;
typedef std::map<byte, int64>          GByteInt64Map;
typedef std::pair<byte, int64>         GByteInt64Pair;
typedef std::map<byte, uint64>         GByteUint64Map;
typedef std::pair<byte, uint64>        GByteUint64Pair;
typedef std::map<byte, msepoch>        GByteMsepochMap;
typedef std::pair<byte, msepoch>       GByteMsepochPair;
typedef std::map<byte, float>          GByteFloatMap;
typedef std::pair<byte, float>         GByteFloatPair;
typedef std::map<byte, double>         GByteDoubleMap;
typedef std::pair<byte, double>        GByteDoublePair;


typedef std::map<char, GString>         GCharStringMap;
typedef std::pair<char, GString>        GCharStringPair;
typedef std::map<char, char>           GCharCharMap;
typedef std::pair<char, char>          GCharCharPair;
typedef std::map<char, byte>           GCharByteMap;
typedef std::pair<char, byte>          GCharBytePair;
typedef std::map<char, uchar>          GCharUcharMap;
typedef std::pair<char, uchar>         GCharUcharPair;
typedef std::map<char, short>          GCharShortMap;
typedef std::pair<char, short>         GCharShortPair;
typedef std::map<char, ushort>         GCharUshortMap;
typedef std::pair<char, ushort>        GCharUshortPair;
typedef std::map<char, int>            GCharIntMap;
typedef std::pair<char, int>           GCharIntPair;
typedef std::map<char, uint>           GCharUintMap;
typedef std::pair<char, uint>          GCharUintPair;
typedef std::map<char, int64>          GCharInt64Map;
typedef std::pair<char, int64>         GCharInt64Pair;
typedef std::map<char, uint64>         GCharUint64Map;
typedef std::pair<char, uint64>        GCharUint64Pair;
typedef std::map<char, msepoch>        GCharMsepochMap;
typedef std::pair<char, msepoch>       GCharMsepochPair;
typedef std::map<char, float>          GCharFloatMap;
typedef std::pair<char, float>         GCharFloatPair;
typedef std::map<char, double>         GCharDoubleMap;
typedef std::pair<char, double>        GCharDoublePair;


typedef std::map<GString, GString>         GStringStringMap;
typedef std::pair<GString, GString>        GStringStringPair;
typedef std::map<GString, char>           GStringCharMap;
typedef std::pair<GString, char>          GStringCharPair;
typedef std::map<GString, byte>           GStringByteMap;
typedef std::pair<GString, byte>          GStringBytePair;
typedef std::map<GString, uchar>          GStringUcharMap;
typedef std::pair<GString, uchar>         GStringUcharPair;
typedef std::map<GString, short>          GStringShortMap;
typedef std::pair<GString, short>         GStringShortPair;
typedef std::map<GString, ushort>         GStringUshortMap;
typedef std::pair<GString, ushort>        GStringUshortPair;
typedef std::map<GString, int>            GStringIntMap;
typedef std::pair<GString, int>           GStringIntPair;
typedef std::map<GString, uint>           GStringUintMap;
typedef std::pair<GString, uint>          GStringUintPair;
typedef std::map<GString, int64>          GStringInt64Map;
typedef std::pair<GString, int64>         GStringInt64Pair;
typedef std::map<GString, uint64>         GStringUint64Map;
typedef std::pair<GString, uint64>        GStringUint64Pair;
typedef std::map<GString, msepoch>        GStringMsepochMap;
typedef std::pair<GString, msepoch>       GStringMsepochPair;
typedef std::map<GString, float>          GStringFloatMap;
typedef std::pair<GString, float>         GStringFloatPair;
typedef std::map<GString, double>         GStringDoubleMap;
typedef std::pair<GString, double>        GStringDoublePair;

//has useful to typedef new type
//typedef std::map<aaaaa, string>         GAaaaaStringMap;
//typedef std::pair<aaaaa, string>        GAaaaaStringPair;
//typedef std::map<aaaaa, char>           GAaaaaCharMap;
//typedef std::pair<aaaaa, char>          GAaaaaCharPair;
//typedef std::map<aaaaa, byte>           GAaaaaByteMap;
//typedef std::pair<aaaaa, byte>          GAaaaaBytePair;
//typedef std::map<aaaaa, uchar>          GAaaaaUcharMap;
//typedef std::pair<aaaaa, uchar>         GAaaaaUcharPair;
//typedef std::map<aaaaa, short>          GAaaaaShortMap;
//typedef std::pair<aaaaa, short>         GAaaaaShortPair;
//typedef std::map<aaaaa, ushort>         GAaaaaUshortMap;
//typedef std::pair<aaaaa, ushort>        GAaaaaUshortPair;
//typedef std::map<aaaaa, int>            GAaaaaIntMap;
//typedef std::pair<aaaaa, int>           GAaaaaIntPair;
//typedef std::map<aaaaa, uint>           GAaaaaUintMap;
//typedef std::pair<aaaaa, uint>          GAaaaaUintPair;
//typedef std::map<aaaaa, int64>          GAaaaaInt64Map;
//typedef std::pair<aaaaa, int64>         GAaaaaInt64Pair;
//typedef std::map<aaaaa, uint64>         GAaaaaUint64Map;
//typedef std::pair<aaaaa, uint64>        GAaaaaUint64Pair;
//typedef std::map<aaaaa, msepoch>        GAaaaaMsepochMap;
//typedef std::pair<aaaaa, msepoch>       GAaaaaMsepochPair;
//typedef std::map<aaaaa, float>          GAaaaaFloatMap;
//typedef std::pair<aaaaa, float>         GAaaaaFloatPair;
//typedef std::map<aaaaa, double>         GAaaaaDoubleMap;
//typedef std::pair<aaaaa, double>        GAaaaaDoublePair;


typedef std::size_t size_t;


static std::istream & gcin  = std::cin;
static std::ostream & gcout = std::cout;
static std::ostream & gcerr = std::cerr;
static std::ostream & gclog = std::clog;

typedef std::ostream & (*GFN_END)(std::ostream&);


static const GString CS_LineEnd("\r\n");


#ifndef GCL_TEST_CHECK
#define GCL_TEST_CHECK( p ) std::cout << #p << (p ? " = true" : " = false") << std::endl
#endif

#ifndef GCL_TEST_INFOLINE
#define GCL_TEST_INFOLINE( p ) std::cout << p << std::endl
#endif

#ifndef GCL_TEST_INFO
#define GCL_TEST_INFO( p ) std::cout << p
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4396)
#pragma warning(pop)
#endif // _MSC_VER

#endif // GCL_PRESTL
