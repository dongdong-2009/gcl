#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QTextCodec>

#include "user_tool.h"

#include <ccxx/cxapplication.h>
#include <ccxx/cxtimer.h>

#undef main
int main(int argc, const char *argv[])
{
    CxApplication::init(argc, argv);

    QApplication a(argc, (char **)argv);
#if (QT_VERSION < QT_VERSION_CHECK(4, 8, 9))
    QTextCodec::setCodecForTr(QTextCodec::codecForMib(106)); //utf-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForMib(106)); //utf-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForMib(106)); //utf-8
#endif
    QTranslator translator;
    translator.load(QObject::tr("qtprojects.qm"));
    a.installTranslator(&translator);

    a.setWindowIcon(QIcon(":/images/sharememory_viewer.png"));

    user_tool w;
    w.show();
    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,(desktop->height() - w.height())/2);

    int iResult = CxApplication::exec();


    return iResult;
}
