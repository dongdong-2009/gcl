#include "processer.h"
#include "applicationwidget.h"


#define GM_BUFFER_SZIE 4096

static int sign = 1;
using namespace std;
std::vector<Processer*> f_oProcesses;
//static std::vector<Processer*> f_oProcesses;

Processer::Processer(std::string sExeFilePath, int iWaitFirstMillisecond, int iProcessType, int iLocalPort, std::string sHBuf, int iTimeOut_receive, std::string sProcessParam, int iMode)
{
    exeFilePath = sExeFilePath;
    size_t found = sExeFilePath.find_last_of("/\\");
    if(found != string::npos)
    {
        exeFileName = sExeFilePath.substr(found+1);
    }
    else
    {
        exeFileName = sExeFilePath;
    }

    waitFirstMillisecond = iWaitFirstMillisecond;
    processType = iProcessType;
    localPort = iLocalPort;
    heartBuffer = sHBuf;
    heartHexString = GlobalFunction::fn_toHexstring(sHBuf.data(), sHBuf.size(), false);
    timeOut_receive = iTimeOut_receive;
    startMode = iMode;
    runParam = sProcessParam;

    startTimes = 0;
    unusalCloseTime= 0;

    hasFirstReceive = false;
    isThreadReceiveError = false;
    isThreadReceiveCancel = false;
    isThreadWaitFinish  = false;
    lastReceiveTime = 0;
    notReceiveLong = 0;

    commandLine = sExeFilePath;
    if(sProcessParam.size() > 0)
    {
        commandLine.push_back(' ');
        commandLine.append(sProcessParam);
    }

}


void Processer::loadTable1LevelByDocument(const TiXmlDocument &doc, std::vector<std::map<string, string> > &rows, const string &sTableName)
{
    const TiXmlElement * oElementLevel1 = doc.FirstChildElement();
    cout << "oElementLevel1 value: " << oElementLevel1->ValueTStr() << endl;
    while (oElementLevel1)
    {
        //*table name
        if (sTableName.size()>0)
        {
            if (oElementLevel1->ValueTStr() != sTableName)
            {
                continue;
            }
        }
        for( const TiXmlElement * oElementLevel2 = oElementLevel1->FirstChildElement(); oElementLevel2; oElementLevel2 = oElementLevel2->NextSiblingElement() )
        {
            cout << "oElementLevel2 value: " << oElementLevel2->ValueTStr() << endl;
            if (! oElementLevel2) break;
            map<string, string> row;
            loadRowByElement(oElementLevel2, row);
            if (row.size()>0)
            {
                rows.push_back(row);
            }
        }

        oElementLevel1 = oElementLevel1->NextSiblingElement();
    }
}
/*
void anlay(const TiXmlDocument &doc, std::vector<std::map<string, string> > &rows, const string &rootNodeName)
{
    TiXmlElement *processElement = doc.FirstChildElement();
    while(processElement)
    {
        if(rootNodeName.size() > 0)
        {
            if( processElement->ValueTStr() != rootNodeName )
            {
                continue;
            }
        }

        TiXmlElement *elementLevel2 = processElement->FirstChildElement();
        for(; elementLevel2; elementLevel2 = elementLevel2->NextSiblingElement())
        {
            if( ! elementLevel2 ) break;
            map<string, string> row;
            loadRowByElement(oElementLevel2, row);

        }
    }
}
*/

////key=value node.name=text
//void CxXml::loadRowByElement(const TiXmlElement *oElementLevel, std::map<std::string, std::string> & row)
//{
//    if (oElementLevel)
//    {
//        string sKey = oElementLevel->ValueTStr();
//        string sValue = text(oElementLevel);
//        row[sKey] = sValue;
//        loadRowByAttribute(oElementLevel, row);
//        for( const TiXmlElement * oElementLevelChild = oElementLevel->FirstChildElement(); oElementLevelChild; oElementLevelChild = oElementLevelChild->NextSiblingElement() )
//        {
//            if (! oElementLevelChild) break;
//            loadRowByElement(oElementLevelChild, row);
//        }
//    }
//}

void Processer::loadRowByElement(const TiXmlElement *oElementLevel, std::map<std::string, std::string> & row)
{
    qDebug() << "Key = value " << QString::fromStdString(oElementLevel->ValueTStr()) << " = " << QString::fromStdString(text(oElementLevel)) << endl;
    if (oElementLevel)
    {
        if(oElementLevel->ValueTStr() != "process")
        {
            string sKey = oElementLevel->ValueTStr();
            string sValue = text(oElementLevel);
            row[sKey] = sValue;
        }
        loadRowByAttribute(oElementLevel, row);
        for( const TiXmlElement * oElementLevelChild = oElementLevel->FirstChildElement(); oElementLevelChild; oElementLevelChild = oElementLevelChild->NextSiblingElement() )
        {
            if (! oElementLevelChild) break;
            loadRowByElement(oElementLevelChild, row);
        }
    }
}

void Processer::loadRowByAttribute(const TiXmlElement *oElementLevel, std::map<string, string> &row)
{
    if (oElementLevel)
    {
        const TiXmlAttribute * oAttribute = oElementLevel->FirstAttribute();
        while (oAttribute)
        {
            qDebug() << "Key = value " << QString::fromStdString(oAttribute->NameTStr()) << " = " << QString::fromStdString(oAttribute->ValueStr()) << endl;
            string sKey = oAttribute->NameTStr();
            string sValue = oAttribute->ValueStr();
            row[sKey] = sValue;
            oAttribute = oAttribute->Next();
        }
    }
}


vector< map<string, string> > Processer::analyseXml(const string configfilepath)
{
    TiXmlDocument doc(configfilepath);
    std::vector<std::map<std::string, std::string> > xmlvector;
    bool loadOK = doc.LoadFile(TIXML_ENCODING_UTF8);
    if(!loadOK)
    {
        printf( "Could not load config file 'gcl_process_manager.xml'. Error='%s'\n", doc.ErrorDesc() );
        exit( 1 );
    }
    cout << "Open gcl_process_manager.xml success\n";

    loadTable1LevelByDocument( doc, xmlvector, "processes");

    return xmlvector;
}


bool Processer::GetNodePointerByName(TiXmlElement *rootElement, const string &strMainNodeName, const string &strMainNodeAttributeName, const string &strNodeName, TiXmlElement * &Node)
{
    TiXmlAttribute *nodeAttribute = rootElement->FirstAttribute();
    if(strMainNodeName == rootElement->ValueTStr())
    {
        if(nodeAttribute->Value() == strMainNodeAttributeName)
        {
            TiXmlElement *nodeElement = rootElement->FirstChildElement();
            if(strMainNodeName == strNodeName)
            {
                Node = rootElement;
                return true;
            }

            for(; nodeElement; nodeElement = nodeElement->NextSiblingElement())
            {
                if(strNodeName == nodeElement->ValueTStr())
                {
                    Node = nodeElement;
                    return true;
                }
                else
                    continue;
            }
        }
    }

    TiXmlElement *pElement = rootElement->FirstChildElement();
    for(; pElement; pElement = pElement->NextSiblingElement())
    {
        if(GetNodePointerByName(pElement, strMainNodeName, strMainNodeAttributeName, strNodeName, Node))
            return true;
    }

    return false;
}


bool Processer::DelNode(const string xmlFilePath, const string &strMainNodeName, const string &strMainNodeAttributeName, const string &strNodeName)
{
    TiXmlDocument *doc = new TiXmlDocument();
    if(NULL == doc)
    {
        cout << "TiXmlDocument failed\n";
        return false;
    }

    doc->LoadFile(xmlFilePath);
    TiXmlElement *rootElement = doc->RootElement();
    if(NULL == rootElement)
    {
        return false;
    }

    TiXmlElement *pNode = NULL;
    GetNodePointerByName(rootElement, strMainNodeName, strMainNodeAttributeName, strNodeName, pNode);
    if(rootElement == pNode)
    {
        if(doc->RemoveChild(rootElement))
        {
            doc->SaveFile(xmlFilePath);
            return true;
        }
        else
            return false;
    }

    if(NULL != pNode)
    {
        TiXmlNode *pParNode = pNode->Parent();
        if(NULL == pParNode)
        {
            return false;
        }

        TiXmlElement *pParentElement = pParNode->ToElement();
        if(NULL != pParentElement)
        {
            if(pParentElement->RemoveChild(pNode))
            {
                doc->SaveFile(xmlFilePath);
                return true;
            }
            else
                return false;
        }
    }

    return false;
}

/*
bool Processer::ModifyNode_Attribute(const string &xmlFile, const string &strMainNodeName, const string &strMainNodeAttributeName, const string &strNodeName, const string &strAttribute)
{
    TiXmlDocument *doc = new TiXmlDocument();
    if(NULL == doc)
    {
        return false;
    }

    doc->LoadFile(xmlFile);
    TiXmlElement *rootElement = doc->RootElement();
    TiXmlElement *pNode = NULL;
}*/

bool Processer::ModifyNode_Text(const string &xmlFile, const string &strMainNodeName, const string &strMainNodeAttributeName, const string &strNodeName, const string &strText)
{
    TiXmlDocument *doc = new TiXmlDocument();
    if(NULL == doc)
    {
        return false;
    }

    doc->LoadFile(xmlFile);
    TiXmlElement *rootElement = doc->RootElement();
    TiXmlElement *pNode = NULL;

    GetNodePointerByName(rootElement, strMainNodeName, strMainNodeAttributeName, strNodeName, pNode);

    if(NULL != pNode)
    {
        pNode->Clear();
        TiXmlText *pValue = new TiXmlText(strText);
        pNode->LinkEndChild(pValue);
        doc->SaveFile(xmlFile);
        return true;
    }
    else
    {
        return false;
    }
}


bool Processer::AddNode(const string &xmlFilePath, const string &strMainNodeName, const string &strMainNodeAttributeName, const string &strNodeName, const string &strText)
{
    TiXmlDocument *doc = new TiXmlDocument();
    if(NULL == doc)
    {
        return false;
    }

    doc->LoadFile(xmlFilePath);
    TiXmlElement *rootElement = doc->RootElement();
    if(NULL == rootElement)
    {
        return false;
    }

    TiXmlElement *pNode = NULL;
    GetNodePointerByName(rootElement, strMainNodeName, strMainNodeAttributeName, strMainNodeName, pNode);

    if(NULL != pNode)
    {
        TiXmlElement *pNewNode = new TiXmlElement(strNodeName);
        if(NULL == pNewNode)
        {
            return false;
        }

        TiXmlText *pNewValue = new TiXmlText(strText);
        pNewNode->LinkEndChild(pNewValue);

        pNode->InsertEndChild(*pNewNode);
        doc->SaveFile(xmlFilePath);
        return true;
    }
    else
        return false;
}

void Processer::prepareProcessInfo(const string filepath)
{
    vector< map<string, string> > processInfoVec = analyseXml(filepath);

    vector<int> iPortes;
    string sHeartBuffer;
    vector<string> sCmdFulls;

    for(size_t i = 0; i < processInfoVec.size(); ++i)
    {
        map<string, string> processInfo = processInfoVec.at(i);
        map<string, string>::const_iterator infoIter = processInfo.find("pf");
        string sExeFilePath;
        if( infoIter !=  processInfo.end())
        {
            sExeFilePath = infoIter->second;
            if(sExeFilePath.empty() < 0)
            {
                cout << "config index : " << i << " 是空路径" << endl;
                continue;
            }

            size_t num = sExeFilePath.find_last_of("/\\");
            if( num == string::npos )
            {
                char pathCharacter = '\\';
                string applicationPath = GlobalFunction::getApplicationPath();
                size_t index = applicationPath.find("\\");
                if(index != string::npos)
                    pathCharacter = '\\';
                else
                    pathCharacter = '/';
                applicationPath.push_back(pathCharacter);
                sExeFilePath = applicationPath.append(sExeFilePath);
            }
        }
        else
        {
            cout << "config index : " << i << "参数pf不存在" << endl;
            continue;
        }


        infoIter = processInfo.find("pt");
        int iProcessType;
        if( infoIter != processInfo.end())
        {
            iProcessType = atoi(infoIter->second.c_str());
        }
        else
        {
            cout << "config index : " << i << "参数pt不存在" << endl;
            continue;
        }

        infoIter = processInfo.find("pw");
        int iWaitFirstMillisecond;
        if(infoIter != processInfo.end())
        {
            iWaitFirstMillisecond= atoi(processInfo["pw"].c_str());
            if (! (iWaitFirstMillisecond >= 0 && iWaitFirstMillisecond < 60 * 1000))
            {
                cout << "config index : " << i << " 首次启动等待多长时间的数值不合法" << endl;
                continue;
            }
        }
        else
        {
            cout << "config index : " << i << "参数pw不存在" << endl;
            continue;
        }

        int iLocalPort;
        infoIter = processInfo.find("np");
        if(infoIter != processInfo.end())
        {
            iLocalPort = atoi(infoIter->second.c_str());
            if (! (iLocalPort >= 0 && iLocalPort < USHRT_MAX))
            {
                cout << "config index : " << i << " 端口不合法" << endl;
                continue;
            }
            if (iLocalPort > 0 && std::find(iPortes.begin(), iPortes.end(), iLocalPort) != iPortes.end())
            {
                cout << "config index : " << i << " 是重复的端口" << endl;
                continue;
            }
            iPortes.push_back(iLocalPort);
        }
        else
        {
            cout << "config index : " << i << "参数np不存在" << endl;
            continue;
        }


        infoIter = processInfo.find("nt");
        int iTimeOut_receive;
        if(infoIter != processInfo.end())
        {
            iTimeOut_receive = atoi(infoIter->second.c_str());
            if (! (iTimeOut_receive >= 0 && iTimeOut_receive < 86400*1000))
            {
                cout << "config index : " << i << " 网络超时的数值不合法" << endl;
                continue;
            }
        }
        else
        {
            cout << "config index : " << i << "参数nt不存在" << endl;
            continue;
        }


        infoIter = processInfo.find("nh");
        string sHBufText;
        string sHBuf;
        if(infoIter != processInfo.end())
        {
            sHeartBuffer = infoIter->second;
            if (sHeartBuffer.size() > 0)
            {
                if (! (GlobalFunction::fn_isValidHexCharater(sHeartBuffer) && (sHeartBuffer.size() % 2 == 0) && (sHeartBuffer.size() < 255) ) )
                {
                    cout << "config index : " << i << " 心跳报文不合法" << endl;
                    continue;
                }
            }

            vector<char> hBuf = GlobalFunction::fn_fromHexstring(sHeartBuffer);
            sHBuf = string(hBuf.data(), hBuf.size());
            sHBufText = "";
        }
        else
        {
            cout << "config index : " << i << "参数nh不存在" << endl;
            continue;
        }


        infoIter = processInfo.find("pp");
        string sPp;
        string sCommandLine = sExeFilePath;

        if(infoIter != processInfo.end())
        {
            sPp = infoIter->second;
            if( sPp.size() > 255 )
            {
                cout << "config index : " << i << " 进程的运行参数过长不合法" << endl;
                continue;
            }

            if ( sPp.size() > 0 )
            {
                sCommandLine.push_back(' ');
                sCommandLine.append( sPp );
            }
            //sCommandLine = fn_toLower(sCommandLine);
            if (std::find(sCmdFulls.begin(), sCmdFulls.end(), sCommandLine) != sCmdFulls.end())
            {
                cout << "config index : " << i << " 是重复的命令行" << endl;
                continue;
            }
            sCmdFulls.push_back(sCommandLine);
        }
        else
        {
            cout << "config index : " << i << "参数pp不存在" << endl;
            continue;
        }


        infoIter = processInfo.find("pm");
        int iMode;
        if(infoIter != processInfo.end())
        {
            iMode = atoi(infoIter->second.c_str());
            if (! (iMode >= 0 && iMode < 5))
            {
                cout << "config index : " << i << " 窗口模式不合理" << endl;
                continue;
            }
        }
        else
        {
            cout << "config index : " << i << "参数pm不存在" << endl;
            continue;
        }

        Processer * oProcesser = new Processer(sExeFilePath, iWaitFirstMillisecond, iProcessType, iLocalPort, sHBufText, iTimeOut_receive, sPp, iMode);
        f_oProcesses.push_back(oProcesser);
    }
}


map<string, string>  Processer::getProcessMessage( string exeName, HANDLE hProcess1, string commandLine, string exeFilePath, DWORD dwprocessId,  msepoch_t startTime, int startTimes, int unusalCloseTime)
{
    map<string, string> proMsg;
    proMsg.clear();
    proMsg["进程名"] = exeName;
    if(!hProcess1 || !dwprocessId)
    {
        cout << exeName << dwprocessId << hProcess1 << "not value" << endl;
        return proMsg;
    }

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if(INVALID_HANDLE_VALUE == hProcessSnap)
    {
        std::cout << "获取进程信息失败 : " << GetLastError() << std::endl;
        return proMsg;
    }


    //进程ID exe 线程数目
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        std::cout << "获取第一个进程信息失败 : " << GetLastError() << std::endl;
        return proMsg;
    }

    do
    {
        if(strcmp(pe32.szExeFile, exeName.data()) == 0)
        {
            proMsg["进程ID"] = GlobalFunction::fn_toString(pe32.th32ProcessID);
            proMsg["线程数"] = GlobalFunction::fn_toString(pe32.cntThreads);
        }
    }while(Process32Next(hProcessSnap, &pe32));

    if(proMsg.size() < 2)
    {
        cout << exeName << "not find" << endl;
        ZeroMemory(&pe32, sizeof(pe32));
        return proMsg;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwprocessId);
    //内存
    PROCESS_MEMORY_COUNTERS pmc;
    if(! GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)) )
    {
        cout << "GetProcessMemoryInfo Error: " << GetLastError() << exeName << endl;
        return proMsg;
    }
    proMsg["工作设置内存"] = (GlobalFunction::fn_toString(pmc.WorkingSetSize/1024)).append("K");
    proMsg["提交大小"] = (GlobalFunction::fn_toString(pmc.PagefileUsage/1024)).append("K");
    proMsg["峰值工作设置"] = (GlobalFunction::fn_toString(pmc.PeakWorkingSetSize/1024)).append("K");

    PERFORMANCE_INFORMATION performmanceInfo;
    memset(&performmanceInfo, 0, sizeof(performmanceInfo));
    if(!::GetPerformanceInfo(&performmanceInfo, sizeof(performmanceInfo)))
    {
        cout << "GetPerformanceInfo failed : " << GetLastError() << std::endl;
    }
    //DWORD pageSize = performmanceInfo.PageSize;

    PSAPI_WORKING_SET_INFORMATION workSetInfo;
    //PSAPI_WORKING_SET_BLOCK *workSetBlock = workSetInfo.WorkingSetInfo;
    memset(&workSetInfo, 0, sizeof(workSetInfo));

    //网络状态
    proMsg["网络状态"] = "NULL";

    //运行时间
    msepoch_t nowTime = GlobalFunction::fn_currentMsepoch();
    msepoch_t proRunTimes = nowTime - startTime;
    proMsg["运行时长"] = GlobalFunction::fn_toString((int)(proRunTimes/(1000*60)));

    //IO读取 写入
    IO_COUNTERS ioCounters;
    if(! GetProcessIoCounters(hProcess, &ioCounters))
    {
        cout << "GetProcessIoCounters Error: " << GetLastError() << endl;
        return proMsg;
    }
    proMsg["IO读取"] = GlobalFunction::fn_toString(ioCounters.ReadOperationCount);
    proMsg["IO写入"] = GlobalFunction::fn_toString(ioCounters.WriteOperationCount);
    proMsg["IO读取字节"] = GlobalFunction::fn_toString(ioCounters.ReadTransferCount);
    proMsg["IO写入字节"] = GlobalFunction::fn_toString(ioCounters.WriteTransferCount);

    //启动次数  优先级
    proMsg["启动次数"] = GlobalFunction::fn_toString(startTimes);

    DWORD proPriority = GetPriorityClass(hProcess);
    string strPriority;
    if(  NORMAL_PRIORITY_CLASS == proPriority )
    {
        strPriority = "普通";
    }
    else if( IDLE_PRIORITY_CLASS == proPriority )
    {
        strPriority = "低";
    }
    else if( HIGH_PRIORITY_CLASS == proPriority )
    {
        strPriority = "高";
    }
    else if( REALTIME_PRIORITY_CLASS == proPriority )
    {
        strPriority = "实时";
    }
    else if( BELOW_NORMAL_PRIORITY_CLASS == proPriority )
    {
        strPriority = "低于标准";
    }
    else
    {
        strPriority = "高于标准";
    }
    proMsg["优先级"] = strPriority;

    //异常关闭次数
    proMsg["异常关闭次数"] = GlobalFunction::fn_toString(unusalCloseTime);
    proMsg["专用工作集"] = "0K";
    proMsg["程序路径"] = exeFilePath;
    proMsg["命令行"] = commandLine;

    CloseHandle(hProcessSnap);
    return proMsg;
}


bool Processer::vectorIsEmpty()
{
    if( f_oProcesses.empty() )
        return true;
    else
        return false;
}

const std::vector<Processer *> &Processer::getProcesses()
{
    return f_oProcesses;
}

static DWORD WINAPI ThreadWaitPrecess(PVOID pParam)
{
    Processer *oProcesser = (Processer *)pParam;
    cout << "Wait Process: " << oProcesser->exeFileName << endl;
    WaitForSingleObject(oProcesser->Processinfo.hProcess, INFINITE);
    cout << "Wait ProcessId : " << oProcesser->Processinfo.dwProcessId  << "Wait Time: " << endl;
    if(sign)
    {
        oProcesser->isThreadWaitFinish = true;
    }
    sign = 1;
    return 0;
}

int Processer::killProcess(const char *szProcessName)
{
    sign = 0;
    HANDLE hprocessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(INVALID_HANDLE_VALUE == hprocessSnap)
    {
        cout << "获取进程信息失败(killProcess)" << endl;
        return -1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if(!Process32First(hprocessSnap, &pe32))
    {
        CloseHandle(hprocessSnap);
        return -2;
    }

    //提升进程权限
    HANDLE hToken;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
    {
        LUID luid;
        if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
        {
            TOKEN_PRIVILEGES TokenPrivileges;
            TokenPrivileges.PrivilegeCount = 1;
            TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            TokenPrivileges.Privileges[0].Luid = luid;
            AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, NULL, NULL);
        }
    }

    int nRet = 0;
    while(Process32Next(hprocessSnap,&pe32))
    {
        if(strcmp(szProcessName, pe32.szExeFile) == 0)
        {
            DWORD processId = pe32.th32ProcessID;
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
            DWORD  ret = TerminateProcess(hProcess,0);
            if(ret == 0)
            {
                cout << "kill faile: " << GetLastError() << endl;
            }
            CloseHandle(hProcess);
            nRet++;
        }
    }
    return nRet;
}

bool Processer::createProcess()
{
    STARTUPINFOA startInfo;

    DWORD dwCreationFlags = 0;
    if(startMode == 0) dwCreationFlags |= CREATE_NO_WINDOW;
    else if(startMode == 1) dwCreationFlags |= CREATE_NEW_CONSOLE;

    ZeroMemory(&startInfo, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    ZeroMemory(&Processinfo, sizeof(Processinfo));


    //CHAR *qs = "E:\\ICS_FES\\gcdaemon.exe";
    if( ! ::CreateProcessA( NULL, const_cast<char *>(commandLine.data()), NULL, NULL, FALSE, dwCreationFlags | NORMAL_PRIORITY_CLASS, NULL, NULL, &startInfo, &Processinfo ))
    {
        cout << "faile 创建进程失败： " << GetLastError() << endl;
        return 0;
    }
    sign = 1;
    startTimes++;
    cout << "以下是子进程的信息 :\n进程ID " << Processinfo.dwProcessId << "\n线程ID  : " << Processinfo.dwThreadId << "\n启动次数 startTimes : " << startTimes << endl;

    DWORD dwThreadId;
    isThreadWaitFinish = false;
    waitProcessThread = ::CreateThread(NULL, 0, ThreadWaitPrecess, this, 0, &dwThreadId);

    bool bSuccess = waitProcessThread != NULL;
    if (bSuccess) startTime = GlobalFunction::fn_currentMsepoch();
    return bSuccess;

}

void Processer::resetReceiveData()
{
    //初始化线程函数中用到的全局参数
    hasFirstReceive = false;
    isThreadReceiveError = false;
    isThreadReceiveCancel = false;
    lastReceiveTime = 0;
    notReceiveLong = 0;
}

void Processer::releaseSocket()
{
    if (socketThread)
    {
        ::CloseHandle(socketThread);
        socketThread = NULL;
    }
    if (socketId)
    {
        ::closesocket(socketId);
        socketId = 0;
    }
}

//接收
static DWORD WINAPI fn_threadReceive( LPVOID lpParam )
{
    Processer * oProcesser = (Processer * ) lpParam;
    //接收数据
    SOCKADDR_IN clientAddr;
    int nClientLen = sizeof(clientAddr);
    char buf[GM_BUFFER_SZIE];    //接收数据缓冲区
    ZeroMemory(buf, GM_BUFFER_SZIE);

    int nTimeRpt = oProcesser->timeOut_receive * 3;
    msepoch_t dtLastReceive = GlobalFunction::fn_currentMsepoch();
    int iReceiveTotal = 0;
    int iReceiveSize = 0;
    string sReceive;

    while (!oProcesser->isThreadReceiveCancel)
    {
        iReceiveSize = ::recvfrom(oProcesser->socketId, buf, GM_BUFFER_SZIE, 0, (SOCKADDR*)&clientAddr, &nClientLen);
        if(iReceiveSize == SOCKET_ERROR)
        {
            cout << oProcesser->exeFileName << " : " << "接收数据失败，失败原因: " << WSAGetLastError() << endl;
            oProcesser->isThreadReceiveError = true;
            return 1;
        }
        else if (iReceiveSize > 0)
        {
            sReceive.append(string(buf, iReceiveSize));
            if (sReceive.size() >= oProcesser->heartBuffer.size())
            {
                size_t found = sReceive.rfind(oProcesser->heartBuffer);
                if (found != string::npos)
                {
                    oProcesser->lastReceiveTime = GlobalFunction::fn_currentMsepoch();
                    oProcesser->notReceiveLong = 0;
                    oProcesser->hasFirstReceive = true;
                    iReceiveTotal += iReceiveSize;
                    if((oProcesser->lastReceiveTime - dtLastReceive) > nTimeRpt)
                    {
                        string sLastReceiveTime = GlobalFunction::fn_toString(oProcesser->lastReceiveTime);
                        cout << oProcesser->exeFileName << " : " ;
                        //printf("LastReceiveTime : %s ; TimeSpan : %lld ; ReceiveTotal : %d\n" , sLastReceiveTime.c_str() , (oProcesser->lastReceiveTime-dtLastReceive)/1000 , iReceiveTotal);
                        iReceiveTotal = 0;
                        dtLastReceive = oProcesser->lastReceiveTime;
                    }

                    sReceive = sReceive.substr(found + oProcesser->heartBuffer.size());
                }
                else
                {
                    sReceive.resize(oProcesser->heartBuffer.size()-1);
                }
            }
        }
    }
    return 0;
}

bool Processer::createSocket()
{
    Processer::resetReceiveData();
    //创建套接字
    socketId = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socketId == INVALID_SOCKET)
    {
        cout <<  "创建套接字失败，失败原因 ： " << ::WSAGetLastError() << endl;
        return false;
    }

    SOCKADDR_IN        servAddr;        //服务器地址
    //服务器地址
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = ::htons((short)localPort);            //端口
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);    //IP

    //绑定
    if (bind(socketId, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        cout << "绑定失败，失败原因 :  " << ::WSAGetLastError() << endl;
        Processer::releaseSocket();
        return false;
    }

    DWORD dwThreadId2;
    socketThread = ::CreateThread(
                NULL,              // default security attributes
                0,                 // use default stack size
                fn_threadReceive,          // thread function
                this,             // argument to thread function
                0,                 // use default creation flags
                &dwThreadId2);   // returns the thread identifier
    return socketThread != NULL;
}

void Processer::releaseProcess()
{
    if(0 == Processinfo.hProcess)
        return;
    TerminateProcess(Processinfo.hProcess, 0);
    WaitForSingleObject(Processinfo.hProcess, INFINITE);
    CloseHandle(waitProcessThread);
    CloseHandle(Processinfo.hProcess);
    CloseHandle(Processinfo.hThread);
    waitProcessThread = 0;
    Processinfo.hProcess = 0;
    Processinfo.hThread = 0;
    string sMsg = GlobalFunction::fn_format("子进程已经退出 : isThreadWaitFinish == %d, hasFirstReceive == %d, notReceiveLong == %d", (int)isThreadWaitFinish, (int)hasFirstReceive, notReceiveLong);
    //cxWarningtOut(sMsg);
}

void Processer::rerunProcess()
{
    releaseProcess();
    unusalCloseTime++;
    isThreadWaitFinish = false;
    notReceiveLong = 0;
    createProcess();
}


void Processer::restartSocket()
{
    releaseSocket();
    createSocket();
}
