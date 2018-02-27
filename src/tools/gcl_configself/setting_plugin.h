#ifndef SETTING_PLUGIN_H
#define SETTING_PLUGIN_H


#include "configself_win.h"


class CatalogUnify
{
public:
    std::string dataName;
    std::string catalogTitle;
    std::string catalogName;

    CatalogUnify(const std::string & sDataName, const std::string & sCatalogName, const std::string & sCatalogTitle) :
        dataName(sDataName),
        catalogTitle(sCatalogTitle),
        catalogName(sCatalogName)
    {}

};

class SettingPlugin : public QObject, public ConfigselfPluginBase
{

    Q_OBJECT

public:
    SettingPlugin();

    ~SettingPlugin();

protected:
    void initImpl(ConfigselfWin * oConfigselfWin);

    void uninitImpl();

    void catalogChangedImpl(const ConfigselfWin *oConfigselfWin);

    void unitChangedImpl(const ConfigselfWin* oConfigselfWin);

private:
    void loadSelfConfig();

    void saveSelfConfig();

    std::vector<CatalogUnify> * findCatalogUnifys(const std::string &sCatalogTitle);

    void initUi();

private:
    //catalogTitle=catalogUnify
    std::map<std::string, std::vector<CatalogUnify> > _catalogUnifys;

    ConfigselfWin * configselfWin;

    QWidget *tab;
    QVBoxLayout *verticalLayout;
    QLabel *unitLb;
    QPushButton *unitSaveBn;
    QLabel *catalogLb;
    QPushButton *catalogsaveBn;

private slots:
    void on_unitSave_clicked();

    void on_catalogSave_clicked();

};

class SettingPluginFactory : public ConfigselfPluginFactoryBase
{
public:
    std::string factoryName(){
        return "SettingPlugin";
    }

    ConfigselfPluginBase* createObject(){
        return new SettingPlugin();
    }

};


#endif // SETTING_PLUGIN_H
