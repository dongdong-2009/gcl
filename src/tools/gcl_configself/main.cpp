#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>

#include <ccxx/cxapplication_qt.h>

#include "configself_win.h"
#include "creator_plugin.h"

#undef main
int main(int argc, const char *argv[])
{
    QApplication a(argc, (char **)argv);

    QTranslator translator;
    translator.load(QObject::tr("qtprojects.qm"));
    a.installTranslator(&translator);

    a.setWindowIcon(QIcon(":/images/configself.png"));

    CxApplicationQt::init(argc, argv);

    ConfigselfWin w;
    w.show();
    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,(desktop->height() - w.height())/2);

    int iResult = CxApplicationQt::exec();

    return iResult;

}
