#ifndef CONFIG_BASE_H
#define CONFIG_BASE_H


#include <ccxx/ccxx.h>


//class ConfigManager : ConfigModels , ConfigDatas
//class ConfigModel : CxDescribeUnit , Ui , Will Save's FilePath and Section
//class configData : Had Save's FilePath and Section
//class ConfigNewMode : Will Save's Mode


//1 single simple win
//2 multi simple win
//3 multi table win


class ConfigCatalogBase
{
public:
    ConfigCatalogBase(const std::string & sCatalogName)
        : _catalogName(sCatalogName)
    {
    }

    virtual ~ConfigCatalogBase() {}

    virtual std::string add(const std::string & sAttach) = 0;

    virtual void remove(const std::string & s) = 0;

    virtual int load() = 0;

    virtual bool save() = 0;

    inline const std::string & modelName() const {
        return _catalogName;
    }

private:
    std::string _catalogName;

    friend class ConfigCatalogManager;

};

//sample :type=name
class ConfigCatalogCouple : public ConfigCatalogBase
{
public:
    ConfigCatalogCouple(const std::string & sCatalogName, const std::string & sFilePath, const std::string & sSection, const std::string & sEntry)
        : ConfigCatalogBase(sCatalogName),
          _filePath(sFilePath),
          _section(sSection),
          _entry(sEntry)
    {
//        doLoadNewValues();
    }

    std::string add(const std::string & sAttach);

    void remove(const std::string & s);

    int load();

    bool save();

    inline const std::string & newFilePath() const {
        return _filePath;
    }

    inline const std::string & newSection() const {
        return _section;
    }

    inline const std::string & newEntry() const {
        return _entry;
    }

private:
    std::string _filePath;
    std::string _section;
    std::string _entry;

    ///
    std::map<std::string, std::string> _catalogs;

    friend class ConfigCatalogManager;
};


class ConfigCatalogManager
{
public:
    static int load();

    static int save();

    static void registerCatalog(ConfigCatalogBase * oMode);

    static ConfigCatalogBase * findByName(const std::string & sName);

};


class ConfigModel
{
public:
    ConfigModel(const std::string & sName);

    ~ConfigModel();

    virtual void toContext(CxISetSkv& context) const;

    virtual void fromContext(const CxIGetSkv& context);

    inline const std::string & modelName() const {
        return _modelName;
    }

    inline const std::string & modelTitle() const {
        return _modelTitle;
    }

    inline bool isTable() const {
        return _modelType == 2;
    }

    inline const CxDescribeUnit & describeUnit() const {
        return _describeUnit;
    }

    inline const std::string dataFilePath() const {
        return _dataFilePath;
    }

    inline const std::string dataSection() const {
        return _dataSection;
    }

    inline const std::string uiFilePath() const {
        return _uiFilePath;
    }

    bool hasNewOperate() const;

    std::string getNewDataSection() const;

private:
    void reloadDecribeTable();

private:
    ///setting in file
    std::string _modelName;
    std::string _modelTitle;
    int _modelType;

    std::string _describeFilePath;

    std::string _dataFilePath;
    std::string _dataSection;

    std::string _uiFilePath;

    std::string _catalogName;
    std::string _newAttach;

    ///
    CxDescribeUnit _describeUnit;

    friend class ConfigManager;

};


class ConfigData
{
public:
    ConfigData(const std::string & sModelName, const std::string & sDataSection, const std::string & sDataTitle);

    ~ConfigData();

    inline const std::string & modelName() const { return _modelName; }

    inline const std::string & dataSection() const { return _dataSection; }

    inline const std::string & dataTitle() const { return _dataTitle; }

    inline const ConfigModel * configModel() const { return _configModel; }

    inline const std::string & dataFilePath() const { return _dataFilePath; }

    inline const std::string & dataKey() const { return _dataKey; }

private:
    ///
    std::string _modelName;
    std::string _dataSection;
    std::string _dataTitle;

    ///
    ConfigModel * _configModel;
    std::string _dataFilePath;
    std::string _dataKey;

};


class ConfigManager
{
public:
    static int load();

    static int save();

    static const std::vector<ConfigModel *> & models();

    static const std::vector<ConfigData> & datas();

    static ConfigModel * findByModelName(const std::string& sModelName);

    static const ConfigData * findByDataKey(const std::string& sDataKey);

    static const ConfigData * findByDataTitle(const std::string& sDataTitle);

    static void addNewConfigData(const std::string& sModelName, const std::string & sSection, const std::string & sTitle);

    static void deleteNewConfigData(const std::string& sDataKey);

private:
    static ConfigModel * loadConfigModel(const std::string &sName, const std::string &sTypeSimpleName, const std::string& sFilePath);

};

#endif // CONFIG_BASE_H
