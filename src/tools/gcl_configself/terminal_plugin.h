#ifndef TERMINAL_PLUGIN_H
#define TERMINAL_PLUGIN_H



#include "configself_win.h"


class TerminalPlugin : public QObject, public ConfigselfPluginBase
{

    Q_OBJECT

public:
    TerminalPlugin();

    ~TerminalPlugin();

protected:
    void initImpl(ConfigselfWin * oConfigselfWin);

    void uninitImpl();

    bool projectChangeImpl(const ConfigselfWin* oConfigselfWin, int iState);

    void catalogChangedImpl(const ConfigselfWin* oConfigselfWin);

private:
    void initUi();

    bool dialogInputNewTerminal(std::string &sProtocolUnitName, std::string &sChannelUnitName, std::string &sTerminalName, std::string &sCatalogTitle);

    bool addTerminal(std::string &sProtocolUnitName, std::string &sChannelUnitName, std::string &sTerminalName, std::string &sCatalogTitle);

    QStringList getProtocolNames();

    QStringList getChannelNames();

    std::string getTypeSimpleNameByUnitName(const std::string &sUnitName);

private:
    ConfigselfWin * configselfWin;

    std::string _terminalsConfigFilePath;

    QMenu * _uiTerminalMenu;
    QAction * _uiAddTerminalAct;
    QAction * _uiRemoveTerminalAct;

private slots:
    void on_newTerminalBn_clicked();

    void on_removeTerminalBn_clicked();

};


class TerminalPluginFactory : public ConfigselfPluginFactoryBase
{
public:
    std::string factoryName(){
        return "TerminalPlugin";
    }

    ConfigselfPluginBase* createObject(){
        return new TerminalPlugin();
    }

};


#endif // TERMINAL_PLUGIN_H
