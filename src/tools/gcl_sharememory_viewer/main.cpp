#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QTextCodec>

#include "sharememoryviewer_win.h"

#include <ccxx/cxapplication_qt.h>
#include <ccxx/cxtimer.h>
#include <vdi/measure_sharememory_all.h>
#include <vdi/fileinfo_sharememory.h>
#include <common/gcl_config.h>

#undef main
int main(int argc, const char *argv[])
{
    CxApplicationQt::init(argc, argv);

    MeasureShareMemoryAll::open();
    FileInfoShareMemory::open();

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

    ShareMemoryViewer_win w;
    w.show();
    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,(desktop->height() - w.height())/2);

    GclConfig::load();

    int iResult = CxApplicationQt::exec();

    MeasureShareMemoryAll::close();
    FileInfoShareMemory::close();

    return iResult;
}
