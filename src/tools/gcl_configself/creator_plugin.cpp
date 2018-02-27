#include "creator_plugin.h"


#include <qtwincontrol/describe_win.h>
#include <qtwincontrol/table_win.h>


using namespace std;


CreatorPluginFactory * fn_oCreatorPluginFactory()
{
    return ConfigselfPluginFactoryManager::createAndRegister<CreatorPluginFactory>();
}
CreatorPluginFactory * f_oCreatorPluginFactory = fn_oCreatorPluginFactory();


CreatorPlugin::CreatorPlugin()
{
    _canCurrentCatalogLoad = true;
}

CreatorPlugin::~CreatorPlugin()
{
}

void CreatorPlugin::catalogChangedImpl(const ConfigselfWin *oConfigselfWin)
{
    _canCurrentCatalogLoad = false;
    dataNameListWidget->clear();
    //clear UnitUi
    const std::vector<UnitUi> & unitUis = oConfigselfWin->currentUnitUis();
    const std::vector<CxUnitAttr*> & unitAttrs = oConfigselfWin->currentUnitAttrs();
    GM_INVALID_RETURE(unitUis.size()==unitAttrs.size());
    for (size_t i = 0; i < unitAttrs.size(); ++i)
    {
        CxUnitAttr * oUnitAttr = unitAttrs.at(i);
        const UnitUi & unitUi = unitUis.at(i);
        CxDescribeUnit * oUnit = oUnitAttr->unit;
        GM_INVALID_CONTINUE(oUnit);
        oUnitAttr->dataName = "";
        if (oUnitAttr->userData)
        {
            TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
            oTableWin->clearData();
        }
        else
        {
            oUnit->restoreDefaultValues();
            DescribeWin::controlsViewOut(oUnit, unitUi.unitContainer);
        }
    }

    //*unitAttrContainer -> setVisible(false);
    for (size_t i = 0; i < unitUis.size(); ++i)
    {
        const UnitUi & unitUi = unitUis.at(i);
        if (unitUi.unitAttrContainer) unitUi.unitAttrContainer->setVisible(false);
    }

    //*add dataNameList
    string sCatalogTitle = oConfigselfWin->currentCatalogTitle();
    vector<CatalogUnify> * oCatalogUnifys = findCatalogUnifys(sCatalogTitle);
    if (oCatalogUnifys)
    {
        for (size_t i = 0; i < oCatalogUnifys->size(); ++i)
        {
            const CatalogUnify & catalogUnify = oCatalogUnifys->at(i);
            dataNameListWidget->addItem(QtCommonString::gbkToQString(catalogUnify.dataName));
        }
    }

    //*set button state
    bool bEnabled = catalogType(sCatalogTitle) > CreatorCatalogType_other;
    newDataNameBn->setEnabled(bEnabled);
    copyDataNameBn->setEnabled(bEnabled && dataNameListWidget->count()>0);
    deleteDataNameBn->setEnabled(bEnabled && dataNameListWidget->currentRow()>0);
    saveDataNameBn->setEnabled(bEnabled && dataNameListWidget->count()>0);
    _canCurrentCatalogLoad = true;
}

void CreatorPlugin::initImpl(ConfigselfWin *oConfigselfWin)
{
    configselfWin = oConfigselfWin;

    loadSelfConfig();

    initUi();
}

void CreatorPlugin::uninitImpl()
{
    return;
    saveSelfConfig();
}

void CreatorPlugin::loadSelfConfig()
{
    _titleCreatorUnit = CxResource::findResourceString("CreatorUnitFile");
    _titleCreatorCatalog = CxResource::findResourceString("CreatorCatalogFile");
    _titleCreatorProject = CxResource::findResourceString("CreatorProjectFile");

    if (_titleCreatorUnit.empty()) _titleCreatorUnit = QtCommonString::gbkToStdString(QString::fromUtf8("生成单元描述文件"));
    if (_titleCreatorCatalog.empty()) _titleCreatorCatalog = QtCommonString::gbkToStdString(QString::fromUtf8("生成编目文件"));
    if (_titleCreatorProject.empty()) _titleCreatorProject = QtCommonString::gbkToStdString(QString::fromUtf8("生成项目文件"));

    string sFilePath = CxApplication::findConfig(CS_SectionProgramConfig, string("DescribeCatalogsFile"), std::string());
    if (sFilePath.empty()) sFilePath = "describe/1.catalog.datas.ini";
    {
        string sDescribeConfigFilePath = CxFileSystem::fullFilePath(CxApplication::applicationDeployPath(), sFilePath);
        CxSkverBase * oProfile = CxSkverManager::beginUse(sDescribeConfigFilePath);
        if (oProfile)
        {
            const std::map<std::string, std::map<std::string, std::string> > & sRows = oProfile->getSectionEntryValues();

            for (map<string, map<string, string> >::const_iterator itRows = sRows.begin(); itRows != sRows.end(); ++itRows)
            {
                //*->CxCatalogAttr application catalog
                const string & sSection = itRows->first;
                if (sSection == CxDescribeStrings::self())
                    continue;

                const map<string, string> & sRow = itRows->second;

                string sDataName = CxContainer::value(sRow, CxDescribeStrings::dataName());
                string sCatalogTitle = CxContainer::value(sRow, CxDescribeStrings::catalogTitle());

                addCatalogUnify(sCatalogTitle, sDataName);
            }
            CxSkverManager::endUse(oProfile);
        }
        if (_catalogUnifys.size()>0)
        {
            _describeCatalogsFilePath = sDescribeConfigFilePath;
        }
    }
    if (_catalogUnifys.empty())
    {
        string sDescribePath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "describe");
        vector<CxFilePathInfo> pathInfos;
        CxFileSystem::scanDir(sDescribePath, pathInfos);
        for (size_t i = 0; i < pathInfos.size(); ++i)
        {
            const CxFilePathInfo & filePathInfo = pathInfos.at(i);
            const string sFileName = filePathInfo.fileName;
            if (CxString::endWith(sFileName, "describe.ini"))
            {
                addCatalogUnify(_titleCreatorUnit, sFileName);
            }
            else if (CxString::endWith(sFileName, "catalog.ini"))
            {
                addCatalogUnify(_titleCreatorCatalog, sFileName);
            }
            else if (CxString::endWith(sFileName, "project.ini"))
            {
                addCatalogUnify(_titleCreatorProject, sFileName);
            }
        }
    }

}

void CreatorPlugin::saveSelfConfig()
{
    if (_describeCatalogsFilePath.size()>0)
    {
        CxSkverBase * oProfile = CxSkverManager::beginUse(_describeCatalogsFilePath, false);
        if (oProfile)
        {
            int iIndex = 1;
            for (std::map<string, vector<CatalogUnify> >::const_iterator it = _catalogUnifys.begin(); it != _catalogUnifys.end(); ++it)
            {
                const string & sCatalogTitle = it->first;
                const vector<CatalogUnify> & catalogUnifies = it->second;

                for (size_t i = 0; i < catalogUnifies.size(); ++i)
                {
                    const CatalogUnify & catalogUnify = catalogUnifies.at(i);
                    oProfile->prepareSet("catalogdata"+CxString::toString(iIndex));
                    oProfile->setValue(CxDescribeStrings::catalogTitle(), sCatalogTitle);
                    oProfile->setValue(CxDescribeStrings::dataName(), catalogUnify.dataName);
                    ++iIndex;
                }
            }
            CxSkverManager::endUse(oProfile, true);
        }
    }
}

vector<CatalogUnify> * CreatorPlugin::findCatalogUnifys(const std::string &sCatalogTitle)
{
    map<string, vector<CatalogUnify> >::iterator it = _catalogUnifys.find(sCatalogTitle);
    if (it != _catalogUnifys.end())
    {
        vector<CatalogUnify> & catalogUnifys = it->second;
        return & catalogUnifys;
    }
    return NULL;
}

CatalogUnify *CreatorPlugin::findCatalogUnify(const string &sCatalogTitle, const string &sDataName)
{
    map<string, vector<CatalogUnify> >::iterator it = _catalogUnifys.find(sCatalogTitle);
    if (it != _catalogUnifys.end())
    {
        vector<CatalogUnify> & catalogUnifys = it->second;
        for (size_t i = 0; i < catalogUnifys.size(); ++i)
        {
            CatalogUnify & catalogUnify = catalogUnifys.at(i);
            if (catalogUnify.dataName == sDataName)
            {
                return & catalogUnify;
            }
        }
    }
    return NULL;
}

CatalogUnify CreatorPlugin::addCatalogUnify(const string &sCatalogTitle, const std::string &sDataName)
{
    vector<CatalogUnify> * oCatalogUnifys = findCatalogUnifys(sCatalogTitle);
    if (! oCatalogUnifys)
    {
        _catalogUnifys[sCatalogTitle] = vector<CatalogUnify>();
        oCatalogUnifys = findCatalogUnifys(sCatalogTitle);
    }

    GM_ASSER_OBJECT(oCatalogUnifys);

    CatalogUnify catalogUnify(sDataName, sCatalogTitle, sCatalogTitle);
    oCatalogUnifys->push_back(catalogUnify);
    return catalogUnify;
}

int CreatorPlugin::removeCatalogUnify(const string &sCatalogTitle, const string &sDataName)
{
    int iResult = 0;

    string sOldFilePath = CxFileSystem::mergeFilePath(configselfWin->dataPath(), sDataName);
    string sFileName = CxFileSystem::extractFileName(sOldFilePath);
    string sNewFilePath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");
    sNewFilePath = CxFileSystem::mergeFilePath(sNewFilePath, sFileName);
    CxFileSystem::renameFile(sOldFilePath, sNewFilePath);

    vector<CatalogUnify> * oCatalogUnifys = findCatalogUnifys(sCatalogTitle);
    if (oCatalogUnifys)
    {
        for(typename vector<CatalogUnify>::iterator it = oCatalogUnifys->begin(); it != oCatalogUnifys->end();)
        {
            const CatalogUnify & catalogUnify = * it;
            if (catalogUnify.dataName == sDataName)
            {
                oCatalogUnifys->erase(it);
                ++iResult;
                continue;
            }
            ++it;
        }
    }
    return iResult;
}

void CreatorPlugin::initUi()
{
    QTabWidget * oTabWidget = configselfWin->getTableWidgetRight();

    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    verticalLayout_2 = new QVBoxLayout(tab_2);
    verticalLayout_2->setSpacing(0);
    verticalLayout_2->setContentsMargins(11, 11, 11, 11);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalLayout_2->setContentsMargins(0, 0, 0, 0);
    dataNameListWidget = new QListWidget(tab_2);
    dataNameListWidget->setObjectName(QString::fromUtf8("dataNameListWidget"));

    verticalLayout_2->addWidget(dataNameListWidget);

    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->setSpacing(6);
    horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));

    verticalLayout_2->addLayout(horizontalLayout_5);

    newDataNameBn = new QPushButton(tab_2);
    //    newDataNameBn->setObjectName(QString::fromUtf8("newDataNameBn_2"));
    newDataNameBn->setMinimumSize(QSize(0, 30));
    newDataNameBn->setText(QString::fromUtf8("新增数据文件"));

    verticalLayout_2->addWidget(newDataNameBn);

    copyDataNameBn = new QPushButton(tab_2);
    //    copyDataNameBn->setObjectName(QString::fromUtf8("copyDataNameBn_2"));
    copyDataNameBn->setMinimumSize(QSize(0, 30));
    copyDataNameBn->setText(QString::fromUtf8("复制数据文件"));

    verticalLayout_2->addWidget(copyDataNameBn);

    deleteDataNameBn = new QPushButton(tab_2);
    //    deleteDataNameBn->setObjectName(QString::fromUtf8("deleteDataNameBn_2"));
    deleteDataNameBn->setMinimumSize(QSize(0, 30));
    deleteDataNameBn->setText(QString::fromUtf8("删除数据文件"));

    verticalLayout_2->addWidget(deleteDataNameBn);

    saveDataNameBn = new QPushButton(tab_2);
//    saveDataNameBn->setObjectName(QString::fromUtf8("saveDataNameBn_2"));
    saveDataNameBn->setMinimumSize(QSize(0, 30));
    saveDataNameBn->setText(QString::fromUtf8("保存到数据文件"));

    verticalLayout_2->addWidget(saveDataNameBn);

    oTabWidget->addTab(tab_2, QString::fromUtf8("数据文件"));

    oTabWidget->setVisible(true);

    connect(dataNameListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(on_dataNameListWidget_currentRowChanged(int)));
    connect(newDataNameBn, SIGNAL(clicked()), this, SLOT(on_newDataNameBn_clicked()));
    connect(copyDataNameBn, SIGNAL(clicked()), this, SLOT(on_copyDataNameBn_clicked()));
    connect(deleteDataNameBn, SIGNAL(clicked()), this, SLOT(on_deleteDataNameBn_clicked()));
    connect(saveDataNameBn, SIGNAL(clicked()), this, SLOT(on_saveDataNameBn_clicked()));
}

void CreatorPlugin::on_newDataNameBn_clicked()
{
    newDataName(false);
}

void CreatorPlugin::newDataName(bool bIsCopy)
{
    string sCatalogTitle = configselfWin->currentCatalogTitle();
    GM_VALID_RETURE(sCatalogTitle.empty());
    QString sDataName0;

    CreatorCatalogTypeEnum eType = catalogType(sCatalogTitle);
    if (eType<=CreatorCatalogType_other)
    {
        cxPrompt() << "can not new data. for " << sCatalogTitle << " is not creator!";
        return;
    }

    if (eType == CreatorCatalogType_describe)
    {
        sDataName0 = QString("describe%1").arg(CxTime::currentMsepoch());
    }
    else if (eType == CreatorCatalogType_catalog)
    {
        sDataName0 = QString("catalog%1").arg(CxTime::currentMsepoch());
    }
    else if (eType == CreatorCatalogType_project)
    {
        sDataName0 = QString("project%1").arg(CxTime::currentMsepoch());
    }

    while (1)
    {
        const QString sTitle = QString::fromUtf8("请输入：");
        const QString sValue1Title = QString::fromUtf8("名称en（唯一）：");
        const QString sValue2Title = QString::fromUtf8("标识  （台头）：");
        QString sDataTitle0;

        if (QtCommonDialog::DialogInput2(sTitle, sValue1Title, sValue2Title, sDataName0, sDataTitle0))
        {
            std::string sDataName1 = QtCommonString::gbkToStdString(sDataName0);
            std::string sDataTitle1 = QtCommonString::gbkToStdString(sDataTitle0);
            std::string sDataName2 = sDataName1;
            if (sDataName2.empty())
            {
                QtCommonDialog::ShowWarning(QString::fromUtf8("数据文件不能为空"));
                continue;
            }
            if (eType == CreatorCatalogType_describe)
            {
                sDataName2 += ".describe.ini";
            }
            else if (eType == CreatorCatalogType_catalog)
            {
                sDataName2 += ".catalog.ini";
            }
            else if (eType == CreatorCatalogType_project)
            {
                sDataName2 += ".project.ini";
            }

            if (findCatalogUnify(sCatalogTitle, sDataName2))
            {
                QtCommonDialog::ShowWarning(QString::fromUtf8("数据文件不能重复"));
                continue;
            }

            //保存当前
            configselfWin->saveCurrentCatalogData();

            _canCurrentCatalogLoad = false;

            //加到内存容器中
            addCatalogUnify(sCatalogTitle, sDataName2);
            //加到界面
            dataNameListWidget->addItem(QtCommonString::gbkToQString(sDataName2));
            dataNameListWidget->setCurrentRow(dataNameListWidget->count()-1);
            //*
            const std::vector<UnitUi> & unitUis = configselfWin->currentUnitUis();
            const std::vector<CxUnitAttr*> & unitAttrs = configselfWin->currentUnitAttrs();
            GM_INVALID_RETURE(unitUis.size()==unitAttrs.size());
            //设置 oDescribe 的 值
            CxDescribeBase * oDescribe;
            for (size_t i = 0; i < unitAttrs.size(); ++i)
            {
                CxUnitAttr * oUnitAttr = unitAttrs.at(i);
                const UnitUi & unitUi = unitUis.at(i);
                CxDescribeUnit * oUnit = oUnitAttr->unit;
                GM_INVALID_CONTINUE(oUnit);
                oUnitAttr->dataName = dataNameMySelf(sDataName2);
                if (oUnitAttr->userData)
                {
                    if (! bIsCopy)
                    {
                        TableWin * oTableWin = (TableWin *)oUnitAttr->userData;
                        oTableWin->clearData();
                    }
                }
                else
                {
                    if (! bIsCopy)
                    {
                        oUnit->restoreDefaultValues();
                    }
                    if (eType == CreatorCatalogType_describe)
                    {
                        oDescribe = oUnit->describe(CxDescribeStrings::unitName());
                        if (oDescribe) oDescribe->valueFromString(sDataName1);
                        oDescribe = oUnit->describe(CxDescribeStrings::unitTitle());
                        if (oDescribe) oDescribe->valueFromString(sDataTitle1);
                    }
                    else if (eType == CreatorCatalogType_catalog)
                    {
                        oDescribe = oUnit->describe(CxDescribeStrings::catalogName());
                        if (oDescribe) oDescribe->valueFromString(sDataName1);
                        oDescribe = oUnit->describe(CxDescribeStrings::catalogTitle());
                        if (oDescribe) oDescribe->valueFromString(sDataTitle1);
                    }
                    else if (eType == CreatorCatalogType_project)
                    {
                        oDescribe = oUnit->describe(CxDescribeStrings::name());
                        if (oDescribe) oDescribe->valueFromString(sDataName1);
                        oDescribe = oUnit->describe(CxDescribeStrings::title());
                        if (oDescribe) oDescribe->valueFromString(sDataTitle1);
                    }
                    DescribeWin::controlsViewOut(oUnit, unitUi.unitContainer);
                }
//                //如果新增，设置界面空白
//                if (! bIsCopy) configselfWin->initUnitData(oUnitAttr, unitUi, false);
            }
            //如果复制，只要保存
            if (bIsCopy) configselfWin->saveCurrentCatalogData();
            _canCurrentCatalogLoad = true;
            break;
        }
        else
        {
            break;
        }
    }
}

CreatorCatalogTypeEnum CreatorPlugin::catalogType(const string &sCatalogTitle)
{
    if (CxString::contain(sCatalogTitle, _titleCreatorUnit))
    {
        return CreatorCatalogType_describe;
    }
    else if (CxString::contain(sCatalogTitle, _titleCreatorCatalog))
    {
        return CreatorCatalogType_catalog;
    }
    else if (CxString::contain(sCatalogTitle, _titleCreatorProject))
    {
        return CreatorCatalogType_project;
    }

    return CreatorCatalogType_none;
}

string CreatorPlugin::dataNameMySelf(const string &sDataName)
{
    return "./describe/" + sDataName;
}

void CreatorPlugin::setCurrentRow(int iCurrentRow, bool bSave)
{
    if (bSave) configselfWin->saveCurrentCatalogData();

    string sDataName = QtCommonString::gbkToStdString(dataNameListWidget->item(iCurrentRow) ? dataNameListWidget->item(iCurrentRow)->text() : "");
    configselfWin->reloadCatalogData(string(), dataNameMySelf(sDataName));
    deleteDataNameBn->setEnabled(iCurrentRow>0);
}

void CreatorPlugin::on_copyDataNameBn_clicked()
{
    newDataName(true);
}

void CreatorPlugin::on_deleteDataNameBn_clicked()
{
    int iCurrentRow = dataNameListWidget->currentRow();
    if (iCurrentRow>0)
    {
        string sCatalogTitle = configselfWin->currentCatalogTitle();
        GM_VALID_RETURE(sCatalogTitle.empty());
        QListWidgetItem * oItem = dataNameListWidget->item(iCurrentRow);
        GM_INVALID_RETURE(oItem);
        QString sDataName1 = oItem->text();
        if (QtCommonDialog::ShowQuery(QString::fromUtf8("? 请确认删除 : ")+sDataName1))
        {
            _canCurrentCatalogLoad = false;
            dataNameListWidget->setCurrentRow(iCurrentRow-1);
            setCurrentRow(iCurrentRow-1, false);
            QListWidgetItem * oItem = dataNameListWidget->takeItem(iCurrentRow);
            if (oItem)
            {
                string sDataName2 = QtCommonString::gbkToStdString(sDataName1);
                string sDataFilePath = CxFileSystem::cd(dataNameMySelf(sDataName2), configselfWin->dataPath());
                CxFileSystem::deleteFile(sDataFilePath);
                removeCatalogUnify(sCatalogTitle, QtCommonString::gbkToStdString(sDataName1));
                delete oItem;
            }
            _canCurrentCatalogLoad = true;
        }
    }
    else
    {
        cxPrompt() << "can not delete current row. for current row less 1!";
    }
}

void CreatorPlugin::on_saveDataNameBn_clicked()
{
    configselfWin->saveCurrentCatalogData();
}

void CreatorPlugin::on_dataNameListWidget_currentRowChanged(int currentRow)
{
    if (_canCurrentCatalogLoad)
    {
        int iCurrentRow = currentRow;
        setCurrentRow(iCurrentRow, true);
    }
}

