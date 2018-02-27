#include "praupdownwin.h"
#include "ui_praupdownwin.h"

#include "fdcmanagerwin.h"

#include "praui.h"

#include "mainwindow.h"

using namespace std;


static PraUpdownWin * f_oPraUpdownWin = NULL;


void fn_processChangeUpDown(int, const void *, int)
{
    if (f_oPraUpdownWin)
    {
        f_oPraUpdownWin->refreshFdcs();
    }
}

PraUpdownWin::PraUpdownWin(QWidget *oParent) :
    QWidget(oParent),
    ui(new Ui::PraUpdownWin)
{
    f_oPraUpdownWin = this;
    MainWindow::registProcessChangeFunction(fn_processChangeUpDown);
    _closed = false;
    f_oChannel = NULL;
    f_oPsm = NULL;
    _times = 0;
    _whoBeginTime = 0;
    _whoReceivedTime = 0;
    _doFileReqTime = 0;
    _doFileReqTimeOut = 3000;
    _doFileIndex = 0;

    ui->setupUi(this);

//    _progressDialog.hide();
//    QFont font("ZYSong18030",12);
//    _progressDialog.setParent(this);
//    _progressDialog.setFont(font);
//    _progressDialog.setWindowModality(Qt::WindowModal);
//    _progressDialog.setMinimumDuration(5);
//    _progressDialog.setWindowTitle(QString::fromUtf8("please wait"));
//    _progressDialog.setCancelButtonText(QString::fromUtf8("取消"));
//    _progressDialog.setMinimumWidth(this->width() / 3);

    PraUi::praHeadInit(ui->prasTw);
    PraUi::praItemHeadInit(ui->praItemsTw);
    PraUi::fdcItemHeadInit(ui->fdcItemsTw);
    ui->prasTw->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->prasTw->setSelectionMode ( QAbstractItemView::SingleSelection);
    ui->praItemsTw->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->praItemsTw->setSelectionMode ( QAbstractItemView::SingleSelection);

    refreshPras();

    this->setAcceptDrops(true);
    ui->praItemsTw->setAcceptDrops(true);

    ui->prasTw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->prasTw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    _tableMenu = new QMenu(oParent);

    _test1Action = new QAction("test 1 ...", oParent);
    connect(_test1Action, SIGNAL(triggered()), this, SLOT(test1Slot()));

    _test2Action = new QAction("test 2 ...", oParent);
    connect(_test2Action, SIGNAL(triggered()), this, SLOT(test2Slot()));

    _test3Action = new QAction("test 3 ...", oParent);
    connect(_test3Action, SIGNAL(triggered()), this, SLOT(test3Slot()));

    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 2);
    ui->splitter->setStretchFactor(2, 5);

    ui->prasTw->installEventFilter(this);
    ui->praItemsTw->installEventFilter(this);
    this->installEventFilter(this);


    _praupdownPluginProxy = new PraupdownPluginProxy();
    loadPlugins();
    _praupdownPluginProxy->init(this);


    _timer.init(this, 100);
    _timer.start();
}

PraUpdownWin::~PraUpdownWin()
{
    f_oPraUpdownWin = NULL;
    _praupdownPluginProxy->uninit();
    _praupdownPluginProxy->deleteAndClear();
    delete ui;
    delete _praupdownPluginProxy;
}

void PraUpdownWin::closeEvent(QCloseEvent * oEvent)
{
    _closed = true;
    _timer.stop();
    disconnectServer();
    oEvent->accept();
}

void PraUpdownWin::on_refreshPraBn_clicked()
{
    refreshPras();
}

void PraUpdownWin::on_prasTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    setPraCurrentRow(ui->prasTw->currentRow());
}

void PraUpdownWin::customContextMenu(const QPoint &pos)
{
    QModelIndex posModelIndex = ui->prasTw->indexAt(pos);
    //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    if(! posModelIndex.isValid())
    {
        return;
    }
    _tableMenu->clear();
    _tableMenu->addAction(_test1Action);
    _tableMenu->addAction(_test2Action);
    _tableMenu->addAction(_test3Action);
    _tableMenu->exec(QCursor::pos());
}

void PraUpdownWin::test1Slot()
{
    for (size_t i = 0; i < 100; ++i)
    {
        Pra pra;
        pra.name = CxString::toString(int(i));
        PraManager::add(pra);
    }
    PraUi::prasRefresh(ui->prasTw, PraManager::getPras());
}

void PraUpdownWin::test2Slot()
{
    QTableWidget * oGrid = ui->prasTw;

    oGrid->insertRow(1);
    oGrid->insertRow(2);
}

void PraUpdownWin::test3Slot()
{

}

void PraUpdownWin::on_connectRemoteBn_clicked()
{
    disconnectServer();
    Pra * oPra = getPraByRow(_currentPraRow);
    if (oPra && CxString::isValidIp(oPra->ip_l) && CxString::isValidPort(oPra->port_l))
    {
        if (oPra && CxString::isValidIp(oPra->ip_r) && CxString::isValidPort(oPra->port_r))
        {
            connectServer(oPra->ip_r, oPra->port_r);
            ui->connectRemoteBn->setEnabled(false);
            _whoCommand = "req.program.who.";
            _whoParam = CxString::format("LocalIpAddress=%s;LocalPort=%d;", oPra->ip_l.c_str(), oPra->port_l);
            postMessage(_whoCommand, _whoParam);
            _whoBeginTime = CxTime::currentSystemTime();
        }
        else if (oPra)
        {
            cxPrompt() << CxString::format("connect fail. connection area=%s; RemoteIp=%s; RemotePort=%d", oPra->name.c_str(), oPra->ip_r.c_str(), oPra->port_r);
        }
    }
    else
    {
        cxPrompt() << CxString::format("connect fail. connection area=%s; LocalIp=%s; LocalPort=%d", oPra->name.c_str(), oPra->ip_l.c_str(), oPra->port_l);
    }
}

void PraUpdownWin::on_disconnectRemoteBn_clicked()
{
    disconnectServer();
}

void PraUpdownWin::on_praItemsTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    setPraItemCurrentRow(ui->praItemsTw->currentRow());
}


void PraUpdownWin::on_remoteFilesCompareBn_clicked()
{
    if (_doFileFdc.items.size()>0)
    {
        doFileBegin();
        PraUi::fdcItemClearMd5_r(ui->fdcItemsTw);
        doFileCompare();
    }
}

void PraUpdownWin::on_downFilesBn_clicked()
{

}

void PraUpdownWin::on_configAtLocalBn_clicked()
{
    Pra * oPra = getPraByRow(_currentPraRow);
    if (oPra)
    {
        Fdc * oFdc = getFdcByRow(_currentPraItemRow);
        if (oFdc)
        {
            MainWindow::processExec(oPra->getConfigArgs(oFdc).c_str());
        }
    }
}

void PraUpdownWin::on_upFilesBn_clicked()
{
    if (_doFileFdc.items.size()>0)
    {
        doFileBegin();
        PraUi::fdcItemClearState(ui->fdcItemsTw);
        doFileUp_message();
    }
}

void PraUpdownWin::on_cancelBn_clicked()
{
    _doFileCancel = true;
}

void PraUpdownWin::doFileProgressOut(const string &sMsg)
{
    string sInfo = CxString::format("共 %d 个，现在第 %d 个，%s", _doFileFdc.items.size(), _doFileIndex+1, sMsg.c_str());
    ui->progressLb->setText(QString::fromUtf8(sInfo.c_str()));
    ui->progressLb->update();
}

void PraUpdownWin::doFileBegin()
{
    if (_doFileFdc.items.size()>0)
    {
        _doFileIndex = 0;
        _doFileCancel = false;
//        _progressDialog.setRange(0,iDoCount);
//        _progressDialog.show();

        setCurrentState(pra_win_state_load);
//        this->setEnabled(false);
    }
}

bool PraUpdownWin::doFileWasCanceled(const char *sPrompt, int iStep)
{
    return false;
//    _progressDialog.setLabelText(QString::fromUtf8(sPrompt));
//    _progressDialog.setValue(iStep);
//    return _progressDialog.wasCanceled();
}

void PraUpdownWin::doFileEnd()
{
//    _progressDialog.hide();
//    this->setEnabled(true);
    setCurrentState(pra_win_state_view);
}

void PraUpdownWin::doFileCompare()
{
    std::vector<FdcItem> & fdcItems = _doFileFdc.items;
    if (fdcItems.size()>0 && _doFileIndex<fdcItems.size())
    {
        const FdcItem & fdcItem = fdcItems.at(_doFileIndex);

        //req.fsys.fileinfo.
        //session=x;filename=x;path=x;
        //filetype=?;filesize=?;md5=?;
        string sCommand = "req.fsys.file.getinfo";
        string sParam = "session=x;filename=%s;path=%s;filetype=?;filesize=?;md5=?;";
        string sFileName = fdcItem.rfilePath;
        string sRpath = _doFileFdc.rpath;
        postMessage(sCommand, CxString::format(sParam.c_str(), sFileName.c_str(), sRpath.c_str()));
        _doFileReqTime = CxTime::currentMsepoch();
        _doFileReqTimeOut = 3000;
        string sMsg = CxString::format("正远程比较文件：%s", sFileName.c_str());
        doFileProgressOut(sMsg);
        if (_doFileCancel)
//        if (doFileWasCanceled(sMsg.c_str(), _doFileIndex))
        {
            doFileEnd();
        }
    }
}

void PraUpdownWin::doFileCompareReceived(const string &sType, const string &sSection, const string &sFileName, std::map<string, string> &sParams)
{
    string sFileType = CxContainer::value(sParams, string("filetype"));
    string sFileSize = CxContainer::value(sParams, string("filesize"));
    string sMd5 = CxContainer::value(sParams, string("md5"));
    if (sMd5.empty()) sMd5 = "remote file invalid";

    PraUi::fdcItemFill(ui->fdcItemsTw, sFileName, sMd5, "", "");

    ++_doFileIndex;
    if (_doFileIndex < _doFileFdc.items.size())
    {
        doFileCompare();
    }
    else
    {
        doFileEnd();
    }
}

void PraUpdownWin::doFileUp_message()
{
    std::vector<FdcItem> & fdcItems = _doFileFdc.items;
    if (fdcItems.size()>0 && _doFileIndex<fdcItems.size())
    {
        const FdcItem & fdcItem = fdcItems.at(_doFileIndex);

        //req.fsys.file.recv
        //session=x;filename=x;md5=%s;
        //new_filename=x;new_path=x;strategy=x;
        string sCommand = "req.fsys.file.recv";
        string sParam = "filename=%s;md5=%s;new_filename=%s;new_path=%s;strategy=%d;";
        //些 sFileName 为下一步对方收到文件时，作比较是否这 up 的文件 CxFileSystem::extractFileName(fdcItem.rfilePath)
        string sFileName = fdcItem.rfilePath;
        string sNewFileName = fdcItem.rfilePath;
        string sNewPath = _doFileFdc.rpath;
        int iStrategy = ( 0x01 << 4) | ( 0x01 << 5);

        string sLocalFilePath = _doFilePra.getProductFilepath(&_doFileFdc, fdcItem);
        string sMd5 = CxCrc::file2md5(sLocalFilePath);
        if (sMd5.size()>0)
        {
            string sMsg = CxString::format("正在发送请求接收<%s>的消息.", sFileName.c_str());
            doFileProgressOut(sMsg);
            if (_doFileCancel)
            {
                doFileEnd();
            }
            else
            {
                postMessage(sCommand, CxString::format(sParam.c_str(), sFileName.c_str(), sMd5.c_str(), sNewFileName.c_str(), sNewPath.c_str(), iStrategy));
                cxPrompt() << "up file has post; filePath=" << sLocalFilePath;
                _doFileReqTime = CxTime::currentMsepoch();
                _doFileReqTimeOut = 3000;
            }
        }
        else
        {
            cxPrompt() << "up file empty; filePath=" << sLocalFilePath;
            PraUi::fdcItemFill(ui->fdcItemsTw, sFileName, "", "", "empty");
            ++_doFileIndex;
            doFileUp_message();
        }
    }
    else
    {
        cxPrompt() << "up file system error!!! _doFileIndex=" << _doFileIndex << "; fdcItems.size()=" << fdcItems.size();
        doFileEnd();
    }
}

void PraUpdownWin::doFileUp_file()
{
    std::vector<FdcItem> & fdcItems = _doFileFdc.items;
    if (fdcItems.size()>0 && _doFileIndex<fdcItems.size())
    {
        const FdcItem & fdcItem = fdcItems.at(_doFileIndex);
        string sLocalFilePath = _doFilePra.getProductFilepath(&_doFileFdc, fdcItem);
        CxFileSystem::PathInfo pathInfo = CxFileSystem::getPathInfo(sLocalFilePath);
        if (pathInfo.fileSize>0)
        {
            string sMsg = CxString::format("正在发送文件：<%s> [ %d KB]... ", fdcItem.rfilePath.c_str(), pathInfo.fileSize / 1024);
            doFileProgressOut(sMsg);
            if (_doFileCancel)
            {
                doFileEnd();
            }
            else
            {
                postFile(sLocalFilePath);
                int iTimeOut = (pathInfo.fileSize / 1024) * 5;
                if (iTimeOut < 3000) iTimeOut=3000;
                cxPrompt() << "up file has post; timeout=" << iTimeOut << "; filePath=" << sLocalFilePath;
                _doFileReqTime = CxTime::currentMsepoch();
                _doFileReqTimeOut = iTimeOut;
            }
        }
        else
        {
            cxPrompt() << "up file fail for fileSize<=0; filePath=" << sLocalFilePath;
            doFileEnd();
        }
    }
    else
    {
        cxPrompt() << "up file system error!!! _doFileIndex=" << _doFileIndex << "; fdcItems.size()=" << fdcItems.size();
        doFileEnd();
    }
}

void PraUpdownWin::doFileUpReceived(const string &sType, const string &sSection, const string &sFileName, std::map<string, string> &sParams)
{
    string sFileType = CxContainer::value(sParams, string("filetype"));
    string sFileSize = CxContainer::value(sParams, string("filesize"));
    string sResult = CxContainer::value(sParams, string("result"));
    if ( sResult == "exist" || sResult == "1" || sResult == "success" || sResult == "finish" )
    {
        PraUi::fdcItemFill(ui->fdcItemsTw, sFileName, "", "", sResult);
        ++_doFileIndex;
        if (_doFileIndex < _doFileFdc.items.size())
        {
            doFileUp_message();
        }
        else
        {
            doFileEnd();
        }
    }
    else if ( sResult.empty() || sResult == "ok" || sResult == "yes" )
    {
        PraUi::fdcItemFill(ui->fdcItemsTw, sFileName, "", "", "sending...");
        doFileUp_file();
    }
    else
    {
        PraUi::fdcItemFill(ui->fdcItemsTw, sFileName, "", "", "result="+sResult);
        doFileEnd();
    }
}






//*--------------------------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************************
//*--------------------------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************************
//*--------------------------------------------------------------------------------------------------------------------------------------

//*ICxTimerNotify
void PraUpdownWin::timer_timeOut(const CxTimer *oTimer)
{
    if (_closed) return;
    if (_times % 30 == 0 && isConnected())
    {
        postMessage(_whoCommand, _whoParam);
    }

    if (_whoBeginTime>0 && CxTime::milliSecondDifferToNow(_whoBeginTime)>5000)
    {
        if (CxTime::milliSecondDifferToNow(_whoReceivedTime)>5000)
        {
            disconnectServer();
        }
    }

    if (_doFileReqTime>0 && CxTime::milliSecondDifferToNow(_doFileReqTime)>_doFileReqTimeOut)
    {
        doFileEnd();
    }

    ++_times;
}


//*CxIChannelSubject
int PraUpdownWin::send_data(const char *pData, int iLength, int iTargetId)
{
    if (f_oChannel)
    {
        return f_oChannel->sendData(pData, iLength);
    }
    return 0;
}

void PraUpdownWin::received_message_command(const PsmExplainer *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach)
{
    string sCommand(pCommand);
    string sParam(pParam, iParamLength);

    if (sCommand.find(".program.who") != string::npos)
    {
        if (_whoReceivedTime == 0)
        {
            _whoReceivedTime = CxTime::currentSystemTime();
            setCurrentState(pra_win_state_view);
        }
        _whoReceivedTime = CxTime::currentSystemTime();
    }
    else if (sCommand.find(".fsys.") != string::npos)
    {
        std::map<string, string> sParams = CxString::splitToMap(sParam, '=', ';');
        string sType = CxContainer::value(sParams, string("type"));
        string sSession = CxContainer::value(sParams, string("session"));
        string sFileName = CxContainer::value(sParams, string("filename"));

        if (sCommand.find(".getinfo") != string::npos)
        {
            doFileCompareReceived(sType, sSession, sFileName, sParams);
        }
        else if (sCommand.find(".recv") != string::npos)
        {
            doFileUpReceived(sType, sSession, sFileName, sParams);
        }
    }
}

void PraUpdownWin::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{

}

void PraUpdownWin::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{

}

void PraUpdownWin::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{

}







void PraUpdownWin::channel_receivedData(const uchar *pData, int iLength, void *oSource)
{
    if (f_oPsm)
    {
        f_oPsm->receivedData(pData, iLength);
    }
}








int PraUpdownWin::postMessage(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postMessageCommand(sCommand, sParam, iParamLength, oAttach);
        else
            return f_oPsm->postMessageCommand(sCommand, sParam, iParamLength, &f_defaultAttach);
    }
    return -1;
}

int PraUpdownWin::postFile(const string &sFilePath, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postFile(sFilePath, oAttach);
        else
            return f_oPsm->postFile(sFilePath, &f_defaultAttach);
    }
    return  -1;
}

int PraUpdownWin::postFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> & fileData, const PsmAttach * oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postFile(pathInfo, fileData, oAttach);
        else {
            return f_oPsm->postFile(pathInfo, fileData, &f_defaultAttach);
        }
    }
    return  -1;
}

int PraUpdownWin::postRealtimeDataRequest(const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
           return f_oPsm->postRealtimeDataRequest(oAttach);
        else
           return f_oPsm->postRealtimeDataRequest(&f_defaultAttach);
    }
    return  -1;
}

int PraUpdownWin::postRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
        else
            return f_oPsm->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, &f_defaultAttach);
    }
    return  -1;
}





//*
void PraUpdownWin::connectServer(const string &sRemoteIp, ushort sRemotePort)
{
    GM_VALID_RETURE(f_oChannel);
    GM_VALID_RETURE(f_oPsm);

    std::string sChannelTypeSimpleName = CS_TypeSimpleName_Udp;
    if (sChannelTypeSimpleName == CS_TypeSimpleName_Tcpclient)
    {
        CxChannelTcpclient * oTcpclient = new CxChannelTcpclient();
        oTcpclient->setRemoteIp(sRemoteIp);
        oTcpclient->setRemotePort(sRemotePort);
        f_oChannel = oTcpclient;
    }
    else
    {
        CxChannelUdp * oUdp = new CxChannelUdp();
        oUdp->setLocalIp("127.0.0.1");
        oUdp->setLocalPort(7921);
        oUdp->setRemoteIp(sRemoteIp);
        oUdp->setRemotePort(sRemotePort);
        f_oChannel = oUdp;
    }
    GM_ASSER_OBJECT(f_oChannel);
    f_oChannel->addObserver(this);
    //独立处理队列
//    f_oChannel->setIsProcessQueue(true);

    f_oPsm = new PsmExplainer();
    f_oPsm->registerPsmExplainNotify(this);
    f_oPsm->registerPsmExplainWrite(this);

    f_oChannel->open();
    f_oPsm->setIsProcessQueue(f_oChannel->isProcessQueue());
    f_oPsm->start();

    //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
//    f_defaultAttach.reason = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentReason, f_defaultAttach.reason, true);
//    f_defaultAttach.containerId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentContainerId, f_defaultAttach.containerId, true);
//    f_defaultAttach.sourceId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentSourceId, f_defaultAttach.sourceId, true);
//    f_defaultAttach.targetId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentTargetId, f_defaultAttach.targetId, true);
//    f_defaultAttach.tag = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentTag, f_defaultAttach.tag, true);
}

void PraUpdownWin::disconnectServer()
{
    //删除 _channel _psm
    if (f_oPsm)
    {
        f_oPsm->stop();
        f_oPsm->registerPsmExplainNotify(NULL);
        f_oPsm->registerPsmExplainWrite(NULL);
        delete f_oPsm;
        f_oPsm = NULL;
    }

    if (f_oChannel)
    {
        f_oChannel->removeObserver(this);
        delete f_oChannel;
        f_oChannel = NULL;
    }

    _whoBeginTime = 0;
    _whoReceivedTime = 0;
    _doFileReqTime = 0;
    setCurrentState(pra_win_state_view);

    cxPrompt() << "disconnect";
}

bool PraUpdownWin::isConnected()
{
    return f_oPsm && f_oChannel && f_oChannel->connected();
}

bool PraUpdownWin::isLogicConnected()
{
    return CxTime::milliSecondDifferToNow(_whoReceivedTime) < 5000;
}









//*--------------------------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************************
//*--------------------------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************************
//*--------------------------------------------------------------------------------------------------------------------------------------

void PraUpdownWin::loadPlugins()
{
    string sUpDownPlugin = CxApplication::findArgument("UpDownPlugins");
    if (sUpDownPlugin.empty())
    {
        sUpDownPlugin = CxApplication::findConfig(CS_SectionProgramConfig, string("UpDownPlugins"), std::string());
    }
    map<string,string> sUpDownPlugins = CxString::splitToMap(sUpDownPlugin, '=', ';');
#ifdef GM_PI_TerminalPlugin
    if (CxContainer::contain(sUpDownPlugins, string("TerminalPlugin")))
        sUpDownPlugins["TerminalPlugin"] = string();
#endif
    for (std::map<string, string >::const_iterator it = sUpDownPlugins.begin(); it != sUpDownPlugins.end(); ++it)
    {
        const string & sPluginName = it->first;
        const string & sPluginParam = it->second;
        _praupdownPluginProxy->add(sPluginName, sPluginParam);
    }
}

void PraUpdownWin::refreshPras()
{
    ui->praItemsTw->setRowCount(0);
    ui->prasTw->setRowCount(0);
    ui->fdcItemsTw->setRowCount(0);

    _currentPraRow = -1;
    _currentPraItemRow = -1;
    _currentState = pra_win_state_none;

    PraUi::prasRefresh(ui->prasTw, PraManager::getPras());
    refreshPrasTitle();
    setCurrentState(pra_win_state_view);
}

void PraUpdownWin::refreshFdcs()
{
    _praupdownPluginProxy->fdcBeforeRefresh(this, &_doFileFdc);
    fdcViewOut(_doFileFdc);
    refreshFdcsTitle();
    setCurrentState(pra_win_state_view);
}

void PraUpdownWin::refreshPrasTitle()
{
    ui->prasTitleLb->setText(QString::fromUtf8("区域共 %1[%2]个，正第 %3行 [%4] %5").arg(ui->prasTw->rowCount()).arg(PraManager::getPras().size()).arg(_currentPraRow).arg(ui->prasTw->currentRow()).arg(QtCommonString::gbkToQString(getPraNameByRow(_currentPraRow))));
}

void PraUpdownWin::refreshFdcsTitle()
{
    Pra * oPra = getPraByRow(_currentPraRow);
    int iPraItemCount = oPra ? oPra->items.size() : -1;
    ui->fdcsTitleLb->setText(QString::fromUtf8("有产品 %1[%2]个，正第 %3行 [%4] %5").arg(ui->praItemsTw->rowCount()).arg(iPraItemCount).arg(_currentPraItemRow).arg(ui->praItemsTw->currentRow()).arg(QtCommonString::gbkToQString(getFdcNameByRow(_currentPraItemRow))));
}

void PraUpdownWin::setPraCurrentRow(int iRow)
{
    if (_currentPraRow != iRow)
    {
        string sName1 = getPraNameByRow(_currentPraRow);
        string sName2 = getPraNameByRow(iRow);

        if (! _praupdownPluginProxy->praBeforeChange(this, sName1, sName2))
        {
            if (_currentPraRow>=0)
            {
                ui->prasTw->selectRow(_currentPraRow);
            }
            return;
        }

        Pra * oPra = praViewOut(sName2);
        if (oPra)
        {
            _currentPraRow = iRow;
            _doFilePra = * oPra;
            refreshPrasTitle();
            _praupdownPluginProxy->praChanged(this);
        }
    }
}

string PraUpdownWin::getPraNameByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->prasTw->item(iRow, 0);
    if (oItem)
    {
        string sName = QtCommonWidget::getItemValue(oItem, string());
        return sName;
    }
    return string();
}

string PraUpdownWin::getPraRpathByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->prasTw->item(iRow, 1);
    if (oItem)
    {
        string sFileName = QtCommonWidget::getItemValue(oItem, string());
        return sFileName;
    }
    return string();
}

Pra *PraUpdownWin::getPraByRow(int iRow)
{
    return PraManager::findByName(getPraNameByRow(iRow));
}

Pra * PraUpdownWin::praViewOut(const string &sName)
{
    Pra * oPra = PraManager::findByName(sName);
    if (oPra)
    {
//            std::string md5;
//            std::string fileName;
//            std::string version;
//            std::string title;
//            std::string moduleName;
//            std::string desc;
//            msepoch_t   modifyTime;
//            std::string modifyContent;
//            std::string modifier;

        ui->areaNameLb->setText(QtCommonString::gbkToQString( oPra->name ));

        QTableWidget * oGrid = ui->praItemsTw;
        oGrid->setRowCount(0);
        std::vector<PraItem> & praItems = oPra->items;
        for (size_t i = 0; i < praItems.size(); ++i)
        {
            const PraItem & praItem = praItems.at(i);
            PraUi::praItemInsert(oGrid, praItem);
        }
        return oPra;
    }
    return NULL;
}

int PraUpdownWin::getRowByPraItemPath(const string &sPath)
{
    QTableWidget * oGrid = ui->praItemsTw;
    QTableWidgetItem * oItem;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        oItem = oGrid->item(i, 0);
        if (QtCommonWidget::getItemValue(oItem, string()) == sPath)
        {
            return i;
        }
    }
    return -1;
}

string PraUpdownWin::getFdcNameByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->praItemsTw->item(iRow, 0);
    if (oItem)
    {
        string sName = QtCommonWidget::getItemValue(oItem, string());
        return sName;
    }
    return string();
}

Fdc *PraUpdownWin::getFdcByRow(int iRow)
{
    return FdcManager::findByName(getFdcNameByRow(iRow));
}

void PraUpdownWin::fdcViewOut(const Fdc & mFdc)
{
//            std::string md5;
//            std::string fileName;
//            std::string version;
//            std::string title;
//            std::string moduleName;
//            std::string desc;
//            msepoch_t   modifyTime;
//            std::string modifyContent;
//            std::string modifier;
    ui->fdcNameLb->setText(QtCommonString::gbkToQString( mFdc.name ));
    
    QTableWidget * oGrid = ui->fdcItemsTw;
    oGrid->setRowCount(0);
    const std::vector<FdcItem> & fdcItems = mFdc.items;
    for (size_t i = 0; i < fdcItems.size(); ++i)
    {
        const FdcItem & fdcItem = fdcItems.at(i);
        PraUi::fdcItemAdd(oGrid, fdcItem, string(), string(), string());
    }
}

void PraUpdownWin::setPraItemCurrentRow(int iRow)
{
    if (_currentPraItemRow != iRow)
    {
        string sName1 = getFdcNameByRow(_currentPraItemRow);
        string sName2 = getFdcNameByRow(iRow);
        if (! _praupdownPluginProxy->fdcBeforeChange(this, sName1, sName2))
        {
            if (_currentPraItemRow>=0)
            {
                ui->praItemsTw->selectRow(_currentPraItemRow);
            }
            return;
        }

        Fdc * oFdc = FdcManager::findByName(sName2);
        _currentPraItemRow = iRow;
        if (oFdc)
            _doFileFdc = * oFdc;
        else
            _doFileFdc = Fdc();
        refreshFdcs();
        _praupdownPluginProxy->fdcChanged(this);
    }
}

//pra_win_state_view
//pra_win_state_edit
//pra_win_state_load
void PraUpdownWin::setCurrentState(int iState)
{
    bool bChannelConnected = isConnected();
    bool bLogicConnected = isLogicConnected();

    ui->refreshPraBn->setEnabled((iState & ( pra_win_state_view )) && ! bChannelConnected);
    ui->connectRemoteBn->setEnabled((iState & ( pra_win_state_view )) && ! bChannelConnected && _currentPraItemRow>=0);
    ui->disconnectRemoteBn->setEnabled((iState & ( pra_win_state_view )) && bChannelConnected);
    ui->remoteFilesCompareBn->setEnabled((iState & ( pra_win_state_view )) && bChannelConnected && bLogicConnected);
    ui->downFilesBn->setEnabled((iState & ( pra_win_state_view )) && bChannelConnected && bLogicConnected);
    ui->upFilesBn->setEnabled((iState & ( pra_win_state_view )) && bChannelConnected && bLogicConnected);
    ui->configAtLocalBn->setEnabled((iState & ( pra_win_state_view )) && _currentPraItemRow>=0);

    ui->prasTw->setEnabled((iState & ( pra_win_state_view )) && ! bChannelConnected);
    ui->praItemsTw->setEnabled((iState & ( pra_win_state_view )));
    ui->fdcItemsTw->setEnabled((iState & ( pra_win_state_view )));

    _currentState = iState;
}























PraupdownPluginProxy::PraupdownPluginProxy()
{
}

PraupdownPluginProxy::~PraupdownPluginProxy()
{
    deleteAndClear();
}

void PraupdownPluginProxy::add(const string &sPluginName, const string &sPluginParam)
{
    PraupdownPluginBase * oPlugin = PraupdownPluginFactoryManager::createObject(sPluginName);
    if (oPlugin)
    {
        _plugins.push_back(oPlugin);
    }
}

void PraupdownPluginProxy::deleteAndClear()
{
    if (_plugins.size()>0)
    {
        for (size_t i = 0; i < _plugins.size(); ++i)
        {
            PraupdownPluginBase * o = _plugins.at(i);
            delete o;
        }
        _plugins.clear();
    }
}

void PraupdownPluginProxy::init(PraUpdownWin *oPraUpdownWin)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        o->initImpl(oPraUpdownWin);
    }
}

void PraupdownPluginProxy::uninit()
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        o->uninitImpl();
    }
}

bool PraupdownPluginProxy::projectChange(const PraUpdownWin *oPraUpdownWin, int iState)
{
    bool r = true;
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        if (! o->projectChangeImpl(oPraUpdownWin, iState))
        {
            r = false;
            break;
        }
    }
    return r;
}

bool PraupdownPluginProxy::praBeforeChange(const PraUpdownWin *oPraUpdownWin, const string &sOldTitle, const string &sNewTitle)
{
    bool r = true;
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        if (! o->praBeforeChangeImpl(oPraUpdownWin, sOldTitle, sNewTitle))
        {
            r = false;
            break;
        }
    }
    return r;
}

void PraupdownPluginProxy::praChanged(const PraUpdownWin *oPraUpdownWin)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        o->praChangedImpl(oPraUpdownWin);
    }
}

bool PraupdownPluginProxy::fdcBeforeChange(const PraUpdownWin *oPraUpdownWin, const string &sOldCaption, const string &sNewCaption)
{
    bool r = true;
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        if (! o->fdcBeforeChangeImpl(oPraUpdownWin, sOldCaption, sNewCaption))
        {
            r = false;
            break;
        }
    }
    return r;
}

void PraupdownPluginProxy::fdcChanged(const PraUpdownWin *oPraUpdownWin)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        o->fdcChangedImpl(oPraUpdownWin);
    }
}

void PraupdownPluginProxy::fdcBeforeRefresh(const PraUpdownWin *oPraUpdownWin, Fdc *oFdc)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        PraupdownPluginBase * o = _plugins.at(i);
        o->fdcBeforeRefreshImpl(oPraUpdownWin, oFdc);
    }
}

std::vector<CxFactoryTemplate<PraupdownPluginBase> *> *PraupdownPluginBase::factoriesContainer()
{
    static std::vector<CxFactoryTemplate<PraupdownPluginBase> *> m;
    return &m;
}


void PraUpdownWin::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{

}
