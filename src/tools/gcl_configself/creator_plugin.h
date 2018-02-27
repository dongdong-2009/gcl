#ifndef CREATOR_PLUGIN_H
#define CREATOR_PLUGIN_H


#include "configself_win.h"


enum CreatorCatalogTypeEnum {
    CreatorCatalogType_none = 0,
    CreatorCatalogType_other,
    CreatorCatalogType_describe,
    CreatorCatalogType_catalog,
    CreatorCatalogType_project
};
typedef enum CreatorCatalogTypeEnum CreatorCatalogTypeEnum;


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

class CreatorPlugin : public QObject, public ConfigselfPluginBase
{

    Q_OBJECT

public:
    CreatorPlugin();

    ~CreatorPlugin();

protected:
    void catalogChangedImpl(const ConfigselfWin *oConfigselfWin);

    void initImpl(ConfigselfWin * oConfigselfWin);

    void uninitImpl();

private:
    void loadSelfConfig();

    void saveSelfConfig();

    std::vector<CatalogUnify> * findCatalogUnifys(const std::string &sCatalogTitle);

    CatalogUnify * findCatalogUnify(const std::string &sCatalogTitle, const std::string &sDataName);

    CatalogUnify addCatalogUnify(const std::string &sCatalogTitle, const std::string &sDataName);

    int removeCatalogUnify(const std::string &sCatalogTitle, const std::string &sDataName);

    void initUi();

    void newDataName(bool bIsCopy);

    CreatorCatalogTypeEnum catalogType(const std::string & sCatalogTitle);

    std::string dataNameMySelf(const std::string &sDataName);

    void setCurrentRow(int iCurrentRow, bool bSave);

private:
    //catalogTitle=catalogUnify
    std::map<std::string, std::vector<CatalogUnify> > _catalogUnifys;

    ConfigselfWin * configselfWin;

    QWidget *tab_2;
    QVBoxLayout *verticalLayout_2;
    QListWidget *dataNameListWidget;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *newDataNameBn;
    QPushButton *copyDataNameBn;
    QPushButton *deleteDataNameBn;
    QPushButton *saveDataNameBn;

    bool _canCurrentCatalogLoad;

    std::string _describeCatalogsFilePath;
    std::string _titleCreatorUnit;
    std::string _titleCreatorCatalog;
    std::string _titleCreatorProject;

private slots:
    void on_newDataNameBn_clicked();

    void on_copyDataNameBn_clicked();

    void on_deleteDataNameBn_clicked();

    void on_saveDataNameBn_clicked();

    void on_dataNameListWidget_currentRowChanged(int currentRow);

};

class CreatorPluginFactory : public ConfigselfPluginFactoryBase
{
public:
    std::string factoryName(){
        return "CreatorPlugin";
    }

    ConfigselfPluginBase* createObject(){
        return new CreatorPlugin();
    }

};


#endif // CREATOR_PLUGIN_H
