#include "config_base.h"

#include "protocol_strings.h"
#include "protocol_config.h"

using namespace std;


static std::vector<ConfigModel *> f_configModels;
static std::vector<ConfigData> f_configDatas;
static std::vector<ConfigCatalogBase *> f_configCatalogs;



string ConfigCatalogCouple::add(const string &sAttach)
{
    string sValue = CxString::newString(sAttach);
    string sKey = CxString::token(sValue, '=');
    for (size_t i = 0; i < 0xFFFF; ++i)
    {
        string s = sKey + CxString::toString(i);
        if (! CxContainer::contain(_catalogs, s))
        {
            sKey = s;
            break;
        }
    }
    _catalogs[sKey] = sValue;
    return sKey;
}

void ConfigCatalogCouple::remove(const string &s)
{
    CxContainer::remove(_catalogs, s);
}

int ConfigCatalogCouple::load()
{
    if (CxFileSystem::isExist(_filePath))
    {
        string sCatalogs;
        CxSkverBase * oProfile = CxSkverManager::beginUse(_filePath);
        if (oProfile)
        {
            sCatalogs = oProfile->getValue(_section, _entry, sCatalogs);
            CxSkverManager::endUse(oProfile);
        }
        if (sCatalogs.size()>0)
        {
            _catalogs = CxString::splitToMap(sCatalogs, '=', ',');
        }
    }
	return true;
}

bool ConfigCatalogCouple::save()
{
    string sCatalogs = CxString::join(_catalogs, "=", ",");
    CxSkverBase * oProfile = CxSkverManager::beginUse(_filePath);
    if (oProfile)
    {
        oProfile->setValue(_section, _entry, sCatalogs);
        CxSkverManager::endUse(oProfile, true);
    }
	return true;
}





int ConfigCatalogManager::load()
{
///configs.catalog.ini
//    [ConfigCatalogs]
//    NameTypes="business=Couple"
//
//    [business]
//    FilePath=app.config.ini
//    Section=Businesses
//    Entry=NameTypes
    string sFilePath = CxFileSystem::mergeFilePath(CxApplication::configPath(), "configs.catalog.ini");
    bool bIsExist = false;
    cxPromptCheck1((bIsExist=CxFileSystem::isExist(sFilePath))==true, ,sFilePath);
    if (bIsExist)
    {
        CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
        if (oProfile)
        {
            std::string sNameTypes = oProfile->getValue(CS_SectionConfigCatalogs, CS_EntryConfigCatalogNameTypes, std::string());
            sNameTypes = CxString::unquote(sNameTypes, '\"');
            std::vector<std::string> nameTypes = CxString::split( sNameTypes, ',' );
            for (size_t i = 0; i < nameTypes.size(); ++i)
            {
                std::string sNameType = nameTypes.at(i);
                std::string sName = CxString::token(sNameType, '=');
                std::string sTypeSimpleName = sNameType;

                ConfigCatalogBase * oCatalog = NULL;
                if (sTypeSimpleName == "Couple")
                {
                    oProfile->prepareGet(sName);
                    std::string sFilePath = oProfile->getValue(CS_EntryFilePath, std::string());
                    std::string sSection = oProfile->getValue(CS_EntrySection, std::string());
                    std::string sEntry = oProfile->getValue(CS_EntryEntry, std::string());
                    sFilePath = CxFileSystem::fullFilePath(CxApplication::applicationDeployPath(), sFilePath);
                    oCatalog = new ConfigCatalogCouple(sName, sFilePath, sSection, sEntry);
                }
                if (oCatalog)
                {
                    oCatalog->load();
                    f_configCatalogs.push_back(oCatalog);
                }
            }
            CxSkverManager::endUse(oProfile);
        }
    }
    return f_configCatalogs.size();
}

int ConfigCatalogManager::save()
{
    for (size_t i = 0; i < f_configCatalogs.size(); ++i)
    {
        ConfigCatalogBase * oCatalog = f_configCatalogs.at(i);
        oCatalog->save();
    }
	return true;
}

void ConfigCatalogManager::registerCatalog(ConfigCatalogBase *oMode)
{
    f_configCatalogs.push_back(oMode);
}

ConfigCatalogBase *ConfigCatalogManager::findByName(const string &sName)
{
    for (size_t i = 0; i < f_configCatalogs.size(); ++i)
    {
        ConfigCatalogBase * oMode = f_configCatalogs.at(i);
        if (oMode->_catalogName == sName)
        {
            return oMode;
        }
    }
    return NULL;
}




ConfigModel::ConfigModel(const std::string &sName)
{
    _modelName = sName;
    _modelTitle = sName;
}

ConfigModel::~ConfigModel()
{

}

void ConfigModel::toContext(CxISetSkv &context) const
{

}

void ConfigModel::fromContext(const CxIGetSkv &context)
{
    _modelName = context.getValue(CS_EntryModelName, _modelName);
    _modelTitle = context.getValue(CS_EntryModelTitle, _modelTitle);
    _modelType = context.getValue(CS_EntryModelType, _modelType);
    _describeFilePath = context.getValue(CS_EntryDescribeFilePath, _describeFilePath);
    _dataFilePath = context.getValue(CS_EntryDataFilePath, _dataFilePath);
    _dataSection = context.getValue(CS_EntryDataSection, _dataSection);
    _uiFilePath = context.getValue(CS_EntryUiFilePath, _uiFilePath);
    _catalogName = context.getValue(CS_EntryCatalogName, _catalogName);
    _newAttach = context.getValue(CS_EntryNewAttach, _newAttach);
}

bool ConfigModel::hasNewOperate() const
{
    return ConfigCatalogManager::findByName(_catalogName);
}

string ConfigModel::getNewDataSection() const
{
    ConfigCatalogBase * oMode = ConfigCatalogManager::findByName(_catalogName);
    if (oMode)
    {
        return oMode->add(_newAttach);
    }
    return "";
}

void ConfigModel::reloadDecribeTable()
{
    CxDescribeUnit * oDescribeUnit = CxDescribeUnit::createDescribes(_describeFilePath);
    if (oDescribeUnit)
    {
        _describeUnit.assignedFrom(oDescribeUnit);
        delete oDescribeUnit;
    }
}





ConfigData::ConfigData(const string &sModelName, const string &sDataSection, const string &sDataTitle)
    : _modelName(sModelName),
      _dataSection(sDataSection)
{
    _configModel = ConfigManager::findByModelName(sModelName);
    if (_dataSection.empty() && _configModel)
    {
        _dataSection = _configModel->dataSection();
    }
    if (_configModel)
    {
        _dataFilePath = _configModel->dataFilePath();
        _dataKey = _configModel->modelName()+"-"+_dataSection;
    }
    _dataTitle = sDataTitle;
}

ConfigData::~ConfigData()
{

}





int ConfigManager::load()
{
    ///catalog
    ConfigCatalogManager::load();

    ///model load
    CxSkverBase * oProfile = CxSkverManager::beginUse(CxFileSystem::mergeFilePath(CxApplication::configPath(), "config.config.ini"));
    if (oProfile)
    {
        std::string sNameTypes = oProfile->getValue(CS_SectionConfigModels, CS_EntryConfigModelNameTypes, std::string());
        sNameTypes = CxString::unquote(sNameTypes, '\"');
        std::vector<std::string> nameTypes = CxString::split( sNameTypes, ',' );
        for (size_t i = 0; i < nameTypes.size(); ++i)
        {
            std::string sNameType = nameTypes.at(i);
            std::string sName = CxString::token(sNameType, '=');
            std::string sTypeSimpleName = sNameType;
            std::string sFilePath = CxFileSystem::mergeFilePath(CxApplication::configPath(), sName + ".model.ini");
            if (! CxFileSystem::isExist(CxFileSystem::mergeFilePath(CxApplication::configPath(), sName + ".model.ini")))
                sFilePath = CxFileSystem::mergeFilePath(CxApplication::configPath(), sTypeSimpleName + ".model.ini");
            loadConfigModel(sName, sTypeSimpleName, sFilePath);
        }
        CxSkverManager::endUse(oProfile);
    }

    ///data load
    vector<string> sLines;
    CxFile::load(CxFileSystem::mergeFilePath(CxApplication::configPath(), "configs.data.txt"), sLines);
    for (size_t i = 0; i < sLines.size(); ++i)
    {
        const string & sLine = sLines.at(i);
        vector<string> sItems = CxString::split(sLine, ',');
        if (sItems.size()>3)
        {
            f_configDatas.push_back(ConfigData(sItems.at(1), sItems.at(2), sItems.at(3)));
        }
    }

    return f_configModels.size() + f_configDatas.size();
}

int ConfigManager::save()
{
    vector<string> sLines;
    for (size_t i = 0; i < f_configDatas.size(); ++i)
    {
        const ConfigData & data = f_configDatas.at(i);
        string sLine = CxString::format("%06d",i);
        sLine.push_back(',');
        sLine.append(data.modelName());
        sLine.push_back(',');
        sLine.append(data.dataSection());
        sLine.push_back(',');
        sLine.append(data.dataTitle());
        sLines.push_back(sLine);
    }

    std::string sFilePath = CxFileSystem::mergeFilePath(CxApplication::configPath(), "configs.data.txt");
    CxFile::save(sFilePath, sLines);

    ConfigCatalogManager::save();
	return true;
}

const std::vector<ConfigModel *> & ConfigManager::models()
{
    return f_configModels;
}


const std::vector<ConfigData> & ConfigManager::datas()
{
    return f_configDatas;
}

ConfigModel *ConfigManager::findByModelName(const string &sModelName)
{
    for (size_t i = 0; i < f_configModels.size(); ++i)
    {
        ConfigModel * oModel = f_configModels.at(i);
        if (oModel->modelName() == sModelName)
        {
            return oModel;
        }
    }
    return NULL;
}

const ConfigData * ConfigManager::findByDataKey(const string &sDataKey)
{
    for (size_t i = 0; i < f_configDatas.size(); ++i)
    {
        const ConfigData & configData = f_configDatas.at(i);
        if (configData.dataKey() == sDataKey)
        {
            return & configData;
        }
    }
    return NULL;
}

const ConfigData *ConfigManager::findByDataTitle(const string &sDataTitle)
{
    for (size_t i = 0; i < f_configDatas.size(); ++i)
    {
        const ConfigData & configData = f_configDatas.at(i);
        if (configData.dataTitle() == sDataTitle)
        {
            return & configData;
        }
    }
    return NULL;
}

void ConfigManager::addNewConfigData(const string &sModelName, const string &sSection, const string &sTitle)
{
    ConfigModel * oModel = NULL;
    cxPromptCheck1((oModel=findByModelName(sModelName)) != NULL, ,sModelName);
    if (oModel)
    {
        ConfigData data(sModelName, sSection, sTitle);
        f_configDatas.push_back(data);
    }
}

void ConfigManager::deleteNewConfigData(const string &sDataKey)
{
    for(std::vector<ConfigData>::iterator it = f_configDatas.begin(); it != f_configDatas.end();)
    {
        const ConfigData & configData = (* it);
        if (configData.dataKey() == sDataKey)
        {
            f_configDatas.erase(it);
            continue;
        }
        ++it;
    }
}

ConfigModel *ConfigManager::loadConfigModel(const string &sName, const string &sTypeSimpleName, const string &sFilePath)
{
    bool bIsExist = false;
    cxPromptCheck1((bIsExist=CxFileSystem::isExist(sFilePath))==true, ,sFilePath);
    if (bIsExist)
    {
        ConfigModel* oModel = new ConfigModel(sName);
        if (oModel)
        {
            CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
            if (oProfile)
            {
                oProfile->prepareGet(CS_SectionConfigModel);
                oModel->fromContext(*oProfile);
                CxSkverManager::endUse(oProfile);
            }
            //

            oModel->_describeFilePath = CxFileSystem::fullFilePath(CxApplication::applicationDeployPath(), oModel->_describeFilePath);
            oModel->_dataFilePath = CxFileSystem::fullFilePath(CxApplication::applicationDeployPath(), oModel->_dataFilePath);
            oModel->_uiFilePath = CxFileSystem::fullFilePath(CxApplication::applicationDeployPath(), oModel->_uiFilePath);
            oModel->reloadDecribeTable();

            f_configModels.push_back(oModel);
        }
        return oModel;
    }
    return NULL;
}




