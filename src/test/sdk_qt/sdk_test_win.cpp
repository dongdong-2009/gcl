#include "sdk_test_win.h"
#include "ui_sdktestwin.h"


#include "cxfile.h"


using namespace std;

const char * CS_XML_CHARS = "a~z!@#$%^*():'\\\"/1~0?[];lA~Z成功,.";

static int f_argc;
static const char ** f_argv = 0;

static int f_iYxCurrentValue = 0;
static double f_dYcCurrentValue = 0;
static int f_iYwCurrentValue = 0;
static int f_iShmCurrentMid = 0;

//--------------------------------------------------
//---------------gcl_sdk.dll------------------------
//--------------------------------------------------

#define MyEvent_MessageCommand (0x11)
#define MyEvent_FileTransfer (0x21)
#define MyEvent_RealtimeRequest (0x31)
#define MyEvent_RealtimePost (0x42)
#define MyEvent_Yn (0x50)
#define MyEvent_Yx1 (0x51)
#define MyEvent_Yc1 (0x52)
#define MyEvent_Yw1 (0x53)


SdkTestWin * f_win = NULL;

const std::string CS_EntryRemoteIpAddress = "RemoteIpAddress";
const std::string CS_EntryRemotePort     = "RemotePort";
const std::string CS_EntryLocalIpAddress = "LocalIpAddress";
const std::string CS_EntryLocalPort     = "LocalPort";

//在子线程中运行的
void fn_message_command( const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach )
{
    f_win->postMessageCommand(sCommand, sParam, iParamLength, oAttach);
}

void fn_file_transfer( const char * sFilePath, const PsmAttach * oAttach )
{
    f_win->postFile(sFilePath, oAttach);
}

void fn_realtime_data_request( const PsmAttach * oAttach )
{
    f_win->postRealtimeDataRequest(oAttach);
}

void fn_realtime_data_post( int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach )
{
    f_win->postRealtimeDataPost( iParamType, pData, iDataLength, iParamCount, oAttach );
}


qlonglong fn_dateTimeFromString(const QString &str)
{
    //    QString dts("2012/12/12 12:12:12");
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 6))
    QByteArray data = str.toLocal8Bit();
#else
    QByteArray data = str.toAscii();
#endif
    if (data.size() == 19)
    {

        int y, m, d, h, mi, s;

        const char zero = '0';

        y = (data[0] - zero) * 1000 + (data[1] - zero) * 100 + (data[2] - zero) * 10 + (data[3] - zero);
        m = (data[5] - zero) * 10 + (data[6] - zero);
        d = (data[8] - zero) * 10 + (data[9] - zero);
        h = (data[11] - zero) * 10 + (data[12] - zero);
        mi = (data[14] - zero) * 10 + (data[15] - zero);
        s = (data[17] - zero) * 10 + (data[18] - zero);

        QDate date(y, m, d);
        QTime time(h, mi, s);
        return QDateTime(date, time).toMSecsSinceEpoch();
    }
    else
    {
        return 0;
    }
}


///************ FromString begin
int fn_fromString(const QString& s, const int& iDefault, bool *ok){
    if (s.length()<=0)
        return iDefault;
    bool b;
    int v = s.toInt(&b, 10);
    if ( ok ) *ok = b;
    if (b)
        return v;
    else
        return iDefault;
}


bool fn_fromString(const QString& s, const bool& bDefault, bool *ok){
    if (s.length()<=0)
        return bDefault;
    bool b;
    int v = s.toInt(&b, 10);
    if ( ok ) *ok = b;
    if (b)
        return (bool)v;
    else
        return bDefault;
}


float fn_fromString(const QString& s, const float& fDefault, bool *ok){
    if (s.length()<=0)
        return fDefault;
    bool b;
    float v = s.toFloat(&b);
    if ( ok ) *ok = b;
    if (b)
        return v;
    else
        return fDefault;
}


double fn_fromString(const QString& s, const double& dDefault, bool *ok){
    if (s.length()<=0)
        return dDefault;
    bool b;
    double v = s.toDouble(&b);
    if ( ok ) *ok = b;
    if (b)
        return v;
    else
        return dDefault;
}


QString fn_fromString(const QString& s, const QString& sDefault, bool *ok){
    if (ok) *ok=true;
    return s;
}

qlonglong fn_fromString(const QString& s, const qlonglong& dtDefault, bool *ok)
{
    if (s.length()<=0)
    {
        return dtDefault;
    }
    qlonglong dt = fn_dateTimeFromString(s);
    if ( ok )
    {
        *ok = dt != 0;
    }
    return dt;
}



#define gm_fromString(value, s) \
{ \
    bool bOk = false; \
    value = CxString::fromString(s.toStdString(), value, &bOk); \
    }

#define gm_fromString_return(value, s) \
{ \
    bool bOk = false; \
    value = CxString::fromString(s.toStdString(), value, &bOk); \
    if (! bOk) \
    return; \
    }

#define gm_fromString_return_(value, s, r) \
{ \
    bool bOk = false; \
    value = CxString::fromString(s.toStdString(), value, &bOk); \
    if (! bOk) \
    return r; \
    }

#define gm_fromString_break(value, s) \
{ \
    bool bOk = false; \
    value = CxString::fromString(s.toStdString(), value, &bOk); \
    if (! bOk) \
    break; \
    }

#define gm_fromString_dt_break(value, s) \
{ \
    bool bOk = false; \
    value = fn_fromString(s, value, &bOk); \
    if (! bOk) \
    break; \
    }


template<typename _Key, typename _Tp>
bool fn_contain(std::map<_Key, _Tp> & map1, const _Key & key1)
{
    typename std::map<_Key,_Tp>::const_iterator it = map1.find(key1);
    if (it != map1.end())
        return true;
    else
        return false;
}

template<typename _Tp>
static int compare_order(const _Tp& v1, const _Tp& v2)
{
    return v2 > v1;
}

template<typename _Tp>
static int compare_reverse(const _Tp& v1, const _Tp& v2)
{
    return v2 < v1;
}

template<typename _Tp>
void fn_sort(std::vector<_Tp> & vec1, bool bOrder = true)
{
    if (bOrder)
        std::sort(vec1.begin(), vec1.end(), compare_order<_Tp>);
    else
        std::sort(vec1.begin(), vec1.end(), compare_reverse<_Tp>);
}


//from cplusplus.com sample
int fn_load(const string &sFileName, std::vector<std::string> & sOut, char cSplit)
{
    FILE * pFile;
    size_t iFileSize;
    size_t iOldSize = sOut.size();

    pFile = fopen ( sFileName.data() , "rb" );
    if (pFile==NULL)
    {
        return sOut.size() - iOldSize;
    }

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    iFileSize = ftell (pFile);
    if (iFileSize <= 0)
    {
        //        fputs ("file is empty",stderr);
        fclose (pFile);
        return sOut.size() - iOldSize;
    }
    rewind (pFile);

    const size_t ci_bufferSize = 512;
    vector<char> buffer(ci_bufferSize, 0);
    char * pBuffer = &(buffer.front());
    string sLine;
    size_t iReadSize = 0;
    while (iReadSize < iFileSize)
    {
        size_t iRemain = iFileSize - iReadSize;
        size_t iReadingLen = (iRemain > ci_bufferSize) ? ci_bufferSize : iRemain;

        // copy the file into the buffer:
        if ( fread (pBuffer,1,iReadingLen,pFile) != iReadingLen )
        {
            // read to file end
            fclose (pFile);
            return sOut.size() - iOldSize;
        }
        else
        {
            sLine.append(string(pBuffer, iReadingLen));

            size_t start = 0;
            size_t end;
            while ((end = sLine.find(cSplit, start)) != string::npos)
            {
                if (start != end)
                    sOut.push_back(sLine.substr(start, end - start));
                start = end + 1;
            }
            if (start > 0)
            {
                sLine = sLine.substr(start);
            }
        }
        iReadSize += iReadingLen;
    }
    if (sLine.size() > 0)
    {
        sOut.push_back(sLine);
    }

    /* the whole file is now loaded in the memory buffer. */
    fclose (pFile);
    return sOut.size() - iOldSize;
}

vector<string> fn_sectionNames(const std::map<std::string, std::map<std::string, std::string> > & sectionEntryValues, int iSort)
{
    vector<string> sectionNames;
    for (map<string, map<string, string> >::const_iterator it = sectionEntryValues.begin(); it != sectionEntryValues.end(); ++it)
    {
        sectionNames.push_back(it->first);
    }
    if (iSort > 0)
        fn_sort(sectionNames, true);
    else if (iSort < 0)
        fn_sort(sectionNames, false);
    return sectionNames;
}

bool fn_loadImpl(std::map<std::string, std::map<std::string, std::string> > & sectionEntryValues, const std::string& sFilePath)
{
    std::string _currentSection;
    std::string _currentEntry;
    std::string _currentValue;

    vector<string> sLines;
    if (fn_load(sFilePath, sLines, '\n'))
    {
        sectionEntryValues.clear();

        map<string, string> entrys;
        for (size_t i = 0; i < sLines.size(); ++i)
        {
            string sLine =  CxString::trim(sLines.at(i));
            switch (sLine[0])
            {
            case '[':
            {
                if (entrys.size() > 0 && _currentSection.size() > 0)
                {
                    sectionEntryValues[_currentSection] = entrys;
                }
                _currentSection = CxString::unquote(sLine, '[', ']');
                entrys = map<string, string>();
            }
                break;
            case '#':
            case ';':
            {
            }
                break;
            default:
            {
                bool bHas;
                _currentValue = CxString::tokenLeft(sLine, '=', &bHas);
                if (! bHas)
                {
                    break;
                }
                _currentValue = CxString::trim(_currentValue);
                _currentEntry = CxString::trim(sLine);
                if (_currentSection.size() > 0 && _currentEntry.size() > 0)
                {
                    entrys[_currentEntry] = _currentValue;
                }
            }
                break;
            }
        }
        if (entrys.size() > 0 && _currentSection.size() > 0)
        {
            sectionEntryValues[_currentSection] = entrys;
        }
        return true;
    }
    return false;
}

bool fn_saveImpl(const std::map<std::string, std::map<std::string, std::string> > & sectionEntryValues, const std::string& sFilePath)
{
    string sPath = CxFileSystem::extractPath(sFilePath);
    if (! CxFileSystem::isExist(sPath)) CxFileSystem::createDirMultiLevel(sPath);

    FILE * pFile;
    pFile = fopen (sFilePath.data(), "wb");
    size_t iWroteTotal = 0;
    if (pFile==NULL)
    {
        return false;
    }
    vector<string> sSectionNames = fn_sectionNames(sectionEntryValues, 1);
    for (size_t i = 0; i < sSectionNames.size(); ++i)
    {
        const string & sSectionName = sSectionNames.at(i);
        map<string, map<string, string> >::const_iterator it = sectionEntryValues.find(sSectionName);
        if (it != sectionEntryValues.end())
        {
            const map<string, string> & sEntrys = it->second;
            string sLine("\r\n[");
            sLine += sSectionName;
            sLine += "]\r\n";
            iWroteTotal += fwrite (const_cast<char *>(sLine.data()) , 1, sLine.size(), pFile);
            for (map<string, string>::const_iterator itEntry = sEntrys.begin(); itEntry != sEntrys.end(); ++itEntry)
            {
                sLine = itEntry->first + "=" + itEntry->second + "\r\n";
                size_t iWrote = fwrite (const_cast<char *>(sLine.data()) , 1, sLine.size(), pFile);
                if (iWrote != sLine.size())
                    return false;
                iWroteTotal += iWrote;
            }
        }
    }
    fclose (pFile);
    return iWroteTotal;
}

map<string, string> * fn_findEntryValues(std::map<std::string, std::map<std::string, std::string> > & sectionEntryValues, const string &sSection)
{
    map<string, map<string, string> >::iterator it = sectionEntryValues.find(sSection);
    if ( it != sectionEntryValues.end() )
    {
        return &(it->second);
    }
    return NULL;
}

string fn_findEntryValue(std::map<std::string, std::map<std::string, std::string> > & sectionEntryValues, const string &sEntry)
{
    for (map<string, map<string, string> >::const_iterator itSection = sectionEntryValues.begin(); itSection != sectionEntryValues.end(); ++itSection)
    {
        const string & sSection = itSection->first;
        const map<string, string> & sKvs = itSection->second;
        std::map<string, string>::const_iterator itEntry = sKvs.find(sEntry);
        if (itEntry != sKvs.end())
        {
            return itEntry->second;
        }
    }
    return "";
}

bool fn_setEntryValue(std::map<std::string, std::map<std::string, std::string> > & sectionEntryValues, const string &sEntry, const string &sValue)
{
    for (map<string, map<string, string> >::iterator itSection = sectionEntryValues.begin(); itSection != sectionEntryValues.end(); ++itSection)
    {
        const string & sSection = itSection->first;
        map<string, string> & sKvs = itSection->second;
        std::map<string, string>::iterator itEntry = sKvs.find(sEntry);
        if (itEntry != sKvs.end())
        {
            itEntry->second = sValue;
            return true;
        }
    }
    return false;
}

//--------------------------------------------------
//------------------SdkTestWin---------------------
//--------------------------------------------------



SdkTestWin::SdkTestWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SdkTestWin)
{
    ui->setupUi(this);

    connect(ui->sendFileBn_1, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_2, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_4, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_3, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_6, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_5, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_7, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_8, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_9, SIGNAL(clicked()), this, SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_10, SIGNAL(clicked()),this,  SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_11, SIGNAL(clicked()),this,  SLOT(on_sendFileBn_clicked()));
    connect(ui->sendFileBn_12, SIGNAL(clicked()),this,  SLOT(on_sendFileBn_clicked()));

    f_win = this;
    connect(this, SIGNAL(subThreadToMainThread(int)), this, SLOT(recvSubThreadToMainThread(int)));

    _timer = new QTimer();
    connect(_timer, SIGNAL(timeout()), SLOT(timeOutSlot()));
    _timer->setInterval(10);
    _timerTimes = 0;
    _timer->start();

    _timerWait = new QTimer();
    connect(_timerWait, SIGNAL(timeout()), SLOT(doAfterOpenSdk()));
    _timerWait->setInterval(30);
    _timerWaitTimes = 0;

    refreshSendYxTb(ui->sendYxTb, 100);
    refreshSendYcTb(ui->sendYcTb, 100);
    refreshSendYwTb(ui->sendYwTb, 20);
    refreshMidTb(ui->shmGetTb, 100);

    ui->tabWidget->setCurrentIndex(0);

    _tableMenu = new QMenu(this);
    _setIncreaseIntegerAction = new QAction("增量设置正数值...", this);
    connect(_setIncreaseIntegerAction, SIGNAL(triggered()), this, SLOT(setIncreaseIntegerSlot()));
    _setIncreaseDoubleAction = new QAction("增量设置小数值...", this);
    connect(_setIncreaseDoubleAction, SIGNAL(triggered()), this, SLOT(setIncreaseDoubleSlot()));

    QTableView * oTableView;
    oTableView = ui->sendYxTb;
    oTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(oTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    oTableView = ui->sendYcTb;
    oTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(oTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    oTableView = ui->sendYwTb;
    oTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(oTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    oTableView = ui->shmGetTb;
    oTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(oTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    _outInfoLineCount = 0;

//#ifdef GM_GCL_GA
//    ui->configFilePathEd->setText( QApplication::applicationDirPath() + "/gcl_sdk/config/apps.config.ini" );
//#else
//    ui->configFilePathEd->setText( QApplication::applicationDirPath() + "/gcl_sdk/terminal/psm_sdk.ini" );
//#endif
//    loadParamsFromConfigFile();
    ui->groupBox->setVisible(false);

    ui->test1Bn->setVisible(false);
    ui->test2Bn->setVisible(false);

    //
    string r = sdk_init();
    outInfo( r.c_str() ) ;

    _timerWaitTimes = 0;
    _timerWait->start();
}

void SdkTestWin::closeEvent( QCloseEvent * )
{
    gci_cleanup();
}

SdkTestWin::~SdkTestWin()
{
    _timerWait->stop();
    _timer->stop();
    delete ui;
    delete _timer;
    delete _timerWait;
}

void SdkTestWin::outInfo(const char *sInfo)
{
    if (_outInfoLineCount % 500 == 0)
        ui->outInfoEd->clear();

    ui->outInfoEd->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
    ui->outInfoEd->append(QString(sInfo));
    ui->outInfoEd->append(QString(""));

    ++_outInfoLineCount;
}

void SdkTestWin::outInfo(const string &sInfo)
{
    if (_outInfoLineCount % 500 == 0)
        ui->outInfoEd->clear();

    ui->outInfoEd->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
    ui->outInfoEd->append(QString(sInfo.data()));
    ui->outInfoEd->append(QString(""));

    ++_outInfoLineCount;
}

void SdkTestWin::outInfo(const QString &sInfo)
{
    if (_outInfoLineCount % 500 == 0)
        ui->outInfoEd->clear();

    ui->outInfoEd->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
    ui->outInfoEd->append(sInfo);
    ui->outInfoEd->append(QString(""));

    ++_outInfoLineCount;
}

void SdkTestWin::sendSubThreadToMainThread(int iEvent)
{
    emit subThreadToMainThread(iEvent);
}

int SdkTestWin::postMessageCommand(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    if (! oAttach)
        return FALSE;
    if (1>0)
    {
        MyMutexScope lock(_locker);
        MyMessageCommand messageCommand;
        messageCommand.command = string(sCommand);
        messageCommand.param = string(sParam, sParam + iParamLength);
        messageCommand.attach = * oAttach;
        messageCommand.isValid = true;
        _messageCommands.push(messageCommand);
    }
    sendSubThreadToMainThread(MyEvent_MessageCommand);
    return TRUE;
}

int SdkTestWin::postFile(const char *sFilePath, const PsmAttach *oAttach)
{
    if (! oAttach)
        return FALSE;
    if (1>0)
    {
        MyMutexScope lock(_locker);
        MyFileTransfer fileTransfer;
        fileTransfer.filePath = string(sFilePath);
        fileTransfer.attach = * oAttach;
        fileTransfer.isValid = true;
        _fileTransfers.push(fileTransfer);
    }
    sendSubThreadToMainThread(MyEvent_FileTransfer);
    return TRUE;
}

int SdkTestWin::postRealtimeDataRequest(const PsmAttach *oAttach)
{
    if (! oAttach)
        return FALSE;
    if (1>0)
    {
        MyMutexScope lock(_locker);
        MyRealtimeDataRequest realtimeDataRequest;
        realtimeDataRequest.attach = * oAttach;
        realtimeDataRequest.isValid = true;
        _realtimeDataRequests.push(realtimeDataRequest);
    }
    sendSubThreadToMainThread(MyEvent_RealtimeRequest);
    return TRUE;
}

int SdkTestWin::postRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    if (! oAttach)
        return FALSE;
    if (1>0)
    {
        MyMutexScope lock(_locker);
        MyRealtimeDataPost realtimeDataPost;
        realtimeDataPost.paramType = iParamType;
        realtimeDataPost.paramCount = iParamCount;
        realtimeDataPost.param = string(pData, iDataLength);
        realtimeDataPost.attach = * oAttach;
        realtimeDataPost.isValid = true;
        _realtimeDataPosts.push(realtimeDataPost);
    }
    sendSubThreadToMainThread(MyEvent_RealtimePost);
    return TRUE;
}

void SdkTestWin::setArg(int argc, const char *argv[])
{
    f_argc = argc;
    f_argv = argv;
}

void SdkTestWin::doSetLocalParams()
{
    string sTerminalInfo(gci_terminal_info());

    outInfo(sTerminalInfo);

    string sPsmSentSourceId = CxString::unquote(sTerminalInfo, CS_EntryPsmSentSourceId+"=", "\n");
    string sPsmSentTargetId = CxString::unquote(sTerminalInfo, CS_EntryPsmSentTargetId+"=", "\n");
    string sPsmSentReason = CxString::unquote(sTerminalInfo, CS_EntryPsmSentReason+"=", "\n");

    int iPsmSentSourceId = CxString::fromString( sPsmSentSourceId, (int)0 );
    int iPsmSentTargetId = CxString::fromString( sPsmSentTargetId, (int)0 );
    int iPsmSentReason   = CxString::fromString( sPsmSentReason  , (int)0 );

    ui->SourceTerminalIdEd->setText( SdkCommon::gbkToQString( CxString::toHexstring(iPsmSentSourceId) ) );
    ui->TargetTerminalIdEd->setText( SdkCommon::gbkToQString( CxString::toHexstring(iPsmSentTargetId) ) );
    ui->ReasonEd->setText( SdkCommon::gbkToQString( CxString::toHexstring(iPsmSentReason) ) );

    string sInfo =
            "LocalIp=" + CxString::unquote(sTerminalInfo, CS_EntryLocalIpAddress+"=", "\n") + "\r\n"
            + "LocalPort=" + CxString::unquote(sTerminalInfo, CS_EntryLocalPort+"=", "\n") + "\r\n"
            + "RemoteIp=" + CxString::unquote(sTerminalInfo, CS_EntryRemoteIpAddress+"=", "\n") + "\r\n"
            + "RemotePort=" + CxString::unquote(sTerminalInfo, CS_EntryRemotePort+"=", "\n");

    outInfo(sInfo);

    ui->infoFixLb->setText(QString(sInfo.data()));
}

string SdkTestWin::getSdkState()
{
    string s = CxTime::toStringSepoch(gci_terminal_online_millisecond());
    return CxString::format("recv t: %s", s.c_str());
}

PsmAttach *SdkTestWin::getSendAttach()
{
    gm_fromString_return_(_sendAttach.sourceId, ui->SourceTerminalIdEd->text(), NULL);
    gm_fromString_return_(_sendAttach.targetId, ui->TargetTerminalIdEd->text(), NULL);
    gm_fromString_return_(_sendAttach.reason, ui->ReasonEd->text(), NULL);
    if (_sendAttach.sourceId >= 0 && _sendAttach.targetId >= 0 && _sendAttach.reason >= 0)
    {
        if (_sendAttach.sourceId > 0 || _sendAttach.targetId > 0 || _sendAttach.reason > 0)
        {
            return &_sendAttach;
        }
    }
    return NULL;
}

void SdkTestWin::refreshSendYxTb(QTableWidget* oTableWidget, int iRowCount)
{
    QTableWidget* oGrid = oTableWidget;
    if (oGrid->rowCount() != iRowCount){
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("No");
        sHeaders.append("MeasureId");
        sHeaders.append("Value");
        sHeaders.append("Quality");
        sHeaders.append("ChangedTime");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
        for (int i = 0; i < iRowCount; ++i) {
            QString sText;
            QTableWidgetItem* oItem;
            //序号
            sText = QString::number(i);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 0, oItem);
            //MeasureId
            sText = QString::number(i + 0x01000000);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 1, oItem);
            //MonsbValue
            sText = QString::number(i % 3);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 2, oItem);
            //MonsbMinValue
            sText = QString::number(1);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 3, oItem);
            //MonsbMaxValue
            sText = QDateTime::currentDateTime().toString(cs_datetime_format_);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 4, oItem);
        }
    }
}

void SdkTestWin::refreshSendYcTb(QTableWidget* oTableWidget, int iRowCount)
{
    QTableWidget* oGrid = oTableWidget;
    if (oGrid->rowCount() != iRowCount){
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("No");
        sHeaders.append("MeasureId");
        sHeaders.append("Value");
        sHeaders.append("Quality");
        sHeaders.append("Time");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
        for (int i = 0; i < iRowCount; ++i) {
            QString sText;
            QTableWidgetItem* oItem;
            //序号
            sText = QString::number(i);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 0, oItem);
            //MeasureId
            sText = QString::number(i + 0x02000000);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 1, oItem);
            //Value
            sText = QString::number((double)i+0.001*i);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 2, oItem);
            //Quality
            sText = QString::number(1);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 3, oItem);
            //ChangedTime
            sText = QDateTime::currentDateTime().toString(cs_datetime_format_);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 4, oItem);
        }
    }
}

void SdkTestWin::refreshSendYwTb(QTableWidget* oTableWidget, int iRowCount)
{
    QTableWidget* oGrid = oTableWidget;
    if (oGrid->rowCount() != iRowCount){
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
		sHeaders.append("No");
		sHeaders.append("MeasureId");
		sHeaders.append("Value");
		sHeaders.append("Quality");
		sHeaders.append("Time");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
        for (int i = 0; i < iRowCount; ++i) {
            QString sText;
            QTableWidgetItem* oItem;
            //序号
            sText = QString::number(i);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 0, oItem);
            //MeasureId
            sText = QString::number(i + 0x03000000);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 1, oItem);
            //MonsbValue
            sText = QString::number(i)+CS_XML_CHARS;
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 2, oItem);
            //MonsbMinValue
            sText = QString::number(1);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 3, oItem);
            //MonsbMaxValue
            sText = QDateTime::currentDateTime().toString(cs_datetime_format_);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(i, 4, oItem);
        }
    }
}

void SdkTestWin::refreshMidTb(QTableWidget *oTableWidget, int iRowCount)
{
    QTableWidget* oGrid = oTableWidget;
    if (oGrid->columnCount()<=0)
    {
        QStringList sHeaders;
        sHeaders.append("No");
        sHeaders.append("MeasureId");
        sHeaders.append("Value");
        sHeaders.append("Quality");
        sHeaders.append("Time");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
    }
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        int iRowIndex = 0;
        for (int i = 0; i < iRowCount; ++i)
        {
            QString sText;
            QTableWidgetItem* oItem;
            //序号
            sText = QString::number(i);
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRowIndex, 0, oItem);
            //MeasureId
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRowIndex, 1, oItem);
            fn_setItemValue(oItem, int(0));
            //值
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 2, oItem);
            fn_setItemValue(oItem, "NULL");
            //品质
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 3, oItem);
            fn_setItemValue(oItem, int(0));
            //修改时间
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 4, oItem);
            fn_setItemValueDateTime(oItem, 0);

            ++iRowIndex;
        }
    }
}

void SdkTestWin::refreshMidTb(QTableWidget *oTableWidget, int iRowIndex, int iMeasureId, const string &sValue, int q, long long t)
{
    QTableWidget* oGrid = oTableWidget;
    int iRowCount = iRowIndex+1;
    if (iRowCount > oGrid->rowCount())
    {
        return;
    }

    QTableWidgetItem* oItem;

    //MeasureId
    oItem = oGrid->item(iRowIndex, 1);
    fn_setItemValue(oItem, iMeasureId);
    //值
    oItem = oGrid->item(iRowIndex, 2);
    fn_setItemValue(oItem, sValue.c_str());
    //品质
    oItem = oGrid->item(iRowIndex, 3);
    fn_setItemValue(oItem, q);
    //修改时间
    oItem = oGrid->item(iRowIndex, 4);
    fn_setItemValueDateTime(oItem, t);
}


void SdkTestWin::timeOutSlot()
{
    _timerTimes += 10;
    QString sqNow = QDateTime::currentDateTime().toString(cs_datetime_format_);
    if (ui->timerMessageCb->isChecked() && _timerTimes % ui->timerMessageEd->value() == 0)
    {
        on_sendMessageBn_clicked();
    }
    if (ui->timerFileCb->isChecked() && _timerTimes % ui->timerFileEd->value() == 0)
    {
        if (ui->timerFileChangCb->isChecked())
        {
            static int iFileNo = 0;
            iFileNo++;
            if (iFileNo>12)
                iFileNo = 0;
            postFileByLineEdit(iFileNo);
        }
        else
        {
            postFileByLineEdit(1);
        }
    }
    if (ui->timerYxCb->isChecked() && _timerTimes % ui->timerYxEd->value() == 0)
    {
        if (ui->timerYxChangEd->value() > 0)
        {

            QTableWidget * oTableView = ui->sendYxTb;
            for (int i = 0; i < oTableView->rowCount(); ++i)
            {
                QTableWidgetItem * oItem = oTableView->item(i, 2);
                oItem->setText(QString::number(f_iYxCurrentValue));
                QTableWidgetItem * oItemDt = oTableView->item(i, 4);
                oItemDt->setText(sqNow);
                f_iYxCurrentValue += ui->timerYxChangEd->value();
            }
        }
        on_sendYxBn_clicked();
    }
    if (ui->randomYxCb->isChecked() && _timerTimes % ui->timerYxEd->value() == 0)
    {
        int iMinNum = ui->randomYxChangMin->value();
        int iMaxNum = ui->randomYxChangMax->value();

        QTableWidget * oTableView = ui->sendYxTb;
        for (int i = 0; i < oTableView->rowCount(); ++i)
        {
            //srand(i);
            int iValue = iMinNum + rand()%(iMaxNum-iMinNum+1);
            QTableWidgetItem * oItem = oTableView->item(i, 2);
            oItem->setText(QString::number(iValue));
            QTableWidgetItem * oItemDt = oTableView->item(i, 4);
            oItemDt->setText(sqNow);
            f_iYxCurrentValue += ui->timerYxChangEd->value();
        }
        on_sendYxBn_clicked();
    }

    if (ui->randomYcCb->isChecked() && _timerTimes % ui->timerYcEd->value() == 0)
    {
        double iMinNum = ui->randomYcChangMin->value();
        double iMaxNum = ui->randomYcChangMax->value();

        QTableWidget * oTableView = ui->sendYcTb;
        for (int i = 0; i < oTableView->rowCount(); ++i)
        {
            //srand(i);
            double iValue =  (double)((rand()*1.0f)/(RAND_MAX*1.0f))*(iMaxNum-iMinNum) + iMinNum;
            QTableWidgetItem * oItem = oTableView->item(i, 2);
            oItem->setText(QString::number(iValue));
            QTableWidgetItem * oItemDt = oTableView->item(i, 4);
            oItemDt->setText(sqNow);
            f_iYxCurrentValue += ui->timerYcChangEd->value();
        }
        on_sendYcBn_clicked();
    }

    if (ui->timerYcCb->isChecked() && _timerTimes % ui->timerYcEd->value() == 0)
    {
        if (ui->timerYcChangEd->value() > 0.001)
        {
            QTableWidget * oTableView = ui->sendYcTb;
            for (int i = 0; i < oTableView->rowCount(); ++i)
            {
                QTableWidgetItem * oItem = oTableView->item(i, 2);
                oItem->setText(QString::number(f_dYcCurrentValue));
                QTableWidgetItem * oItemDt = oTableView->item(i, 4);
                oItemDt->setText(sqNow);
                f_dYcCurrentValue += ui->timerYcChangEd->value();
            }
        }
        on_sendYcBn_clicked();
    }
    if (ui->timerYwCb->isChecked() && _timerTimes % ui->timerYwEd->value() == 0)
    {
        if (ui->timerYwChangEd->value() > 0)
        {
            QTableWidget * oTableView = ui->sendYwTb;
            for (int i = 0; i < oTableView->rowCount(); ++i)
            {
                QTableWidgetItem * oItem = oTableView->item(i, 2);
                oItem->setText(QString::number(f_iYwCurrentValue)+CS_XML_CHARS);
                QTableWidgetItem * oItemDt = oTableView->item(i, 4);
                oItemDt->setText(sqNow);
                f_iYwCurrentValue += ui->timerYwChangEd->value();
            }
        }
        on_sendYwBn_clicked();
    }
    if (ui->timerShmGetCb->isChecked() && _timerTimes % ui->timerShmGetEd->value() == 0)
    {
        if (ui->timerShmGetChangEd1->value() > 0 && ui->timerShmGetChangEd2->value() > 0)
        {
            QTableWidget * oTableView = ui->shmGetTb;
            int iInc = ui->timerShmGetChangEd2->value();
            for (int i = 0; i < oTableView->rowCount(); ++i)
            {
                QTableWidgetItem * oItem = oTableView->item(i, 1);
                oItem->setText(QString::number(f_iShmCurrentMid)); f_iShmCurrentMid+=iInc;
            }
        }
        on_shmGetBn_clicked();
    }
    if (_timerTimes % 1000 == 0)
    {
        ui->sdkStateStringLb->setText(QString(getSdkState().data()));
    }
    if (_timerTimes % 2000 == 0)
    {
        ui->currentDtEd->setText(QString::number(CxTime::currentMsepoch()));
    }
}

void SdkTestWin::customContextMenu(const QPoint &pos)
{
    QTableView * oTableView;
    if (ui->tabWidget->currentIndex() == 3)
    {
        oTableView = ui->sendYxTb;
    }
    else if (ui->tabWidget->currentIndex() == 5)
    {
        oTableView = ui->sendYcTb;
    }
    else if (ui->tabWidget->currentIndex() == 7)
    {
        oTableView = ui->sendYwTb;
    }
    else if (ui->tabWidget->currentIndex() == 8)
    {
        oTableView = ui->shmGetTb;
    }
    else
    {
        return;
    }

    QStandardItemModel * _tableItemModel = (QStandardItemModel *)oTableView->model();

    QModelIndex posModelIndex = oTableView->indexAt(pos);
    //获取当前被点击的节点
    if (posModelIndex.column() > 0)
    {
        _tableMenu->clear();
        _tableMenu->addAction(_setIncreaseIntegerAction);
        if (posModelIndex.column() == 2 && ui->tabWidget->currentIndex() == 5)
        {
            _tableMenu->addAction(_setIncreaseDoubleAction);
        }
        _tableMenu->exec(QCursor::pos());
    }
}

void SdkTestWin::setIncreaseIntegerSlot()
{
    QTableWidget * oTableView;
    if (ui->tabWidget->currentIndex() == 3)
    {
        oTableView = ui->sendYxTb;
    }
    else if (ui->tabWidget->currentIndex() == 5)
    {
        oTableView = ui->sendYcTb;
    }
    else if (ui->tabWidget->currentIndex() == 7)
    {
        oTableView = ui->sendYwTb;
    }
    else if (ui->tabWidget->currentIndex() == 8)
    {
        oTableView = ui->shmGetTb;
    }
    else
    {
        return;
    }
    QString sValue1;
    QString sValue2;
    if (! SdkCommon::DialogInput2("Please-Input", "Start:", "Step:", sValue1, sValue2))
    {
        return;
    }

    long long iValue1;
    long long iValue2;
    gm_fromString_return(iValue1, sValue1);
    gm_fromString_return(iValue2, sValue2);
    QItemSelectionModel * selections = oTableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    foreach (QModelIndex index, selected)
    {
        QTableWidgetItem * oItem = oTableView->item(index.row(), index.column());
        if (! oItem) continue;
        if (index.column()==1 || index.column()==2 || index.column()==3)
        {
            oItem->setText(QString::number(iValue1));
        }
        else if (index.column()==4)
        {
            oItem->setText(QDateTime::fromMSecsSinceEpoch(iValue1).toString(cs_datetime_format_));
        }
        iValue1 += iValue2;
    }
}

void SdkTestWin::setIncreaseDoubleSlot()
{
    QTableWidget * oTableView;
    if (ui->tabWidget->currentIndex() == 3)
    {
        oTableView = ui->sendYxTb;
    }
    else if (ui->tabWidget->currentIndex() == 5)
    {
        oTableView = ui->sendYcTb;
    }
    else if (ui->tabWidget->currentIndex() == 7)
    {
        oTableView = ui->sendYwTb;
    }
    else if (ui->tabWidget->currentIndex() == 8)
    {
        oTableView = ui->shmGetTb;
    }
    else
    {
        return;
    }
    QString sValue1;
    QString sValue2;
	if (!SdkCommon::DialogInput2("Please-Input", "Start:", "Step:", sValue1, sValue2))
    {
        return;
    }

    double iValue1;
    double iValue2;
    gm_fromString_return(iValue1, sValue1);
    gm_fromString_return(iValue2, sValue2);
    QItemSelectionModel * selections = oTableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    foreach (QModelIndex index, selected)
    {
        QTableWidgetItem * oItem = oTableView->item(index.row(), index.column());
        if (! oItem) continue;
        if (index.column()==2)
        {
            oItem->setText(QString::number(iValue1));
        }
        iValue1 += iValue2;
    }
}

void SdkTestWin::recvSubThreadToMainThread(int iEvent)
{
    switch (iEvent)
    {
    case MyEvent_MessageCommand:
    {
        MyMessageCommand messageCommand;
        do
        {
            messageCommand.isValid = false;

            {
                MyMutexScope lock(_locker);
                if (_messageCommands.size()>0)
                {
                    messageCommand = _messageCommands.front();
                    _messageCommands.pop();
                }
            }

            if (messageCommand.isValid)
            {
                string sMsg = CxString::format("Received Sdk Message [Command][Param]: \n%s\n%s\nSourceId=%d;TargetId=%d;", messageCommand.command.c_str(), messageCommand.param.c_str(), messageCommand.attach.sourceId, messageCommand.attach.targetId);
                f_win->outInfo(sMsg.c_str());
            }
        } while (messageCommand.isValid);
    }
        break;

    case MyEvent_FileTransfer:
    {
        MyFileTransfer fileTransfer;
        do
        {
            fileTransfer.isValid = false;

            {
                MyMutexScope lock(_locker);
                if (_fileTransfers.size()>0)
                {
                    fileTransfer = _fileTransfers.front();
                    _fileTransfers.pop();
                }
            }

            if (fileTransfer.isValid)
            {
                string sMsg = CxString::format("Received Sdk File [FilePath]: \n%s\nSourceId=%d;TargetId=%d;", fileTransfer.filePath.c_str(), fileTransfer.attach.sourceId, fileTransfer.attach.targetId);
                f_win->outInfo(sMsg.c_str());
            }
        } while (fileTransfer.isValid);
    }
        break;

    case MyEvent_RealtimeRequest:
    {
        MyRealtimeDataRequest realtimeDataRequest;
        do
        {
            realtimeDataRequest.isValid = false;

            {
                MyMutexScope lock(_locker);
                if (_realtimeDataRequests.size()>0)
                {
                    realtimeDataRequest = _realtimeDataRequests.front();
                    _realtimeDataRequests.pop();
                }
            }

            if (realtimeDataRequest.isValid)
            {
                string sMsg = CxString::format("Received Sdk Realtime Data Request: \nSourceId=%d;TargetId=%d;ContainerId=%d;", realtimeDataRequest.attach.sourceId, realtimeDataRequest.attach.targetId, realtimeDataRequest.attach.containerId);
                f_win->outInfo(sMsg.c_str());
            }
        } while (realtimeDataRequest.isValid);
    }
        break;

    case MyEvent_RealtimePost:
    {
        MyRealtimeDataPost realtimeDataPost;
        do
        {
            realtimeDataPost.isValid = false;

            {
                MyMutexScope lock(_locker);
                if (_realtimeDataPosts.size()>0)
                {
                    realtimeDataPost = _realtimeDataPosts.front();
                    _realtimeDataPosts.pop();
                }
            }

            if (realtimeDataPost.isValid)
            {
                if (realtimeDataPost.paramType == gcd_address_int32_vqt_t::getType() || realtimeDataPost.paramType == gcd_address_vqt_intText1_t::getType())
                {
                    dealRealtimeDataPost(MyEvent_Yx1, SdkCommon::dealYx_01(realtimeDataPost.paramType, realtimeDataPost.param.data(), realtimeDataPost.param.size(), realtimeDataPost.paramCount, &realtimeDataPost.attach));
                    return;
                }
                else if(realtimeDataPost.paramType == gcd_firstAddress_vqt_intText1_t::getType())
                {
                    SdkCommon::dealYx_02(realtimeDataPost.paramType, realtimeDataPost.param.data(), realtimeDataPost.param.size(), realtimeDataPost.paramCount, &realtimeDataPost.attach);
                }
                else if(realtimeDataPost.paramType == gcd_address_double_vqt_t::getType() || realtimeDataPost.paramType == gcd_address_vqt_realText15_t::getType())
                {
                    dealRealtimeDataPost(MyEvent_Yc1, SdkCommon::dealYc_01(realtimeDataPost.paramType, realtimeDataPost.param.data(), realtimeDataPost.param.size(), realtimeDataPost.paramCount, &realtimeDataPost.attach));
                    return;
                }
                else if(realtimeDataPost.paramType == gcd_firstAddress_vqt_realText15_t::getType())
                {
                    SdkCommon::dealYc_02(realtimeDataPost.paramType, realtimeDataPost.param.data(), realtimeDataPost.param.size(), realtimeDataPost.paramCount, &realtimeDataPost.attach);
                }
                else if(realtimeDataPost.paramType == gcd_address_vqt_string128_t::getType())
                {
                    dealRealtimeDataPost(MyEvent_Yw1, SdkCommon::dealYw_01(realtimeDataPost.paramType, realtimeDataPost.param.data(), realtimeDataPost.param.size(), realtimeDataPost.paramCount, &realtimeDataPost.attach));
                    return;
                }
                else
                {
                    dealRealtimeDataPost(MyEvent_Yn, vector<int>());
                }
                string sMsg = CxString::format("Received Sdk Realtime Data Post: \nSourceId=%d;TargetId=%d;ParamType=%d;ParamCount=%d;Size=%d;", realtimeDataPost.attach.sourceId, realtimeDataPost.attach.targetId, realtimeDataPost.paramType, realtimeDataPost.paramCount, realtimeDataPost.param.size());
                f_win->outInfo(sMsg.c_str());
            }
        } while (realtimeDataPost.isValid);
    }
        break;

    default:
        break;
    }
}

void SdkTestWin::dealRealtimeDataPost(int iType, const std::vector<int> &addresses)
{
    switch (iType)
    {
    case MyEvent_Yx1:
    {
        string sMsg = "Received Sdk Realtime Data Yx -addresses : \n";
        for (size_t i = 0; i < addresses.size(); ++i)
        {
            sMsg += CxString::toString(addresses.at(i)) + " ";
        }
        f_win->outInfo(sMsg.c_str());

        QTableWidget* oGrid = ui->recvYxTb;
        const vector<yx_t> & yxs = SdkCommon::yxs();
        MeasureWin::reportMonsbManager(oGrid, yxs);
    }
        break;

    case MyEvent_Yc1:
    {
        string sMsg = "Received Sdk Realtime Data Yc -addresses : \n";
        for (size_t i = 0; i < addresses.size(); ++i)
        {
            sMsg += CxString::toString(addresses.at(i)) + " ";
        }
        f_win->outInfo(sMsg.c_str());

        QTableWidget* oGrid = ui->recvYcTb;
        const vector<yc_t> & ycs = SdkCommon::ycs();
        MeasureWin::reportYcaddManager(oGrid, ycs);
    }
        break;

    case MyEvent_Yw1:
    {
        string sMsg = "Received Sdk Realtime Data Yw -addresses : \n";
        for (size_t i = 0; i < addresses.size(); ++i)
        {
            sMsg += CxString::toString(addresses.at(i)) + " ";
        }
        f_win->outInfo(sMsg.c_str());

        QTableWidget* oGrid = ui->recvYwTb;
        const vector<yw_t> & yws = SdkCommon::yws();
        MeasureWin::reportStrawManager(oGrid, yws);
    }
        break;

    case MyEvent_Yn:
    {
        string sMsg = "Received Sdk Realtime Data ( but not deal ) !!!";
        f_win->outInfo(sMsg.c_str());
    }
        break;
    default:
        break;
    }
}

string SdkTestWin::sdk_init()
{
    string r;
    int iResult = gci_init(f_argc, (char **)f_argv);
    r.append("gci_init : " + CxString::toString(iResult) + "\n");
    if (iResult > 0)
    {
        //message
        iResult = gci_register_message_command_notify(fn_message_command);
        r.append("gci_register_message_command_notify : " + CxString::toString(iResult) + "\n");
        //file
        iResult = gci_register_file_transfer_notify(fn_file_transfer);
        r.append("gci_register_file_transfer_notify : " + CxString::toString(iResult) + "\n");
        //realtime data request
        iResult = gci_register_realtime_data_request_notify(fn_realtime_data_request);
        r.append("gci_register_realtime_data_request_notify : " + CxString::toString(iResult) + "\n");
        //realtime data post
        iResult = gci_register_realtime_data_post_notify(fn_realtime_data_post);
        r.append("gci_register_realtime_data_post_notify : " + CxString::toString(iResult) + "\n");

        ui->sdkInitBn->setEnabled(false);
        ui->remoteIpAddressEd->setEnabled(false);
        ui->remotePortEd->setEnabled(false);
        ui->localIpAddressEd->setEnabled(false);
        ui->localPortEd->setEnabled(false);
    }
    return r;
}

void SdkTestWin::loadParamsFromConfigFile()
{
    string sFilePath = SdkCommon::gbkToStdString( ui->configFilePathEd->text() );
    bool bOk = fn_loadImpl(_configSectionEntryValues, sFilePath);
    if (_configSectionEntryValues.empty()) return;

    int _cfgSentReason = 0;
    int _cfgSentSourceId = 0;
    int _cfgSentTargetId = 0;
    int _cfgHeartJumpInterval;
    _cfgSentReason = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryPsmSentReason), _cfgSentReason);
    _cfgSentSourceId = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryPsmStationNumSource), _cfgSentSourceId);
    if (_cfgSentSourceId == 0)
        _cfgSentSourceId = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryPsmSentSourceId), _cfgSentSourceId);
    _cfgSentTargetId = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryPsmStationNumTarget), _cfgSentTargetId);
    if (_cfgSentTargetId == 0)
        _cfgSentTargetId = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryPsmSentTargetId), _cfgSentTargetId);
    _cfgHeartJumpInterval = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryPsmHeartJumpInterval), _cfgHeartJumpInterval);

    ui->SourceTerminalIdEd->setText( SdkCommon::gbkToQString( CxString::toHexstring(_cfgSentSourceId) ) );
    ui->TargetTerminalIdEd->setText( SdkCommon::gbkToQString( CxString::toHexstring(_cfgSentTargetId) ) );
    ui->ReasonEd->setText( SdkCommon::gbkToQString( CxString::toHexstring(_cfgSentReason) ) );

    string _cfgRemoteIp;
    int _cfgRemotePort = 0;
    string _cfgLocalIp;
    int _cfgLocalPort = 0;

    _cfgRemoteIp = fn_findEntryValue(_configSectionEntryValues, CS_EntryRemoteIpAddress);
    _cfgRemotePort = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryRemotePort), _cfgRemotePort);
    _cfgLocalIp = fn_findEntryValue(_configSectionEntryValues, CS_EntryLocalIpAddress);
    _cfgLocalPort = CxString::fromString(fn_findEntryValue(_configSectionEntryValues, CS_EntryLocalPort), _cfgLocalPort);

    ui->remoteIpAddressEd->setText( SdkCommon::gbkToQString( _cfgRemoteIp ) );
    ui->remotePortEd->setValue( _cfgRemotePort );
    ui->localIpAddressEd->setText( SdkCommon::gbkToQString( _cfgLocalIp ) );
    ui->localPortEd->setValue( _cfgLocalPort );

    outInfo("load config from file : " + sFilePath + " " + CxString::toString(_configSectionEntryValues.size()));
}

void SdkTestWin::saveParamsToConfigFile()
{
    bool bOk;
    CxString::fromString(SdkCommon::gbkToStdString(ui->SourceTerminalIdEd->text()), int(0), &bOk);
    if (bOk)
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryPsmStationNumSource, SdkCommon::gbkToStdString(ui->SourceTerminalIdEd->text()));
        fn_setEntryValue(_configSectionEntryValues, CS_EntryPsmSentSourceId, SdkCommon::gbkToStdString(ui->SourceTerminalIdEd->text()));
        fn_setEntryValue(_configSectionEntryValues, "TerminalId", SdkCommon::gbkToStdString(ui->SourceTerminalIdEd->text()));
    }
    CxString::fromString(SdkCommon::gbkToStdString(ui->TargetTerminalIdEd->text()), int(0), &bOk);
    if (bOk)
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryPsmStationNumTarget, SdkCommon::gbkToStdString(ui->TargetTerminalIdEd->text()));
        fn_setEntryValue(_configSectionEntryValues, CS_EntryPsmSentTargetId, SdkCommon::gbkToStdString(ui->TargetTerminalIdEd->text()));
    }
    CxString::fromString(SdkCommon::gbkToStdString(ui->ReasonEd->text()), int(0), &bOk);
    if (bOk)
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryPsmSentReason, SdkCommon::gbkToStdString(ui->ReasonEd->text()));
    }
    if ( CxString::isValidIp( SdkCommon::gbkToStdString( ui->remoteIpAddressEd->text() ) ) )
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryRemoteIpAddress, SdkCommon::gbkToStdString(ui->remoteIpAddressEd->text()));
    }
    fn_fromString(ui->remotePortEd->text(), int(0), &bOk);
    if (bOk)
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryRemotePort, SdkCommon::gbkToStdString(ui->remotePortEd->text()));
    }
    if ( CxString::isValidIp( SdkCommon::gbkToStdString( ui->localIpAddressEd->text() ) ) )
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryLocalIpAddress, SdkCommon::gbkToStdString(ui->localIpAddressEd->text()));
    }
    fn_fromString(ui->localPortEd->text(), int(0), &bOk);
    if (bOk)
    {
        fn_setEntryValue(_configSectionEntryValues, CS_EntryLocalPort, SdkCommon::gbkToStdString(ui->localPortEd->text()));
    }

    if (_configSectionEntryValues.empty()) return;
    string sFilePath = SdkCommon::gbkToStdString( ui->configFilePathEd->text() );
    bOk = fn_saveImpl(_configSectionEntryValues, sFilePath);

    outInfo("save params to ConfigFile : " + sFilePath + " " + CxString::toString(bOk));
}

void SdkTestWin::on_sendYxInsertBn_clicked()
{
    QString sCount;
    if (SdkCommon::dialogInput("请输入遥信个数：", sCount))
    {
        int iCount;
        gm_fromString_return(iCount, sCount);
        refreshSendYxTb(ui->sendYxTb, iCount);
    }
}

void SdkTestWin::on_sendYxBn_clicked()
{
    QTableWidget* oGrid = ui->sendYxTb;
    if (! ui->setYxCb->isChecked())
    {
        vector<gcd_address_int32_vqt_t> yxs;
        int iCountFix = ui->frameSizeEd->value() / sizeof(gcd_address_double_vqt_t);
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            gcd_address_int32_vqt_t yx;
            gm_fromString_break(yx.address, oGrid->item(i,1)->text());
            gm_fromString_break(yx.value, oGrid->item(i,2)->text());
            gm_fromString_break(yx.quality, oGrid->item(i,3)->text());
            gm_fromString_dt_break(yx.datetime, oGrid->item(i,4)->text());
            yxs.push_back(yx);

            if (yxs.size() >= iCountFix)
            {
                int iResult = gci_realtime_data_post(ci_yx_address_int32_vqt_t, (char*)yxs.data(), yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), getSendAttach());
                string sMsg = CxString::format("send yx : result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
                outInfo(sMsg);
                yxs.clear();
            }
        }
        if (yxs.size()>0)
        {
            int iResult = gci_realtime_data_post(ci_yx_address_int32_vqt_t, (char*)yxs.data(), yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), getSendAttach());
            string sMsg = CxString::format("send yx : result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
            outInfo(sMsg);
        }
    }
    else
    {
        vector<gcd_address_int32_vqt_t> yxs;
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            gcd_address_int32_vqt_t yx;
            gm_fromString_break(yx.address, oGrid->item(i,1)->text());
            gm_fromString_break(yx.value, oGrid->item(i,2)->text());
            gm_fromString_break(yx.quality, oGrid->item(i,3)->text());
            gm_fromString_dt_break(yx.datetime, oGrid->item(i,4)->text());
            yxs.push_back(yx);
        }
        if (yxs.size()>0)
        {
            int iResult = gci_realtime_data_set_list(ci_yx_address_int32_vqt_t, (char*)yxs.data(), yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), getSendAttach());
            string sMsg = CxString::format("send yx : hadSentCount=%d, needSendCount=%d", iResult, yxs.size());
            outInfo(sMsg);
        }
    }
}

void SdkTestWin::on_sendYcInsertBn_clicked()
{
    QString sCount;
    if (SdkCommon::dialogInput("请输入遥测个数：", sCount))
    {
        int iCount;
        gm_fromString_return(iCount, sCount);
        refreshSendYcTb(ui->sendYcTb, iCount);
    }
}

void SdkTestWin::on_sendYcBn_clicked()
{
    QTableWidget* oGrid = ui->sendYcTb;
    if (! ui->setYcCb->isChecked())
    {
        vector<gcd_address_double_vqt_t> ycs;
        int iCountFix = ui->frameSizeEd->value() / sizeof(gcd_address_double_vqt_t);
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            gcd_address_double_vqt_t yc;
            gm_fromString_break(yc.address, oGrid->item(i,1)->text());
            gm_fromString_break(yc.value, oGrid->item(i,2)->text());
            gm_fromString_break(yc.quality, oGrid->item(i,3)->text());
            gm_fromString_dt_break(yc.datetime, oGrid->item(i,4)->text());
            ycs.push_back(yc);
            if (ycs.size() >= iCountFix)
            {
                int iResult = gci_realtime_data_post(ci_yc_address_double_vqt_t, (char*)ycs.data(), ycs.size()*sizeof(gcd_address_double_vqt_t), ycs.size(), getSendAttach());
                string sMsg = CxString::format("send yc : result=%d, count=%d, size=%d", iResult, ycs.size(), ycs.size()*sizeof(gcd_address_double_vqt_t));
                outInfo(sMsg);
                ycs.clear();
            }
        }
        if (ycs.size()>0)
        {
            int iResult = gci_realtime_data_post(ci_yc_address_double_vqt_t, (char*)ycs.data(), ycs.size()*sizeof(gcd_address_double_vqt_t), ycs.size(), getSendAttach());
            string sMsg = CxString::format("send yc : result=%d, count=%d, size=%d", iResult, ycs.size(), ycs.size()*sizeof(gcd_address_double_vqt_t));
            outInfo(sMsg);
        }
    }
    else
    {
        vector<gcd_address_double_vqt_t> ycs;
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            gcd_address_double_vqt_t yc;
            gm_fromString_break(yc.address, oGrid->item(i,1)->text());
            gm_fromString_break(yc.value, oGrid->item(i,2)->text());
            gm_fromString_break(yc.quality, oGrid->item(i,3)->text());
            gm_fromString_dt_break(yc.datetime, oGrid->item(i,4)->text());
            ycs.push_back(yc);
        }
        if (ycs.size()>0)
        {
            int iResult = gci_realtime_data_set_list(ci_yc_address_double_vqt_t, (char*)ycs.data(), ycs.size()*sizeof(gcd_address_double_vqt_t), ycs.size(), getSendAttach());
            string sMsg = CxString::format("send yc : hadSentCount=%d, needSendCount=%d", iResult, ycs.size());
            outInfo(sMsg);
        }
    }
}

void SdkTestWin::on_sendYwInsertBn_clicked()
{
    QString sCount;
    if (SdkCommon::dialogInput("请输入遥文个数：", sCount))
    {
        int iCount;
        gm_fromString_return(iCount, sCount);
        //if (iCount > 26) iCount = sCount;
        refreshSendYwTb(ui->sendYwTb, iCount);
    }
}

void SdkTestWin::on_sendYwBn_clicked()
{
    QTableWidget* oGrid = ui->sendYwTb;
    if (! ui->setYwCb->isChecked())
    {
        vector<gcd_address_vqt_string128_t> yws;
        int iCountFix = ui->frameSizeEd->value() / sizeof(gcd_address_vqt_string128_t);
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            gcd_address_vqt_string128_t yw;
            gm_fromString_break(yw.address, oGrid->item(i,1)->text());
            string sValue = oGrid->item(i,2)->text().toStdString();
            CxStringC::set(yw.value, sizeof(yw.value), sValue.c_str());
            gm_fromString_break(yw.quality, oGrid->item(i,3)->text());
            gm_fromString_dt_break(yw.datetime, oGrid->item(i,4)->text());
            yws.push_back(yw);

            if (yws.size() >= iCountFix)
            {
                int iResult = gci_realtime_data_post(ci_yw_address_vqt_string128_t, (char*)yws.data(), yws.size()*sizeof(gcd_address_vqt_string128_t), yws.size(), getSendAttach());
                string sMsg = CxString::format("send yw : result=%d, count=%d, size=%d", iResult, yws.size(), yws.size()*sizeof(gcd_address_vqt_string128_t));
                outInfo(sMsg);
                yws.clear();
            }
        }
        if (yws.size()>0)
        {
            int iResult = gci_realtime_data_post(ci_yw_address_vqt_string128_t, (char*)yws.data(), yws.size()*sizeof(gcd_address_vqt_string128_t), yws.size(), getSendAttach());
            string sMsg = CxString::format("send yw : result=%d, count=%d, size=%d", iResult, yws.size(), yws.size()*sizeof(gcd_address_vqt_string128_t));
            outInfo(sMsg);
        }
    }
    else
    {
        vector<gcd_address_vqt_string128_t> yws;
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            gcd_address_vqt_string128_t yw;
            gm_fromString_break(yw.address, oGrid->item(i,1)->text());
            string sValue = oGrid->item(i,2)->text().toStdString();
            CxStringC::set(yw.value, sizeof(yw.value), sValue.c_str());
            gm_fromString_break(yw.quality, oGrid->item(i,3)->text());
            gm_fromString_dt_break(yw.datetime, oGrid->item(i,4)->text());
            yws.push_back(yw);
        }
        if (yws.size()>0)
        {
            int iResult = gci_realtime_data_set_list(ci_yw_address_vqt_string128_t, (char*)yws.data(), yws.size()*sizeof(gcd_address_vqt_string128_t), yws.size(), getSendAttach());
            string sMsg = CxString::format("send yw : hadSentCount=%d, needSendCount=%d", iResult, yws.size());
            outInfo(sMsg);
        }
    }
}

void SdkTestWin::on_sendMessageBn_clicked()
{
    string sCommond = ui->messageCommandEd->currentText().toStdString();
    string sParam = ui->messageParamEd->toPlainText().toStdString();
    int iResult = gci_message_command_send(sCommond.c_str(), sParam.c_str(), sParam.size(), getSendAttach());
    outInfo(QString("send message : %1").arg(iResult));
}

void SdkTestWin::on_sendFileBn_clicked()
{
    QObject * oSender = sender();
    if (oSender)
    {
        string sName1 = oSender->objectName().toStdString();
        CxString::token(sName1, "_");
        postFileByLineEdit(CxString::toInt32(sName1));
    }
}

void SdkTestWin::postFileByLineEdit(int iNo)
{
    QString sName("filePathEd_" + QString::number(iNo));
    QLineEdit * oLineEdit = QObject::findChild<QLineEdit*>(sName);
    if (oLineEdit)
    {
        string sFilePath = oLineEdit->text().toStdString();
        int iResult = gci_file_transfer_send(sFilePath.c_str(), getSendAttach());
        outInfo(CxString::format("send file : %d - %s", iResult, sFilePath.c_str()));
    }
}

void SdkTestWin::on_sdkInitBn_clicked()
{
    if (_timerWait->isActive()) return;

    saveParamsToConfigFile();

    string r = sdk_init();
    outInfo( r.c_str() ) ;

    _timerWaitTimes = 0;
    _timerWait->start();
}

void SdkTestWin::on_sdkRequestInfoBn_clicked()
{
    outInfo( gci_terminal_info() );
}

void SdkTestWin::on_clearBn_clicked()
{
    ui->outInfoEd->clear();
}

void SdkTestWin::on_sdkSaveBn_clicked()
{
    saveParamsToConfigFile();
}

void SdkTestWin::doAfterOpenSdk()
{
    ++_timerWaitTimes;
    if (_timerWaitTimes>300)
        _timerWait->stop();

    if (gci_terminal_status()>0)
    {
        doSetLocalParams();
        _timerWait->stop();
    }
}

//typedef gcd_address_vqt_intText1_t TMeasure ;
//typedef gcd_address_vqt_realText15_t TMeasure ;
typedef gcd_address_vqt_string128_t TMeasure ;
#define TMeasureIdBegin (0x03000000)

void SdkTestWin::on_test1Bn_clicked()
{
    static int iTimes = 0;
    //    for (int i = iTimes*1000; i < iTimes*1000+1000; ++i)
    for (int i = 0; i < 1000; ++i)
    {
        const int iSize = 25;
        TMeasure measures[iSize];
        for (int j = 0; j < 25; ++j)
        {
            measures[j].address = TMeasureIdBegin+i*25+j;
            //            measures[j].value[0] = iTimes+0x30;
            measures[j].value[0] = 0;
            //            measures[j].value[1] = 0;
        }
        int iResult = gci_realtime_data_post(TMeasure::getType(), (char*)measures, sizeof(measures), sizeof(measures)/sizeof(TMeasure), getSendAttach());
        string sMsg = CxString::format("send yx : result=%d, count=%d, size=%d", iResult, iSize, iSize*sizeof(TMeasure));
        outInfo(sMsg);
    }
    ++iTimes;
}

void SdkTestWin::on_test2Bn_clicked()
{
    int iResult = gci_realtime_data_post(TMeasure::getType(), NULL, 0, 0, getSendAttach());
    string sMsg = CxString::format("send yx : result=%d, count=%d, size=%d", iResult, 0, 0);
    outInfo(sMsg);
}

void SdkTestWin::on_shmGetInsertBn_clicked()
{
    QString sCount;
    if (SdkCommon::dialogInput("Please Input Mid Count: ", sCount))
    {
        int iCount;
        gm_fromString_return(iCount, sCount);
        refreshMidTb(ui->shmGetTb, iCount);
    }
}

void SdkTestWin::on_shmGetBn_clicked()
{
    QTableWidget* oGrid = ui->shmGetTb;

    int iYxCount = 0;
    int iYcCount = 0;
    int iYwCount = 0;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        int iMeasureId = 0;
        gm_fromString_break(iMeasureId, oGrid->item(i,1)->text());
        if (iMeasureId>=0x01000000 && iMeasureId<0x02000000)
        {
            gcd_address_int32_vqt_t yx;
            int iResult = gci_realtime_data_get(iMeasureId, ci_yx_address_int32_vqt_t, (char *)(&yx), sizeof(gcd_address_int32_vqt_t)*1);
            if (iResult>0)
            {
                refreshMidTb(oGrid, i, yx.address, CxString::toString(yx.value), yx.quality, yx.datetime);
                ++iYxCount;
            }
            else
            {
                refreshMidTb(oGrid, i, yx.address, "NULL", 0, 0);
            }
        }
        else if (iMeasureId>=0x02000000 && iMeasureId<0x03000000)
        {
            gcd_address_double_vqt_t yc;
            int iResult = gci_realtime_data_get(iMeasureId, ci_yc_address_double_vqt_t, (char *)(&yc), sizeof(gcd_address_double_vqt_t)*1);
            if (iResult>0)
            {
                refreshMidTb(oGrid, i, yc.address, CxString::toString(yc.value), yc.quality, yc.datetime);
                ++iYcCount;
            }
            else
            {
                refreshMidTb(oGrid, i, yc.address, "NULL", 0, 0);
            }
        }
        else if (iMeasureId>=0x03000000 && iMeasureId<0x04000000)
        {
            gcd_address_vqt_string128_t yw;
            int iResult = gci_realtime_data_get(iMeasureId, ci_yw_address_vqt_string128_t, (char *)(&yw), sizeof(gcd_address_vqt_string128_t)*1);
            if (iResult>0)
            {
                refreshMidTb(oGrid, i, yw.address, yw.value, yw.quality, yw.datetime);
                ++iYwCount;
            }
            else
            {
                refreshMidTb(oGrid, i, yw.address, "NULL", 0, 0);
            }
        }
    }

    string sMsg = CxString::format("gci_realtime_data_get success : hadGetYx=%d, hadGetYc=%d, hadGetYw=%d", iYxCount, iYcCount, iYwCount);
    outInfo(sMsg);
}

void SdkTestWin::on_timerYxCb_clicked()
{
    f_iYxCurrentValue = 0;
}

void SdkTestWin::on_timerYcCb_clicked()
{
    f_dYcCurrentValue = 0;
}

void SdkTestWin::on_timerYwCb_clicked()
{
    f_iYwCurrentValue = 0;
}

void SdkTestWin::on_timerShmGetCb_clicked()
{
    f_iShmCurrentMid = ui->timerShmGetChangEd1->value();
}
