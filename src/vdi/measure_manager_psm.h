#ifndef MEASURE_MANAGER_PSM_H
#define MEASURE_MANAGER_PSM_H


#include "monsb_measure.h"
#include "ycadd_measure.h"
#include "straw_measure.h"


#include <psm/psm_explain.h>


class GM_GCL_VDI_API MeasureManagerPsm : public IMonsbManagerSubject, public IYcaddManagerSubject, public IStrawManagerSubject
{
public:
    //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
    static void start( );

    static void stop( );

    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag );

    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag );

    static void timerTimeout(int iInterval);

    static int dealRrealtimeDataPost( const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach );

protected:
    void measures_valueChanged( const std::map<const MonsbMeasure*, int>& monsbOldValues, int iMeasureSonType, int iSourceId, int iTag );

    void measures_valueChanged( const std::map<const YcaddMeasure*, double>& ycaddOldValues, int iMeasureSonType, int iSourceId, int iTag );

    void measures_valueChanged( const std::map<const StrawMeasure*, std::string>& strawOldValues, int iMeasureSonType, int iSourceId, int iTag );

};

#endif // MEASURE_MANAGER_PSM_H
