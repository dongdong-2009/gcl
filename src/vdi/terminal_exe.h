#ifndef TERMINAL_EXE_H
#define TERMINAL_EXE_H

#include "protocol_global.h"
#include <psm/psm_explain.h>

#include "vdi_declspec.h"

class GM_GCL_VDI_API TerminalExe : public CxLogFileTactics_I
{
public:
    std::string log_directory() const;

    std::string log_fileName( int type, int reason, int source, int target, int tag ) const;

    bool log_hasFileName( int type, int reason, int source, int target, int tag ) const;

public:
    static int load(int argc, const char * argv[]);

    static int start();

    static void stop();

    static int exe(int argc, const char * argv[]);

    static void registAfterLoad(fn_void_t fn);

    //GA收到消息的处理
    static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

    //GA收到文件的处理
    static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

    //GA已经发出去文件的处理
    static int dealResultFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach );

private:
    static std::vector<fn_void_t> *getFnAfterLoads();

    //目前处理遥控做了两件事
    //1、根据遥控所在的TerminalId转发走
    //2、Push给共享内存
    //send.yk measure=0x01000011;value=1;reason=2;strategy=3;method=4\nk11=v11;k12=v12\nk21=v21;k22=v22\n
    static void dealPsmMeasureControl(const std::string & sCommand, const std::string &sParam, const PsmAttach *oAttach);


};

#endif // TERMINAL_EXE_H
