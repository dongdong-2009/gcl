#ifndef PRAUPDOWNWIN_H
#define PRAUPDOWNWIN_H

#include <QtGui>
#include <QMdiArea>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QProgressDialog>

#include "pra.h"
#include <psm/psm_explain.h>

namespace Ui {
class PraUpdownWin;
}


class PraupdownPluginBase;
class PraupdownPluginProxy;


class PraUpdownWin : public QWidget , public ICxTimerNotify, public CxIChannelSubject, public CxIChannelSend, public IPsmExplainNotify
{
    Q_OBJECT

public:
    explicit PraUpdownWin(QWidget *parent = 0);
    ~PraUpdownWin();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::PraUpdownWin *ui;

private slots:
    void on_refreshPraBn_clicked();

    void on_prasTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void customContextMenu(const QPoint &pos);

    void test1Slot();

    void test2Slot();

    void test3Slot();

    void on_connectRemoteBn_clicked();

    void on_disconnectRemoteBn_clicked();

    void on_praItemsTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_remoteFilesCompareBn_clicked();

    void on_downFilesBn_clicked();

    void on_configAtLocalBn_clicked();

    void on_upFilesBn_clicked();

    void on_cancelBn_clicked();

public:
    inline int currentPraRow() const { return _currentPraRow; }
    inline int currentPraItemRow() const { return _currentPraItemRow; }
    inline int currentState() const { return _currentState; }

    inline const Pra & doFilePra () const { return          _doFilePra;        }
    inline const Fdc & doFileFdc () const { return          _doFileFdc;        }
    inline int         doFileIndex () const { return        _doFileIndex;      }
    inline msepoch_t   doFileReqTime () const { return      _doFileReqTime;    }
    inline int         doFileReqTimeOut () const { return   _doFileReqTimeOut; }
    inline bool        doFileCancel () const { return       _doFileCancel;     }

private:
    QMenu* _tableMenu;
    QAction* _test1Action;
    QAction* _test2Action;
    QAction* _test3Action;

    int _currentPraRow;
    int _currentPraItemRow;

    int _currentState;

    CxChannelBase * f_oChannel;
    PsmExplainer * f_oPsm;

    PsmAttach f_defaultAttach;
    CxTimer _timer;
    int _times;

    std::string _whoCommand;
    std::string _whoParam;
    msepoch_t _whoReceivedTime;
    msepoch_t _whoBeginTime;

    bool _closed;

    //*dofile
    //_doFileIndex < _doFileFdc.items.size()
    Pra         _doFilePra;
    Fdc         _doFileFdc;
    int         _doFileIndex;
    msepoch_t   _doFileReqTime;
    int         _doFileReqTimeOut;
    volatile bool _doFileCancel;
//    QProgressDialog _progressDialog;
    void doFileProgressOut(const std::string &sMsg);
    void doFileBegin();
    bool doFileWasCanceled(const char * sPrompt, int iStep);
    void doFileEnd();
    //*compare
    void doFileCompare();
    void doFileCompareReceived(const std::string & sType, const std::string & sSection, const std::string &sFileName, std::map<std::string, std::string> & sParams);
    //*upload
    void doFileUp_message();
    void doFileUp_file();
    void doFileUpReceived(const std::string & sType, const std::string & sSection, const std::string &sFileName, std::map<std::string, std::string> & sParams);

    PraupdownPluginProxy * _praupdownPluginProxy;

protected:
    void timer_timeOut(const CxTimer* oTimer);

protected:
    int send_data( const char* pData, int iLength, int iTargetId );

    inline int received_packet( const PsmExplainer * oPsm, int oTag = 0) { return 0; }

    void received_message_command( const PsmExplainer * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL );

    void received_file_write( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL );

    void received_realtime_data_request( const PsmExplainer * oPsm, const PsmAttach * oAttach = NULL );

    void received_realtime_data_post( const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL );

    void result_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach = NULL);

protected:
    bool channel_canChangeConnect( const CxChannelBase* oChannel, bool bOldConnect, bool bNewConnect) { return true; }

    void channel_connectChanged( const CxChannelBase* oChannel) {}

    void channel_beforeDelete( const CxChannelBase* oChannel) {}

    void channel_receivedData( const uchar* pData, int iLength, void * oSource );

private:
//*
    //对外发送消息
    inline int postMessage( const std::string &sCommand, const std::string &sParam, const PsmAttach * oAttach = NULL ) { return postMessage(sCommand.c_str(), sParam.data(), sParam.length(), oAttach); }

    //对外发送消息
    int postMessage( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const PsmAttach * oAttach = NULL );

    //对外发送文件
    int postFile( const std::string & sFilePath, const PsmAttach * oAttach = NULL );

    //对外发送文件
    int postFile(const CxFileSystem::PathInfo &pathInfo, const std::vector<std::string> &fileData, const PsmAttach * oAttach = NULL );

    //对外发送实时数据请求
    int postRealtimeDataRequest( const PsmAttach * oAttach = NULL );

    //对外发送实时数据
    int postRealtimeDataPost( int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL );

    void connectServer(const std::string &sRemoteIp, ushort sRemotePort);

    void disconnectServer( );

    bool isConnected( );

    bool isLogicConnected( );

//*
    void loadPlugins();

    void refreshPras();

public:
    void refreshFdcs();

private:
    void refreshPrasTitle();

    void refreshFdcsTitle();

    void setPraCurrentRow(int iRow);

    std::string getPraNameByRow(int iRow);

    std::string getPraRpathByRow(int iRow);

    Pra * getPraByRow(int iRow);

    Pra *praViewOut(const std::string & sName);

    int getRowByPraItemPath(const std::string & sPath);

    std::string getFdcNameByRow(int iRow);

    Fdc * getFdcByRow(int iRow);

    void fdcViewOut(const Fdc & mFdc);

    void setPraItemCurrentRow(int iRow);

    void setCurrentState(int iState);

};


class PraupdownPluginBase
{
public:
    PraupdownPluginBase() {}

    virtual ~PraupdownPluginBase() {}

    static std::vector<CxFactoryTemplate<PraupdownPluginBase>*>* factoriesContainer();

    static void factoriesCreateAndRegister() {}

protected:
    virtual void initImpl(PraUpdownWin * oPraUpdownWin) = 0;

    virtual void uninitImpl() {}

    virtual bool projectChangeImpl(const PraUpdownWin* oPraUpdownWin, int iState) { return true; }

    virtual bool praBeforeChangeImpl(const PraUpdownWin* oPraUpdownWin, const std::string & sOldTitle, const std::string & sNewTitle) { return true; }

    virtual void praChangedImpl(const PraUpdownWin* oPraUpdownWin) {}

    virtual bool fdcBeforeChangeImpl(const PraUpdownWin* oPraUpdownWin, const std::string & sOldCaption, const std::string & sNewCaption) { return true; }

    virtual void fdcChangedImpl(const PraUpdownWin* oPraUpdownWin) {}

    virtual void fdcBeforeRefreshImpl(const PraUpdownWin* oPraUpdownWin, Fdc * oFdc) {}

private:
    friend class PraupdownPluginProxy;

};

class PraupdownPluginFactoryBase : public CxFactoryTemplate<PraupdownPluginBase>
{
};

class PraupdownPluginFactoryManager : public CxFactoryManagerTemplate<PraupdownPluginBase>
{
};


class PraupdownPluginProxy
{
public:
    PraupdownPluginProxy();

    ~PraupdownPluginProxy();

    inline const std::vector<PraupdownPluginBase*> & subjects() const { return _plugins; }

    void add(const std::string &sPluginName, const std::string &sPluginParam);

    void deleteAndClear();

    void init(PraUpdownWin * oPraUpdownWin);

    void uninit();

    bool projectChange(const PraUpdownWin* oPraUpdownWin, int iState);

    bool praBeforeChange(const PraUpdownWin* oPraUpdownWin, const std::string & sOldTitle, const std::string & sNewTitle);

    void praChanged(const PraUpdownWin* oPraUpdownWin);

    bool fdcBeforeChange(const PraUpdownWin* oPraUpdownWin, const std::string & sOldCaption, const std::string & sNewCaption);

    void fdcChanged(const PraUpdownWin* oPraUpdownWin);

    void fdcBeforeRefresh(const PraUpdownWin* oPraUpdownWin, Fdc * oFdc);

private:
    std::vector<PraupdownPluginBase*> _plugins;

};




#endif // PRAUPDOWNWIN_H
