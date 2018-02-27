#include "configself_win.h"
#include "ui_configself_win.h"


#include <qtwincontrol/describe_win.h>
#include <qtwincontrol/table_win.h>


#include <QtGui>
#include <QMessageBox>
#include <QToolBar>


using namespace std;

ConfigselfWin::ConfigselfWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigselfWin)
{
    _isDeleting = false;

    initCurrent();

    ui->setupUi(this);

    createActions();
    createMenus();
    createToolBars();

    ui->tabWidgetRight->setVisible(false);

    CxInterinfoOut::addObserver(this);

    _configselfPluginProxy = new ConfigselfPluginProxy();
    loadPlugins();
    _configselfPluginProxy->init(this);
    createMenus2();

    string sFilePath = CxApplication::findArgument("pf");
    if (sFilePath.empty())
    {
        sFilePath = CxApplication::findConfig(CS_SectionProgramConfig, string("pf"), std::string());
    }
#ifdef GM_PI_CreatorPlugin
    //特性
    if (sFilePath.empty())
    {
        sFilePath = "./describe/1.creator.project.ini";
    }
#endif
    if (sFilePath.size()>0)
    {
        loadProject(CxFileSystem::cd(sFilePath, CxApplication::applicationDeployPath()));
    }
}

ConfigselfWin::~ConfigselfWin()
{
    saveCurrentCatalog();
    saveProject();
    _configselfPluginProxy->uninit();
    _configselfPluginProxy->deleteAndClear();
    CxInterinfoOut::removeObserver(this);
    delete ui;
    delete _configselfPluginProxy;
}

void ConfigselfWin::uiMemuClear()
{
    menuBar()->clear();
}

void ConfigselfWin::uiToolBarClear()
{
    removeToolBar(_uiWinsToolBar);
}

const std::vector<CxUnitAttr *> & ConfigselfWin::currentUnitAttrs() const
{
    static std::vector<CxUnitAttr*> m;
    return _currentCatalogAttr && _currentCatalogAttr->catalog ? _currentCatalogAttr->catalog->unitAttrs() : m;
}

void ConfigselfWin::interinfo_out(const string &sInfo, const string &sTitle, int type, int reason, int source, int target, int tag)
{
    stringstream sOutInfo;

    sOutInfo << CxTime::currentMsepochString() << " ";

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
    ui->infoEd->append(QtCommonString::gbkToQString( sOutInfo.str()) );
}

void ConfigselfWin::on_saveAll_clicked()
{
    saveCurrentCatalog();
    saveProject();
}

void ConfigselfWin::on_about_clicked()
{
    QMessageBox::about(this, tr("About ICS"),
             tr("The <b>ICS</b> Config Program Is Valid From 2015 to 2016"
                "document interface applications using CCXX and GCL."));
}

void ConfigselfWin::on_exit_clicked()
{
    this->close();
}

void ConfigselfWin::on_loadProjectBn_clicked()
{
    loadProject();
}

void ConfigselfWin::loadProject()
{
    QString sFilePath;
    if (QtCommonDialog::DialogOpen(sFilePath))
    {
        loadProject(QtCommonString::gbkToStdString(sFilePath));
    }
}

void ConfigselfWin::loadProject(const string &sFilePath)
{
    if (CxFileSystem::isExist(sFilePath))
    {
        if (! _configselfPluginProxy->projectChange(this, ConfigselfProjectState_beforeLoad))
        {
            return;
        }

        saveCurrentCatalog();
        saveProject();
        initCurrent();

        cxPrompt() << "loadProject begin: " << sFilePath;

        if (CxDescribeManager::loadProject(_describeProject, sFilePath) > 0)
        {
            if (! _configselfPluginProxy->projectChange(this, ConfigselfProjectState_afterLoad))
            {
                return;
            }

            _projectFilePath = sFilePath;
            _describePath = CxFileSystem::extractPath(sFilePath);
            _dataPath = CxFileSystem::parentPath(_describePath);

            if (! _configselfPluginProxy->projectChange(this, ConfigselfProjectState_beforeRefresh))
            {
                return;
            }

            QString sWindowTitle = QtCommonString::gbkToQString(_describeProject.projectTitle());
            sWindowTitle += QString::fromUtf8("ICS配置器 - 描述目录=%1; 数据目录=%2;").arg(QtCommonString::gbkToQString(_describePath)).arg(QtCommonString::gbkToQString(_dataPath));
            this->setWindowTitle(sWindowTitle);
            refreshProjectWin();

            if (! _configselfPluginProxy->projectChange(this, ConfigselfProjectState_afterRefresh))
            {
                return;
            }

        }

        cxPrompt() << "loadProject end. ";
    }
}

void ConfigselfWin::createActions()
{
    _uiProjectLocadAct = new QAction(QIcon(":/images/misc1.png"), QString::fromUtf8("项目加载"), this);
//    showFdcsWinAct->setShortcuts(QKeySequence::New);
    _uiProjectLocadAct->setStatusTip(tr("showFdcsWin"));
    connect(_uiProjectLocadAct, SIGNAL(triggered()), this, SLOT(on_loadProjectBn_clicked()));

    _uiProjectSaveAct = new QAction(QIcon(":/images/misc2.png"), QString::fromUtf8("项目保存"), this);
//    showFdvsWinAct->setShortcuts(QKeySequence::Open);
    _uiProjectSaveAct->setStatusTip(tr("showFdvsWin"));
    connect(_uiProjectSaveAct, SIGNAL(triggered()), this, SLOT(on_saveProjectBn_clicked()));

    _uiSaveCatalogAct = new QAction(QIcon(":/images/misc-blue.png"), QString::fromUtf8("当前编目保存"), this);
//    showAreaConfigAct->setShortcuts(QKeySequence::SaveAs);
    _uiSaveCatalogAct->setStatusTip(tr("Save the document under a new name"));
    connect(_uiSaveCatalogAct, SIGNAL(triggered()), this, SLOT(showAreaConfigWin()));

    _uiUnitSaveAct = new QAction(QIcon(":/images/button blue alt.png"), QString::fromUtf8("当前单元保存"), this);
//    showAreaManagerWinAct->setShortcuts(QKeySequence::Save);
    _uiUnitSaveAct->setStatusTip(tr("Save the document to disk"));
    connect(_uiUnitSaveAct, SIGNAL(triggered()), this, SLOT(showAreaManagerWin()));

//! [0]
    _exitAct = new QAction(QString::fromUtf8("退出程序"), this);
    _exitAct->setShortcuts(QKeySequence::Quit);
    _exitAct->setStatusTip(tr("Exit the application"));
    connect(_exitAct, SIGNAL(triggered()), this, SLOT(on_exit_clicked()));

    _saveAct = new QAction(QString::fromUtf8("保存全部"), this);
    _saveAct->setShortcuts(QKeySequence::Quit);
    _saveAct->setStatusTip(tr("Save the application"));
    connect(_saveAct, SIGNAL(triggered()), this, SLOT(on_saveAll_clicked()));
//! [0]

    _aboutAct = new QAction(tr("&About"), this);
    _aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(_aboutAct, SIGNAL(triggered()), this, SLOT(on_about_clicked()));
}

void ConfigselfWin::createMenus()
{
    menuBar()->addSeparator();

    _uiProjectMenu = menuBar()->addMenu(QString::fromUtf8("&P配置项目"));
    _uiProjectMenu->addAction(_uiProjectLocadAct);
    _uiProjectMenu->addAction(_uiProjectSaveAct);
    _uiProjectMenu->addSeparator();

    menuBar()->addSeparator();

    _uiCatalogMenu = menuBar()->addMenu(QString::fromUtf8("&C配置编目"));
    _uiCatalogMenu->addAction(_uiSaveCatalogAct);
    _uiCatalogMenu->addSeparator();

    menuBar()->addSeparator();

    _uiUnitMenu = menuBar()->addMenu(QString::fromUtf8("&U配置单元"));
    _uiUnitMenu->addAction(_uiUnitSaveAct);
    _uiUnitMenu->addSeparator();
}

void ConfigselfWin::createMenus2()
{
    menuBar()->addSeparator();

    _uiOtherMenu = menuBar()->addMenu(QString::fromUtf8("&O其它选项"));
    QAction *action = _uiOtherMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    _uiOtherMenu->addAction(_saveAct);
    _uiOtherMenu->addAction(_exitAct);

    menuBar()->addSeparator();

    _uiHelpMenu = menuBar()->addMenu(QString::fromUtf8("&H系统帮助"));
    _uiHelpMenu->addAction(_aboutAct);
}

void ConfigselfWin::createToolBars()
{
    _uiWinsToolBar = addToolBar(tr("File"));
    _uiWinsToolBar->addAction(_uiProjectLocadAct);
    _uiWinsToolBar->addAction(_uiProjectSaveAct);
    _uiWinsToolBar->addSeparator();
    _uiWinsToolBar->addAction(_uiUnitSaveAct);
    _uiWinsToolBar->addAction(_uiSaveCatalogAct);
    _uiWinsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void ConfigselfWin::refreshProjectWin()
{
    clearProjectWin();

    refreshCatalogsWin();

    if (ui->listWidget->count()>0)
    {
        ui->listWidget->setCurrentRow(0);
    }
}

void ConfigselfWin::refreshCatalogsWin()
{
    const std::vector<CxCatalogAttr*> & catalogAttrs = _describeProject.catalogAttrs();
    for (size_t i = 0; i < catalogAttrs.size(); ++i)
    {
        CxCatalogAttr * oCatalogAttr = catalogAttrs.at(i);
        GM_ASSER_OBJECT(addCatalogTabWidget(oCatalogAttr));
    }
}

QTabWidget * ConfigselfWin::addCatalogTabWidget(CxCatalogAttr * oCatalogAttr)
{
    QTabWidget * oTabWidget = new QTabWidget();
    connect(oTabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabWidget_currentChanged(int)));
    ui->stackedWidget->addWidget(oTabWidget);
    oTabWidget->setToolTip(QtCommonString::gbkToQString(oCatalogAttr->catalogTitle()));

    QListWidgetItem * oItem = new QListWidgetItem();
    ui->listWidget->addItem(oItem);
    QtCommonWidget::setItemValue(oItem, oCatalogAttr->catalogTitle());
    return oTabWidget;
}

void ConfigselfWin::setCurrentRow(int iCurrentRow, bool bSave)
{
    string sCatalogTitle;

    QListWidgetItem * oListWidgetItem = ui->listWidget->item(iCurrentRow);
    if (oListWidgetItem)
    {
        sCatalogTitle = QtCommonString::gbkToStdString(oListWidgetItem->text());
    }

    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sCatalogTitle);
    if (oCatalogAttr)
    {
        if (! _configselfPluginProxy->catalogBeforeChange(this, _currentCatalogTitle, sCatalogTitle))
        {
            if (_currentRow>=0)
            {
                ui->listWidget->setCurrentRow(_currentRow);
            }
            return;
        }
    }

    if (bSave)
    {
        saveCurrentCatalog();
    }

    //设置当前 Catalog
    _currentRow = iCurrentRow;
    _currentCatalogTitle = sCatalogTitle;
    _currentCatalogAttr = oCatalogAttr;

    ui->stackedWidget->setCurrentIndex(iCurrentRow);
    QTabWidget * oTabWidget = (QTabWidget *)(ui->stackedWidget->widget(iCurrentRow));
    refreshCatalogWin(sCatalogTitle, oTabWidget);

    //设置当前 Unit
    setCurrentUnit();

    //raise change
    _configselfPluginProxy->catalogChanged(this);
    _configselfPluginProxy->unitChanged(this);
}

void ConfigselfWin::refreshAttribute()
{
    if (_currentCatalogAttr)
    {
        QString sCatalogTile = QtCommonString::gbkToQString(_currentCatalogAttr->catalogTitle());
        QString sCatalogName = QtCommonString::gbkToQString(_currentCatalogAttr->catalogName());
        QString sCatalogParam = QtCommonString::gbkToQString(_currentCatalogAttr->catalogParam());
        QString sCatalogFileName = QtCommonString::gbkToQString(CxFileSystem::extractFileName(_currentCatalogAttr->catalogFilePath));
        QString sAttributeCatalog = QString::fromUtf8("标识：%1\n名称：%2\n参数：%3\n文件：%4\n单元数：%5\n")
                .arg(sCatalogTile)
                .arg(sCatalogName)
                .arg(sCatalogParam)
                .arg(sCatalogFileName)
                .arg(_currentUnitUis.size());
        ui->attributeCatalogLb->setText(sAttributeCatalog);
    }

    if (_currentUnitAttr)
    {
        QString sUnitTile = QtCommonString::gbkToQString(_currentUnitAttr->unitTitle());
        QString sUnitCaption = QtCommonString::gbkToQString(_currentUnitAttr->unitCaption);
        QString sUnitName = QtCommonString::gbkToQString(_currentUnitAttr->unitName());
        QString sDataType = QtCommonString::gbkToQString(_currentUnitAttr->dataType);
        QString sDataPath = QtCommonString::gbkToQString(_currentUnitAttr->dataPath);
        QString sDataSection = QtCommonString::gbkToQString(_currentUnitAttr->dataSection);
        QString sDataName = QtCommonString::gbkToQString(_currentUnitAttr->dataName);
        QString sDataParam = QtCommonString::gbkToQString(_currentUnitAttr->dataParam);
        QString sDataRewrite = QtCommonString::gbkToQString(CxString::toString(_currentUnitAttr->dataRewrite));
        QString sAttributeUnit = QString::fromUtf8("标识：%1\n标题：%2\n名称：%3\n数据类型：%4\n数据存放：%5\n数据截面：%6\n数据名称：%7\n数据参数：%8\n数据重写：%9\n")
                .arg(sUnitTile)
                .arg(sUnitCaption)
                .arg(sUnitName)
                .arg(sDataType)
                .arg(sDataPath)
                .arg(sDataSection)
                .arg(sDataName)
                .arg(sDataParam)
                .arg(sDataRewrite);
        ui->attributeUnitLb->setText(sAttributeUnit);
    }


//    int _currentRow;
//    std::string _currentCatalogTitle;
//    CxCatalogAttr * _currentCatalogAttr;
//    std::vector<UnitUi> _currentUnitUis;
//    std::string _currentUnitCaption;
//    CxUnitAttr * _currentUnitAttr;
//    int _currentTab;
//    UnitUi _currentUnitUi;

}

void ConfigselfWin::refreshCatalogWin(const string &sCatalogTitle, QTabWidget *oTabWidget)
{
    if (oTabWidget && oTabWidget->count() > 0) return;

    vector<UnitUi> unitUis;
    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sCatalogTitle);
    GM_INVALID_RETURE(oCatalogAttr);
    CxDescribeCatalog * oCatalog = oCatalogAttr->catalog;
    GM_INVALID_RETURE(oCatalog);
    const std::vector<CxUnitAttr*> & units = oCatalog->unitAttrs();
    for (size_t i = 0; i < units.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = units.at(i);
        CxDescribeUnit * oUnit = oUnitAttr->unit;

        //加载 CxDescribeUnit 到界面 各 CxDescribeBase
        QWidget * oTabConfig = NULL;
        if (oUnitAttr->dataSection.size()>0)
        {
            //是单个对象类型
            oTabConfig = DescribeWin::loadControls(oUnit);
//            oUnit->loadValues(sDataFilePath, oUnitAttr->dataSection, oUnitAttr->dataType);
//            DescribeWin::controlsViewOut(oUnit, oTabConfig);
        }
        else
        {
            //是表类型
            QGroupBox * oGroupBox = new QGroupBox();
            oGroupBox->setTitle(QString::fromUtf8("配置数据"));
            oTabConfig = oGroupBox;
            TableWin * oTableWin = new TableWin();
            oTableWin->setDescribeUnit(oUnit);
            oTableWin->initUi(oTabConfig);
//            oTableWin->loadWin(oTabConfig, sDataFilePath, "", oUnitAttr->dataType, oUnitAttr->dataParam);
            oUnitAttr->userData = oTableWin;
        }

        //加载 CxUnitAttr 到界面 oUnitAttr->dataType, oUnitAttr->dataParam
        QWidget * oTabData = DescribeWin::loadControls(oUnitAttr);
        QWidget * oBn;
        oBn = DescribeWin::findControl(oTabData, "reloadDataNameBn");
        if (oBn && oBn->metaObject()->className() == QPushButton::staticMetaObject.className())
        {
            QPushButton * reloadDataNameBn = reinterpret_cast<QPushButton*>(oBn);
            connect(reloadDataNameBn, SIGNAL(clicked()), this, SLOT(on_reloadDataNameBn_clicked()));
        }
        oBn = DescribeWin::findControl(oTabData, "saveDataNameBn");
        if (oBn && oBn->metaObject()->className() == QPushButton::staticMetaObject.className())
        {
            QPushButton * saveDataNameBn = reinterpret_cast<QPushButton*>(oBn);
            connect(saveDataNameBn, SIGNAL(clicked()), this, SLOT(on_saveDataNameBn_clicked()));
        }
        oBn = DescribeWin::findControl(oTabData, "redefaultDataNamBn");
        if (oBn && oBn->metaObject()->className() == QPushButton::staticMetaObject.className())
        {
            QPushButton * redefaultDataNamBn = reinterpret_cast<QPushButton*>(oBn);
            connect(redefaultDataNamBn, SIGNAL(clicked()), this, SLOT(on_redefaultDataNamBn_clicked()));
        }

        //把 CxDescribeUnit面板 与 CxUnitAttr面板放一起
        UnitUi unitUi = createUnitUi(oTabConfig, oTabData);
        unitUi.unitCaption = oUnitAttr->unitCaption;
        oTabWidget->addTab(unitUi.tab, QtCommonString::gbkToQString(unitUi.unitCaption));
        unitUis.push_back(unitUi);

        //加载 CxDescribeUnit 对应的数据文件
        string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, _dataPath);
        loadUnitData(oUnitAttr, unitUi, sDataFilePath);
    }
    _catalogUis[sCatalogTitle] = unitUis;
}

void ConfigselfWin::addCatalogWin(const string &sCatalogTitle)
{
    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sCatalogTitle);
    GM_INVALID_RETURE(oCatalogAttr);
    addCatalogTabWidget(oCatalogAttr);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
}

CxCatalogAttr * ConfigselfWin::removeCatalogWin(const string &sCatalogTitle)
{
    int iRow = -1;
    std::vector<UnitUi> unitUis;
    CxCatalogAttr * oCatalogAttr = findCatalogAttr(sCatalogTitle, iRow, unitUis);
    if (iRow>0 && QtCommonDialog::ShowQuery(QString::fromUtf8("是否删除当前编目[%1] ? ").arg(QtCommonString::gbkToQString(sCatalogTitle))))
    {
        QTabWidget * oTabWidget = (QTabWidget *)ui->stackedWidget->widget(iRow);
        ui->stackedWidget->removeWidget(oTabWidget);
        delete oTabWidget;

        _isDeleting = true;
        ui->listWidget->takeItem(iRow);
        ui->listWidget->setCurrentRow(iRow-1);
        setCurrentRow(iRow-1, false);
        _isDeleting = false;
        return oCatalogAttr;
    }
    else
    {
        cxPrompt() << "remove catalog fail. for current row less 1";
    }
    return NULL;
}

void ConfigselfWin::reloadCatalogData(const string &sCatalogTitle, const std::string &sDataName)
{
    string sCatalogTitleLoad = sCatalogTitle.empty() ? _currentCatalogTitle : sCatalogTitle;

    std::vector<UnitUi> unitUis;
    unitUis = CxContainer::value(_catalogUis, sCatalogTitleLoad, unitUis);

    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sCatalogTitleLoad);
    GM_INVALID_RETURE(oCatalogAttr);
    CxDescribeCatalog * oCatalog = oCatalogAttr->catalog;
    GM_INVALID_RETURE(oCatalog);
    const std::vector<CxUnitAttr*> & unitAttrs = oCatalog->unitAttrs();
    GM_INVALID_RETURE(unitAttrs.size()==unitUis.size());

    for (size_t i = 0; i < unitAttrs.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = unitAttrs.at(i);
        const UnitUi & unitUi = unitUis.at(i);
        if (sDataName.size()>0) oUnitAttr->dataName = sDataName;

        string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, _dataPath);
        if (oUnitAttr->dataName.size()>0 && ! CxString::isValidFullPath(sDataFilePath))
        {
            cxPrompt() << "reload DataName " << oUnitAttr->unitCaption << " fail. file : " << sDataFilePath << " is invalid!!!";
        }

        loadUnitData(oUnitAttr, unitUi, sDataFilePath);
    }
}


void ConfigselfWin::saveCatalogData(bool bUiViewin, const string &sCatalogTitle)
{
    string sSaveCatalogTitle = sCatalogTitle;

    if (sSaveCatalogTitle.empty())
    {
        sSaveCatalogTitle = _currentCatalogTitle;
    }
    if (sSaveCatalogTitle.empty())
    {
        cxPrompt() << "save catalog fail. sSaveCatalogTitle is empty!!!";
        return;
    }

    std::vector<UnitUi> unitUis;
    unitUis = CxContainer::value(_catalogUis, sSaveCatalogTitle, unitUis);

    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sSaveCatalogTitle);
    GM_INVALID_RETURE(oCatalogAttr);
    CxDescribeCatalog * oCatalog = oCatalogAttr->catalog;
    GM_INVALID_RETURE(oCatalog);
    const std::vector<CxUnitAttr*> & unitAttrs = oCatalog->unitAttrs();
    GM_INVALID_RETURE(unitAttrs.size()==unitUis.size());

    for (size_t i = 0; i < unitAttrs.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = unitAttrs.at(i);
        const UnitUi & unitUi = unitUis.at(i);
        saveUnitData(oUnitAttr, unitUi, _dataPath, bUiViewin);
    }
}

void ConfigselfWin::saveCatalog(const string &sCatalogTitle)
{
    string sCatalogTitle2 = sCatalogTitle;

    if (sCatalogTitle2.empty())
    {
        QListWidgetItem * oItem = ui->listWidget->currentItem();
        if (!oItem)
        {
            cxPrompt() << "save catalog fail. ui->listWidget is NULL!!!";
            return;
        }
        QTabWidget * oTabWidget = (QTabWidget *)(ui->stackedWidget->widget(ui->listWidget->currentRow()));
        if (!oTabWidget || oTabWidget->count()<=0)
        {
            cxPrompt() << "save catalog fail. ui->stackedWidget count == 0 !!!";
            return;
        }

        sCatalogTitle2 = QtCommonString::gbkToStdString(oItem->text());
    }

    std::vector<UnitUi> unitUis;
    unitUis = CxContainer::value(_catalogUis, sCatalogTitle2, unitUis);

    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sCatalogTitle2);
    GM_INVALID_RETURE(oCatalogAttr);
    CxDescribeCatalog * oCatalog = oCatalogAttr->catalog;
    GM_INVALID_RETURE(oCatalog);
    const std::vector<CxUnitAttr*> & units = oCatalog->unitAttrs();
    GM_INVALID_RETURE(units.size()==unitUis.size());

    for (size_t i = 0; i < units.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = units.at(i);
        const UnitUi & unitUi = unitUis.at(i);
        saveUnitData(oUnitAttr, unitUi, _dataPath);
    }

    string sDataFilePath = CxFileSystem::mergeFilePath(_describePath, oCatalogAttr->catalogFilePath);
    if (! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "save catalog fail. file : " << sDataFilePath << " is invalid!!!";
        return;
    }

    //保存 catalog (个体是unitAttr)
    CxDescribeManager::saveCatalog(*oCatalog, sDataFilePath);
    cxPrompt() << "save catalog success. file : " << sDataFilePath;
}

bool ConfigselfWin::saveUnitData(CxUnitAttr * oUnitAttr, const UnitUi & unitUi, const std::string & sDataPath, bool bUiViewin)
{
    GM_INVALID_RETURE_(oUnitAttr, false);
    GM_INVALID_RETURE_(unitUi.unitAttrContainer, false);
    GM_INVALID_RETURE_(unitUi.unitContainer, false);
    GM_INVALID_RETURE_(CxFileSystem::isExist(sDataPath), false);

    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);

    if (bUiViewin)
    {
        DescribeWin::controlsViewIn(unitUi.unitAttrContainer, oUnitAttr);
    }

    string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, sDataPath);
    if (oUnitAttr->dataName.empty() || ! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "save unit " << oUnitAttr->unitCaption << " fail. file : " << sDataFilePath << " is invalid!!!";
        return false;
    }

    int r = 0;
    if (oUnitAttr->dataSection.size()>0)
    {
        DescribeWin::controlsViewIn(oUnit, unitUi.unitContainer);
        r = oUnit->saveValues(sDataFilePath, oUnitAttr->dataSection, true, oUnitAttr->dataType);
    }
    else
    {
        if (oUnitAttr->userData)
        {
            TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
            r = oTableWin->saveWin(sDataFilePath, oUnitAttr->dataType, oUnitAttr->dataParam);
        }
    }
    if (r<0)
    {
        cxPrompt() << "save unit " << oUnitAttr->unitCaption << " fail. file : " << sDataFilePath << " can not save!!!";
        return false;
    }
    else
    {
        cxPrompt() << "save unit " << oUnitAttr->unitCaption << " sucess. file : " << sDataFilePath;
        return true;
    }
}

bool ConfigselfWin::loadUnitData(CxUnitAttr *oUnitAttr, const UnitUi & unitUi, const string &sDataFilePath)
{
    GM_INVALID_RETURE_(oUnitAttr, false);
    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);

    if (oUnitAttr->dataSection.size()>0)
    {
        if (CxFileSystem::isExist(sDataFilePath))
            oUnit->loadValues(sDataFilePath, oUnitAttr->dataSection, oUnitAttr->dataType);
        DescribeWin::controlsViewOut(oUnit, unitUi.unitContainer);
        return true;
    }
    else
    {
        if (oUnitAttr->userData)
        {
            TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
            oTableWin->loadData(sDataFilePath, oUnitAttr->dataType, oUnitAttr->dataParam);
            return true;
        }
        else
        {
            cxPrompt() << "load unit " << oUnitAttr->unitCaption << " userData is empty!!!";
        }
    }
    return false;
}

bool ConfigselfWin::initUnitData(CxUnitAttr *oUnitAttr, const UnitUi &unitUi, bool bToDefault)
{
    GM_INVALID_RETURE_(oUnitAttr, false);
    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);

    if (oUnitAttr->dataSection.size()>0)
    {
        if (bToDefault)
        {
            oUnit->restoreDefaultValues();
        }
        DescribeWin::controlsViewOut(oUnit, unitUi.unitContainer);
        cxPrompt() << "load unit success.";
        return true;
    }
    else
    {
        if (oUnitAttr->userData)
        {
            TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
            oTableWin->clearData();
            cxPrompt() << "load unit success.";
            return true;
        }
    }
    return false;
}

void ConfigselfWin::saveCurrentUnit()
{
    UnitUi unitUi = _currentUnitUi;
    CxUnitAttr * oUnitAttr = _currentUnitAttr;
    if (! oUnitAttr)
    {
        cxPrompt() << "save DataName fail. oUnitAttr==NULL";
        return;
    }
    saveUnitData(oUnitAttr, unitUi, _dataPath);
}

void ConfigselfWin::saveCurrentUnitData()
{
    UnitUi unitUi = _currentUnitUi;
    CxUnitAttr * oUnitAttr = _currentUnitAttr;
    if (! oUnitAttr)
    {
        cxPrompt() << "save DataName fail. oUnitAttr==NULL";
        return;
    }
    saveUnitData(oUnitAttr, unitUi, _dataPath, false);
}

void ConfigselfWin::clearProjectWin()
{
    while (ui->stackedWidget->count()>0)
    {
        QWidget * oWidget = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(oWidget);
        delete oWidget;
    }
    ui->listWidget->clear();
    _catalogUis.clear();
}

CxCatalogAttr *ConfigselfWin::findCatalogAttr(const std::string &sCatalogTitle, int &iRow, std::vector<UnitUi> &unitUis)
{
    CxCatalogAttr * oCatalogAttr = _describeProject.findByTitle(sCatalogTitle);
    GM_INVALID_RETURE_(oCatalogAttr, NULL);

    QString sCatalogTitle2 = QtCommonString::gbkToQString(sCatalogTitle);
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        if (ui->listWidget->item(i)->text() == sCatalogTitle2)
        {
            iRow = i;
            break;
        }
    }

    std::vector<UnitUi> unitUisEmpty;
    unitUis = CxContainer::value(_catalogUis, sCatalogTitle, unitUisEmpty);

    return oCatalogAttr;
}

CxUnitAttr *ConfigselfWin::findUnitAttr(const std::string &sCatalogTitle, const string &sUnitCaption, int &iTab, UnitUi &unitUi)
{
    QString sCatalogTitle2 = QtCommonString::gbkToQString(sCatalogTitle);
    QString sUnitCation2 = QtCommonString::gbkToQString(sUnitCaption);
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        QWidget * oWidget = ui->stackedWidget->widget(i);
        if (oWidget && oWidget->metaObject()->className() == QTabWidget::staticMetaObject.className())
        {
            QTabWidget * oTabWidget = reinterpret_cast<QTabWidget*>(oWidget);
            if (oTabWidget->toolTip() == sCatalogTitle2)
            {
                for (int j = 0; j < oTabWidget->count(); ++j)
                {
                    if (oTabWidget->tabText(j) == sUnitCation2)
                    {
                        iTab = j;
                    }
                }
            }
        }
    }

    std::vector<UnitUi> unitUis;
    unitUis = CxContainer::value(_catalogUis, sCatalogTitle, unitUis);
    for (int i = 0; i < unitUis.size(); ++i)
    {
        if (unitUis.at(i).unitCaption == sUnitCaption)
        {
            unitUi = unitUis.at(i);
        }
    }

    const std::vector<CxCatalogAttr*> & _catalogAttrs = _describeProject.catalogAttrs();
    for (size_t i = 0; i < _catalogAttrs.size(); ++i)
    {
        CxCatalogAttr * oCa = _catalogAttrs.at(i);
        const std::vector<CxUnitAttr*> & unitAttrs = oCa->catalog->unitAttrs();
        for (size_t j = 0; j < unitAttrs.size(); ++j)
        {
            CxUnitAttr * oUa = unitAttrs.at(i);
            if (oUa->unitCaption == sUnitCaption)
            {
                return oUa;
            }
        }
    }
    return NULL;
}

QTabWidget *ConfigselfWin::getTableWidgetLeft()
{
    return ui->tabWidgetLeft;
}

QTabWidget *ConfigselfWin::getTableWidgetRight()
{
    return ui->tabWidgetRight;
}

void ConfigselfWin::saveProject()
{
    const std::vector<CxCatalogAttr*> & catalogAttrs = _describeProject.catalogAttrs();
    for (size_t i = 0; i < catalogAttrs.size(); ++i)
    {
        const CxCatalogAttr * oCatalogAttr = catalogAttrs.at(i);
        saveCatalog(oCatalogAttr->catalogTitle());
    }
    int iResult = CxDescribeManager::saveProject(_describeProject, _projectFilePath);
    cxPrompt() << "save project result=" << iResult << " file : " << _projectFilePath;
}

UnitUi ConfigselfWin::createUnitUi(QWidget * oTabConfig, QWidget * oTabData)
{
    QWidget *oParentWidget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;

    oParentWidget = new QWidget();
//    oParentWidget->setObjectName(QString::fromUtf8("tab"));
    verticalLayout_2 = new QVBoxLayout(oParentWidget);
    verticalLayout_2->setSpacing(6);
    verticalLayout_2->setContentsMargins(11, 11, 11, 11);
//    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    splitter = new QSplitter(oParentWidget);
//    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Horizontal);
//    groupBox_2 = new QGroupBox(splitter);
//    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(5);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(oTabConfig->sizePolicy().hasHeightForWidth());
    oTabConfig->setSizePolicy(sizePolicy);
    splitter->addWidget(oTabConfig);
//    groupBox_1 = new QGroupBox(splitter);
//    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(oTabData->sizePolicy().hasHeightForWidth());
    oTabData->setSizePolicy(sizePolicy1);


    splitter->addWidget(oTabData);

    verticalLayout_2->addWidget(splitter);

    UnitUi unitUi;
    unitUi.tab = oParentWidget;
    unitUi.unitContainer = oTabConfig;
    unitUi.unitAttrContainer = oTabData;

    return unitUi;
}

void ConfigselfWin::on_listWidget_currentRowChanged(int currentRow)
{
    if (_isDeleting) return;

    if (_currentRow == currentRow) return;

    setCurrentRow(currentRow, true);
}

void ConfigselfWin::setCurrentUnit()
{
    //设置当前 Unit
    int iTab = -1;
    std::vector<UnitUi> unitUis;
    string sUnitCaption;
    CxUnitAttr * oUnitAttr = NULL;
    UnitUi unitUi;
    QWidget * oWidget = ui->stackedWidget->currentWidget();
    if (_currentCatalogAttr && oWidget)
    {
        if (oWidget && oWidget->metaObject()->className() == QTabWidget::staticMetaObject.className())
        {
            QTabWidget * oTabWidget = reinterpret_cast<QTabWidget*>(oWidget);
            iTab = oTabWidget->currentIndex();
            unitUis = CxContainer::value(_catalogUis, _currentCatalogTitle, unitUis);
            if (iTab>=0 && iTab<unitUis.size())
            {
                unitUi = unitUis.at(iTab);
            }
            CxDescribeCatalog * oCatalog = _currentCatalogAttr->catalog;
            GM_ASSER_OBJECT(oCatalog);
            const std::vector<CxUnitAttr*> & units = oCatalog->unitAttrs();
            if (iTab>=0 && iTab<units.size())
            {
                oUnitAttr = units.at(iTab);
                sUnitCaption = oUnitAttr->unitCaption;
            }
        }
    }

    _currentUnitUis = unitUis;
    _currentUnitCaption = sUnitCaption;
    _currentUnitAttr = oUnitAttr;
    _currentTab = iTab;
    _currentUnitUi = unitUi;

    refreshAttribute();
}

void ConfigselfWin::on_tabWidget_currentChanged(int index)
{
    setCurrentUnit();

    _configselfPluginProxy->unitChanged(this);
}

void ConfigselfWin::on_saveProjectBn_clicked()
{
    saveProject();
}

void ConfigselfWin::on_saveCatalogBn_clicked()
{
    saveCatalog();
}

void ConfigselfWin::on_saveCatalogDataBn_clicked()
{
    saveCurrentCatalog();
}

void ConfigselfWin::on_reloadDataNameBn_clicked()
{
    UnitUi unitUi = _currentUnitUi;
    CxUnitAttr * oUnitAttr = _currentUnitAttr;
    if (! oUnitAttr)
    {
        cxPrompt() << "reload DataName fail. oUnitAttr==NULL";
        return;
    }
    GM_ASSER_OBJECT(unitUi.unitAttrContainer);
    GM_ASSER_OBJECT(unitUi.unitContainer);

    DescribeWin::controlsViewIn(unitUi.unitAttrContainer, oUnitAttr);

    string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, _dataPath);
    if (oUnitAttr->dataName.size()>0 && ! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "reload DataName " << oUnitAttr->unitCaption << " fail. file : " << sDataFilePath << " is invalid!!!";
    }

    loadUnitData(oUnitAttr, unitUi, sDataFilePath);
}

void ConfigselfWin::on_saveDataNameBn_clicked()
{
    saveCurrentUnit();
}

void ConfigselfWin::on_redefaultDataNamBn_clicked()
{
    UnitUi unitUi = _currentUnitUi;
    CxUnitAttr * oUnitAttr = _currentUnitAttr;
    if (! oUnitAttr)
    {
        cxPrompt() << "redefault DataName fail. oUnitAttr==NULL ";
        return;
    }
    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);
    GM_ASSER_OBJECT(unitUi.unitAttrContainer);
    GM_ASSER_OBJECT(unitUi.unitContainer);

    DescribeWin::controlsViewIn(unitUi.unitAttrContainer, oUnitAttr);

    if (oUnitAttr->dataSection.size()>0)
    {
        DescribeWin::controlsViewOut(oUnit, unitUi.unitContainer, true);
        cxPrompt() << "redefault DataName Success.";
    }
    else
    {
        if (oUnitAttr->userData)
        {
            TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
            oTableWin->redefault();
            cxPrompt() << "redefault DataName .";
        }
    }
}

void ConfigselfWin::initCurrent()
{
    _currentRow = -1;
    _currentCatalogTitle.clear();
    _currentCatalogAttr = NULL;
    _currentUnitUis.clear();

    _currentUnitCaption.clear();
    _currentUnitAttr = NULL;
    _currentTab = -1;
    _currentUnitUi = UnitUi();
}

void ConfigselfWin::loadPlugins()
{
    string sDescribePlugin = CxApplication::findArgument("pi");
    if (sDescribePlugin.empty())
    {
        sDescribePlugin = CxApplication::findConfig(CS_SectionProgramConfig, string("pi"), std::string());
    }
    cxPrompt() << "loadPlugins begin: " << sDescribePlugin;
    map<string,string> sDescribePlugins = CxString::splitToMap(sDescribePlugin, '=', ';');
#ifdef GM_PI_SettingPlugin
    if (!CxContainer::contain(sDescribePlugins, string("SettingPlugin")))
        sDescribePlugins["SettingPlugin"] = string();
#endif
#ifdef GM_PI_TerminalPlugin
    if (!CxContainer::contain(sDescribePlugins, string("TerminalPlugin")))
        sDescribePlugins["TerminalPlugin"] = string();
#endif
#ifdef GM_PI_CreatorPlugin
    if (!CxContainer::contain(sDescribePlugins, string("CreatorPlugin")))
        sDescribePlugins["CreatorPlugin"] = string();
#endif
    for (std::map<string, string>::const_iterator it = sDescribePlugins.begin(); it != sDescribePlugins.end(); ++it)
    {
        const string & sPluginName = it->first;
        const string & sPluginParam = it->second;
        _configselfPluginProxy->add(sPluginName, sPluginParam);
        cxPrompt() << "loadPlugins : " << sPluginName;
    }
    cxPrompt() << "loadPlugins end. ";
}


ConfigselfPluginProxy::ConfigselfPluginProxy()
{
}

ConfigselfPluginProxy::~ConfigselfPluginProxy()
{
    deleteAndClear();
}

void ConfigselfPluginProxy::add(const string &sPluginName, const string &sPluginParam)
{
    ConfigselfPluginBase * oPlugin = ConfigselfPluginFactoryManager::createObject(sPluginName);
    if (oPlugin)
    {
        _plugins.push_back(oPlugin);
    }
}

void ConfigselfPluginProxy::deleteAndClear()
{
    if (_plugins.size()>0)
    {
        for (size_t i = 0; i < _plugins.size(); ++i)
        {
            ConfigselfPluginBase * o = _plugins.at(i);
            delete o;
        }
        _plugins.clear();
    }
}

void ConfigselfPluginProxy::init(ConfigselfWin *oConfigselfWin)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        o->initImpl(oConfigselfWin);
    }
}

void ConfigselfPluginProxy::uninit()
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        o->uninitImpl();
    }
}

bool ConfigselfPluginProxy::projectChange(const ConfigselfWin *oConfigselfWin, int iState)
{
    bool r = true;
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        if (! o->projectChangeImpl(oConfigselfWin, iState))
        {
            r = false;
            break;
        }
    }
    return r;
}

bool ConfigselfPluginProxy::catalogBeforeChange(const ConfigselfWin *oConfigselfWin, const string &sOldCatalogTitle, const string &sNewCatalogTitle)
{
    bool r = true;
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        if (! o->catalogBeforeChangeImpl(oConfigselfWin, sOldCatalogTitle, sNewCatalogTitle))
        {
            r = false;
            break;
        }
    }
    return r;
}

void ConfigselfPluginProxy::catalogChanged(const ConfigselfWin *oConfigselfWin)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        o->catalogChangedImpl(oConfigselfWin);
    }
}

bool ConfigselfPluginProxy::unitBeforeChange(const ConfigselfWin *oConfigselfWin, const string &sOldUnitCaption, const string &sNewUnitCaption)
{
    bool r = true;
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        if (! o->unitBeforeChangeImpl(oConfigselfWin, sOldUnitCaption, sNewUnitCaption))
        {
            r = false;
            break;
        }
    }
    return r;
}

void ConfigselfPluginProxy::unitChanged(const ConfigselfWin *oConfigselfWin)
{
    for (size_t i = 0; i < _plugins.size(); ++i)
    {
        ConfigselfPluginBase * o = _plugins.at(i);
        o->unitChangedImpl(oConfigselfWin);
    }
}

std::vector<CxFactoryTemplate<ConfigselfPluginBase> *> *ConfigselfPluginBase::factoriesContainer()
{
    static std::vector<CxFactoryTemplate<ConfigselfPluginBase> *> m;
    return &m;
}

