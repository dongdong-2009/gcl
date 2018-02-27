#include "configurator_win.h"
#include "ui_configurator_win.h"

#include <qtwincontrol/describe_win.h>
#include <qtwincontrol/measure_win.h>
#include <qtwincontrol/task_win.h>

#include <QTableWidget>

using namespace std;

const QString CS_None = "NONE";


ConfiguratorWin::ConfiguratorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfiguratorWin)
{
    _channelPn = NULL;
    _channelWin = NULL;

    _businessPn = NULL;
    _businessWin = NULL;

    _protocolPn = NULL;
    _protocolWin = NULL;

    _workerTablePn = NULL;
    _workerTableWin = NULL;

    _singleTableDialog = NULL;
    _singleTableWin = NULL;

    _configDialog = NULL;
    _configSimpleWin = NULL;
    _configTableWin = NULL;

    _currentConfigModel = NULL;
    _currentConfigData = NULL;

    _currentTerminal = NULL;
    _newProtoclWin = NULL;
    _onsbOperateWin = NULL;

    _bCreating = false;

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    CxInterinfoOut::addObserver(this);

    _configNewActionGroup = NULL;
    _configEditActionGroup = NULL;
    _configDeleteActionGroup = NULL;

    initControls();

    createActions();
    createMenus();
    recreateConfigNewActions();
    recreateConfigEditActions();
    recreateConfigDeleteActions();
}

ConfiguratorWin::~ConfiguratorWin()
{
    CxInterinfoOut::removeObserver(this);

    delete ui;

    if (_protocolWin)
        delete _protocolWin;

    if (_workerTableWin)
        delete _workerTableWin;

    if (_channelWin)
        delete _channelWin;

    if (_businessWin)
        delete _businessWin;

    if (_singleTableWin)
        delete _singleTableWin;

    if (_configTableWin)
        delete _configTableWin;

    if (_configSimpleWin)
        delete _configSimpleWin;
}

QWidget* ConfiguratorWin::createChannelPn()
{
    _channelPn = new QWidget(ui->ChannelTab);
    _channelPn->setObjectName(QString::fromUtf8("ChannelConfigPn"));
    ui->ChannelLayout->addWidget(_channelPn);
    return _channelPn;
}

QWidget *ConfiguratorWin::createBusinessPn()
{
    _businessPn = new QWidget(ui->BusinessTab);
    _businessPn->setObjectName(QString::fromUtf8("BusinessConfigPn"));
    ui->BusinessLayout->addWidget(_businessPn);
    return _businessPn;
}

QWidget* ConfiguratorWin::createProtocolPn()
{
    _protocolPn = new QWidget(ui->ProtocolTab);
    _protocolPn->setObjectName(QString::fromUtf8("ProtocolConfigPn"));
    ui->ProtocolLayout->addWidget(_protocolPn);
    return _protocolPn;
}

QWidget* ConfiguratorWin::createWorkerTablePn()
{
    _workerTablePn = new QWidget(ui->WorkerTableTab);
    _workerTablePn->setObjectName(QString::fromUtf8("TableConfigPn"));
    ui->WorkerTableLayout->addWidget(_workerTablePn);
    return _workerTablePn;
}

QWidget* ConfiguratorWin::createSingleTablePn(bool bIsPrompt)
{
    _singleTableDialog = new QDialog(this);
    QVBoxLayout* oLayou0 = new QVBoxLayout(_singleTableDialog);
    QHBoxLayout* oLayou01 = new QHBoxLayout();
    oLayou0->addLayout(oLayou01);
    QGroupBox* oTableViewParent = new QGroupBox(_singleTableDialog);
    oLayou01->addWidget(oTableViewParent);
    QHBoxLayout* oLayou02 = new QHBoxLayout();
    oLayou0->addLayout(oLayou02);
    QSpacerItem* oSpacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    oLayou02->addItem(oSpacerItem);
    if (bIsPrompt)
    {
        QPushButton* oCloseBn = new QPushButton(_singleTableDialog);
        oLayou02->addWidget(oCloseBn);
        oCloseBn->setText("close");
        oCloseBn->resize(80, 30);
        QObject::connect(oCloseBn, SIGNAL(clicked()), this, SLOT(measureTableCancelSlot()));
    }
    else
    {
        QPushButton* oSaveBn = new QPushButton(_singleTableDialog);
        oLayou02->addWidget(oSaveBn);
        oSaveBn->setText("save");
        oSaveBn->resize(80, 30);
        QPushButton* oCancelBn = new QPushButton(_singleTableDialog);
        oLayou02->addWidget(oCancelBn);
        oCancelBn->setText("cancel");
        oCancelBn->resize(80, 30);
        QObject::connect(oSaveBn, SIGNAL(clicked()), this, SLOT(measureTableSaveSlot()));
        QObject::connect(oCancelBn, SIGNAL(clicked()), this, SLOT(measureTableCancelSlot()));
    }
    return oTableViewParent;
}

QWidget *ConfiguratorWin::createConfigPn()
{
    if (_configDialog)
    {
        delete _configDialog;
        _configDialog = NULL;
    }
    _configDialog = new QDialog(this);
    QVBoxLayout* oLayou0 = new QVBoxLayout(_configDialog);
    QHBoxLayout* oLayou01 = new QHBoxLayout();
    oLayou0->addLayout(oLayou01);
    QGroupBox* oParent = new QGroupBox(_configDialog);
    oLayou01->addWidget(oParent);
    QHBoxLayout* oLayou02 = new QHBoxLayout();
    oLayou0->addLayout(oLayou02);
    QSpacerItem* oSpacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    oLayou02->addItem(oSpacerItem);
    QPushButton* oSaveBn = new QPushButton(_configDialog);
    oLayou02->addWidget(oSaveBn);
    oSaveBn->setText("save");
    oSaveBn->resize(80, 30);
    QPushButton* oCancelBn = new QPushButton(_configDialog);
    oLayou02->addWidget(oCancelBn);
    oCancelBn->setText("cancel");
    oCancelBn->resize(80, 30);
    QObject::connect(oSaveBn, SIGNAL(clicked()), this, SLOT(configSaveSlot()));
    QObject::connect(oCancelBn, SIGNAL(clicked()), this, SLOT(configCancelSlot()));
    return oParent;
}

void ConfiguratorWin::on_terminalDeleteEd_clicked()
{
    if (_currentTerminal && QtCommonString::gbkToQString(_currentTerminal->terminalName()) == ui->terminalNameCb->currentText())
    {
        TerminalBase* oTerminal = _currentTerminal;
        QString sTerminalName = QtCommonString::gbkToQString( oTerminal->terminalName() );
        if (QtCommonDialog::ShowQuery(QString("Are you delete the Terminal : %1 ?").arg(sTerminalName)))
        {
            //delete file
            const ProtocolBase * oProtocol = oTerminal->protocol();
            const vector<WorkerTableBase*> * oWorkerTables = oProtocol->workerTables();
            for (size_t i = 0; i < oWorkerTables->size(); ++i)
            {
                WorkerTableBase* oWorkerTable = oWorkerTables->at(i);
                std::string sWorkerTableFileName = TerminalManager::workerTableFileName(oTerminal, oWorkerTable->tableName());
                std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), sWorkerTableFileName);
                if (CxFileSystem::isExist(sFilePath))
                {
                    CxFileSystem::deleteFile(sFilePath);
                }
            }
            std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), _currentTerminal->terminalName() + ".ini");
            //            QString sTerminalConfigFilePath = QtCommonString::gbkToQString(terminalFilePath);
            if (CxFileSystem::isExist(terminalFilePath))
            {
                CxFileSystem::deleteFile(terminalFilePath);
            }
            //delete terminal
            if (TerminalManager::deleteTerminal(oTerminal))
            {
                _currentTerminal = NULL;
                ui->terminalNameCb->removeItem(ui->terminalNameCb->currentIndex());
            }
        }
    }
}

void ConfiguratorWin::initControls()
{
    _bCreating = true;

    QStringList sTerminalNames = QtCommonString::gbkToQStrings( TerminalManager::terminalNames() );
    ui->terminalNameCb->addItems(sTerminalNames);

    //    QStringList sProtocolSimpleNames = ProtocolFactoryManager::FactoryNames();
    //    ui->NewProtocolSimpleNameCb->addItems(sProtocolSimpleNames);

    QStringList sChannelSimpleNames = QtCommonString::gbkToQStrings( CxChannelFactoryManager::factoryNames() );
    ui->channelCb->addItem(CS_None);
    ui->channelCb->addItems(sChannelSimpleNames);

    QStringList sBusinessSimpleNames = QtCommonString::gbkToQStrings( BusinessFactoryManager::factoryNames() );
    ui->businessCb->addItem(CS_None);
    ui->businessCb->addItems(sBusinessSimpleNames);

    _bCreating = false;

    refreshWinByTerminalName(ui->terminalNameCb->currentText());
}

void ConfiguratorWin::showMonsbMsWin(MonsbTypeEnum eMonsbType)
{
    //support : eMonsbType == MonsbType_0
    //delete ui
    //delete win
    if (_singleTableDialog)
    {
        delete _singleTableDialog;
        _singleTableDialog = NULL;
    }
    if (_singleTableWin)
    {
        delete _singleTableWin;
        _singleTableWin = NULL;
    }
    if (eMonsbType == MonsbType_0)
        return;
    ///channel
    ///channel setting
    QWidget* oTableViewParent = createSingleTablePn(false);
    _singleTableWin = new TableWin();
    CxDescribeUnit* oDescribeUnit = _singleTableWin->describeUnit();
    oDescribeUnit->setUnitName(MonsbMeasure::TYPESIMPLENAME());
    MonsbManager::describeSelf(oDescribeUnit);
    CxDescribeEnum* oMonsbTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryMonsbType);
    if (oMonsbTypeDescribe)
    {
        oMonsbTypeDescribe->initValue(eMonsbType);
    }
    oDescribeUnit->setFilterName(CS_EntryMonsbType);
    oDescribeUnit->setFilterValue(CxString::toString(eMonsbType));
    std::string sFilePath = GclConfig::monsbMeasurePath();
    _singleTableWin->loadWin(oTableViewParent, sFilePath);

    _singleTableDialog->resize(800, 600);
    _singleTableDialog->show();
}

void ConfiguratorWin::showYcaddMsWin(YcaddTypeEnum eYcaddType)
{
    //support : eYcaddType == YcaddType_0
    //delete ui
    //delete win
    if (_singleTableDialog)
    {
        delete _singleTableDialog;
        _singleTableDialog = NULL;
    }
    if (_singleTableWin)
    {
        delete _singleTableWin;
        _singleTableWin = NULL;
    }
    if (eYcaddType == YcaddType_0)
        return;
    ///channel
    ///channel setting
    QWidget* oTableViewParent = createSingleTablePn(false);
    _singleTableWin = new TableWin();
    CxDescribeUnit* oDescribeUnit = _singleTableWin->describeUnit();
    oDescribeUnit->setUnitName(YcaddMeasure::TYPESIMPLENAME());
    YcaddManager::describeSelf(oDescribeUnit);
    CxDescribeEnum* oYcaddTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryYcaddType);
    if (oYcaddTypeDescribe)
    {
        oYcaddTypeDescribe->initValue(eYcaddType);
    }
    oDescribeUnit->setFilterName(CS_EntryYcaddType);
    oDescribeUnit->setFilterValue(CxString::toString(eYcaddType));
    std::string sFilePath = GclConfig::ycaddMeasurePath();
    _singleTableWin->loadWin(oTableViewParent, sFilePath);

    _singleTableDialog->resize(800, 600);
    _singleTableDialog->show();
}

void ConfiguratorWin::showStrawMsWin(StrawTypeEnum eStrawType)
{
    //support : eStrawType == StrawType_0
    //delete ui
    //delete win
    if (_singleTableDialog)
    {
        delete _singleTableDialog;
        _singleTableDialog = NULL;
    }
    if (_singleTableWin)
    {
        delete _singleTableWin;
        _singleTableWin = NULL;
    }
    if (eStrawType == StrawType_0)
        return;
    ///channel
    ///channel setting
    QWidget* oTableViewParent = createSingleTablePn(false);
    _singleTableWin = new TableWin();
    CxDescribeUnit* oDescribeUnit = _singleTableWin->describeUnit();
    oDescribeUnit->setUnitName(StrawMeasure::TYPESIMPLENAME());
    StrawManager::describeSelf(oDescribeUnit);
    CxDescribeEnum* oStrawTypeDescribe = oDescribeUnit->describeSon<CxDescribeEnum>(CS_EntryStrawType);
    if (oStrawTypeDescribe)
    {
        oStrawTypeDescribe->initValue(eStrawType);
    }
    oDescribeUnit->setFilterName(CS_EntryStrawType);
    oDescribeUnit->setFilterValue(CxString::toString(eStrawType));
    std::string sFilePath = GclConfig::strawMeasurePath();
    _singleTableWin->loadWin(oTableViewParent, sFilePath);

    _singleTableDialog->resize(800, 600);
    _singleTableDialog->show();
}

void ConfiguratorWin::configShow()
{
    const ConfigModel * oModel = NULL;
    if (_currentConfigIsNewAction)
    {
        assert(_currentConfigModel != NULL);
        oModel = _currentConfigModel;
    }
    else
    {
        assert(_currentConfigData != NULL);
        oModel = _currentConfigData->configModel();
    }
    assert(oModel != NULL);

    GM_DELETEANDNULL_OBJECT(_configDialog);
    GM_DELETEANDNULL_OBJECT(_configTableWin);
    GM_DELETEANDNULL_OBJECT(_configSimpleWin);

    QWidget * oParent = createConfigPn();
    if (oModel->isTable())
    {
        _configTableWin = new TableWin();
        _configTableWin->describeUnit()->assignedFrom(oModel->describeUnit());
        string sFilePath;
        if (_currentConfigIsNewAction)
        {
            sFilePath = oModel->dataFilePath();
        }
        else
        {
            sFilePath = _currentConfigData->dataFilePath();
        }
        _configTableWin->loadWin(oParent, sFilePath);
    }
    else
    {
        _configSimpleWin = new SimpleWin();
        _configSimpleWin->describeUnit()->assignedFrom(oModel->describeUnit());
        QWidget* oPannel = _configSimpleWin->loadWin(oParent, oModel->uiFilePath());
        if (oPannel)
        {
            QVBoxLayout* oParentLayout = new QVBoxLayout(oParent);
            oParentLayout->addWidget(oPannel);
        }
        if (_currentConfigIsNewAction)
        {
            _configSimpleWin->describeUnit()->restoreDefaultValues();
        }
        else
        {
            _configSimpleWin->describeUnit()->loadValues(_currentConfigData->dataFilePath(), _currentConfigData->dataSection());
        }
        reinitDescribeValueScopes(_configSimpleWin->describeUnit());
        _configSimpleWin->controlsViewOut();
    }
    QString sTitle;
    if (_currentConfigIsNewAction)
    {
        sTitle = GM_QString_TR("新建配置 - ") + QtCommonString::gbkToQString( _currentConfigNewTitle );
    }
    else
    {
        sTitle = GM_QString_TR("修改配置 - ") + QtCommonString::gbkToQString( _currentConfigData->dataTitle() );
    }
    _configDialog->setWindowTitle(sTitle);
    _configDialog->resize(800, 600);
    _configDialog->show();
}

void ConfiguratorWin::reinitDescribeValueScopes(CxDescribeUnit *oDescribeUnit)
{
    const std::vector<CxDescribeBase*> & describes = oDescribeUnit->describes();
    for (size_t i = 0; i < describes.size(); ++i)
    {
        CxDescribeBase * oDescribe = describes.at(i);
        if (oDescribe->foreignKey().size() > 0)
        {
            if (oDescribe->foreignKey() == CS_SectionTerminals)
            {
                oDescribe->setValueScopes(TerminalManager::terminalNames());
            }
        }
    }
}

void ConfiguratorWin::refreshWinByTerminalName(const QString& sTerminalName)
{
    string sTerminalName2 = QtCommonString::gbkToStdString(sTerminalName);
    if (_currentTerminal && _currentTerminal->terminalName() == sTerminalName2)
        return;
    saveCurrentTerminal();
    TerminalBase * oTerminal = TerminalManager::terminal(sTerminalName2, false);
    refreshWinByTerminal(oTerminal);
}

void ConfiguratorWin::refreshWinByTerminal(TerminalBase *oTerminal)
{
    //support : oTerminal == NULL
    ProtocolBase* oProtocol = NULL;
    BusinessBase* oBusiness = NULL;
    const CxChannelBase* oChannel = NULL;
    if (oTerminal)
    {
        oProtocol = oTerminal->protocol();
        oBusiness = oTerminal->business();
    }
    QString sProtocolTitle = "";
    if (oProtocol)
    {
        sProtocolTitle = QString("Protocol Id : %1 , Protocol Name : %2.").arg(oProtocol->protocolId()).arg(QtCommonString::gbkToQString( oProtocol->protocolName()) );
        oChannel = oProtocol->channel();
    }
    refreshProtocolWin(oProtocol);
    ui->ProtocolTitleLb->setText(sProtocolTitle);
    refreshWorkerTablesWin(oProtocol);

    _bCreating = true;
    if (oChannel)
    {
        ui->channelCb->setCurrentIndex(ui->channelCb->findText(QtCommonString::gbkToQString( oChannel->typeSimpleName())) );
    }
    refreshChannelWin(oChannel);
    if (oBusiness)
    {
        ui->businessCb->setCurrentIndex(ui->businessCb->findText(QtCommonString::gbkToQString( oBusiness->typeSimpleName())) );
    }
    refreshBusinessWin(oBusiness);
    _bCreating = false;

    _currentTerminal = oTerminal;
}

void ConfiguratorWin::on_terminalNameCb_currentIndexChanged(const QString &arg1)
{
    if (_bCreating)
        return;
    refreshWinByTerminalName(arg1);
}

void ConfiguratorWin::on_workerTableCb_currentIndexChanged(const QString &arg1)
{
    if (_bCreating)
        return;
    if (! _currentTerminal || ! _currentTerminal->protocol())
        return;
    ProtocolBase* oProtocol = _currentTerminal->protocol();
    std::string sWorkerTableName = QtCommonString::gbkToStdString(arg1);
    if (sWorkerTableName != _workerTableWin->describeUnit()->unitName()){
        saveCurrentWorkerTable();
        WorkerTableBase* oWorkerTable = oProtocol->workerTable(sWorkerTableName);
        refreshWorkerTableWin(oWorkerTable, oProtocol);
    }
}

void ConfiguratorWin::showWorkerMsWin(CxDescribeUnit *oDescribeUnit)
{
}

QString ConfiguratorWin::getMonsbTypeText(const QString &name)
{
    QString sText;
    int i = 0;
    if(name == "AllMonsb"){
        i = 0;
    }else if (name == "RtuState") {
        i = 1;
    }else if (name == "KeyState") {
        i = 2;
    }else if (name == "KbqState") {
        i = 3;
    }else if (name == "DixianState") {
        i = 4;
    }else if (name == "DixianLocation") {
        i = 5;
    }else if (name == "WfOperation") {
        i = 6;
    }

    switch(i){
    case 0:
        sText = tr("AllMonsb");
        break;
    case 1: sText = tr("RtuState");
        break;
    case 2: sText = tr("KeyState");
        break;
    case 3: sText = tr("KbqState");
        break;
    case 4: sText = tr("DixianState");
        break;
    case 5: sText = tr("DixianLocation");
        break;
    case 6: sText = tr("WfOperation");
        break;
    }
    return sText;
}

QString ConfiguratorWin::getYcaddTypeText(const QString &name)
{
    QString sText="";
    int i = 0;
    if(name == "AllYcadd"){
        i = 0;
    }else if (name == "RtuValue") {
        i = 1;
    }else if (name == "DixianPlace") {
        i = 2;
    }else if (name == "PdaPlace") {
        i = 3;
    }

    switch(i)
    {
    case 0:
        sText = tr("AllYcadd");
        break;
    case 1: sText = tr("RtuState");
        break;
    case 2: sText = tr("DixianPlace");
        break;
    case 3: sText = tr("PdaPlace");
        break;
    }
    return sText;
}

QString ConfiguratorWin::getStrawTypeText(const QString &name)
{
    QString sText="";
    int i = 0;
    if(name == "AllStraw"){
        i = 0;
    }else if (name == "RtuText") {
        i = 1;
    }

    switch(i)
    {
    case 0:
        sText = tr("AllStraw");
        break;
    case 1: sText = tr("RtuText");
        break;
    }
    return sText;
}

bool ConfiguratorWin::saveCurrentTerminal()
{
    if (! _currentTerminal)
        return false;
    //    QString sTerminalConfigFilePath = GFunc_TerminalFilePath(_currentTerminal->terminalName());
    std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), _currentTerminal->terminalName() + ".ini");

    CxSkverBase * oProfile = CxSkverManager::beginUse(terminalFilePath);
    if (!oProfile)
        return false;
    ProtocolBase* oProtocol = _currentTerminal->protocol();
    BusinessBase* oBusiness = _currentTerminal->business();
    if (oProtocol && _protocolWin)
    {
        _protocolWin->controlsViewIn();
        _protocolWin->describeUnit()->saveValues(terminalFilePath, oProtocol->protocolName());
        oProfile->prepareGet(oProtocol->protocolName());
        oProtocol->fromContext(*oProfile);
        if (oProtocol->protocolId()<0)
        {
            oProtocol->setProtocolId(_currentTerminal->terminalId());
        }
        saveCurrentWorkerTable();
    }
    if (oBusiness && _businessWin)
    {
        _businessWin->controlsViewIn();
        _businessWin->describeUnit()->saveValues(terminalFilePath, oBusiness->businessName());
        oProfile->prepareGet(oBusiness->businessName());
        oBusiness->fromContext(*oProfile);
    }
    if (oProtocol)
    {
        const CxChannelBase * oChannel = oProtocol->channel();
        if (oChannel && _channelWin){
            _channelWin->controlsViewIn();
            _channelWin->describeUnit()->saveValues(terminalFilePath, oChannel->channelName());
            oProfile->prepareGet(oChannel->channelName());
            oProtocol->channelFromContext(*oProfile);
        }
    }
    CxSkverManager::endUse(oProfile);
    return true;
}

bool ConfiguratorWin::saveCurrentWorkerTable()
{
    if (_workerTableWin && _currentTerminal && _currentTerminal->protocol())
    {
        ProtocolBase* oProtocol = _currentTerminal->protocol();
        std::string sWorkerTableName = _workerTableWin->describeUnit()->unitName();
        WorkerTableBase * oWorkerTable = oProtocol->workerTable(sWorkerTableName);
        if (oWorkerTable && _workerTableWin)
        {
            std::string sWorkerTableFileName = TerminalManager::workerTableFileName(_currentTerminal, oWorkerTable->tableName());
            std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), sWorkerTableFileName);
            _workerTableWin->saveWin(sFilePath);
            oProtocol->loadTable(sWorkerTableName, sFilePath);
        }
        return true;
    }
    else
    {
        return false;
    }
}

void ConfiguratorWin::on_channelCb_currentIndexChanged(const QString &arg1)
{
    if (_bCreating)
        return;
    if (! _currentTerminal || ! _currentTerminal->protocol())
        return;
    ProtocolBase * oProtocol = _currentTerminal->protocol();
    QString sChannelTypeSimpleName = arg1;
    CxChannelBase * oChannel = NULL;
    if (sChannelTypeSimpleName != CS_None)
    {
        oChannel = CxChannelManager::createChannel(QtCommonString::gbkToStdString( sChannelTypeSimpleName) );
    }
    if (oChannel)
    {
        oChannel->setChannelName(TerminalManager::channelName(_currentTerminal));
    }
    oProtocol->setChannel(oChannel);
    refreshChannelWin(oChannel);
}

void ConfiguratorWin::on_businessCb_currentIndexChanged(const QString &arg1)
{
    if (_bCreating)
        return;
    if (! _currentTerminal || ! _currentTerminal->protocol())
        return;
    QString sBusinessTypeSimpleName = arg1;
    BusinessBase * oBusiness = NULL;
    if (sBusinessTypeSimpleName != CS_None)
    {
        oBusiness = BusinessFactoryManager::createObject(QtCommonString::gbkToStdString( sBusinessTypeSimpleName) );
    }
    if (oBusiness)
    {
        oBusiness->setBusinessName(TerminalManager::businessName(_currentTerminal));
    }
    _currentTerminal->setBusiness(oBusiness);
    refreshBusinessWin(oBusiness);
}

void ConfiguratorWin::refreshProtocolWin(ProtocolBase *oProtocol)
{
    //support : oProtocol == NULL
    //delete ui  --  v
    //delete win  -- c
    if (_protocolPn){
        delete _protocolPn;
        _protocolPn = NULL;
    }
    if (_protocolWin){
        delete _protocolWin;
        _protocolWin = NULL;
    }
    if (! oProtocol)
    {
        return;
    }
    //create ui
    //create win
    QWidget* oParent = createProtocolPn();
    _protocolWin = new SimpleWin();
    CxDescribeUnit* oDescribeUnit = _protocolWin->describeUnit();
    oProtocol->describeSelf(oDescribeUnit);
    oDescribeUnit->setUnitName(oProtocol->protocolName());
    QWidget* oPannel = _protocolWin->loadWin(oParent, GclConfig::protocolUiFilePath(oProtocol->typeSimpleName()));
    if (oPannel)
    {
        QVBoxLayout* oParentLayout = new QVBoxLayout(oParent);
        oParentLayout->addWidget(oPannel);
    }
    _protocolWin->controlsViewOut();
}

void ConfiguratorWin::refreshWorkerTablesWin(ProtocolBase *oProtocol)
{
    //support : oProtocol == NULL
    _bCreating = true;
    ui->workerTableCb->clear();
    _bCreating = false;
    const vector<WorkerTableBase*>* oWorkerTables = NULL;
    if (! oProtocol)
    {
        refreshWorkerTableWin(NULL, NULL);
        return;
    }
    oWorkerTables = oProtocol->workerTables();
    QStringList sWorkerTableNames;
    for (size_t i = 0; i < oWorkerTables->size(); ++i)
    {
        WorkerTableBase* oWorkerTable = oWorkerTables->at(i);
        sWorkerTableNames.push_back(QtCommonString::gbkToQString( oWorkerTable->tableName()) );
    }
    _bCreating = true;
    ui->workerTableCb->addItems(sWorkerTableNames);
    _bCreating = false;
    WorkerTableBase* oWorkerTable = oProtocol->workerTable(QtCommonString::gbkToStdString( ui->workerTableCb->currentText()) );
    refreshWorkerTableWin(oWorkerTable, oProtocol);
}

void ConfiguratorWin::refreshWorkerTableWin(WorkerTableBase *oWorkerTable, ProtocolBase* oProtocol)
{
    //support : oWorkerTable == NULL || oProtocol == NULL
    //delete ui
    //delete win
    if (_workerTablePn){
        delete _workerTablePn;
        _workerTablePn = NULL;
    }
    if (_workerTableWin){
        delete _workerTableWin;
        _workerTableWin = NULL;
    }
    if (! oWorkerTable || ! oProtocol)
        return;
    ///channel
    ///channel setting
    QWidget* oParent = createWorkerTablePn();
    _workerTableWin = new TableWin();
    CxDescribeUnit* oDescribeUnit = _workerTableWin->describeUnit();
    oDescribeUnit->setUnitName(oWorkerTable->tableName());
    oProtocol->describeWorkerTable(oDescribeUnit, oWorkerTable);
    std::string sWorkerTableFileName = TerminalManager::workerTableFileName(oProtocol, oWorkerTable->tableName());
    std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::tempPath(), sWorkerTableFileName);
    oWorkerTable->saveToFile(sFilePath);
    _workerTableWin->loadWin(oParent, sFilePath);
}

void ConfiguratorWin::refreshChannelWin(const CxChannelBase * oChannel)
{
    //support : oChannel == NULL
    //delete ui
    //delete win
    ///channel
    if (_channelPn)
    {
        delete _channelPn;
        _channelPn = NULL;
    }
    if (_channelWin)
    {
        delete _channelWin;
        _channelWin = NULL;
    }
    if (! oChannel)
    {
        ui->channelCb->setCurrentIndex(ui->channelCb->findText(CS_None));
        return;
    }
    ///channel setting
    QWidget* oParent = createChannelPn();
    _channelWin = new SimpleWin();
    CxDescribeUnit * oDescribeUnit = _channelWin->describeUnit();
    oChannel->describeSelf(*oDescribeUnit);
    oDescribeUnit->setUnitName(oChannel->channelName());
    QWidget* oPannel = _channelWin->loadWin(oParent, GclConfig::channelUiFilePath(oChannel->typeSimpleName()));
    if (oPannel)
    {
        QVBoxLayout* oParentLayout = new QVBoxLayout(oParent);
        oParentLayout->addWidget(oPannel);
    }
    _channelWin->controlsViewOut();
}

void ConfiguratorWin::refreshBusinessWin(const BusinessBase *oBusiness)
{
    //support : oBusiness == NULL
    //delete ui
    //delete win
    ///business
    if (_businessPn)
    {
        delete _businessPn;
        _businessPn = NULL;
    }
    if (_businessWin)
    {
        delete _businessWin;
        _businessWin = NULL;
    }
    if (! oBusiness)
    {
        ui->businessCb->setCurrentIndex(ui->businessCb->findText(CS_None));
        return;
    }
    ///business setting
    QWidget* oParent = createBusinessPn();
    _businessWin = new SimpleWin();
    CxDescribeUnit * oDescribeUnit = _businessWin->describeUnit();
    oBusiness->describeSelf(oDescribeUnit);
    oDescribeUnit->setUnitName(oBusiness->businessName());
    QWidget* oPannel = _businessWin->loadWin(oParent, GclConfig::businessUiFilePath(oBusiness->typeSimpleName()));
    if (oPannel)
    {
        QVBoxLayout* oParentLayout = new QVBoxLayout(oParent);
        oParentLayout->addWidget(oPannel);
    }
    _businessWin->controlsViewOut();
}

void ConfiguratorWin::interinfo_out(const std::string &sInfo, const std::string &sTitle, int type, int reason, int source, int target, int tag)
{
    stringstream sOutInfo;
    if (sTitle.size()>0)
        sOutInfo << "[title]=" << sTitle;
    if (type)
        sOutInfo << "[type]=" << type;
    if (reason)
        sOutInfo << "[reason]=" << reason;
    if (source)
        sOutInfo << "[source]=" << source;
    if (target)
        sOutInfo << "[target]=" << target;
    if (tag)
        sOutInfo << "[tag]=" << tag;
    if (sOutInfo.tellp() > 0)
        sOutInfo << CxGlobal::lineString;
    sOutInfo << sInfo;
    ui->OutInfoTb->append(QtCommonString::gbkToQString( sOutInfo.str()) );
}

void ConfiguratorWin::measureTableSaveSlot()
{
    if (!_singleTableDialog)
        return;
    if (_singleTableWin->describeUnit()->unitName() == MonsbMeasure::TYPESIMPLENAME())
    {
        std::string sFileName = CxFileSystem::extractFileName(GclConfig::monsbMeasurePath());
        std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::tempPath(), sFileName);
        _singleTableWin->saveWin(sFilePath);
        int iMeasureSonType = QtCommonString::FromString(QtCommonString::gbkToQString(_singleTableWin->describeUnit()->filterValue()), MonsbType_0);
        MonsbManager::loadFromFile(sFilePath, iMeasureSonType);
        sFilePath = GclConfig::monsbMeasurePath();
        MonsbManager::saveToFile(sFilePath);

        //refresh WorkerTableWin
        if (_currentTerminal && _currentTerminal->protocol())
        {
            ProtocolBase* oProtocol = _currentTerminal->protocol();
            std::string sWorkerTableName = _workerTableWin->describeUnit()->unitName();
            WorkerTableBase* oWorkerTable = oProtocol->workerTable(sWorkerTableName);
            if (oWorkerTable->typeSimpleName() == MonsbAddressWorkerTable::TYPESIMPLENAME())
            {
                MonsbAddressWorkerTable* oMonsbAddressWorkerTable = reinterpret_cast<MonsbAddressWorkerTable*>(oWorkerTable);
                if (oMonsbAddressWorkerTable->monsbType() == iMeasureSonType)
                {
                    refreshWorkerTableWin(oWorkerTable, oProtocol);
                }
            }
        }
    }
    else if (_singleTableWin->describeUnit()->unitName() == YcaddMeasure::TYPESIMPLENAME())
    {
        std::string sFileName = CxFileSystem::extractFileName(GclConfig::ycaddMeasurePath());
        std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::tempPath(), sFileName);
        _singleTableWin->saveWin(sFilePath);
        int iMeasureSonType = QtCommonString::FromString(QtCommonString::gbkToQString( _singleTableWin->describeUnit()->filterValue() ), YcaddType_0);
        YcaddManager::loadFromFile(sFilePath, iMeasureSonType);
        sFilePath = GclConfig::ycaddMeasurePath();
        YcaddManager::saveToFile(sFilePath);

        //refresh WorkerTableWin
        if (_currentTerminal && _currentTerminal->protocol())
        {
            ProtocolBase* oProtocol = _currentTerminal->protocol();
            std::string sWorkerTableName = _workerTableWin->describeUnit()->unitName();
            WorkerTableBase* oWorkerTable = oProtocol->workerTable(sWorkerTableName);
            if (oWorkerTable->typeSimpleName() == YcaddAddressWorkerTable::TYPESIMPLENAME())
            {
                YcaddAddressWorkerTable* oYcaddAddressWorkerTable = reinterpret_cast<YcaddAddressWorkerTable*>(oWorkerTable);
                if (oYcaddAddressWorkerTable->ycaddType() == iMeasureSonType)
                {
                    refreshWorkerTableWin(oWorkerTable, oProtocol);
                }
            }
        }
    }
    else if (_singleTableWin->describeUnit()->unitName() == StrawMeasure::TYPESIMPLENAME())
    {
        std::string sFileName = CxFileSystem::extractFileName(GclConfig::strawMeasurePath());
        std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::tempPath(), sFileName);
        _singleTableWin->saveWin(sFilePath);
        int iMeasureSonType = QtCommonString::FromString(QtCommonString::gbkToQString( _singleTableWin->describeUnit()->filterValue() ), StrawType_0);
        StrawManager::loadFromFile(sFilePath, iMeasureSonType);
        sFilePath = GclConfig::strawMeasurePath();
        StrawManager::saveToFile(sFilePath);

        //refresh WorkerTableWin
        if (_currentTerminal && _currentTerminal->protocol())
        {
            ProtocolBase* oProtocol = _currentTerminal->protocol();
            std::string sWorkerTableName = _workerTableWin->describeUnit()->unitName();
            WorkerTableBase* oWorkerTable = oProtocol->workerTable(sWorkerTableName);
            if (oWorkerTable->typeSimpleName() == StrawAddressWorkerTable::TYPESIMPLENAME())
            {
                StrawAddressWorkerTable* oStrawAddressWorkerTable = reinterpret_cast<StrawAddressWorkerTable*>(oWorkerTable);
                if (oStrawAddressWorkerTable->strawType() == iMeasureSonType)
                {
                    refreshWorkerTableWin(oWorkerTable, oProtocol);
                }
            }
        }
    }
    _singleTableDialog->close();
}

void ConfiguratorWin::measureTableCancelSlot()
{
    if (_singleTableDialog)
        _singleTableDialog->close();
}

void ConfiguratorWin::showMonsbManagerWin()
{
    //delete ui
    //delete win
    if (_singleTableDialog)
    {
        delete _singleTableDialog;
        _singleTableDialog = NULL;
    }
    QWidget* oTableViewParent = createSingleTablePn(true);
    QVBoxLayout* oLayout = new QVBoxLayout(oTableViewParent);
    QTableWidget* oTableWidget = new QTableWidget(oTableViewParent);
    oLayout->addWidget(oTableWidget);
    MeasureWin::reportMonsbManager(oTableWidget);
    _singleTableDialog->resize(800, 600);
    _singleTableDialog->show();
}

void ConfiguratorWin::showYcaddManagerWin()
{
    //delete ui
    //delete win
    if (_singleTableDialog)
    {
        delete _singleTableDialog;
        _singleTableDialog = NULL;
    }
    QWidget* oTableViewParent = createSingleTablePn(true);
    QVBoxLayout* oLayout = new QVBoxLayout(oTableViewParent);
    QTableWidget* oTableWidget = new QTableWidget(oTableViewParent);
    oLayout->addWidget(oTableWidget);
    MeasureWin::reportYcaddManager(oTableWidget);
    _singleTableDialog->resize(800, 600);
    _singleTableDialog->show();
}

void ConfiguratorWin::showStrawManagerWin()
{
    //delete ui
    //delete win
    if (_singleTableDialog)
    {
        delete _singleTableDialog;
        _singleTableDialog = NULL;
    }
    QWidget* oTableViewParent = createSingleTablePn(true);
    QVBoxLayout* oLayout = new QVBoxLayout(oTableViewParent);
    QTableWidget* oTableWidget = new QTableWidget(oTableViewParent);
    oLayout->addWidget(oTableWidget);
    MeasureWin::reportStrawManager(oTableWidget);
    _singleTableDialog->resize(800, 600);
    _singleTableDialog->show();
}

void ConfiguratorWin::showMonsbControlActionsWin()
{
    if (_onsbOperateWin)
    {
        delete _onsbOperateWin;
        _onsbOperateWin = NULL;
    }
    _onsbOperateWin = new MonsbOperateWin(this);
    _onsbOperateWin->show();
}

void ConfiguratorWin::on_protocolDefaultBn_clicked()
{
    if (_protocolWin)
    {
        _protocolWin->controlsViewIn();
        _protocolWin->controlsViewOutCalc();
    }
}

void ConfiguratorWin::createActions()
{
    _isAutoRunAct = new QAction(tr("AutoRun"), this);
    _isAutoRunAct->setCheckable(true);
    //    _autoRunAct->setShortcut(QKeySequence::Bold);
    _isAutoRunAct->setStatusTip("AutoRun after system boot.");
    connect(_isAutoRunAct, SIGNAL(triggered()), this, SLOT(isAutoRunSlot()));

    //    QFont boldFont = _autoRunAct->font();
    //    boldFont.setBold(true);
    //    _autoRunAct->setFont(boldFont);

    _isAutoRunAct->setChecked(GclConfig::isAutoStart());

    _isTestingAct = new QAction(tr("Testing"), this);
    _isTestingAct->setCheckable(true);
    _isTestingAct->setStatusTip("Testing after system boot.");
    connect(_isTestingAct, SIGNAL(triggered()), this, SLOT(isTestingSlot()));
    _isTestingAct->setChecked(GclConfig::isTesting());

    _isAutoLogAct = new QAction(tr("AutoLog"), this);
    _isAutoLogAct->setCheckable(true);
    _isAutoLogAct->setStatusTip("AutoLog after system boot.");
    connect(_isAutoLogAct, SIGNAL(triggered()), this, SLOT(isAutoLogSlot()));
    _isAutoLogAct->setChecked(GclConfig::isWriteLog());

    _isStartWatchdogAct = new QAction(tr("StartWatchdog"), this);
    _isStartWatchdogAct->setCheckable(true);
    _isStartWatchdogAct->setStatusTip("StartWatchdog after system boot.");
    connect(_isStartWatchdogAct, SIGNAL(triggered()), this, SLOT(isStartWatchdogSlot()));
    _isStartWatchdogAct->setChecked(GclConfig::isStartWatchdog());

    _monsbTypeActionGroup = new QActionGroup(this);
    for (size_t i = 0; i < sizeof(CS_MonsbTypeNames) / sizeof(CS_MonsbTypeNames[0]); ++i)
    {
        QString name = QtCommonString::gbkToQString( CS_MonsbTypeNames[i] );
        QAction * act = new QAction(getMonsbTypeText(name), this);
        act->setCheckable(true);
        act->setObjectName(name);
        act->setStatusTip(name);
        connect(act, SIGNAL(triggered()), this, SLOT(monsbTypeSlot()));
        _monsbTypeActionGroup->addAction(act);
    }

    _ycaddTypeActionGroup = new QActionGroup(this);
    for (size_t i = 0; i < sizeof(CS_YcaddTypeNames) / sizeof(CS_YcaddTypeNames[0]); ++i)
    {
        QString name = QtCommonString::gbkToQString( CS_YcaddTypeNames[i] );
        QAction * act = new QAction(getYcaddTypeText(name), this);
        act->setCheckable(true);
        act->setObjectName(name);
        act->setStatusTip(name);
        connect(act, SIGNAL(triggered()), this, SLOT(ycaddTypeSlot()));
        _ycaddTypeActionGroup->addAction(act);
    }

    _strawTypeActionGroup = new QActionGroup(this);
    for (size_t i = 0; i < sizeof(CS_StrawTypeNames) / sizeof(CS_StrawTypeNames[0]); ++i)
    {
        QString name = QtCommonString::gbkToQString( CS_StrawTypeNames[i] );
        QAction * act = new QAction(getStrawTypeText(name), this);
        act->setCheckable(true);
        act->setObjectName(name);
        act->setStatusTip(name);
        connect(act, SIGNAL(triggered()), this, SLOT(strawTypeSlot()));
        _strawTypeActionGroup->addAction(act);
    }

    _monsbOperatesAct = new QAction(tr("Controls"), this);
    _monsbOperatesAct->setStatusTip("Monsb Controls.");
    connect(_monsbOperatesAct, SIGNAL(triggered()), this, SLOT(monsbOperatesSlot()));

    _saveAct = new QAction(tr("Save"), this);
    _saveAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
    _saveAct->setStatusTip("Save.");
    connect(_saveAct, SIGNAL(triggered()), this, SLOT(saveSlot()));

    _closeAct = new QAction(tr("Exit"), this);
    _closeAct->setStatusTip("Exit.");
    connect(_closeAct, SIGNAL(triggered()), this, SLOT(closeSlot()));

    _testAct = new QAction(tr("Test"), this);
    _testAct->setStatusTip("Test.");
    connect(_testAct, SIGNAL(triggered()), this, SLOT(testSlot()));
}

void ConfiguratorWin::recreateConfigNewActions()
{
    GM_DELETEANDNULL_OBJECT(_configNewActionGroup);
    _configNewActionGroup = new QActionGroup(this);
    const vector<ConfigModel*> & models = ConfigManager::models();
    for (size_t i = 0; i < models.size(); ++i)
    {
        const ConfigModel * oModel = models.at(i);
        if (! oModel->hasNewOperate())
            continue;
        QString sText = QtCommonString::gbkToQString( oModel->modelTitle() );
//        QString sText = QObject::trUtf8(oModel->modelTitle().data());
        QString sName = QtCommonString::gbkToQString( oModel->modelName() );
        QAction * act = new QAction(sText, this);
        act->setCheckable(true);
        act->setObjectName(sName);
        act->setStatusTip(sText);
        connect(act, SIGNAL(triggered()), this, SLOT(configNewSlot()));
        _configNewActionGroup->addAction(act);
    }
    QList<QAction*> actions = _configNewActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        _configNewMenu->addAction(oAction);
    }
}

void ConfiguratorWin::recreateConfigEditActions()
{
    _configEditMenu->clear();
    GM_DELETEANDNULL_OBJECT(_configEditActionGroup);
    _configEditActionGroup = new QActionGroup(this);
    const vector<ConfigData> & datas = ConfigManager::datas();
    for (size_t i = 0; i < datas.size(); ++i)
    {
        const ConfigData & data = datas.at(i);
        if (! data.configModel() || data.dataSection().empty() || data.dataFilePath().empty())
            continue;
        QString sText = QtCommonString::gbkToQString( data.dataTitle() );
        QString sName = QtCommonString::gbkToQString( data.dataKey() );
        QAction * act = new QAction(sText, this);
        act->setCheckable(true);
        act->setObjectName(sName);
        act->setStatusTip(sText);
        connect(act, SIGNAL(triggered()), this, SLOT(configEditSlot()));
        _configEditActionGroup->addAction(act);
    }
    QList<QAction*> actions = _configEditActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        _configEditMenu->addAction(oAction);
    }
}

void ConfiguratorWin::recreateConfigDeleteActions()
{
    _configDeleteMenu->clear();
    GM_DELETEANDNULL_OBJECT(_configDeleteActionGroup);
    _configDeleteActionGroup = new QActionGroup(this);
    const vector<ConfigData> & datas = ConfigManager::datas();
    for (size_t i = 0; i < datas.size(); ++i)
    {
        const ConfigData & data = datas.at(i);
        if (! data.configModel() || data.dataSection().empty() || data.dataFilePath().empty())
            continue;
        QString sText = QtCommonString::gbkToQString( data.dataTitle() );
        QString sName = QtCommonString::gbkToQString( data.dataKey() );
        QAction * act = new QAction(sText, this);
        act->setCheckable(true);
        act->setObjectName(sName);
        act->setStatusTip(sText);
        connect(act, SIGNAL(triggered()), this, SLOT(configDeleteSlot()));
        _configDeleteActionGroup->addAction(act);
    }
    QList<QAction*> actions = _configDeleteActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        _configDeleteMenu->addAction(oAction);
    }
}

void ConfiguratorWin::createMenus()
{
    _programConfigMenu = menuBar()->addMenu(tr("&Options"));
    _programConfigMenu->addAction(_isAutoRunAct);
    _programConfigMenu->addAction(_isTestingAct);
    _programConfigMenu->addAction(_isAutoLogAct);
    _programConfigMenu->addAction(_isStartWatchdogAct);

    _monsbTypesMenu = menuBar()->addMenu(tr("&Monsbs"));
    QList<QAction*> monsbActions = _monsbTypeActionGroup->actions();
    for (int i = 0; i < monsbActions.count(); ++i)
    {
        QAction * oAction = monsbActions.at(i);
        _monsbTypesMenu->addAction(oAction);
    }

    _ycaddTypesMenu = menuBar()->addMenu(tr("&Ycadds"));
    QList<QAction*> ycaddActions = _ycaddTypeActionGroup->actions();
    for (int i = 0; i < ycaddActions.count(); ++i)
    {
        QAction * oAction = ycaddActions.at(i);
        _ycaddTypesMenu->addAction(oAction);
    }

    _strawTypesMenu = menuBar()->addMenu(tr("&Straws"));
    QList<QAction*> strawActions = _strawTypeActionGroup->actions();
    for (int i = 0; i < strawActions.count(); ++i)
    {
        QAction * oAction = strawActions.at(i);
        _strawTypesMenu->addAction(oAction);
    }

    _configNewMenu = menuBar()->addMenu(tr("&New-Config"));
    _configEditMenu = menuBar()->addMenu(tr("&Edit-Config"));
    _configDeleteMenu = menuBar()->addMenu(tr("&Delete-Config"));

    _othersMenu = menuBar()->addMenu(tr("&Others"));
    _othersMenu->addAction(_monsbOperatesAct);
    _othersMenu->addAction(_saveAct);
    _othersMenu->addAction(_closeAct);
    _othersMenu->addAction(_testAct);
}

void ConfiguratorWin::isAutoRunSlot()
{
    GclConfig::setIsAutoStart(_isAutoRunAct->isChecked());
    if (_isAutoRunAct->isChecked())
    {
        CxSystem::setAutoStart(true);
    }
    else
    {
        CxSystem::setAutoStart(false);
    }
}

void ConfiguratorWin::isTestingSlot()
{
    GclConfig::setIsTesting(_isTestingAct->isChecked());
}

void ConfiguratorWin::isAutoLogSlot()
{
    GclConfig::setIsWriteLog(_isAutoLogAct->isChecked());
}

void ConfiguratorWin::isStartWatchdogSlot()
{
    GclConfig::setIsStartWatchdog(_isStartWatchdogAct->isChecked());
}

void ConfiguratorWin::monsbTypeSlot()
{
    QList<QAction*> actions = _monsbTypeActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        if (oAction->isChecked()) {
            QString sTypeName = oAction->objectName();
            MonsbTypeEnum eMonsbType = g_fn_monsbTypeEnum(QtCommonString::gbkToStdString(sTypeName));
            if (eMonsbType == MonsbType_0)
            {
                showMonsbManagerWin();
            }
            else
            {
                showMonsbMsWin(eMonsbType);
            }

            return;
        }
    }
}

void ConfiguratorWin::ycaddTypeSlot()
{
    QList<QAction*> actions = _ycaddTypeActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        if (oAction->isChecked())
        {
            QString sTypeName = oAction->objectName();
            YcaddTypeEnum eYcaddType = g_fn_ycaddTypeEnum(QtCommonString::gbkToStdString(sTypeName));
            if (eYcaddType == YcaddType_0)
            {
                showYcaddManagerWin();
            }
            else
            {
                showYcaddMsWin(eYcaddType);
            }
            return;
        }
    }
}

void ConfiguratorWin::strawTypeSlot()
{
    QList<QAction*> actions = _strawTypeActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        if (oAction->isChecked())
        {
            QString sTypeName = oAction->objectName();
            StrawTypeEnum eStrawType = g_fn_strawTypeEnum(QtCommonString::gbkToStdString(sTypeName));
            if (eStrawType == StrawType_0)
            {
                showStrawManagerWin();
            }
            else
            {
                showStrawMsWin(eStrawType);
            }
            return;
        }
    }
}

void ConfiguratorWin::configNewSlot()
{
    QList<QAction*> actions = _configNewActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        if (oAction->isChecked())
        {
            QString sModelName = oAction->objectName();
            QString sModelTitle = oAction->text();
            ConfigModel * oModel = ConfigManager::findByModelName(QtCommonString::gbkToStdString(sModelName));
            if (oModel)
            {
                _currentConfigNewSection = oModel->getNewDataSection();
                if (_currentConfigNewSection.size()>0)
                {
                    bool bExistTitle = true;
                    while (bExistTitle)
                    {
                        if (QtCommonDialog::DialogInput(GM_QString_TR("请输入新配置名称"), sModelTitle) && sModelTitle.size()>0)
                        {
                            bExistTitle = ConfigManager::findByDataTitle(QtCommonString::gbkToStdString(sModelTitle)) != NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (sModelTitle.size() > 0 && ! bExistTitle)
                    {
                        _currentConfigNewTitle = QtCommonString::gbkToStdString(sModelTitle);
                        _currentConfigModel = oModel;
                        _currentConfigIsNewAction = true;
                        configShow();
                    }
                }
                else
                {
                    cxPromptOut(QtCommonString::gbkToStdString(sModelName)+" can not getNewDataSection() !!!");
                }
            }
            return;
        }
    }
}

void ConfiguratorWin::configEditSlot()
{
    QList<QAction*> actions = _configEditActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        if (oAction->isChecked())
        {
            QString sDataKey = oAction->objectName();
            const ConfigData * oData = ConfigManager::findByDataKey(QtCommonString::gbkToStdString(sDataKey));
            if (oData)
            {
                _currentConfigData = oData;
                _currentConfigIsNewAction = false;
                configShow();
            }
            break;
        }
    }
}

void ConfiguratorWin::configDeleteSlot()
{
    QList<QAction*> actions = _configDeleteActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        QAction * oAction = actions.at(i);
        if (oAction->isChecked())
        {
            QString sDataKey = oAction->objectName();
            QString sTitle = oAction->text();
            if (QtCommonDialog::ShowQuery(GM_QString_TR("请确认是否删除配置 ： ")+sTitle))
            {
                ConfigManager::deleteNewConfigData(QtCommonString::gbkToStdString(sDataKey));
                recreateConfigEditActions();
                recreateConfigDeleteActions();
            }
            break;
        }
    }
}

void ConfiguratorWin::monsbOperatesSlot()
{
    showMonsbControlActionsWin();
}

void ConfiguratorWin::closeSlot()
{
    saveCurrentTerminal();
    TerminalLoader::save(CxApplication::configFilePath());
    CxApplication::saveConfig();
    close();
}

void ConfiguratorWin::saveSlot()
{
    try
    {
        this->setEnabled(false);
        cxPrompt() << "Please wait a moment, saving data ... ...";

        if ( saveCurrentTerminal() )
        {
            TerminalLoader::save(CxApplication::configFilePath());
            CxApplication::saveConfig();
            cxPrompt() << "Save data success!";
        }
        else
        {
            cxPrompt() << "Save data fail!";
        }
        this->setEnabled(true);
    }
    catch (...)
    {
        this->setEnabled(true);
    }
}

void ConfiguratorWin::testSlot()
{
    if (_protocolPn)
    {
        DescribeWin::saveControls(_protocolPn, QtCommonString::gbkToQString(CxFileSystem::mergeFilePath(GclConfig::tempPath(), "001.ui")));
    }
}

void ConfiguratorWin::closeEvent(QCloseEvent *event)
{
    saveCurrentTerminal();
    TerminalLoader::save(CxApplication::configFilePath());
    CxApplication::saveConfig();
}

void ConfiguratorWin::on_newTerminalBtn_clicked()
{
    if(!_newProtoclWin)
    {
        delete _newProtoclWin;
        _newProtoclWin = NULL;
    }

    _newProtoclWin = new NewProtocolWin(this);
    _newProtoclWin->exec();

    std::string sProtocolSimpleName = QtCommonString::gbkToStdString( _newProtoclWin->getProtocolName() );
    std::string sTerminalName = QtCommonString::gbkToStdString( _newProtoclWin->getTerminalName() );

    TerminalBase * oTerminal = TerminalManager::terminal(sTerminalName, true);
    if (! oTerminal)
    {
        oTerminal = TerminalManager::createTerminal(sTerminalName, "Custom", sProtocolSimpleName, "", "");
        if (oTerminal)
        {
            ui->terminalNameCb->addItem(QtCommonString::gbkToQString( oTerminal->terminalName()) );
        }
    }
    else
    {
        QtCommonDialog::ShowPrompt(QString("Terminal [%1] had exist! Please Fill Other Name !!!").arg(_newProtoclWin->getTerminalName()));
    }
}

void ConfiguratorWin::configSaveSlot()
{
    const ConfigModel * oModel = NULL;
    if (_currentConfigIsNewAction)
    {
        assert(_currentConfigModel != NULL);
        oModel = _currentConfigModel;
    }
    else
    {
        assert(_currentConfigData != NULL);
        oModel = _currentConfigData->configModel();
    }
    assert(oModel != NULL);

    bool bNew = false;
    if (oModel->isTable())
    {
        string sFilePath;
        if (_currentConfigIsNewAction)
        {
            sFilePath = oModel->dataFilePath();
            bNew = true;
        }
        else
        {
            sFilePath = _currentConfigData->dataFilePath();
        }
        _configTableWin->saveWin(sFilePath);
    }
    else
    {
        string sFilePath;
        string sSection;
        if (_currentConfigIsNewAction)
        {
            sFilePath = oModel->dataFilePath();
            sSection = _currentConfigNewSection;
        }
        else
        {
            sFilePath = _currentConfigData->dataFilePath();
            sSection = _currentConfigData->dataSection();
        }
        if (sFilePath.size() > 0 && sSection.size()>0)
        {
            _configSimpleWin->controlsViewIn();
            _configSimpleWin->describeUnit()->saveValues(sFilePath, sSection);
            if (_currentConfigIsNewAction)
            {
                ConfigManager::addNewConfigData(_currentConfigModel->modelName(), sSection, _currentConfigNewTitle);
                bNew = true;
            }
        }
        else
        {
            cxPromptCheck(sFilePath.size()>0);
            cxPromptCheck(sSection.size()>0);
        }
    }
    _configDialog->close();
    if (bNew)
    {
        recreateConfigEditActions();
        recreateConfigDeleteActions();
    }
}

void ConfiguratorWin::configCancelSlot()
{
    _configDialog->close();
}

