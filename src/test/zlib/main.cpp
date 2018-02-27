#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

#include <ccxx/cxapplication.h>

#undef main
int main(int argc, const char *argv[])
{
    QApplication a(argc, (char **)argv);

    CxApplication::init(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(4, 8, 9))
    QTextCodec::setCodecForTr(QTextCodec::codecForMib(106)); //utf-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForMib(106)); //utf-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForMib(106)); //utf-8
#endif

    MainWindow w;
    w.show();

    return CxApplication::exec();
}
