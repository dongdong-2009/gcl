#include "praupdownterminalplugin.h"


#include <QtGui>

using namespace std;



TerminalPluginFactory * fn_oTerminalPluginFactory()
{
    return PraupdownPluginFactoryManager::createAndRegister<TerminalPluginFactory>();
}
TerminalPluginFactory * f_oTerminalPluginFactory = fn_oTerminalPluginFactory();


PraUpDownTerminalPlugin::PraUpDownTerminalPlugin()
{

}

PraUpDownTerminalPlugin::~PraUpDownTerminalPlugin()
{

}

void PraUpDownTerminalPlugin::initImpl(PraUpdownWin *oPraUpdownWin)
{
    configselfWin = oPraUpdownWin;
}

void PraUpDownTerminalPlugin::uninitImpl()
{
}

void PraUpDownTerminalPlugin::fdcBeforeRefreshImpl(const PraUpdownWin *oPraUpdownWin, Fdc *oFdc)
{
    GM_INVALID_RETURE(oPraUpdownWin);
    GM_INVALID_RETURE(oFdc);
    GM_INVALID_RETURE(oFdc->varName.size()>0);

    std::map<string, string> varNames = CxString::splitToMap(oFdc->varName, '=', ';');
    typename std::map<string,string>::const_iterator it = varNames.find(string("terminal"));
    if (it != varNames.end())
    {
        string sFileName = it->second;
        string sProductPath = oPraUpdownWin->doFilePra().getProductPath(oFdc);
        string sFilePath = CxFileSystem::mergeFilePath(sProductPath, sFileName);
        CxSkverBase * oProfile = CxSkverManager::beginUse( sFilePath );
        if (oProfile)
        {
            std::string sTerminalNameTypes;
            sTerminalNameTypes = oProfile->getValue("Terminals", string("NameTypes"), std::string());
            sTerminalNameTypes = CxString::unquote(sTerminalNameTypes, '\"');
            std::vector<std::string> terminalNameTypes = CxString::split( sTerminalNameTypes, ',' );
            if (terminalNameTypes.size()>0)
            {
                std::vector<FdcItem> & fdcItems = oFdc->items;
                vector<CxFilePathInfo> pathInfos;
                CxFileSystem::scanDir(CxFileSystem::mergeFilePath(sProductPath, "terminal"), pathInfos);
                for (size_t i = 0; i < pathInfos.size(); ++i)
                {
                    const CxFilePathInfo & filePathInfo = pathInfos.at(i);
                    const string sFileName = filePathInfo.fileName;
                    for (size_t j = 0; j < terminalNameTypes.size(); ++j)
                    {
                        std::string terminalNameType = terminalNameTypes.at(j);
                        std::string terminalName = CxString::token(terminalNameType, '=');
                        terminalName = terminalName + ".";
                        string sRfilePath = CxFileSystem::mergeFilePath("terminal", sFileName);
                        if (CxString::beginWith(sFileName, terminalName) && ! oFdc->find(sRfilePath))
                        {
                            FdcItem fdcItem;
                            fdcItem.fdc = oFdc->name;
                            fdcItem.rfilePath = sRfilePath;
                            fdcItem.beupdate = 0;
//                            fdcItem.version = ;
                            fdcItems.push_back(fdcItem);
                        }
                    }
                }
            }
            CxSkverManager::endUse(oProfile);
        }
    }
}

