#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>

#include <ccxx/cxapplication.h>

#include "psm_kits_win.h"

#undef main
int main(int argc, const char *argv[])
{
    QApplication a(argc, (char **)argv);

    QTranslator translator;
    translator.load(QObject::tr("qtprojects.qm"));
    a.installTranslator(&translator);

    a.setWindowIcon(QIcon(":/images/configself.png"));

    CxApplication::init(argc, argv);

    PsmKitsWin w;
    w.show();
    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,(desktop->height() - w.height())/2);

    int iResult = CxApplication::exec(0);

    return iResult;
}
