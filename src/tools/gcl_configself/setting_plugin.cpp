#include "setting_plugin.h"


#include <QtGui>
#include <QAction>
#include <QMenuBar>
#include <QDialog>
#include <QGroupBox>
#include <QComboBox>
#include <QToolBar>
#include <QApplication>


using namespace std;


SettingPluginFactory * fn_oSettingPluginFactory()
{
    return ConfigselfPluginFactoryManager::createAndRegister<SettingPluginFactory>();
}
SettingPluginFactory * f_oSettingPluginFactory = fn_oSettingPluginFactory();


SettingPlugin::SettingPlugin()
{
}

SettingPlugin::~SettingPlugin()
{

}

void SettingPlugin::initImpl(ConfigselfWin *oConfigselfWin)
{
    configselfWin = oConfigselfWin;

    initUi();
}

void SettingPlugin::uninitImpl()
{
}

void SettingPlugin::catalogChangedImpl(const ConfigselfWin *oConfigselfWin)
{
    GM_INVALID_RETURE(oConfigselfWin);

    const std::vector<UnitUi> & unitUis = oConfigselfWin->currentUnitUis();
    for (size_t i = 0; i < unitUis.size(); ++i)
    {
        const UnitUi & unitUi = unitUis.at(i);
        if (unitUi.unitAttrContainer) unitUi.unitAttrContainer->setVisible(false);
    }
}

void SettingPlugin::unitChangedImpl(const ConfigselfWin *oConfigselfWin)
{
}


void SettingPlugin::initUi()
{
    GM_INVALID_RETURE(configselfWin);

    configselfWin->uiProjectMenu()->setVisible(false);
    configselfWin->uiProjectMenu()->setEnabled(false);

    configselfWin->uiWinsToolBar()->removeAction(configselfWin->uiProjectLocadAct());
    configselfWin->uiWinsToolBar()->removeAction(configselfWin->uiProjectSaveAct());

    QTabWidget * oTabWidget = configselfWin->getTableWidgetRight();
    GM_INVALID_RETURE(oTabWidget);

    oTabWidget->setVisible(true);

    return;

    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    verticalLayout = new QVBoxLayout(tab);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    catalogLb = new QLabel(tab);
    catalogLb->setObjectName(QString::fromUtf8("catalogLb"));

    verticalLayout->addWidget(catalogLb);

    catalogsaveBn = new QPushButton(tab);
    catalogsaveBn->setObjectName(QString::fromUtf8("catalogsaveBn"));
    catalogsaveBn->setMinimumSize(QSize(0, 30));
    catalogsaveBn->setMaximumSize(QSize(16777215, 30));
    catalogsaveBn->setText(QString::fromUtf8("保存当前配置编目"));

    verticalLayout->addWidget(catalogsaveBn);

    unitLb = new QLabel(tab);
    unitLb->setObjectName(QString::fromUtf8("unitLb"));

    verticalLayout->addWidget(unitLb);

    unitSaveBn = new QPushButton(tab);
    unitSaveBn->setObjectName(QString::fromUtf8("unitSaveBn"));
    unitSaveBn->setMinimumSize(QSize(0, 30));
    unitSaveBn->setMaximumSize(QSize(16777215, 30));
    unitSaveBn->setText(QString::fromUtf8("保存当前配置单元"));

    verticalLayout->addWidget(unitSaveBn);

    oTabWidget->addTab(tab, QString::fromUtf8("数据文件"));

    oTabWidget->setVisible(true);

    connect(unitSaveBn, SIGNAL(clicked()), this, SLOT(on_unitSave_clicked()));
    connect(catalogsaveBn, SIGNAL(clicked()), this, SLOT(on_catalogSave_clicked()));
}

void SettingPlugin::on_unitSave_clicked()
{
    configselfWin->saveCurrentUnitData();
}

void SettingPlugin::on_catalogSave_clicked()
{
    configselfWin->saveCurrentCatalogData();
}

