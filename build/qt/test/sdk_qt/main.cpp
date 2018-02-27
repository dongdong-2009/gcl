#include "sdk_test_win.h"
#include <QApplication>
#include <QTextCodec>

#include "gcl_common_sdk.h"

#undef main
int main(int argc, const char *argv[])
{
    QApplication a(argc, (char **)argv);

#if (QT_VERSION < QT_VERSION_CHECK(4, 8, 9))
    QTextCodec::setCodecForTr(QTextCodec::codecForMib(106)); //utf-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForMib(106)); //utf-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForMib(106)); //utf-8
#endif

    SdkTestWin::setArg(argc, argv);
    SdkTestWin w;
    w.show();

    return a.exec();
}
