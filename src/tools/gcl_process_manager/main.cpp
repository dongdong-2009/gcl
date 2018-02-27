#include "mainwindow.h"
#include "global.h"
#include "processer.h"
#include "logwidget.h"
#include <QApplication>
#include <QMainWindow>

#undef main
int main(int argc, const char *argv[])
{

    QApplication app(argc, (char **)argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));


    CxApplication::init(argc, argv);
    MainWindow *mainWin = new MainWindow();
    mainWin->setWindowTitle("管理器");
    mainWin->show();

    cout << CxApplication::configExtendFilePath() << endl;

    return  CxApplication::exec();
    //return app.exec();
}
