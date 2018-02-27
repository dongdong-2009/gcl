#ifndef SDK_TEST_WIN_H
#define SDK_TEST_WIN_H


#include <QMainWindow>
#include <QTimer>
#include <QStandardItem>
#include <QStandardItemModel>

#include "sdk_common.h"
#include "sdk_measure_win.h"

namespace Ui {
class SdkTestWin;
}


class SdkTestWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit SdkTestWin(QWidget *parent = 0);
    ~SdkTestWin();

    void outInfo(const char * sInfo);

    void outInfo(const std::string & sInfo);

    void outInfo(const QString & sInfo);

    void sendSubThreadToMainThread(int);

    int postMessageCommand( const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach);

    int postFile( const char * sFilePath, const PsmAttach * oAttach );

    int postRealtimeDataRequest( const PsmAttach * oAttach );

    int postRealtimeDataPost(  int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach );

    static void setArg(int argc, const char *argv[]);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void doSetLocalParams();

    std::string getSdkState();

    PsmAttach * getSendAttach();

    void refreshSendYxTb(QTableWidget *oTableWidget, int iRowCount);

    void refreshSendYcTb(QTableWidget *oTableWidget, int iRowCount);

    void refreshSendYwTb(QTableWidget *oTableWidget, int iRowCount);

    void refreshMidTb(QTableWidget *oTableWidget, int iRowCount);

    void refreshMidTb(QTableWidget *oTableWidget, int iRowIndex, int iMeasureId, const std::string & sValue, int q, long long t);

    //处理实时数据
    void dealRealtimeDataPost( int iType, const std::vector<int> & addresses );

    void postFileByLineEdit( int iNo );

signals:
    void subThreadToMainThread(int);

private slots:
    void timeOutSlot();

    void customContextMenu(const QPoint &pos);

    void setIncreaseIntegerSlot();

    void setIncreaseDoubleSlot();

    //处理子线程中收回来的数据，是子线程 post 过来的
    //在主线程中处理
    void recvSubThreadToMainThread(int);

    void on_sendYxInsertBn_clicked();

    void on_sendYxBn_clicked();

    void on_sendYcInsertBn_clicked();

    void on_sendYcBn_clicked();

    void on_sendYwInsertBn_clicked();

    void on_sendYwBn_clicked();

    void on_sendMessageBn_clicked();

    void on_sendFileBn_clicked();

    void on_sdkInitBn_clicked();

    void on_sdkRequestInfoBn_clicked();

    void on_clearBn_clicked();

    void on_sdkSaveBn_clicked();

    void doAfterOpenSdk();

    void on_test1Bn_clicked();

    void on_test2Bn_clicked();

    void on_shmGetInsertBn_clicked();

    void on_shmGetBn_clicked();

    void on_timerYxCb_clicked();

    void on_timerYcCb_clicked();

    void on_timerYwCb_clicked();

    void on_timerShmGetCb_clicked();

private:
    std::string sdk_init();

    void loadParamsFromConfigFile();

    void saveParamsToConfigFile();

private:
    Ui::SdkTestWin * ui;
    QMenu* _tableMenu;
    QAction* _setIncreaseIntegerAction;
    QAction* _setIncreaseDoubleAction;
    QTimer * _timer;
    int _timerTimes;
    QTimer * _timerWait;
    int _timerWaitTimes;
    QMutex _locker;
    std::queue<MyMessageCommand> _messageCommands;
    std::queue<MyFileTransfer> _fileTransfers;
    std::queue<MyRealtimeDataRequest> _realtimeDataRequests;
    std::queue<MyRealtimeDataPost> _realtimeDataPosts;
    PsmAttach _sendAttach;
    int _outInfoLineCount;
    std::map<std::string, std::map<std::string, std::string> > _configSectionEntryValues;


};

#endif // SDK_TEST_WIN_H
