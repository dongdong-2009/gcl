#ifndef CONFIGURATOR_WIN_H
#define CONFIGURATOR_WIN_H


#include <QMainWindow>
#include <QDialog>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include <qtwincontrol/table_win.h>
#include <qtwincontrol/simple_win.h>
#include <qtwincontrol/new_protocol_win.h>
#include <qtwincontrol/monsb_operate_win.h>
#include <ccxx/qtcommon.h>
#include <vdi/terminal_manager.h>
#include <vdi/protocol_config.h>
#include <vdi/terminal_loader.h>
#include <vdi/config_base.h>

namespace Ui {
    class ConfiguratorWin;
}

class ConfiguratorWin : public QMainWindow, public CxInterinfoOut_I
{
    Q_OBJECT

public:
    explicit ConfiguratorWin(QWidget *parent = 0);

    ~ConfiguratorWin();

protected:
    void interinfo_out( const std::string& sInfo, const std::string& sTitle, int type = 0, int reason = 0, int source = 0, int target = 0, int tag = 0 );

    CxInterinfo::PlatformEnum platformValue() { return CxInterinfo::Platform_Desktop; }

private slots:
    //monsb/ycadd manager edit and all view
    void measureTableSaveSlot();

    void measureTableCancelSlot();

    //terminal delete / new
    void on_terminalNameCb_currentIndexChanged(const QString &arg1);

    void on_terminalDeleteEd_clicked();

    //protocol worker / channel
    void on_workerTableCb_currentIndexChanged(const QString &arg1);

    void on_channelCb_currentIndexChanged(const QString &arg1);

    void on_businessCb_currentIndexChanged(const QString &arg1);

    void on_protocolDefaultBn_clicked();

    void isAutoRunSlot();

    void isTestingSlot();

    void isAutoLogSlot();

    void isStartWatchdogSlot();

    void monsbTypeSlot();

    void ycaddTypeSlot();

    void strawTypeSlot();

    void configNewSlot();

    void configEditSlot();

    void configDeleteSlot();

    //control action manager view
    void monsbOperatesSlot();

    //save SaveCurrentTerminal
    void saveSlot();

    //close win
    void closeSlot();

    void testSlot();

    void on_newTerminalBtn_clicked();

    void configSaveSlot();

    void configCancelSlot();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::ConfiguratorWin *ui;

    QWidget* _channelPn;
    QWidget* createChannelPn();
    SimpleWin* _channelWin;

    QWidget* _businessPn;
    QWidget* createBusinessPn();
    SimpleWin* _businessWin;

    QWidget* _protocolPn;
    QWidget* createProtocolPn();
    SimpleWin* _protocolWin;

    QWidget* _workerTablePn;
    QWidget* createWorkerTablePn();
    TableWin* _workerTableWin;

    QDialog* _singleTableDialog;
    QWidget* createSingleTablePn(bool bIsPrompt);
    TableWin* _singleTableWin;

    QDialog* _configDialog;
    QWidget* createConfigPn();
    TableWin* _configTableWin;
    SimpleWin* _configSimpleWin;

    const ConfigModel * _currentConfigModel;
    const ConfigData * _currentConfigData;
    bool _currentConfigIsNewAction;
    std::string _currentConfigNewSection;
    std::string _currentConfigNewTitle;

    TerminalBase* _currentTerminal;

    NewProtocolWin* _newProtoclWin;
    MonsbOperateWin* _onsbOperateWin;

    bool _bCreating;

//method
private:
    void initControls();

    //monsb/ycadd/straw manager edit and all view
    void showMonsbManagerWin();

    void showYcaddManagerWin();

    void showStrawManagerWin();

    void showMonsbMsWin(MonsbTypeEnum eMonsbType);

    void showYcaddMsWin(YcaddTypeEnum eYcaddType);

    void showStrawMsWin(StrawTypeEnum eStrawType);

    void configShow();

    void reinitDescribeValueScopes(CxDescribeUnit * oDescribeUnit);

    //control action manager view
    void showMonsbControlActionsWin();

    //terminal delete / new
    void refreshWinByTerminalName(const QString& sTerminalName);

    void refreshWinByTerminal(TerminalBase* oTerminal);

    bool saveCurrentTerminal();

    //protocol worker / channel
    void refreshProtocolWin(ProtocolBase* oProtocol);

    void refreshWorkerTablesWin(ProtocolBase* oProtocol);

    void refreshWorkerTableWin(WorkerTableBase* oWorkerTable, ProtocolBase* oProtocol);

    bool saveCurrentWorkerTable();

    void refreshChannelWin(const CxChannelBase* oChannel);

    void refreshBusinessWin(const BusinessBase* oBusiness);

    void showWorkerMsWin(CxDescribeUnit* oDescribeUnit);

    QString getMonsbTypeText(const QString& name);

    QString getYcaddTypeText(const QString& name);

    QString getStrawTypeText(const QString& name);

    void createActions();

    void recreateConfigNewActions();
    void recreateConfigEditActions();
    void recreateConfigDeleteActions();

    void createMenus();

    QMenu * _programConfigMenu;
    QMenu * _monsbTypesMenu;
    QMenu * _ycaddTypesMenu;
    QMenu * _strawTypesMenu;
    QMenu * _configNewMenu;
    QMenu * _configEditMenu;
    QMenu * _configDeleteMenu;
    QMenu * _othersMenu;

    QAction * _isAutoRunAct;
    QAction * _isTestingAct;
    QAction * _isAutoLogAct;
    QAction * _isStartWatchdogAct;

    QActionGroup * _monsbTypeActionGroup;
    QActionGroup * _ycaddTypeActionGroup;
    QActionGroup * _strawTypeActionGroup;
    QActionGroup * _configNewActionGroup;
    QActionGroup * _configEditActionGroup;
    QActionGroup * _configDeleteActionGroup;

    QAction * _monsbOperatesAct;

    QAction * _saveAct;

    QAction * _closeAct;

    QAction * _testAct;

};
#endif // CONFIGURATOR_WIN_H
