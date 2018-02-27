
#include "station_local_server.h"


using namespace std;


//本单元的全局变量


//主线程发送文件的队列，缓存区
//主线程发送文件的队列，缓存区
//主线程发送文件的队列，缓存区
volatile int f_sls_sf_count;
CxMutex f_sls_sf_lock;
std::vector<CxFileSystem::PathInfo > f_sls_sf_bufferFileInfos1;
std::vector<CxFileSystem::PathInfo > f_sls_sf_bufferFileInfos2;
std::vector<std::vector<std::string> > f_sls_sf_bufferFileDatas1;
std::vector<std::vector<std::string> > f_sls_sf_bufferFileDatas2;
std::vector<PsmAttach> f_sls_sf_bufferFileAttachs1;
std::vector<PsmAttach> f_sls_sf_bufferFileAttachs2;
std::vector<CxFileSystem::PathInfo > * volatile f_sls_sf_pushBufferFileInfos;
std::vector<CxFileSystem::PathInfo > * volatile f_sls_sf_popBufferFileInfos;
std::vector<std::vector<std::string> > * volatile f_sls_sf_pushBufferFileDatas;
std::vector<std::vector<std::string> > * volatile f_sls_sf_popBufferFileDatas;
std::vector<PsmAttach> * volatile f_sls_sf_pushBufferFileAttachs;
std::vector<PsmAttach> * volatile f_sls_sf_popBufferFileAttachs;



//主线程发送消息的队列，缓存区
//主线程发送消息的队列，缓存区
//主线程发送消息的队列，缓存区
volatile int f_sls_sm_count;
CxMutex f_sls_sm_lock;
std::vector<std::string> f_sls_sm_bufferMessageCommands1;
std::vector<std::string> f_sls_sm_bufferMessageCommands2;
std::vector<std::string> f_sls_sm_bufferMessageParams1;
std::vector<std::string> f_sls_sm_bufferMessageParams2;
std::vector<PsmAttach> f_sls_sm_bufferMessageAttachs1;
std::vector<PsmAttach> f_sls_sm_bufferMessageAttachs2;
std::vector<std::string> * volatile f_sls_sm_pushBufferMessageCommands;
std::vector<std::string> * volatile f_sls_sm_popBufferMessageCommands;
std::vector<std::string> * volatile f_sls_sm_pushBufferMessageParams;
std::vector<std::string> * volatile f_sls_sm_popBufferMessageParams;
std::vector<PsmAttach> * volatile f_sls_sm_pushBufferMessageAttachs;
std::vector<PsmAttach> * volatile f_sls_sm_popBufferMessageAttachs;


//主线程接收文件的队列，缓存区
struct FileRecvDeal
{
    string md5;
    int strategy;
    string respCommand;
    string respParam;
    PsmAttach attach;
};
CxMutex f_sls_file_recv_lock;
std::map<std::string, FileRecvDeal> f_sls_file_recvs;


//*
//*
//* regist
//*
//*
static StationLocalServer * fn_oSlsServer()
{
    CxApplication::registStartFunction(StationLocalServer::start);
    CxApplication::registStopFunction(StationLocalServer::stop);
    GeneralAccess::registerMessageCommandDeal(StationLocalServer::dealMessage);
    GeneralAccess::registerFileWriteDeal(StationLocalServer::dealFileWrite);
    static StationLocalServer m;
    return &m;
}

static StationLocalServer * m_pTicketSvr = fn_oSlsServer();


static string fn_sRootPath()
{
    static char m[256];
    static char * s = NULL;
    //如果第1次大家都访问，就完蛋了。 +_+``
    if (! s)
    {
        string sRootPath = CxFileSystem::parentPath(CxApplication::applicationDeployPath());
        CxStringC::set(m, sRootPath.size(), sRootPath.c_str(), 256);
        s = m;
    }
    return s;
}





//push给主线程
int fn_sls_sf_pushFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    CxMutexScope lock(f_sls_sf_lock);
    f_sls_sf_pushBufferFileInfos->push_back(pathInfo);
    f_sls_sf_pushBufferFileDatas->push_back(fileData);
    if (oAttach)
    {
        f_sls_sf_pushBufferFileAttachs->push_back(* oAttach);
    }
    else
    {
        f_sls_sf_pushBufferFileAttachs->push_back(PsmAttach());
    }
    f_sls_sf_count++;
    return TRUE;
}


//push给主线程
int fn_sls_sf_pushMessage(const string & sCommand, const string & sParam, const PsmAttach *oAttach)
{
    CxMutexScope lock(f_sls_sm_lock);
    f_sls_sm_pushBufferMessageCommands->push_back(CxString::newString(sCommand));
    f_sls_sm_pushBufferMessageParams->push_back(CxString::newString(sParam));
    if (oAttach)
    {
        f_sls_sm_pushBufferMessageAttachs->push_back(* oAttach);
    }
    else
    {
        f_sls_sm_pushBufferMessageAttachs->push_back(PsmAttach());
    }
    f_sls_sm_count++;
    return TRUE;
}


//主线程的文件发送run
void fn_sls_sf_run()
{
    if (f_sls_sf_count>0)
    {
        CxMutexScope lock(f_sls_sf_lock);
        std::vector<CxFileSystem::PathInfo > * tmpInfos = f_sls_sf_pushBufferFileInfos;
        f_sls_sf_pushBufferFileInfos = f_sls_sf_popBufferFileInfos;
        f_sls_sf_popBufferFileInfos = tmpInfos;
        std::vector<std::vector<std::string> > * tmpDatas = f_sls_sf_pushBufferFileDatas;
        f_sls_sf_pushBufferFileDatas = f_sls_sf_popBufferFileDatas;
        f_sls_sf_popBufferFileDatas = tmpDatas;
        std::vector<PsmAttach> * tmpAttachs = f_sls_sf_pushBufferFileAttachs;
        f_sls_sf_pushBufferFileAttachs = f_sls_sf_popBufferFileAttachs;
        f_sls_sf_popBufferFileAttachs = tmpAttachs;
        f_sls_sf_count=0;
    }

    if (f_sls_sf_popBufferFileInfos->size()>0)
    {
        if (f_sls_sf_popBufferFileInfos->size() < 100)
        {
            for (size_t i = 0; i < f_sls_sf_popBufferFileInfos->size(); ++i)
            {
                const CxFileSystem::PathInfo & fileInfo = f_sls_sf_popBufferFileInfos->at(i);
                const std::vector<std::string> & fileData = f_sls_sf_popBufferFileDatas->at(i);
                const PsmAttach & fileAttach = f_sls_sf_popBufferFileAttachs->at(i);

                int iResult = GeneralAccess::postFile(fileInfo, fileData, & fileAttach);

                StationLocalServer::outSlsPrompt() << "GeneralAccess::postFileData fileName=" << fileInfo.fileName << "; fileSize=" << fileInfo.fileSize << "; result=" << iResult;
            }
        }
        else
        {
            StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! fn_sls_sf_run f_sls_sf_popBufferFileInfos->size()>100 , f_sls_sf_popBufferFileInfos->clear()!!!";
            StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! fn_sls_sf_run f_sls_sf_popBufferFileInfos->size()>100 , f_sls_sf_popBufferFileInfos->clear()!!!";
            StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! fn_sls_sf_run f_sls_sf_popBufferFileInfos->size()>100 , f_sls_sf_popBufferFileInfos->clear()!!!";
        }
        f_sls_sf_popBufferFileInfos->clear();
        f_sls_sf_popBufferFileDatas->clear();
        f_sls_sf_popBufferFileAttachs->clear();
    }
}

//主线程的消息发送run
void fn_sls_sm_run()
{
    if (f_sls_sm_count>0)
    {
        CxMutexScope lock(f_sls_sm_lock);
        std::vector<string> * tmpMessageCommands = f_sls_sm_pushBufferMessageCommands;
        f_sls_sm_pushBufferMessageCommands = f_sls_sm_popBufferMessageCommands;
        f_sls_sm_popBufferMessageCommands = tmpMessageCommands;
        std::vector<std::string> * tmpMessageParams = f_sls_sm_pushBufferMessageParams;
        f_sls_sm_pushBufferMessageParams = f_sls_sm_popBufferMessageParams;
        f_sls_sm_popBufferMessageParams = tmpMessageParams;
        std::vector<PsmAttach> * tmpAttachs = f_sls_sm_pushBufferMessageAttachs;
        f_sls_sm_pushBufferMessageAttachs = f_sls_sm_popBufferMessageAttachs;
        f_sls_sm_popBufferMessageAttachs = tmpAttachs;
        f_sls_sm_count=0;
    }

    if (f_sls_sm_popBufferMessageCommands->size()>0)
    {
        if (f_sls_sm_popBufferMessageCommands->size() < 100)
        {
            for (size_t i = 0; i < f_sls_sm_popBufferMessageCommands->size(); ++i)
            {
                string & sMessageCommand = f_sls_sm_popBufferMessageCommands->at(i);
                string & sMessageParam = f_sls_sm_popBufferMessageParams->at(i);
                PsmAttach & messageAttach = f_sls_sm_popBufferMessageAttachs->at(i);

                int iResult = GeneralAccess::postMessage(sMessageCommand, sMessageParam, & messageAttach);

                StationLocalServer::outSlsPrompt() << "GeneralAccess::postMessage MessageCommand=" << sMessageCommand << "; MessageParam=" << sMessageParam << "; result=" << iResult;
            }
        }
        else
        {
            StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! fn_sls_sf_run f_sls_sm_popBufferMessageCommands->size()>100 , f_sls_sm_popBufferMessageCommands->clear()!!!";
            StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! fn_sls_sf_run f_sls_sm_popBufferMessageCommands->size()>100 , f_sls_sm_popBufferMessageCommands->clear()!!!";
            StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! fn_sls_sf_run f_sls_sm_popBufferMessageCommands->size()>100 , f_sls_sm_popBufferMessageCommands->clear()!!!";
        }
        f_sls_sm_popBufferMessageCommands->clear();
        f_sls_sm_popBufferMessageParams->clear();
        f_sls_sm_popBufferMessageAttachs->clear();
    }
}



//*
//*
//* 接收到消息，对文件操作后，形成参数
//*
//*
void fn_doFileGetinfo(const string & sType, const string & sSection, CxFileSystem::PathInfo mPathInfo, const std::map<string, string> & sParams, string & sRespParam)
{
    string sFileType = CxContainer::value(sParams, string("filetype"));
    string sFileSize = CxContainer::value(sParams, string("filesize"));
    string sMd5 = CxContainer::value(sParams, string("md5"));
    if (sFileType.size()>0)
    {
        sRespParam += CxString::format("filetype=%d;", int(mPathInfo.pathType));
    }
    if (sFileSize.size()>0)
    {
        sRespParam += CxString::format("filesize=%d;", int(mPathInfo.fileSize));
    }
    if (sMd5.size()>0)
    {
        if (mPathInfo.pathType==CxFileSystem::PathTypeFile)
        {
            string sMd5 = CxCrc::file2md5(mPathInfo.filePath());
            sRespParam += CxString::format("md5=%s;", sMd5.c_str());
        }
        else
        {
            sRespParam += "md5=;";
        }
    }
}

void fn_doFileMove(const string & sType, const string & sSection, CxFileSystem::PathInfo mPathInfo, std::map<string, string> & sParams, string & sRespParam)
{
    int iResult = 0;
    int iProcess = 0;
    if (mPathInfo.pathType==CxFileSystem::PathTypeFile)
    {
        string sNewFileName = CxContainer::value(sParams, string("new_filename"));
        string sNewPath = CxContainer::value(sParams, string("new_path"));
        string sNewFilePath = CxFileSystem::fullFilePath(fn_sRootPath(), sNewPath, sNewFileName);
        int iStrategy = CxContainer::valueTo(sParams, string("strategy"), ci_int_zero);

        sRespParam += CxString::format("new_filename=%s;new_path=%s;strategy=%d;", sNewFileName.c_str(), sNewPath.c_str(), iStrategy);

        bool bCreateDir = ( iStrategy >> 4) & 0x01;
        bool bExistReturn = ( iStrategy >> 5) & 0x01;

        string sNewDir = CxFileSystem::parentPath(sNewFilePath);
        if (! CxFileSystem::isExist(sNewDir))
        {
            if (bCreateDir)
            {
                CxFileSystem::createDirMultiLevel(sNewDir);
                iProcess |= 0x04;
            }
        }

        if (CxFileSystem::isExist(sNewDir))
        {
            bool bExistNewFile = CxFileSystem::isExist(sNewFilePath);
            if (bExistNewFile && ! bExistReturn)
            {
                CxFileSystem::deleteFile(sNewFilePath);
                //存在新文件，并且删除
                iProcess |= 0x05;
            }
            if (! CxFileSystem::isExist(sNewFilePath))
            {
                iResult = CxFileSystem::renameFile(mPathInfo.filePath(), sNewFilePath);
            }
            else
            {
                //新文件已经存在并返回
                iResult = -6;
            }
        }
        else
        {
            //新目录不存在
            iResult = -5;
        }
    }
    else
    {
        //原文件不存在
        iResult = -4;
    }
    sRespParam += CxString::format("process=%d;result=%d;", iProcess, iResult);
}

void fn_doFileSave(const std::string &sNewFilePath, int iSize, int iStrategy, const std::vector<string> &fileData, int * pResult = NULL, int * pProcess = NULL)
{
    int iResult = 0;
    int iProcess = 0;

    bool bCreateDir = ( iStrategy >> 4) & 0x01;
    bool bExistReturn = ( iStrategy >> 5) & 0x01;

    string sNewDir = CxFileSystem::parentPath(sNewFilePath);
    if (! CxFileSystem::isExist(sNewDir))
    {
        CxFileSystem::createDirMultiLevel(sNewDir);
        iProcess |= (0x01 << 4);
    }

    if (CxFileSystem::isExist(sNewDir))
    {
        iResult = CxFile::save(sNewFilePath, fileData, string());
    }
    else
    {
        //新目录不存在
        iResult = -5;
    }
    if (pResult) *pResult = iResult;
    if (pProcess) *pProcess = iProcess;
}

void fn_doFileCopy(const string & sType, const string & sSection, CxFileSystem::PathInfo mPathInfo, std::map<string, string> & sParams, string & sRespParam)
{
    int iResult = 0;
    int iProcess = 0;
    int iBeCopy = -1;
    if (mPathInfo.pathType==CxFileSystem::PathTypeFile)
    {
        string sNewFileName = CxContainer::value(sParams, string("new_filename"));
        string sNewPath = CxContainer::value(sParams, string("new_path"));
        string sNewFilePath = CxFileSystem::fullFilePath(fn_sRootPath(), sNewPath, sNewFileName);
        int iStrategy = CxContainer::valueTo(sParams, string("strategy"), ci_int_zero);

        sRespParam += CxString::format("new_filename=%s;new_path=%s;strategy=%d;", sNewFileName.c_str(), sNewPath.c_str(), iStrategy);

        bool bCreateDir = ( iStrategy >> 4) & 0x01;
        bool bExistReturn = ( iStrategy >> 5) & 0x01;
        bool bOverWrite = ( iStrategy >> 6) & 0x01;

        string sNewDir = CxFileSystem::parentPath(sNewFilePath);
        if (! CxFileSystem::isExist(sNewDir))
        {
            if (bCreateDir)
            {
                CxFileSystem::createDirMultiLevel(sNewDir);
                iProcess |= 0x04;
            }
        }

        if (CxFileSystem::isExist(sNewDir))
        {
            bool bExistNewFile = CxFileSystem::isExist(sNewFilePath);
            if (! bExistNewFile || ! bExistReturn)
            {
                iResult = CxFileSystem::copyFile(mPathInfo.filePath(), sNewFilePath, bOverWrite, &iBeCopy);
                //存在新文件，并且覆盖
                if (bExistNewFile)
                    iProcess |= 0x05;
            }
            else
            {
                //新文件已经存在并返回
                iResult = -6;
            }
        }
        else
        {
            //新目录不存在
            iResult = -5;
        }
    }
    else
    {
        //原文件不存在
        iResult = -4;
    }
    sRespParam += CxString::format("process=%d;result=%d;becopy=%d", iProcess, iResult, iBeCopy);
}

void fn_doFilesRecv(const string & sMessageCommand, const string & sMessageParam, const PsmAttach & mMessageAttach)
{
    //message的处理
    //                上传文件数据	req.fsys.files.recv	"session=x;filename=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                resp.fsys.files.recv（如果接收文件成功就发送回应)	session=x;result=x;

    std::map<string, string> sParams = CxString::splitToMap(sMessageParam, '=', ';');
    string sType = CxContainer::value(sParams, string("type"));
    string sSession = CxContainer::value(sParams, string("session"));

    string sRespCommand = CxString::replace(sMessageCommand, "req.", "resp.");
    string sRespParam = CxString::format("type=%s;session=%s;", sType.c_str(), sSession.c_str());
    PsmAttach mSendAttach;
    mSendAttach.sourceId = mMessageAttach.targetId;
    mSendAttach.targetId = mMessageAttach.sourceId;

    if (sType == "begin")
    {
        f_sls_file_recvs.clear();
        sRespParam += CxString::format("result=%d;", 1);
        int rResult = GeneralAccess::postMessage(sRespCommand, sRespParam, & mSendAttach);
        StationLocalServer::outSlsPrompt() << "GeneralAccess::postMessage MessageCommand=" << sRespCommand << "; MessageParam=" << sRespParam << "; result=" << rResult;
        cxPrompt() << sMessageCommand << " message_begin.";
    }
    else if (sType == "end")
    {
        cxPrompt() << sMessageCommand << " message_end.";
    }
    else
    {
        string sFileName = CxContainer::value(sParams, string("filename"));
        string sPath = CxContainer::value(sParams, string("path"));
        msepoch_t dtNow = CxTime::currentMsepoch();

        string sNewFileName = CxContainer::value(sParams, string("new_filename"));
        string sNewPath = CxContainer::value(sParams, string("new_path"));
        string sMd5 = CxContainer::value(sParams, string("md5"));
        string sNewFilePath = CxFileSystem::fullFilePath(fn_sRootPath(), sNewPath, sFileName);
        int iStrategy = CxContainer::valueTo(sParams, string("strategy"), ci_int_zero);

        sRespParam += CxString::format("filename=%s;path=%s;new_filename=%s;new_path=%s;strategy=%d;", sFileName.c_str(), sPath.c_str(), sNewFileName.c_str(), sNewPath.c_str(), iStrategy);

        FileRecvDeal fileRecvDeal;
        fileRecvDeal.md5 = sMd5;
        fileRecvDeal.strategy = iStrategy;
        fileRecvDeal.respCommand = sRespCommand;
        fileRecvDeal.respParam = sRespParam;
        fileRecvDeal.attach = mSendAttach;
        f_sls_file_recvs[sNewFilePath] = fileRecvDeal;

        cxPrompt() << sMessageCommand << sNewFilePath;
    }
}

//接收文件的消息，下一个收到文件时，如果文件名不是 f_doFileRecv_fileName ， 就清空 f_doFileRecv_fileName
static string f_doFileRecv_fileName;
static PsmAttach f_doFileRecv_attach;
static string f_doFileRecv_newFilePath;
static int    f_doFileRecv_strategy;

void fn_doFileRecv(const string & sMessageCommand, const string & sMessageParam, const PsmAttach & mMessageAttach)
{
    //message的处理
    //                上传文件数据	req.fsys.file.recv	"session=x;filename=x;md5=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                resp.fsys.file.recv（如果接收文件成功就发送回应)	session=x;result=x;

    std::map<string, string> sParams = CxString::splitToMap(sMessageParam, '=', ';');
    string sType = CxContainer::value(sParams, string("type"));
    string sSession = CxContainer::value(sParams, string("session"));

    string sFileName = CxContainer::value(sParams, string("filename"));
    string sNewFileName = CxContainer::value(sParams, string("new_filename"));
    string sNewPath = CxContainer::value(sParams, string("new_path"));
    string sMd5 = CxContainer::value(sParams, string("md5"));
    int iStrategy = CxContainer::valueTo(sParams, string("strategy"), ci_int_zero);

    string sRespCommand = CxString::replace(sMessageCommand, "req.", "resp.");
    string sRespParam = sMessageParam;
    PsmAttach mSendAttach;
    mSendAttach.sourceId = mMessageAttach.targetId;
    mSendAttach.targetId = mMessageAttach.sourceId;

    string sResult;
    if (f_sls_file_recvs.size()>0)
    {
        sResult = "busy";
    }
    else
    {
        string sNewFilePath = CxFileSystem::fullFilePath(fn_sRootPath(), sNewPath, sNewFileName);
        string sMd5_l = CxCrc::file2md5(sNewFilePath);
        if (sMd5.size()>0 && sMd5 == sMd5_l)
        {
            sResult = "exist";
        }
        else
        {
            sResult = "ok";
            f_doFileRecv_fileName = sFileName;
            f_doFileRecv_newFilePath = sNewFilePath;
            f_doFileRecv_strategy = iStrategy;
            f_doFileRecv_attach = mSendAttach;
        }
    }
    sRespParam += CxString::format("result=%s;", sResult.c_str());

    int rResult = GeneralAccess::postMessage(sRespCommand, sRespParam, & mSendAttach);
    StationLocalServer::outSlsPrompt() << "doFileRecv : GeneralAccess::postMessage MessageCommand=" << sRespCommand << "; MessageParam=" << sRespParam << "; result=" << rResult;
}

void fn_doFile(const string & sMessageCommand, const string & sMessageParam, const PsmAttach & mMessageAttach, bool bThread)
{
    std::map<string, string> sParams = CxString::splitToMap(sMessageParam, '=', ';');
    string sType = CxContainer::value(sParams, string("type"));
    string sSession = CxContainer::value(sParams, string("session"));
    string sFileName = CxContainer::value(sParams, string("filename"));
    string sPath = CxContainer::value(sParams, string("path"));

    msepoch_t dtNow = CxTime::currentMsepoch();

    string sRespCommand = CxString::replace(sMessageCommand, "req.", "resp.");
    string sRespParam = CxString::format("session=%s;filename=%s;path=%s;", sSession.c_str(), sFileName.c_str(), sPath.c_str());
    PsmAttach mSendAttach;
    mSendAttach.sourceId = mMessageAttach.targetId;
    mSendAttach.targetId = mMessageAttach.sourceId;

    string sFilePath = CxFileSystem::fullFilePath(fn_sRootPath(), sPath, sFileName);
    CxFileSystem::PathInfo mPathInfo = CxFileSystem::getPathInfo(sFilePath);

    //                请求文件信息	req.fsys.files.getinfo	"session=x;filename=x;path=x;
    //                filetype=?;filesize=?;md5=?;"
    //                （【未必有】返回执行结果）	resp.fsys.files.getinfo	"session=x;filename=x;path=x;
    //                filetype=?;filesize=?;md5=?;"
    if (sMessageCommand.find(".getinfo") != string::npos)
    {
        fn_doFileGetinfo(sType, sSession, mPathInfo, sParams, sRespParam);
        if (bThread)
        {
            fn_sls_sf_pushMessage(sRespCommand, sRespParam, &mSendAttach);
        }
        else
        {
            GeneralAccess::postMessage(sRespCommand, sRespParam, &mSendAttach);
        }
    }

    //                请求文件数据	req.fsys.files.getdata.	session=x;filename=x;path=x;
    //                    "（如果加载成功就发送文件就OK了：session_filename)
    //                或 req.fsys.files.getdata."	session=x;filename=x;path=x;result=x;
    else if (sMessageCommand.find(".getdata") != string::npos)
    {
        int iResult = -1;
        if (mPathInfo.pathType==CxFileSystem::PathTypeFile)
        {
            std::vector<string> sendFileData;
            if ( CxFile::load(sFilePath, sendFileData, 1024 * 2) > 0)
            {
                if (bThread)
                {
                    fn_sls_sf_pushFile(mPathInfo, sendFileData, &mSendAttach);
                }
                else
                {
                    GeneralAccess::postFile(mPathInfo, sendFileData, &mSendAttach);
                }
                iResult = 1;
            }
            else
            {
                iResult = -3;
            }
        }
        else
        {
            iResult = -2;
        }
        if (iResult != 1)
        {
            sRespParam += CxString::format("result=%d;", iResult);
            if (bThread)
            {
                fn_sls_sf_pushMessage(sRespCommand, sRespParam, &mSendAttach);
            }
            else
            {
                GeneralAccess::postMessage(sRespCommand, sRespParam, &mSendAttach);
            }
        }
    }

    //                请求文件操作 拷贝	req.fsys.files.copy	"session=x;filename=x;path=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                （【未必有】返回执行结果）	resp.fsys.files.copy	session=x;result=x;
    //strategy： （1<<4）新目录不存在是否创建；（1<<5）新文件存在是否返回；（1<<6）新文件存在是否追加；
    else if (sMessageCommand.find(".copy") != string::npos)
    {
        fn_doFileCopy(sType, sSession, mPathInfo, sParams, sRespParam);
        if (bThread)
        {
            fn_sls_sf_pushMessage(sRespCommand, sRespParam, &mSendAttach);
        }
        else
        {
            GeneralAccess::postMessage(sRespCommand, sRespParam, &mSendAttach);
        }
    }

    //                请求文件操作 移动	req.fsys.files.move	"session=x;filename=x;path=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                （【未必有】返回执行结果）	resp.fsys.files.move	session=x;result=x;
    else if (sMessageCommand.find(".move") != string::npos)
    {
        fn_doFileMove(sType, sSession, mPathInfo, sParams, sRespParam);
        if (bThread)
        {
            fn_sls_sf_pushMessage(sRespCommand, sRespParam, &mSendAttach);
        }
        else
        {
            GeneralAccess::postMessage(sRespCommand, sRespParam, &mSendAttach);
        }
    }

    //                请求文件操作 删除	req.fsys.files.delete	session=x;filename=x;path=x;
    //                （【未必有】返回执行结果）	resp.fsys.files.delete	session=x;result=x;
    else if (sMessageCommand.find(".delete") != string::npos)
    {
        int iResult = 0;
        if (mPathInfo.pathType==CxFileSystem::PathTypeFile)
        {
            if (CxFileSystem::deleteFile(sFilePath))
                iResult = TRUE;
            else
                iResult = -1;
        }
        sRespParam += CxString::format("result=%d;", iResult);
        if (bThread)
        {
            fn_sls_sf_pushMessage(sRespCommand, sRespParam, &mSendAttach);
        }
        else
        {
            GeneralAccess::postMessage(sRespCommand, sRespParam, &mSendAttach);
        }
    }

    //                请求文件操作 创建链接	req.fsys.files.link	"session=x;filename=x;path=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                （【未必有】返回执行结果）	resp.fsys.files.link	session=x;result=x;


    //                请求文件操作 压缩	req.fsys.files.compress	"session=x;filename=x;path=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                （【未必有】返回执行结果）	resp.fsys.files.compress	session=x;result=x;


    //                请求文件操作 解压	req.fsys.files.extract	"session=x;filename=x;path=x;
    //                new_filename=x;new_path=x;strategy=x;"
    //                （【未必有】返回执行结果）	resp.fsys.files.extract	session=x;result=x;

    StationLocalServer::outSlsPrompt() << "fsys message_cost_time=" << CxTime::milliSecondDifferToNow(dtNow) << "; resp_message_command=" << sRespCommand << "; message_param=" << sRespParam << "; toTarget=" << mSendAttach.targetId;
}


//* Thread Receive Message
class SlsMessageRunThread : public CxJoinableThread
{
private:
    volatile int _status;
    volatile int _count;
    CxMutex _lock;
    std::vector<std::string> _bufferMessageCommands1;
    std::vector<std::string> _bufferMessageCommands2;
    std::vector<std::string> _bufferMessageParams1;
    std::vector<std::string> _bufferMessageParams2;
    std::vector<PsmAttach> _bufferMessageAttachs1;
    std::vector<PsmAttach> _bufferMessageAttachs2;
    std::vector<std::string> * volatile _pushBufferMessageCommands;
    std::vector<std::string> * volatile _popBufferMessageCommands;
    std::vector<std::string> * volatile _pushBufferMessageParams;
    std::vector<std::string> * volatile _popBufferMessageParams;
    std::vector<PsmAttach> * volatile _pushBufferMessageAttachs;
    std::vector<PsmAttach> * volatile _popBufferMessageAttachs;

public:
    void init()
    {
        _status = 1;
        _count = 0;
        _pushBufferMessageCommands = & _bufferMessageCommands1;
        _popBufferMessageCommands = & _bufferMessageCommands2;
        _pushBufferMessageParams = & _bufferMessageParams1;
        _popBufferMessageParams = & _bufferMessageParams2;
        _pushBufferMessageAttachs = & _bufferMessageAttachs1;
        _popBufferMessageAttachs = & _bufferMessageAttachs2;
    }

    inline void stop() { _status = 0; join(); }

    int pushMessageCommand(const string & sCommand, const string & sParam, const PsmAttach *oAttach)
    {
        CxMutexScope lock(_lock);
        _pushBufferMessageCommands->push_back(CxString::newString(sCommand));
        _pushBufferMessageParams->push_back(CxString::newString(sParam));
        if (oAttach)
        {
            _pushBufferMessageAttachs->push_back(* oAttach);
        }
        else
        {
            _pushBufferMessageAttachs->push_back(PsmAttach());
        }
        _count++;
        return TRUE;
    }

protected:
    void run() {
        _status = 1;
        while ( _status )
        {
            if (_count>0)
            {
                CxMutexScope lock(_lock);
                std::vector<string> * tmpMessageCommands = _pushBufferMessageCommands;
                _pushBufferMessageCommands = _popBufferMessageCommands;
                _popBufferMessageCommands = tmpMessageCommands;
                std::vector<std::string> * tmpMessageParams = _pushBufferMessageParams;
                _pushBufferMessageParams = _popBufferMessageParams;
                _popBufferMessageParams = tmpMessageParams;
                std::vector<PsmAttach> * tmpAttachs = _pushBufferMessageAttachs;
                _pushBufferMessageAttachs = _popBufferMessageAttachs;
                _popBufferMessageAttachs = tmpAttachs;
                _count=0;
            }

            if (_popBufferMessageCommands->size()>100)
            {
                _popBufferMessageCommands->clear();
                _popBufferMessageParams->clear();
                _popBufferMessageAttachs->clear();
                StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! SlsMessageRunThread MessageCommands->size()>100 , MessageCommands->clear()!!!";
                StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! SlsMessageRunThread MessageCommands->size()>100 , MessageCommands->clear()!!!";
                StationLocalServer::outSlsPrompt() << "Error ! Error ! Error ! SlsMessageRunThread MessageCommands->size()>100 , MessageCommands->clear()!!!";
            }


            //filetype： 文件类型；（-1:不知道的类型; 0：不存在；1：文件；2：目录；3：链接；4：IO；5:disk；6：fifo；7:socket；）

            for (size_t i = 0; i < _popBufferMessageCommands->size(); ++i)
            {
                string & sMessageCommand = _popBufferMessageCommands->at(i);
                string & sMessageParam = _popBufferMessageParams->at(i);
                PsmAttach & mMessageAttach = _popBufferMessageAttachs->at(i);

                fn_doFile(sMessageCommand, sMessageParam, mMessageAttach, true);
            }
            _popBufferMessageCommands->clear();
            _popBufferMessageParams->clear();
            _popBufferMessageAttachs->clear();

            CxThread::sleep(30);
        }

    }

    void exit() {
        _status = 0;
        CxJoinableThread::exit();
    }

};

SlsMessageRunThread * volatile f_oSlsMessageRunThread = NULL;




void StationLocalServer::start()
{
    f_sls_sf_count = 0;
    f_sls_sf_pushBufferFileInfos = & f_sls_sf_bufferFileInfos1;
    f_sls_sf_popBufferFileInfos = & f_sls_sf_bufferFileInfos2;
    f_sls_sf_pushBufferFileDatas = & f_sls_sf_bufferFileDatas1;
    f_sls_sf_popBufferFileDatas = & f_sls_sf_bufferFileDatas2;
    f_sls_sf_pushBufferFileAttachs = & f_sls_sf_bufferFileAttachs1;
    f_sls_sf_popBufferFileAttachs = & f_sls_sf_bufferFileAttachs2;

    f_sls_sm_count = 0;
    f_sls_sm_pushBufferMessageCommands = & f_sls_sm_bufferMessageCommands1;
    f_sls_sm_popBufferMessageCommands = & f_sls_sm_bufferMessageCommands2;
    f_sls_sm_pushBufferMessageParams = & f_sls_sm_bufferMessageParams1;
    f_sls_sm_popBufferMessageParams = & f_sls_sm_bufferMessageParams2;
    f_sls_sm_pushBufferMessageAttachs = & f_sls_sm_bufferMessageAttachs1;
    f_sls_sm_popBufferMessageAttachs = & f_sls_sm_bufferMessageAttachs2;

    if (! f_oSlsMessageRunThread)
    {
        f_oSlsMessageRunThread = new SlsMessageRunThread();
        f_oSlsMessageRunThread->init();
        f_oSlsMessageRunThread->start();
    }
    CxTimerManager::startTimer(StationLocalServer::timerRunSls, 30);
}

void StationLocalServer::stop()
{
    if (f_oSlsMessageRunThread)
    {
        f_oSlsMessageRunThread->stop();
        f_oSlsMessageRunThread = NULL;
    }
}

void StationLocalServer::timerRunSls(int iInterval)
{
    fn_sls_sf_run();
}

int StationLocalServer::dealMessage(const PsmExplainer *oPsm, const string &sMessageCommand, const string &sMessageParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);
    GM_ASSER_OBJECT(f_oSlsMessageRunThread);


    if(sMessageCommand.find(".fsys.") != string::npos)
    {
        //单文件方式
        if (sMessageCommand.find(".file.") != string::npos)
        {
            if (sMessageCommand.find(".recv") != string::npos)
            {
                fn_doFileRecv(sMessageCommand, sMessageParam, *oAttach);
            }
            else
            {
                fn_doFile(sMessageCommand, sMessageParam, *oAttach, false);
            }
        }
        //多文件、文件队列方式
        else if (sMessageCommand.find(".files.") != string::npos)
        {
            //接收文件
            if (sMessageCommand.find(".recv") != string::npos)
            {
                fn_doFilesRecv(sMessageCommand, sMessageParam, *oAttach);
            }
            else
            {
                f_oSlsMessageRunThread->pushMessageCommand(sMessageCommand, sMessageParam, oAttach);
            }
        }
        return TRUE;
    }
    else if(sMessageCommand.find("req.program.who") != string::npos)
    {
        std::map<string, string> sParams = CxString::splitToMap(sMessageParam, '=', ';');
        string sLocalIpAddress = CxContainer::value(sParams, CS_EntryLocalIpAddress, string());
        int iPort = CxContainer::valueTo(sParams, CS_EntryLocalPort, ci_int_zero);
        int r = -2;
        if (sLocalIpAddress.size()>0 && CxString::isValidPort(iPort))
        {
            GeneralAccess::setRemoteIpAddress(sLocalIpAddress, iPort);
            r = GeneralAccess::postMessage("resp.program.who.");
        }
        cxPrompt() << sMessageCommand << " ;deal_result=" << r;
    }
    return FALSE;
}

int StationLocalServer::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (fileData.empty())
        return FALSE;

    cxDebug() << pathInfo.fileName;

    if (f_doFileRecv_fileName.size()>0)
    {
        int iResult = 0;
        int iProcess = 0;
        if (CxFileSystem::extractFileName(f_doFileRecv_fileName)==pathInfo.fileName)
        {
            fn_doFileSave(f_doFileRecv_newFilePath, pathInfo.fileSize, f_doFileRecv_strategy, fileData, &iResult, &iProcess);
        }
        else
        {
            f_doFileRecv_fileName = "";
        }
        string sResult = iResult==1 ? "success" : CxString::toString(iResult);
        string sRespParam = CxString::format("filename=%s;result=%s;process=%d", f_doFileRecv_fileName.c_str(), sResult.c_str(), iProcess);
        int rResult = GeneralAccess::postMessage("resp.fsys.file.recv", sRespParam, & f_doFileRecv_attach);
        StationLocalServer::outSlsPrompt() << "doFileRecv GeneralAccess::postMessage MessageParam=" << sRespParam << "; result=" << rResult;
        return TRUE;
    }

    //多文件、文件队列方式
    if (f_sls_file_recvs.size()>0)
    {
        string sMd5 = CxCrc::md5HexCode(fileData);
        string mNewFilePath;
        FileRecvDeal mFileRecvDeal;
        {
            for(typename std::map<string,FileRecvDeal>::iterator it = f_sls_file_recvs.begin(); it != f_sls_file_recvs.end(); ++it)
            {
                const string & sNewFilePath = it->first;
                const FileRecvDeal & fileRecvDeal= it->second;
                if (fileRecvDeal.md5 == sMd5)
                {
                    mNewFilePath = sNewFilePath;
                    mFileRecvDeal = fileRecvDeal;
                    f_sls_file_recvs.erase(it);
                    break;
                }
            }
        }
        if (mNewFilePath.size() > 0)
        {
            int iResult = 0;
            int iProcess = 0;
            if ( sMd5 != CxCrc::file2md5(mNewFilePath) )
            {
                fn_doFileSave(mNewFilePath, pathInfo.fileSize, mFileRecvDeal.strategy, fileData, &iResult, &iProcess);
            }
            else
            {
                iResult = -1;
            }
            string sRespParam = mFileRecvDeal.respParam;
            sRespParam += CxString::format("filesize=%d;process=%d;result=%d;", pathInfo.fileSize, iProcess, iResult);
            int rResult = GeneralAccess::postMessage(mFileRecvDeal.respCommand, sRespParam, & mFileRecvDeal.attach);
            StationLocalServer::outSlsPrompt() << "GeneralAccess::postMessage MessageCommand=" << mFileRecvDeal.respCommand << "; MessageParam=" << sRespParam << "; result=" << rResult;
            return TRUE;
        }
    }
    return FALSE;
}

