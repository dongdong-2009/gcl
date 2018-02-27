#include "terminal_loader.h"

#include "custom_terminal.h"

void TerminalLoader::loadProtocolTables(ProtocolBase* oProtocol, const std::string& sPath)
{
    const std::vector<WorkerTableBase*>* oWorkerTables = oProtocol->workerTables();
    for (size_t i = 0; i < oWorkerTables->size(); ++i) {
        WorkerTableBase* oWorkerTable = oWorkerTables->at(i);
        std::string sFileName = TerminalManager::workerTableFileName(oProtocol->protocolName(), oWorkerTable->tableName());
        std::string sFilePath = CxFileSystem::mergeFilePath(sPath, sFileName);
        oProtocol->loadTable(oWorkerTable->tableName(), sFilePath);
    }
}

TerminalBase* TerminalLoader::loadTerminal(const std::string &sTerminalName, const std::string &sTerminalTypeSimpleName, const std::string& sFilePath)
{
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile)
    {
        std::string sProtocolName = TerminalManager::protocolName(sTerminalName);
        std::string sBusinessName = TerminalManager::businessName(sTerminalName);
        std::string sChannelName  = TerminalManager::channelName(sProtocolName);
        std::string sProtocolTypeSimpleName = oProfile->getValue(CS_SectionTerminal, CS_EntryProtocol, std::string(""));
        std::string sBusinessTypeSimpleName = oProfile->getValue(CS_SectionTerminal, CS_EntryBusiness, std::string(""));
        std::string sChannelTypeSimpleName = oProfile->getValue(sProtocolName, CS_EntryChannel, std::string(""));
        TerminalBase* oTerminal = TerminalManager::createTerminal(sTerminalName, sTerminalTypeSimpleName, sProtocolTypeSimpleName, sBusinessTypeSimpleName, sChannelTypeSimpleName);
        if (oTerminal)
        {
            oProfile->prepareGet(CS_SectionTerminal);
            oTerminal->fromContext(*oProfile);
            ProtocolBase* oProtocol = oTerminal->_protocol;
            if (oProtocol)
            {
                oProfile->prepareGet(sProtocolName);
                oProtocol->fromContext(*oProfile);

                oProfile->prepareGet(CS_SectionMeasure);
                oProtocol->measureFromContext(*oProfile);

                oProfile->prepareGet(sChannelName);
                oProtocol->channelFromContext(*oProfile);
                loadProtocolTables(oProtocol, oProfile->path());
            }
            TBIBase* oBusiness = oTerminal->_business;
            if (oBusiness)
            {
                oProfile->prepareGet(sBusinessName);
                oBusiness->fromContext(*oProfile);
            }
            return oTerminal;
        }
        CxSkverManager::endUse(oProfile);
    }
    return NULL;
}

void TerminalLoader::reloadTerminalContexts(TerminalBase *oTerminal, const std::string &sFilePath)
{
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile) {
        oProfile->prepareGet(CS_SectionTerminal);
        oTerminal->fromContext(*oProfile);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol)
        {
            oProfile->prepareGet(oProtocol->protocolName());
            oProtocol->fromContext(*oProfile);
            if (oProtocol->channel())
            {
                oProfile->prepareGet(oProtocol->channel()->channelName());
                oProtocol->channelFromContext(*oProfile);
            }
            loadProtocolTables(oProtocol, oProfile->path());
        }
        TBIBase* oBusiness = oTerminal->business();
        if (oBusiness)
        {
            oProfile->prepareGet(oBusiness->businessName());
            oBusiness->fromContext(*oProfile);
        }
        CxSkverManager::endUse(oProfile);
    }
}

void TerminalLoader::reloadProtocolTable(TerminalBase* oTerminal, const std::string &sTableName, const std::string &sFilePath)
{
    oTerminal->_protocol->loadTable(sTableName, sFilePath);
}

void TerminalLoader::saveProtocolTables(ProtocolBase* oProtocol, const std::string& sPath)
{
    const std::vector<WorkerTableBase*>* oBuilders = oProtocol->workerTables();
    for (size_t i = 0; i < oBuilders->size(); ++i) {
        WorkerTableBase* oBuilder = oBuilders->at(i);
        std::string sFileName = TerminalManager::workerTableFileName(oProtocol->protocolName(), oBuilder->tableName());
        std::string sFilePath = CxFileSystem::mergeFilePath(sPath, sFileName);
        oProtocol->saveTable(oBuilder->tableName(), sFilePath);
    }
}

void TerminalLoader::saveTerminalContexts(TerminalBase *oTerminal, const std::string &sFilePath)
{
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile) {
        oProfile->prepareSet(CS_SectionTerminal);
        oTerminal->toContext(*oProfile);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol)
        {
            oProfile->prepareSet(oProtocol->protocolName());
            oProtocol->toContext(*oProfile);
            if (oProtocol->channel())
            {
                oProfile->prepareSet(oProtocol->channel()->channelName());
                oProtocol->channel()->toContext(*oProfile);
            }
        }
        TBIBase* oBusiness = oTerminal->business();
        if (oBusiness)
        {
            oProfile->prepareGet(oBusiness->businessName());
            oBusiness->fromContext(*oProfile);
        }
        CxSkverManager::endUse(oProfile, true);
    }
}

void TerminalLoader::saveTerminal(TerminalBase* oTerminal, const std::string& sFilePath)
{
    if (! oTerminal)
        return;
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile) {
        oProfile->prepareSet(CS_SectionTerminal);
        oTerminal->toContext(*oProfile);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol){
            oProfile->prepareSet(oProtocol->protocolName());
            oProtocol->toContext(*oProfile);
            saveProtocolTables(oProtocol, oProfile->defaultSaveFilePath());
            const CxChannelBase* oChannel = oProtocol->channel();
            if (oChannel){
                oProfile->prepareSet(oChannel->channelName());
                oChannel->toContext(*oProfile);
            }
        }
        TBIBase* oBusiness = oTerminal->business();
        if (oBusiness)
        {
            oProfile->prepareGet(oBusiness->businessName());
            oBusiness->fromContext(*oProfile);
        }
        CxSkverManager::endUse(oProfile, true);
    }
}



void TerminalLoader::load(const std::string & filePath)
{
    if (filePath.length() > 0 && CxFileSystem::isExist(filePath))
    {
        CxSkverBase * oProfile = CxSkverManager::beginUse(filePath);
        if (oProfile)
        {
            std::string sTerminalNameTypes;
            sTerminalNameTypes = oProfile->getValue(CS_SectionTerminals, CS_EntryTerminalNameTypes, sTerminalNameTypes);
            if (sTerminalNameTypes.find('\"') != string::npos) sTerminalNameTypes = CxString::unquote(sTerminalNameTypes, '\"');
            std::vector<std::string> terminalNameTypes = CxString::split( sTerminalNameTypes, ',' );
            for (size_t i = 0; i < terminalNameTypes.size(); ++i)
            {
                std::string terminalNameType = terminalNameTypes.at(i);
                std::string terminalName = CxString::token(terminalNameType, '=');
                std::string terminalTypeSimpleName = terminalNameType;
                std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), terminalName + ".ini");
                loadTerminal(terminalName, terminalTypeSimpleName, terminalFilePath);
            }
            CxSkverManager::endUse(oProfile);
        }
    }
}

void TerminalLoader::save(const std::string &filePath)
{
//    CxSkverBase * oProfile = CxSkverManager::beginUse(filePath, true);
//    if (oProfile)
//    {
        std::vector<std::string> terminalNameTypes;
        const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
        for (size_t i = 0; i < oTerminals->size(); ++i) {
            TerminalBase* oTerminal = oTerminals->at(i);
            std::string sTerminalName = oTerminal->terminalName();
            std::string sTerminalTypeSimpleName = oTerminal->typeSimpleName();
            terminalNameTypes.push_back(sTerminalName + "=" + sTerminalTypeSimpleName);
            std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), sTerminalName + ".ini");
            saveTerminal(oTerminal, terminalFilePath);
        }
        std::string sTerminalNameTypes = CxString::join(terminalNameTypes, ',');
        CxApplication::saveConfig(CS_SectionTerminals, CS_EntryTerminalNameTypes, "\"" + sTerminalNameTypes + "\"");
//        oProfile->setValue(CSTerminals, CSTerminalNameTypes, "\"" + sTerminalNameTypes + "\"");
//        CxSkverManager::endUse(oProfile, true);
//    }
}

void TerminalLoader::load(const std::string & filePath,const std::string &sTerminalName)
{
//    std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), sTerminalName + ".ini");
//    loadTerminal(sTerminalName, "Custom", terminalFilePath);

    if (filePath.length() > 0 && CxFileSystem::isExist(filePath))
    {
        CxSkverBase * oProfile = CxSkverManager::beginUse(filePath);
        if (oProfile)
        {
            std::string sTerminalNameTypes;
            sTerminalNameTypes = oProfile->getValue(CS_SectionTerminals, CS_EntryTerminalNameTypes, sTerminalNameTypes);
            sTerminalNameTypes = CxString::unquote(sTerminalNameTypes, '\"');
            std::vector<std::string> terminalNameTypes = CxString::split( sTerminalNameTypes, ',' );
            for (size_t i = 0; i < terminalNameTypes.size(); ++i)
            {
                std::string terminalNameType = terminalNameTypes.at(i);
                std::string terminalName = CxString::token(terminalNameType, '=');

                if(sTerminalName!=terminalName)continue;

                std::string terminalTypeSimpleName = terminalNameType;

                std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), terminalName + ".ini");
                loadTerminal(terminalName, terminalTypeSimpleName, terminalFilePath);
            }
            CxSkverManager::endUse(oProfile);
        }
    }


}
