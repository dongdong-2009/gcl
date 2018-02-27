#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>

#include <vdi/terminal_exe.h>
#include <tools/gcl_configurator/configurator_win.h>
#include <vdi/config_base.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QObject::tr("qtprojects.qm"));
    a.installTranslator(&translator);

    a.setWindowIcon(QIcon(":/images/configurator.png"));

    TerminalExe::load(argc, argv);

    //注册新配置 添加策略
//    ConfigCatalogCouple * oBusinessNewMode = new ConfigCatalogCouple(CS_NewModeBusiness, CxApplication::applicationConfigFilePath(), CS_SectionBusinesses, CS_EntryBusinessNameTypes);
//    ConfigCatalogManager::registerNewMode(oBusinessNewMode);

    ConfigManager::load();

    ConfiguratorWin w;
    w.show();
    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,(desktop->height() - w.height())/2);

    int iResult = CxApplication::exec(0);

    TerminalExe::stop();

    ConfigManager::save();

    CxApplication::saveConfig();

    return iResult;

}
