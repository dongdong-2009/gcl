#include "terminal_plugin.h"

#include <QtGui>
#include <QAction>
#include <QMenuBar>
#include <QDialog>
#include <QGroupBox>
#include <QComboBox>
#include <QToolBar>
#include <QApplication>

using namespace std;


const string cs_terminalCustom = "terminal.custom";


string fn_stringMapAdd(const string &ss, char cMid, char cSplitCharacter, const std::string &sKey, const std::string &sValue)
{
    if (ss.empty())
        return sKey + cMid + sValue;
    else
        return ss + cSplitCharacter + sKey + cMid + sValue;
}

string fn_stringMapRemove(const string &ss, char cMid, char cSplitCharacter, const std::string &sKey)
{
    vector<string> sLines = CxString::split(ss, cSplitCharacter);
    string sFind = sKey+cMid;
    for (size_t i = 0; i < sLines.size(); ++i)
    {
        const string & sLine = sLines.at(i);
        if (sLine.find(sFind) != string::npos)
        {
            sLines.erase(sLines.begin()+i);
            break;
        }
    }
    return CxString::join(sLines, cSplitCharacter);
}


TerminalPluginFactory * fn_oTerminalPluginFactory()
{
    return ConfigselfPluginFactoryManager::createAndRegister<TerminalPluginFactory>();
}
TerminalPluginFactory * f_oTerminalPluginFactory = fn_oTerminalPluginFactory();


TerminalPlugin::TerminalPlugin()
{

}

TerminalPlugin::~TerminalPlugin()
{

}

void TerminalPlugin::initImpl(ConfigselfWin *oConfigselfWin)
{
    GM_INVALID_RETURE(oConfigselfWin);
    configselfWin = oConfigselfWin;

    initUi();
}

void TerminalPlugin::uninitImpl()
{
}

void TerminalPlugin::initUi()
{
    GM_INVALID_RETURE(configselfWin);

    _uiAddTerminalAct = new QAction(QIcon(":/images/add.png"), QString::fromUtf8("添加通信终端"), this);
    _uiAddTerminalAct->setStatusTip(tr("add terminal"));
    connect(_uiAddTerminalAct, SIGNAL(triggered()), this, SLOT(on_newTerminalBn_clicked()));

    _uiRemoveTerminalAct = new QAction(QIcon(":/images/remove.png"), QString::fromUtf8("移除通信终端"), this);
    _uiRemoveTerminalAct->setStatusTip(tr("remove terminal"));
    connect(_uiRemoveTerminalAct, SIGNAL(triggered()), this, SLOT(on_removeTerminalBn_clicked()));

    _uiTerminalMenu = configselfWin->menuBar()->addMenu(QString::fromUtf8("&T通信终端"));
    _uiTerminalMenu->addAction(_uiAddTerminalAct);
    _uiTerminalMenu->addAction(_uiRemoveTerminalAct);

    configselfWin->uiWinsToolBar()->addAction(_uiAddTerminalAct);
    configselfWin->uiWinsToolBar()->addAction(_uiRemoveTerminalAct);
}

bool TerminalPlugin::projectChangeImpl(const ConfigselfWin *oConfigselfWin, int iState)
{
    if (iState == ConfigselfProjectState_afterRefresh)
    {
        GM_INVALID_RETURE_(oConfigselfWin, false);

//        string sPath = CxFileSystem::mergeFilePath(oConfigselfWin->dataPath(), "terminal");
//        if (!CxFileSystem::isExist(sPath))
//        {
//            CxFileSystem::createDir(sPath);
//            cxPrompt() << "TerminalPlugin createDir :" << sPath;
//        }

        _terminalsConfigFilePath = CxApplication::findArgument("tf");
        if (_terminalsConfigFilePath.empty())
        {
            _terminalsConfigFilePath = CxApplication::findConfig(CS_SectionProgramConfig, string("tf"), std::string());
        }
        _terminalsConfigFilePath = CxFileSystem::cd(_terminalsConfigFilePath, configselfWin->dataPath());
        cxPrompt() << "TerminalPlugin terminalsConfigFilePath=" << _terminalsConfigFilePath;

        CxSkverBase * oProfile = CxSkverManager::beginUse( _terminalsConfigFilePath );
        if (oProfile)
        {
            std::string sTerminalNameTypes;
            string sProtocolUnitNames;
            string sChannelUnitNames;
            string sCatalogTitles;
            sTerminalNameTypes = oProfile->getValue("Terminals", string("NameTypes"), std::string());
            sProtocolUnitNames = oProfile->getValue("Terminals", string("ProtocolUnitNames"), std::string());
            sChannelUnitNames = oProfile->getValue("Terminals", string("ChannelUnitNames"), std::string());
            sCatalogTitles = oProfile->getValue("Terminals", string("CatalogTitles"), std::string());
            sTerminalNameTypes = CxString::unquote(sTerminalNameTypes, '\"');

            map<string, string> mapTerminalNameTypes = CxString::splitToMap(sTerminalNameTypes, '=', ',');
            map<string, string> mapProtocolUnitNames = CxString::splitToMap(sProtocolUnitNames, '=', ';');
            map<string, string> mapChannelUnitNames = CxString::splitToMap(sChannelUnitNames, '=', ';');
            map<string, string> mapCatalogTitles = CxString::splitToMap(sCatalogTitles, '=', ';');

            if (mapTerminalNameTypes.size() != mapProtocolUnitNames.size()
                    || mapChannelUnitNames.size() != mapCatalogTitles.size()
                    || mapTerminalNameTypes.size() != mapCatalogTitles.size()
                    )
            {
                cxPrompt() << "TerminalPlugin Terminals NameTypes load fail. for sTerminalNameTypes=" << sTerminalNameTypes
                           << "; sProtocolUnitNames=" << sProtocolUnitNames;
            }

            string sTerminalName;
            string sProtocolUnitName;
            string sChannelUnitName;
            string sCatalogTitle;
            for (std::map<string, string>::const_iterator it = mapTerminalNameTypes.begin(); it != mapTerminalNameTypes.end(); ++it)
            {
                sTerminalName = it->first;
                sProtocolUnitName = CxContainer::value(mapProtocolUnitNames, sTerminalName);
                sChannelUnitName = CxContainer::value(mapChannelUnitNames, sTerminalName);
                sCatalogTitle = CxContainer::value(mapCatalogTitles, sTerminalName);
                if (sTerminalName.empty()
                        || sProtocolUnitName.empty()
                        || sChannelUnitName.empty()
                        || sCatalogTitle.empty()
                        )
                {
                    cxPrompt() << "TerminalPlugin Terminals NameTypes load fail. for sTerminalName=" << sTerminalName
                               << "; sProtocolUnitName=" << sProtocolUnitName;
                    continue;
                }
                addTerminal(sProtocolUnitName, sChannelUnitName, sTerminalName, sCatalogTitle);
            }
            CxSkverManager::endUse(oProfile, true);
        }
    }

    return true;
}

void TerminalPlugin::catalogChangedImpl(const ConfigselfWin *oConfigselfWin)
{
    GM_INVALID_RETURE(oConfigselfWin);
    GM_INVALID_RETURE(_uiRemoveTerminalAct);
    CxCatalogAttr * oCatalogAttr = oConfigselfWin->currentCatalogAttr();
    if (oCatalogAttr)
    {
        const std::map<std::string, std::string> & catalogParams = oCatalogAttr->catalogParams();
        if (CxContainer::value(catalogParams, CxDescribeStrings::use()) == "Terminal")
        {
            _uiRemoveTerminalAct->setEnabled(true);
            return;
        }
    }
    _uiRemoveTerminalAct->setEnabled(false);
}

void TerminalPlugin::on_newTerminalBn_clicked()
{
    string sProtocolUnitName;
    string sChannelUnitName;
    string sTerminalName;
    string sCatalogTitle;

    if (dialogInputNewTerminal(sProtocolUnitName, sChannelUnitName, sTerminalName, sCatalogTitle))
    {
        addTerminal(sProtocolUnitName, sChannelUnitName, sTerminalName, sCatalogTitle);
    }
}

void TerminalPlugin::on_removeTerminalBn_clicked()
{
    GM_INVALID_RETURE(configselfWin);

    CxCatalogAttr * oCatalogAttr = configselfWin->removeCatalogWin(configselfWin->currentCatalogTitle());
    if (oCatalogAttr)
    {
        CxSkverBase * oProfile = CxSkverManager::beginUse( _terminalsConfigFilePath );
        if (oProfile)
        {
            string sTerminalName = oCatalogAttr->catalogName();
            std::string sTerminalNameTypes;
            string sProtocolUnitNames;
            string sChannelUnitNames;
            string sCatalogTitles;
            sTerminalNameTypes = oProfile->getValue("Terminals", string("NameTypes"), std::string());
            sProtocolUnitNames = oProfile->getValue("Terminals", string("ProtocolUnitNames"), std::string());
            sChannelUnitNames = oProfile->getValue("Terminals", string("ChannelUnitNames"), std::string());
            sCatalogTitles = oProfile->getValue("Terminals", string("CatalogTitles"), std::string());
            sTerminalNameTypes = CxString::unquote(sTerminalNameTypes, '\"');

            sTerminalNameTypes = fn_stringMapRemove(sTerminalNameTypes, '=', ',', sTerminalName);
            sProtocolUnitNames = fn_stringMapRemove(sProtocolUnitNames, '=', ';', sTerminalName);
            sChannelUnitNames = fn_stringMapRemove(sChannelUnitNames, '=', ';', sTerminalName);
            sCatalogTitles = fn_stringMapRemove(sCatalogTitles, '=', ';', sTerminalName);
            oProfile->setValue("Terminals", string("NameTypes"), string("\"")+sTerminalNameTypes+'\"');
            oProfile->setValue("Terminals", string("ProtocolUnitNames"), sProtocolUnitNames);
            oProfile->setValue("Terminals", string("ChannelUnitNames"), sChannelUnitNames);
            oProfile->setValue("Terminals", string("CatalogTitles"), sCatalogTitles);
            CxSkverManager::endUse(oProfile, true);
        }
        configselfWin->describeProject().deleteCatalogAttr(oCatalogAttr);
    }
}

bool TerminalPlugin::dialogInputNewTerminal(std::string &sProtocolUnitName, std::string &sChannelUnitName, std::string &sTerminalName,
                                            std::string &sCatalogTitle)
{
    QDialog dialog;

    QDialog * oDialog = & dialog;

    QPushButton *yesBn;
    QPushButton *cancelBn;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QComboBox *protocolNameCb;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *channelNameCb;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLineEdit *terminalNameEd;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *catalogTitleEd;

    {
        if (dialog.objectName().isEmpty())
            dialog.setObjectName(QString::fromUtf8("dialog"));
        dialog.resize(501, 293);
        yesBn = new QPushButton(oDialog);
        yesBn->setObjectName(QString::fromUtf8("yesBn"));
        yesBn->setGeometry(QRect(90, 240, 141, 41));
        cancelBn = new QPushButton(oDialog);
        cancelBn->setObjectName(QString::fromUtf8("cancelBn"));
        cancelBn->setGeometry(QRect(270, 240, 161, 41));
        groupBox = new QGroupBox(oDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(30, 20, 441, 201));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);
        label_3->setMinimumSize(QSize(80, 0));

        horizontalLayout->addWidget(label_3);

        protocolNameCb = new QComboBox(groupBox);
        protocolNameCb->setObjectName(QString::fromUtf8("protocolNameCb"));
        protocolNameCb->addItems(getProtocolNames());

        horizontalLayout->addWidget(protocolNameCb);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setMinimumSize(QSize(80, 0));

        horizontalLayout_2->addWidget(label_2);

        channelNameCb = new QComboBox(groupBox);
        channelNameCb->setObjectName(QString::fromUtf8("channelNameCb"));
        channelNameCb->addItems(getChannelNames());

        horizontalLayout_2->addWidget(channelNameCb);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(80, 0));

        horizontalLayout_3->addWidget(label);

        terminalNameEd = new QLineEdit(groupBox);
        terminalNameEd->setObjectName(QString::fromUtf8("terminalNameEd"));

        horizontalLayout_3->addWidget(terminalNameEd);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setMinimumSize(QSize(80, 0));

        horizontalLayout_4->addWidget(label_4);

        catalogTitleEd = new QLineEdit(groupBox);
        catalogTitleEd->setObjectName(QString::fromUtf8("catalogTitleEd"));

        horizontalLayout_4->addWidget(catalogTitleEd);

        verticalLayout->addLayout(horizontalLayout_4);

        dialog.setWindowTitle(QApplication::translate("dialog", "Dialog", 0));
        yesBn->setText(QApplication::translate("dialog", "\346\226\260\345\242\236", 0));
        cancelBn->setText(QApplication::translate("dialog", "\345\217\226\346\266\210", 0));
        groupBox->setTitle(QApplication::translate("dialog", "\351\205\215\347\275\256\346\226\260\347\273\210\347\253\257", 0));
        label_3->setText(QApplication::translate("dialog", "\350\247\204\347\272\246\345\220\215\347\247\260", 0));
        label_2->setText(QApplication::translate("dialog", "\351\200\232\351\201\223\347\261\273\345\236\213", 0));
        label->setText(QApplication::translate("dialog", "\347\273\210\347\253\257\345\220\215\347\247\260(en)", 0));
        label_4->setText(QApplication::translate("dialog", "\347\233\256\345\275\225\346\240\207\347\255\276(cn)", 0));
    } // setupUi

    dialog.connect( yesBn, SIGNAL(clicked()), &dialog, SLOT(accept()) );
    dialog.connect( cancelBn, SIGNAL(clicked()), &dialog, SLOT(reject()) );

    while (1)
    {
        if ( dialog.exec() == QDialog::Accepted )
        {
            sProtocolUnitName = QtCommonString::gbkToStdString( protocolNameCb->currentText() );
            sChannelUnitName = QtCommonString::gbkToStdString( channelNameCb->currentText() );
            sTerminalName = QtCommonString::gbkToStdString( terminalNameEd->text() );
            sCatalogTitle = QtCommonString::gbkToStdString( catalogTitleEd->text() );

            if (sTerminalName.empty())
            {
                QtCommonDialog::ShowWarning( label->text() + " can not empty!!!" );
                continue;
            }

            if (sCatalogTitle.empty())
            {
                QtCommonDialog::ShowWarning( label_4->text() + " can not empty!!!" );
                continue;
            }

            string sCatalogFilePath = sTerminalName + ".catalog.ini";
            if (configselfWin->describeProject().findByFilePath(sCatalogFilePath))
            {
                QtCommonDialog::ShowWarning( label->text() + " had exist!!!" );
                continue;
            }

            return true;
        }
        else
        {
            return false;
        }
    }
}

bool TerminalPlugin::addTerminal(string &sProtocolUnitName, string &sChannelUnitName, string &sTerminalName, string &sCatalogTitle)
{
    CxDescribeProject & describeProject = configselfWin->describeProject();

    if (sTerminalName.empty())
    {
        cxPrompt() << " add terminal fail. TerminalName=" << sTerminalName << " can not empty!!!";
        return false;
    }

    if (sCatalogTitle.empty())
    {
        cxPrompt() << " add terminal fail. CatalogTitle=" << sCatalogTitle << " can not empty!!!";
        return false;
    }

    string sCatalogFilePath = CxString::format("%s.catalog.ini",sTerminalName.c_str());
    if (describeProject.findByFilePath(sCatalogFilePath))
    {
        cxPrompt() << " add terminal fail. CatalogFilePath=" << sCatalogFilePath << " had exist!!!";
        return false;
    }

    CxCatalogAttr * oCatalogAttr = describeProject.addCatalogAttr(sTerminalName, sCatalogTitle, "terminal", sCatalogFilePath);
    if (! oCatalogAttr)
    {
        cxPrompt() << " add terminal fail. TerminalName=" << sTerminalName << " CatalogTitle=" << sCatalogTitle << " is not valid!!!";
        return false;
    }

    CxDescribeCatalog * oCatalog = oCatalogAttr->catalog;
    GM_ASSER_OBJECT(oCatalog);

    oCatalog->setCatalogParam("Use=Terminal");

    CxDescribeUnit * oTerminalUnit = CxDescribeManager::findDescribeUnit(cs_terminalCustom);
    if (! oTerminalUnit)
    {
        cxPrompt() << " add terminal fail. UnitName=terminal.custom can not find!!!";
        return false;
    }
    CxDescribeUnit * oProtocolUnit = CxDescribeManager::findDescribeUnit(sProtocolUnitName);
    CxDescribeUnit * oChannelUnit = CxDescribeManager::findDescribeUnit(sChannelUnitName);

    GM_ASSER_OBJECT(oProtocolUnit);
    GM_ASSER_OBJECT(oChannelUnit);

    CxDescribeBase * oDescribe;

    //增加 Unit terminal
    CxUnitAttr * oTerminalUnitAttr = oCatalog->addUnitAttr(oTerminalUnit);
    oTerminalUnitAttr->dataName = CxString::format("./terminal/%s.ini",sTerminalName.c_str());
    oTerminalUnitAttr->dataSection = "Terminal";
    oTerminalUnitAttr->unit->restoreDefaultValues();
    oTerminalUnitAttr->unitCaption = oTerminalUnit->unitTitle();
    oDescribe = oTerminalUnitAttr->unit->describe("Protocol");
    if (oDescribe)
    {
        oDescribe->valueFromString(getTypeSimpleNameByUnitName(sProtocolUnitName));
    }

    //增加 Unit protocol
    CxUnitAttr * oProtocolUnitAttr = oCatalog->addUnitAttr(oProtocolUnit);
    oProtocolUnitAttr->dataName = CxString::format("./terminal/%s.ini",sTerminalName.c_str());
    oProtocolUnitAttr->dataSection = sTerminalName + ".Protocol";
    oProtocolUnitAttr->unit->restoreDefaultValues();
    oProtocolUnitAttr->unitCaption = oProtocolUnit->unitTitle();
    oDescribe = oProtocolUnitAttr->unit->describe("ProtocolName");
    if (oDescribe)
    {
        oDescribe->valueFromString(oProtocolUnitAttr->dataSection);
    }
    oDescribe = oProtocolUnitAttr->unit->describe("Channel");
    if (oDescribe)
    {
        oDescribe->valueFromString(getTypeSimpleNameByUnitName(sChannelUnitName));
    }

    //增加 Unit channel
    CxUnitAttr * oChannelUnitAttr =  oCatalog->addUnitAttr(oChannelUnit);
    oChannelUnitAttr->dataName = CxString::format("./terminal/%s.ini",sTerminalName.c_str());
    oChannelUnitAttr->dataSection = sTerminalName + ".Protocol.Channel";
    oChannelUnitAttr->unit->restoreDefaultValues();
    oChannelUnitAttr->unitCaption = oChannelUnit->unitTitle();
    oDescribe = oChannelUnitAttr->unit->describe("ChannelName");
    if (oDescribe)
    {
        oDescribe->valueFromString(oChannelUnitAttr->dataSection);
    }

    //加载 workertable
    string sWorkerTablesCatalogFilePath = CxFileSystem::cd("0.protocol.common.workertables.ini", configselfWin->describePath());
    if (CxFileSystem::isExist(sWorkerTablesCatalogFilePath))
    {
        CxDescribeCatalog mDescribeCatalog;
        CxDescribeManager::loadCatalog(mDescribeCatalog, sWorkerTablesCatalogFilePath, configselfWin->describePath());
        const std::vector<CxUnitAttr*> & unitAttrs = mDescribeCatalog.unitAttrs();
        for (size_t i = 0; i < unitAttrs.size(); ++i)
        {
            CxUnitAttr * oUnitAttr = unitAttrs.at(i);
            if (oUnitAttr->unit)
            {
                CxUnitAttr * oNewUnitAttr = oCatalog->addUnitAttr(oUnitAttr->unit);
                oNewUnitAttr->dataName = CxString::format("./terminal/%s.Protocol.%s.txt",sTerminalName.c_str(),oUnitAttr->dataName.c_str());
                oNewUnitAttr->dataRewrite = oUnitAttr->dataRewrite;
                oNewUnitAttr->dataType = oUnitAttr->dataType;
                oNewUnitAttr->unitCaption = oUnitAttr->unitCaption;
            }
        }
    }

    configselfWin->addCatalogWin(oCatalogAttr->catalogTitle());

    CxSkverBase * oProfile = CxSkverManager::beginUse( _terminalsConfigFilePath );
    if (oProfile)
    {
        std::string sTerminalNameTypes;
        string sProtocolUnitNames;
        string sChannelUnitNames;
        string sCatalogTitles;
        sTerminalNameTypes = oProfile->getValue("Terminals", string("NameTypes"), std::string());
        sProtocolUnitNames = oProfile->getValue("Terminals", string("ProtocolUnitNames"), std::string());
        sChannelUnitNames = oProfile->getValue("Terminals", string("ChannelUnitNames"), std::string());
        sCatalogTitles = oProfile->getValue("Terminals", string("CatalogTitles"), std::string());
        sTerminalNameTypes = CxString::unquote(sTerminalNameTypes, '\"');

        sTerminalNameTypes = fn_stringMapAdd(sTerminalNameTypes, '=', ',', sTerminalName, "Custom");
        sProtocolUnitNames = fn_stringMapAdd(sProtocolUnitNames, '=', ';', sTerminalName, sProtocolUnitName);
        sChannelUnitNames = fn_stringMapAdd(sChannelUnitNames, '=', ';', sTerminalName, sChannelUnitName);
        sCatalogTitles = fn_stringMapAdd(sCatalogTitles, '=', ';', sTerminalName, sCatalogTitle);
        oProfile->setValue("Terminals", string("NameTypes"), string("\"")+sTerminalNameTypes+'\"');
        oProfile->setValue("Terminals", string("ProtocolUnitNames"), sProtocolUnitNames);
        oProfile->setValue("Terminals", string("ChannelUnitNames"), sChannelUnitNames);
        oProfile->setValue("Terminals", string("CatalogTitles"), sCatalogTitles);
        int iResult = CxSkverManager::endUse(oProfile, true);
        cxPrompt() << "save Terminals NameTypes result=" << iResult << " file : " << _terminalsConfigFilePath;
    }

    return true;
}

QStringList TerminalPlugin::getProtocolNames()
{
    QStringList sProtocolNames;
    const std::map<std::string, CxDescribeUnit* > &  describeUnits = CxDescribeManager::describeUnits();
    for (std::map<std::string, CxDescribeUnit* >::const_iterator it = describeUnits.begin(); it != describeUnits.end(); ++it)
    {
        const std::string & sUnitName = it->first;
        if (CxString::containCase(sUnitName, "protocol"))
        {
            sProtocolNames.append(QtCommonString::gbkToQString(sUnitName));
        }
    }
    return sProtocolNames;
}

QStringList TerminalPlugin::getChannelNames()
{
    QStringList sChannelNames;
    const std::map<std::string, CxDescribeUnit* > &  describeUnits = CxDescribeManager::describeUnits();
    for (std::map<std::string, CxDescribeUnit* >::const_iterator it = describeUnits.begin(); it != describeUnits.end(); ++it)
    {
        const std::string & sUnitName = it->first;
        if (CxString::containCase(sUnitName, "channel"))
        {
            sChannelNames.append(QtCommonString::gbkToQString(sUnitName));
        }
    }
    return sChannelNames;
}

string TerminalPlugin::getTypeSimpleNameByUnitName(const string &sUnitName)
{
    CxDescribeUnit* oUnit = CxDescribeManager::findDescribeUnit(sUnitName);
    if (oUnit)
    {
        const std::map<std::string, std::string>& unitParams = oUnit->unitParams();
        return CxContainer::value(unitParams, CxDescribeStrings::typeSimpleName());
    }
    return "";
}


