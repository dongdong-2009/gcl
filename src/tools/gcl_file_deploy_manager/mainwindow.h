#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "messageboxwin.h"
#include "fdcmanagerwin.h"
#include "fdvmanagerwin.h"
#include "dragwidget.h"
#include "areaviewwin.h"
#include "pramanagerwin.h"
#include "praupdownwin.h"


namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE


class MainWindow : public QMainWindow, public CxInterinfoOut_I
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    bool eventFilter(QObject *, QEvent *);
    void closeEvent(QCloseEvent *event);

protected:
    void interinfo_out( const std::string& sInfo, const std::string& sTitle, int type = 0, int reason = 0, int source = 0, int target = 0, int iTag = 0 );

    inline CxInterinfo::PlatformEnum platformValue() { return CxInterinfo::Platform_Desktop; }

private:
    Ui::MainWindow *ui;

    QMenu * _fileMenu;
    QMenu * _editMenu;
    QMenu * _otherMenu;
    QMenu * _helpMenu;

    QToolBar * _winsToolBar;

    QAction * _showFdcsWinAct;
    QAction * _showFdvsWinAct;

    QAction * _showTidsWinAct;
    QAction * _showMidsWinAct;
    QAction * _showAreaManagerWinAct;
    QAction * _showAreaConfigAct;

    QAction * _saveAct;
    QAction * _exitAct;
    QAction * _aboutAct;

    void createActions();
    void createMenus();
    void createToolBars();
    void readSettings();
    void writeSettings();

private slots:
    //
    void showFdcsWin();
    void showFdvsWin();
    //
    void showTidsWin();
    void showMidsWin();
    void showAreaManagerWin();
    void showAreaConfigWin();
    //
    void saveAll();
    void about();
    void switchLayoutDirection();

public:
    static void processNotify(pid_os_t pid, const std::string & sArgs);

    static void processExec(const char * sArgs);

    static void registProcessChangeFunction(fn_void_tlv_t fn);

    FdcManagerWin * _fdcManagerWin;
    FdvManagerWin * _fdvManagerWin;
    PraManagerWin * _praManagerWin;
    PraUpdownWin * _praUpdownWin;

};

#endif // MAINWINDOW_H
