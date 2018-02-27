#ifndef MEASURE_BASE_H
#define MEASURE_BASE_H


#include <ccxx/cxprofile.h>
#include <ccxx/cxdescribe.h>
#include <common/gcl_resource.h>
#include <common/gcl_config.h>
#include <common/measure_strings.h>
#include "vdi_declspec.h"


#define GC_MEASURE_ERROR  -1


class GM_GCL_VDI_API MeasureBase
{
public:
    MeasureBase();

    ~MeasureBase();

    virtual const std::string& typeSimpleName() const = 0;

    virtual MeasureTypeEnum measureType() const = 0;

    virtual int measureSonType() const = 0;

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    virtual std::string reportSelf();

    inline const int& measureId() const { return _measureId; }
    inline void setMeasureId(int value) { _measureId = value; }

    inline const std::string& measureName() const { return _measureName; }
    inline void setMeasureName(const std::string& value) { _measureName = value; }

//    inline std::string measureNameThread() const { return _measureNameThread; }

//    inline const char * measureNameThread() const { return _measureNameThread; }

    inline const std::string& url() const { return _url; }
    inline void setUrl(const std::string& value) { _url = value; }

    inline const std::string& property() const { return _property; }
    inline void setProperty(const std::string& value) { _property = value; }

    inline const int& containerId() const { return _containerId; }
    inline void setContainerId(int value) { _containerId = value; }

    inline const int& containerType() const { return _containerType; }
    inline void setContainerType(int value) { _containerType = value; }

    //串行化
    int fromBuffer(uchar *buffer,int len);
    int toBuffer(uchar *buffer,int len);
    inline int size(){return sizeof(int)*2;}

    inline const int &measurePos()const{return _measurePos;}
    inline void setMeasurePos(int value) { _measurePos = value; }

protected :
    virtual void toReport(std::vector<std::string>& sReports) const;

private:
    int _measureId;
    int _measurePos;//相对偏移量
    int _containerId;
    int _containerType;
    std::string _measureName;
    std::string _url;
    std::string _property;
    std::map<std::string, std::string> _attributes;

};
#endif // !defined(MEASURE_BASE_H)
