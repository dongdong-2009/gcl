#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "global.h"
#include "configwidget.h"
#include "applicationwidget.h"
#include "processer.h"
#include "logwidget.h"
#include "system_tray.h"
#include "logmemo.h"
#include <QMainWindow>
#include <QString>
#include <QDialog>

class QAction;
class QMenu;
class QTextEdit;
class QFileInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow();
    static void warningMessagebox(const QString text);
    void showWidget();
    static int &getValue();

    QPlainTextEdit *log_memo;
    CLogMemo *log;

private slots:
    void startMainProcess();
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
    void rightShowWidget();
    void quit_slot();

private:
    void createActions();
    void createMenus();
    void createToolBars();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *startAction;
    QAction *exitAct;
    TabWidget *centralwidget;
    QTabWidget *tabWidget;
    SystemTray *system_tray; //托盘项

    //QDialog *dialog;

};




#endif // MAINWINDOW_H
