#ifndef GETPROCESSMESSAGE_H
#define GETPROCESSMESSAGE_H

#include "global.h"

class Processer
{
public:
    inline static std::string attribute(const TiXmlElement * element, const std::string & sName) {
        if (element)
        {
            const char * pch = element->Attribute( sName.c_str() );
            if (pch)
            {
                return pch;
            }
        }
        return "";
    }

    inline static std::string text(const TiXmlElement * element) {
        if (element)
        {
            const char * pch = element->GetText();
            if (pch)
            {
                return pch;
            }
        }
        return "";
    }

    Processer(std::string, int, int, int, std::string, int, std::string, int);

    static std::vector<std::map<std::string, std::string> > analyseXml(const std::string configfilepath);

    static void prepareProcessInfo(const std::string filepath);

    static const std::vector<Processer*> & getProcesses();

    static bool vectorIsEmpty();

    static void loadTable1LevelByDocument(const TiXmlDocument &doc, std::vector<std::map<std::string, std::string> > & rows, const std::string & sTableName = "");

    static void loadRowByElement(const TiXmlElement *oElementLevel, std::map<std::string, std::string> & row);

    static void loadRowByAttribute(const TiXmlElement *oElementLevel, std::map<std::string, std::string> &row);

    static bool GetNodePointerByName(TiXmlElement *rootElement, const std::string &strMainNodeName, const std::string &strMainNodeAttributeName, const std::string &strNodeName, TiXmlElement *&Node);

    static bool DelNode(const std::string xmlFilePath, const std::string &strMainNodeName, const std::string &strMainNodeAttributeName, const std::string &strNodeName);

    static bool ModifyNode_Text(const std::string &xmlFile, const std::string &strMainNodeName, const std::string &strMainNodeAttributeName, const std::string &strNodeName, const std::string &strText);

    static bool AddNode(const std::string &xmlFilePath, const std::string &strMainNodeName, const std::string &strMainNodeAttributeName, const std::string &strNodeName, const std::string &strText);


    std::map<std::string, std::string> getProcessMessage(std::string exeName, HANDLE hProcess, std::string commandLine, std::string exeFilePath, DWORD dwprocessId, msepoch_t startTime, int startTimes, int unusalCloseTime);

    bool createProcess();
    int killProcess(const char *szProcessName);
    bool createSocket();
    void resetReceiveData();
    void releaseSocket();
    void releaseProcess();
    void rerunProcess();
    void restartSocket();

public:
    std::string exeFilePath;
    std::string exeFileName;
    std::string runParam;
    std::string heartBuffer;
    std::string heartHexString;
    std::string commandLine;

    int processType;
    int waitFirstMillisecond;
    int localPort;
    int startMode;
    int startTimes;
    int unusalCloseTime;
    unsigned int timeOut_receive;
    msepoch_t startTime;
    //进程
    PROCESS_INFORMATION Processinfo; //进程信息
    HANDLE waitProcessThread;
    volatile bool isThreadWaitFinish;

    //套接字
    SOCKET socketId;
    HANDLE socketThread;

    //接收数据
    volatile bool hasFirstReceive;
    volatile bool isThreadReceiveError;
    volatile bool isThreadReceiveCancel;
    msepoch_t lastReceiveTime;
    volatile unsigned int notReceiveLong;



    //std::vector<std::map<std::string, std::string> > xmlvector;



};

#endif // GETPROCESSMESSAGE_H
