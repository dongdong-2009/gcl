#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QMdiArea>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>


#include <ccxx/qtcommon.h>

using namespace std;

static MainWindow * f_oMainWindow = NULL;

template<class TWidget>
QMdiSubWindow * createOrShowMdi(QMdiArea * oMdiArea, QMdiSubWindow * oMid)
{
    QMdiSubWindow * r = oMid;
    if (! r)
    {
        TWidget * oWidget = new TWidget(oMdiArea);
        r = oMdiArea->addSubWindow(oWidget);
        oWidget->showMaximized();
    }
//    QMdiSubWindow * oChild = _mdiArea->findChild(_fdcManagerWin);
//    if (oChild) _mdiArea->setActiveSubWindow(oChild);
    oMdiArea->setActiveSubWindow(r);
    oMdiArea->showMaximized();
    return r;
}

template<class TWidget>
TWidget * createOrShowPage(QStackedWidget * oStackedWidget, TWidget * oMid)
{
    TWidget * r = oMid;
    if (! r)
    {
        r = new TWidget(oStackedWidget);
        oStackedWidget->addWidget(r);
    }
    oStackedWidget->setCurrentIndex(oStackedWidget->indexOf(r));
    return r;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _fdcManagerWin = NULL;
    _fdvManagerWin = NULL;
    _praManagerWin = NULL;
    _praUpdownWin = NULL;

    ui->setupUi(this);

    f_oMainWindow = this;

    CxInterinfoOut::addObserver(this);
    CxProcess::registProcessNotify(MainWindow::processNotify);

    createActions();
    createMenus();
    createToolBars();
    readSettings();

    ui->infoEd->installEventFilter(this);

    setWindowTitle(QString::fromUtf8("ICS配置管理系统 - 产品根目录：") + QtCommonString::gbkToQString( FdcManager::getProductRootPath() )
                   + QString::fromUtf8(" - 项目根目录：") + QtCommonString::gbkToQString( PraManager::getProjectRootPath() ) );
    setUnifiedTitleAndToolBarOnMac(true);

    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 1);
}

MainWindow::~MainWindow()
{
    CxInterinfoOut::removeObserver(this);
    delete ui;
}

bool MainWindow::eventFilter(QObject * oObject, QEvent * oEvent)
{
    if (oEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);

        if (keyEvent->modifiers() & Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_D)
            {
                ui->infoEd->clear();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(oObject, oEvent);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_praUpdownWin)
    {
        _praUpdownWin->close();
        delete _praUpdownWin;
        _praUpdownWin = NULL;
    }
    event->accept();
//    _mdiArea->closeAllSubWindows();
//    if (_mdiArea->currentSubWindow()) {
//        event->ignore();
//    } else {
//        writeSettings();
//        event->accept();
//    }
}

void MainWindow::interinfo_out(const std::string &sInfo, const std::string &sTitle, int type, int reason, int source, int target, int iTag)
{
    static int iCount = 0;

    if (iCount % 100 == 0) ui->infoEd->clear();

    ++iCount;

    stringstream sOutInfo;

    sOutInfo << CxTime::currentMsepochString() << " ";

    if (sTitle.size()>0)
        sOutInfo << "[title]=" << sTitle;
    if (type)
        sOutInfo << "[type]=" << type;
    if (reason)
        sOutInfo << "[reason]=" << reason;
    if (source)
        sOutInfo << "[source]=" << source;
    if (target)
        sOutInfo << "[target]=" << target;
    if (iTag)
        sOutInfo << "[tag]=" << iTag;
    if (sOutInfo.tellp() > 0)
        sOutInfo << CxGlobal::lineString;
    sOutInfo << sInfo;
    ui->infoEd->append(QtCommonString::gbkToQString( sOutInfo.str()) );
}

//void MainWindow::newAreaManagerWin()
//{
//    QWidget * oMainWidget = new QWidget;
//    _mdiArea->addSubWindow(oMainWidget);
//    QHBoxLayout *horizontalLayout = new QHBoxLayout;
//    horizontalLayout->addWidget(new AreaViewWin);
//    AreaViewWin * o = new AreaViewWin;
//    horizontalLayout->addWidget(o);

//    oMainWidget->setLayout(horizontalLayout);
//    oMainWidget->setWindowTitle(QObject::tr("Draggable Icons"));
//    oMainWidget->show();

//    o->drawLineTest();
//}

void MainWindow::showFdcsWin()
{
    _fdcManagerWin = createOrShowPage<FdcManagerWin>(ui->stackedWidget, _fdcManagerWin);
}

void MainWindow::showFdvsWin()
{
    _fdvManagerWin = createOrShowPage<FdvManagerWin>(ui->stackedWidget, _fdvManagerWin);
}

void MainWindow::showTidsWin()
{
    _fdcManagerWin = createOrShowPage<FdcManagerWin>(ui->stackedWidget, _fdcManagerWin);
}

void MainWindow::showMidsWin()
{
    _fdcManagerWin = createOrShowPage<FdcManagerWin>(ui->stackedWidget, _fdcManagerWin);
}

void MainWindow::showAreaManagerWin()
{
    _praManagerWin = createOrShowPage<PraManagerWin>(ui->stackedWidget, _praManagerWin);
}

void MainWindow::showAreaConfigWin()
{
    _praUpdownWin = createOrShowPage<PraUpdownWin>(ui->stackedWidget, _praUpdownWin);
}

void MainWindow::saveAll()
{
    FdvManager::save();
    FdcManager::save();
    PraManager::save();
    cxPrompt() << "save all finish.";
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About ICS"),
            tr("The <b>ICS</b> Config Manager Is Valid From 2015 to 2016"
               "document interface applications using CCXX and GCL."));
}

void MainWindow::createActions()
{
    _showFdcsWinAct = new QAction(QIcon(":/images/1.png"), QString::fromUtf8("产品管理"), this);
//    showFdcsWinAct->setShortcuts(QKeySequence::New);
    _showFdcsWinAct->setStatusTip(tr("showFdcsWin"));
    connect(_showFdcsWinAct, SIGNAL(triggered()), this, SLOT(showFdcsWin()));

    _showFdvsWinAct = new QAction(QIcon(":/images/2.png"), QString::fromUtf8("版本登记"), this);
//    showFdvsWinAct->setShortcuts(QKeySequence::Open);
    _showFdvsWinAct->setStatusTip(tr("showFdvsWin"));
    connect(_showFdvsWinAct, SIGNAL(triggered()), this, SLOT(showFdvsWin()));

    _showTidsWinAct = new QAction(QIcon(":/images/3.png"), QString::fromUtf8("tid总表"), this);
//    showTidsWinAct->setShortcuts(QKeySequence::New);
    _showTidsWinAct->setStatusTip(tr("Create a new file"));
    connect(_showTidsWinAct, SIGNAL(triggered()), this, SLOT(showTidsWin()));

    _showMidsWinAct = new QAction(QIcon(":/images/4.png"), QString::fromUtf8("mid总表"), this);
//    showMidsWinAct->setShortcuts(QKeySequence::Open);
    _showMidsWinAct->setStatusTip(tr("Open an existing file"));
    connect(_showMidsWinAct, SIGNAL(triggered()), this, SLOT(showMidsWin()));

    _showAreaManagerWinAct = new QAction(QIcon(":/images/5.png"), QString::fromUtf8("区域管理"), this);
//    showAreaManagerWinAct->setShortcuts(QKeySequence::Save);
    _showAreaManagerWinAct->setStatusTip(tr("Save the document to disk"));
    connect(_showAreaManagerWinAct, SIGNAL(triggered()), this, SLOT(showAreaManagerWin()));

    _showAreaConfigAct = new QAction(QIcon(":/images/6.png"), QString::fromUtf8("区域配置"), this);
//    showAreaConfigAct->setShortcuts(QKeySequence::SaveAs);
    _showAreaConfigAct->setStatusTip(tr("Save the document under a new name"));
    connect(_showAreaConfigAct, SIGNAL(triggered()), this, SLOT(showAreaConfigWin()));

//! [0]
    _exitAct = new QAction(tr("Exit"), this);
    _exitAct->setShortcuts(QKeySequence::Quit);
    _exitAct->setStatusTip(tr("Exit the application"));
    connect(_exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    _saveAct = new QAction(QString::fromUtf8("保存全部"), this);
    _saveAct->setShortcuts(QKeySequence::Quit);
    _saveAct->setStatusTip(tr("Save the application"));
    connect(_saveAct, SIGNAL(triggered()), this, SLOT(saveAll()));
//! [0]

    _aboutAct = new QAction(tr("&About"), this);
    _aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    menuBar()->addSeparator();

    _fileMenu = menuBar()->addMenu(QString::fromUtf8("ICS产品"));
    _fileMenu->addAction(_showFdcsWinAct);
    _fileMenu->addAction(_showFdvsWinAct);

    menuBar()->addSeparator();

    _editMenu = menuBar()->addMenu(QString::fromUtf8("ICS工程"));
    _editMenu->addAction(_showAreaManagerWinAct);
    _editMenu->addAction(_showAreaConfigAct);
    _editMenu->addSeparator();

    menuBar()->addSeparator();

    _otherMenu = menuBar()->addMenu(QString::fromUtf8("Other"));
    QAction *action = _otherMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    _otherMenu->addAction(_saveAct);
    _otherMenu->addAction(_exitAct);

    menuBar()->addSeparator();

    _helpMenu = menuBar()->addMenu(QString::fromUtf8("&Help"));
    _helpMenu->addAction(_aboutAct);

}

void MainWindow::createToolBars()
{
    _winsToolBar = addToolBar(tr("File"));
    _winsToolBar->addAction(_showFdcsWinAct);
    _winsToolBar->addAction(_showFdvsWinAct);
    _winsToolBar->addSeparator();
    _winsToolBar->addAction(_showAreaManagerWinAct);
    _winsToolBar->addAction(_showAreaConfigAct);
    _winsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void MainWindow::readSettings()
{
    QSettings settings("Trolltech", "MDI Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("Trolltech", "MDI Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

//void MainWindow::setActiveSubWindow(QWidget *window)
//{
//    if (!window)
//        return;
//    _mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
//}

std::vector<fn_void_tlv_t> * fn_getFnProcessChanges()
{
    static vector<fn_void_tlv_t> fnProcessChanges;
    return & fnProcessChanges;
}


void MainWindow::processNotify(pid_os_t pid, const string &sArgs)
{
    cxPrompt() << CxString::format("config finish. [%d][%s].", pid.dwProcessId, sArgs.c_str());
    if (f_oMainWindow)
    {
//        cxPrompt() << QtCommonString::gbkToStdString(QString::fromUtf8("进程[]已经退出[%1]").arg(mPid.dwProcessId));
        vector<fn_void_tlv_t> * oFnProcessChanges = fn_getFnProcessChanges();
        for (size_t i = 0; i < oFnProcessChanges->size(); ++i)
        {
            fn_void_tlv_t fn = oFnProcessChanges->at(i);
            fn(0, 0, 0);
        }
        f_oMainWindow->show();
    }
}

void MainWindow::processExec(const char *sArgs)
{
    pid_os_t pid = CxProcess::exec(sArgs);
    if (CxProcess::isvalid(pid) && f_oMainWindow)
    {
        f_oMainWindow->hide();
    }
}

void MainWindow::registProcessChangeFunction(fn_void_tlv_t fn)
{
    vector<fn_void_tlv_t> * oFnProcessChanges = fn_getFnProcessChanges();
    if (std::find(oFnProcessChanges->begin(), oFnProcessChanges->end(), fn) == oFnProcessChanges->end())
        oFnProcessChanges->push_back(fn);
}

