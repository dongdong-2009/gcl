#include "setup_win.h"
#include "ui_setup_win.h"



#include <qtwincontrol/describe_win.h>
#include <qtwincontrol/table_win.h>


using namespace std;

SetupWin::SetupWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SetupWin)
{
    ui->setupUi(this);

    CxInterinfoOut::addObserver(this);
}

SetupWin::~SetupWin()
{
    CxInterinfoOut::removeObserver(this);
    delete ui;
}

void SetupWin::interinfo_out(const string &sInfo, const string &sTitle, int type, int reason, int source, int target, int tag)
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
    ui->infoEd->append(QtCommonString::gbkToQString( sOutInfo.str()) );
}

void SetupWin::on_loadProjectBn_clicked()
{
    loadProject();
}

void SetupWin::loadProject()
{
    QString sFilePath;
    if (QtCommonDialog::DialogOpen(sFilePath))
    {
        string sFilePath2 = QtCommonString::gbkToStdString(sFilePath);
        if (CxFileSystem::isExist(sFilePath2))
        {
            if (CxDescribeManager::loadProject(_describeProject, sFilePath2) > 0)
            {
                this->setWindowTitle(QtCommonString::gbkToQString(_describeProject.projectTitle()));
                _describePath = CxFileSystem::extractPath(sFilePath2);
                ui->projectPathEd->setText(QtCommonString::gbkToQString(CxFileSystem::parentPath(_describePath)));
                refreshProjectWin();
            }
        }
    }
}

void SetupWin::refreshProjectWin()
{
    clearProjectWin();

    refreshCollectionsWin();
}

void SetupWin::refreshCollectionsWin()
{
    QListWidget * oLw = ui->listWidget;
    const std::vector<CxCollectionAttr*> & collectionAttrs = _describeProject.collectionAttrs();
    for (size_t i = 0; i < collectionAttrs.size(); ++i)
    {
        CxCollectionAttr * oCollectionAttr = collectionAttrs.at(i);
//        CxDescribeCollection * oCollection = oCollectionAttr->collection;
        QTabWidget * oTabWidget = new QTabWidget();
        ui->stackedWidget->addWidget(oTabWidget);
        QListWidgetItem * oItem = new QListWidgetItem();
        oLw->addItem(oItem);
        QtCommonWidget::setItemValue(oItem, oCollectionAttr->title);
    }
}

void SetupWin::refreshCollectionWin(const QString &sCollectionTitle, QTabWidget *oTabWidget)
{
    if (oTabWidget->count() > 0) return;

    string sProjectPath = QtCommonString::gbkToStdString(ui->projectPathEd->text());

    vector<UnitUi> unitUis;
    CxCollectionAttr * oCollectionAttr = _describeProject.findByTitle(QtCommonString::gbkToStdString(sCollectionTitle));
    GM_INVALID_RETURE(oCollectionAttr);
    CxDescribeCollection * oCollection = oCollectionAttr->collection;
    GM_INVALID_RETURE(oCollection);
    const std::vector<CxUnitAttr*> & units = oCollection->unitAttrs();
    for (size_t i = 0; i < units.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = units.at(i);
        CxDescribeUnit * oUnit = oUnitAttr->unit;


        QWidget * oTabConfig = NULL;
        string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, sProjectPath);
        if (oUnitAttr->dataSection.size()>0)
        {
            oTabConfig = DescribeWin::loadControls(oUnit);
            oUnit->loadValues(sDataFilePath, oUnitAttr->dataSection, oUnitAttr->dataType);
            DescribeWin::controlsViewOut(oUnit, oTabConfig);
        }
        else
        {
            QGroupBox * oGroupBox = new QGroupBox();
            oGroupBox->setTitle(QString::fromUtf8("配置数据"));
            oTabConfig = oGroupBox;
            TableWin * oTableWin = new TableWin();
            oTableWin->setDescribeUnit(oUnit);
            oTableWin->loadWin(oTabConfig, sDataFilePath, "", oUnitAttr->dataType, oUnitAttr->dataParam);
            oUnitAttr->userData = oTableWin;
        }

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

        UnitUi unitUi = createUnitUi(oTabConfig, oTabData);
        oTabWidget->addTab(unitUi.tab, QtCommonString::gbkToQString(oUnitAttr->title));
        unitUis.push_back(unitUi);
    }
    if (unitUis.size()>0)
    {
        collectionUis.push_back(unitUis);
    }
}

void SetupWin::clearProjectWin()
{
    while (ui->stackedWidget->count()>0)
    {
        QWidget * oWidget = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(oWidget);
        delete oWidget;
    }
    ui->listWidget->clear();
    collectionUis.clear();
}

CxUnitAttr *SetupWin::getCurrentUnit(int &iRow, int &iTab, UnitUi &unitUi)
{
    const std::vector<CxCollectionAttr*> & collectionAttrs = _describeProject.collectionAttrs();
    iRow = ui->stackedWidget->currentIndex();
    QWidget * oWidget = ui->stackedWidget->currentWidget();
    if (oWidget && oWidget->metaObject()->className() == QTabWidget::staticMetaObject.className())
    {
        QTabWidget * oTabWidget = reinterpret_cast<QTabWidget*>(oWidget);
        iTab = oTabWidget->currentIndex();

        if (iRow>=0 && iRow<collectionUis.size())
        {
            const std::vector<UnitUi> & unitUis = collectionUis.at(iRow);
            if (iTab>=0 && iTab<unitUis.size())
            {
                unitUi = unitUis.at(iTab);
            }
        }
        if (iRow>=0 && iRow<collectionAttrs.size())
        {
            CxCollectionAttr * oCollectionAttr = collectionAttrs.at(iRow);
            GM_ASSER_OBJECT(oCollectionAttr);
            CxDescribeCollection * oCollection = oCollectionAttr->collection;
            GM_ASSER_OBJECT(oCollection);
            const std::vector<CxUnitAttr*> & units = oCollection->unitAttrs();
            if (iTab>=0 && iTab<units.size())
            {
                return units.at(iTab);
            }
        }
    }

    return NULL;
}

void SetupWin::saveProject()
{

}

void SetupWin::saveCollectionWin(const QString &sCollectionTitle, const std::vector<UnitUi> &unitUis)
{
    CxCollectionAttr * oCollectionAttr = _describeProject.findByTitle(QtCommonString::gbkToStdString(sCollectionTitle));
    GM_INVALID_RETURE(oCollectionAttr);
    CxDescribeCollection * oCollection = oCollectionAttr->collection;
    GM_INVALID_RETURE(oCollection);
    const std::vector<CxUnitAttr*> & units = oCollection->unitAttrs();
    GM_INVALID_RETURE(units.size()==unitUis.size());

    string sProjectPath = QtCommonString::gbkToStdString(ui->projectPathEd->text());
    if (!CxFileSystem::isExist(sProjectPath))
    {
        cxPrompt() << "save collection fail. path : " << sProjectPath << " do not exist!!!";
        return;
    }

    for (size_t i = 0; i < units.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = units.at(i);
        const UnitUi & unitUi = unitUis.at(i);
        saveUnit(oUnitAttr, unitUi, sProjectPath);
    }

    string sDataFilePath = CxFileSystem::mergeFilePath(_describePath, oCollectionAttr->decribeFilePath);
    if (! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "save collection fail. file : " << sDataFilePath << " is invalid!!!";
        return;
    }
    CxDescribeManager::saveCollection(*oCollection, sDataFilePath);
}

bool SetupWin::saveUnit(CxUnitAttr * oUnitAttr, UnitUi unitUi, const std::string & sProjectPath)
{
    GM_INVALID_RETURE_(oUnitAttr, false);
    GM_INVALID_RETURE_(unitUi.unitAttrContainer, false);
    GM_INVALID_RETURE_(unitUi.unitContainer, false);
    GM_INVALID_RETURE_(CxFileSystem::isExist(sProjectPath), false);

    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);

    DescribeWin::controlsViewIn(unitUi.unitAttrContainer, oUnitAttr);

    string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, sProjectPath);
    if (oUnitAttr->dataName.size()>0 && ! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "save unit " << oUnitAttr->title << " fail. file : " << sDataFilePath << " is invalid!!!";
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
        cxPrompt() << "save unit " << oUnitAttr->title << " fail. file : " << sDataFilePath << " can not save!!!";
        return false;
    }
    else
    {
        cxPrompt() << "save unit " << oUnitAttr->title << " sucess. file : " << sDataFilePath;
        return true;
    }
}

UnitUi SetupWin::createUnitUi(QWidget * oTabConfig, QWidget * oTabData)
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

void SetupWin::on_listWidget_currentRowChanged(int currentRow)
{
    QListWidgetItem * oItem = ui->listWidget->item(currentRow);
    GM_INVALID_RETURE(oItem);

    ui->stackedWidget->setCurrentIndex(currentRow);
    QTabWidget * oTabWidget = (QTabWidget *)(ui->stackedWidget->widget(currentRow));
    refreshCollectionWin(oItem->text(), oTabWidget);
}

void SetupWin::on_saveProjectBn_clicked()
{
    saveProject();
}

void SetupWin::on_saveCollectionBn_clicked()
{
    QListWidgetItem * oItem = ui->listWidget->currentItem();
    GM_INVALID_RETURE(oItem);

    QTabWidget * oTabWidget = (QTabWidget *)(ui->stackedWidget->widget(ui->listWidget->currentRow()));
    GM_INVALID_RETURE(oTabWidget);
    GM_INVALID_RETURE(oTabWidget->count()>0);

    const std::vector<UnitUi> & unitUis = collectionUis.at(ui->listWidget->currentRow());
    saveCollectionWin(oItem->text(), unitUis);
}

void SetupWin::on_reloadDataNameBn_clicked()
{
    string sProjectPath = QtCommonString::gbkToStdString(ui->projectPathEd->text());
    if (!CxFileSystem::isExist(sProjectPath))
    {
        cxPrompt() << "reload DataName fail. path : " << sProjectPath << " do not exist!!!";
        return;
    }

    int iRow;
    int iTab;
    UnitUi unitUi;
    CxUnitAttr * oUnitAttr = getCurrentUnit(iRow, iTab, unitUi);
    if (! oUnitAttr)
    {
        cxPrompt() << "reload DataName fail. oUnitAttr==NULL iRow=" << iRow;
        return;
    }
    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);
    GM_ASSER_OBJECT(unitUi.unitAttrContainer);
    GM_ASSER_OBJECT(unitUi.unitContainer);

    DescribeWin::controlsViewIn(unitUi.unitAttrContainer, oUnitAttr);

    string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, sProjectPath);
    if (oUnitAttr->dataName.size()>0 && ! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "reload DataName " << oUnitAttr->title << " fail. file : " << sDataFilePath << " is invalid!!!";
    }

    if (oUnitAttr->dataSection.size()>0)
    {
        oUnit->loadValues(sDataFilePath, oUnitAttr->dataSection, oUnitAttr->dataType);
        DescribeWin::controlsViewOut(oUnit, unitUi.unitContainer);
        cxPrompt() << "reload DataName Success.";
    }
    else
    {
        if (oUnitAttr->userData)
        {
            TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
            oTableWin->reloadWin(sDataFilePath, oUnitAttr->dataType, oUnitAttr->dataParam);
            cxPrompt() << "reload DataName Success.";
        }
    }
}

void SetupWin::on_saveDataNameBn_clicked()
{
    string sProjectPath = QtCommonString::gbkToStdString(ui->projectPathEd->text());
    if (!CxFileSystem::isExist(sProjectPath))
    {
        cxPrompt() << "save DataName fail. path : " << sProjectPath << " do not exist!!!";
        return;
    }

    int iRow;
    int iTab;
    UnitUi unitUi;
    CxUnitAttr * oUnitAttr = getCurrentUnit(iRow, iTab, unitUi);
    if (! oUnitAttr)
    {
        cxPrompt() << "save DataName fail. oUnitAttr==NULL iRow=" << iRow;
        return;
    }
    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);
    GM_ASSER_OBJECT(unitUi.unitAttrContainer);
    GM_ASSER_OBJECT(unitUi.unitContainer);

    saveUnit(oUnitAttr, unitUi, sProjectPath);
}

void SetupWin::on_redefaultDataNamBn_clicked()
{
    string sProjectPath = QtCommonString::gbkToStdString(ui->projectPathEd->text());
    if (!CxFileSystem::isExist(sProjectPath))
    {
        cxPrompt() << "redefault DataName fail. path : " << sProjectPath << " do not exist!!!";
        return;
    }

    int iRow;
    int iTab;
    UnitUi unitUi;
    CxUnitAttr * oUnitAttr = getCurrentUnit(iRow, iTab, unitUi);
    if (! oUnitAttr)
    {
        cxPrompt() << "redefault DataName fail. oUnitAttr==NULL iRow=" << iRow;
        return;
    }
    CxDescribeUnit * oUnit = oUnitAttr->unit;
    GM_ASSER_OBJECT(oUnit);
    GM_ASSER_OBJECT(unitUi.unitAttrContainer);
    GM_ASSER_OBJECT(unitUi.unitContainer);

    DescribeWin::controlsViewIn(unitUi.unitAttrContainer, oUnitAttr);

    string sDataFilePath = CxFileSystem::cd(oUnitAttr->dataName, sProjectPath);
    if (oUnitAttr->dataName.size()>0 && ! CxString::isValidFullPath(sDataFilePath))
    {
        cxPrompt() << "redefault DataName " << oUnitAttr->title << " fail. file : " << sDataFilePath << " is invalid!!!";
    }

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
