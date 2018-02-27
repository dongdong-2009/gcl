#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>

#include <ccxx/cxapplication.h>

#include "setup_win.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QObject::tr("qtprojects.qm"));
    a.installTranslator(&translator);

    a.setWindowIcon(QIcon(":/images/setup.png"));

    CxApplication::init(argc, argv);

    SetupWin w;
    w.show();
    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,(desktop->height() - w.height())/2);

    int iResult = CxApplication::exec(1);

    CxApplication::saveConfig();

    return iResult;

}
