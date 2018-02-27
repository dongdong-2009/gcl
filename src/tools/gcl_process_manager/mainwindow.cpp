#include "mainwindow.h"
#include <QFile>
#include <QtGui>
#include <QMessageBox>

using namespace std;
int refresh_num = 0;

MainWindow::MainWindow()
{    

    createActions();
    createMenus();
    createToolBars();
    centralwidget = new TabWidget;
    resize(640, 480);
    tabWidget = new QTabWidget(centralwidget);
    setCentralWidget(tabWidget);
    //setCentralWidget(centralwidget);
    //GeneralTab *genWidget = new GeneralTab(xmlvector);
    system_tray = new SystemTray;
    system_tray->setIcon(QIcon(":/images/gcl_process_manager.png"));
    connect(system_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    connect(system_tray->action_open, SIGNAL(triggered()), this, SLOT(rightShowWidget()));
    connect(system_tray->action_logout, SIGNAL(triggered()), this, SLOT(quit_slot()));
    system_tray->show();

    log_memo = new QPlainTextEdit;
    log = new CLogMemo (log_memo);

    tabWidget->addTab(new ApplicationsTab(), "应用状态");
    tabWidget->addTab(new ConfigTab(), "参数管理");
    tabWidget->addTab(new LogWidget(), "运行记录");
    tabWidget->addTab(log_memo, "Log");

}

void MainWindow::quit_slot()
{
    log->log("cannot open %1 because of: %2");
    qDebug() << "提示输入密码!!!!!";
    bool ok;
    QString str = QInputDialog::getText(this, "密码核对", "请输入密码：", QLineEdit::Password, "", &ok);
    if(ok)
    {
        if(str == QString("ygct"))
        {
            qApp->quit();
        }
        else
        {
            MainWindow::warningMessagebox("密码错误");
            this->show();
        }
    }
    this->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(system_tray->isVisible())
        this->hide();
    else
    { 
        QMessageBox::information(this, "Notice", "You can't close the window");
    }
    event->ignore();
}

void MainWindow::showWidget()
{
    this->showNormal();
    this->raise();
    this->activateWindow();
}

void MainWindow::rightShowWidget()
{
    this->showNormal();
    this->raise();
    this->activateWindow();
}

void MainWindow::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        //点击托盘图标之后松开
    case QSystemTrayIcon::Trigger:
        {
            showWidget();
            break;
        }
        //双击托盘图标
    case QSystemTrayIcon::DoubleClick:
        {
            showWidget();
            break;
        }
    default:
        break;
    }
}

void MainWindow::warningMessagebox(const QString text)
{
    int iRet = QMessageBox::information( NULL, "警告", text, QMessageBox::Yes|QMessageBox::No );
    switch(iRet)
    {
        case QMessageBox::Yes:
            break;
        case QMessageBox::No:
            break;
        default:
            break;
    }
}


static DWORD WINAPI launchProcess(PVOID pParam)
{
    Processer::prepareProcessInfo(GlobalFunction::getApplicationConfigFilePath());
    //Processer::prepareProcessInfo(CxApplication::);

    if( Processer::vectorIsEmpty() )
    {
        MainWindow::warningMessagebox("没有要守护的应用程序，检查参数及其正确性!!!");
    }

    cout << (string("processwatcher start time : ") + GlobalFunction::fn_toString(GlobalFunction::fn_currentMsepoch())) << endl;

    WSADATA            wsd;
    if (::WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        MainWindow::warningMessagebox("初始化套接字动态库失败!!!");
    }

    const std::vector<Processer *> processes = Processer::getProcesses();
    for(size_t i = 0; i < processes.size(); ++i)
    {
        Processer *oProcesser = processes.at(i);
        int ret = oProcesser->killProcess(const_cast<char *>(oProcesser->exeFileName.data()) );
        cout << "Kill " << ret << oProcesser->exeFileName << endl;
    }

    for(size_t i = 0; i < processes.size(); ++i)
    {
        Processer *oProcesser = processes.at(i);
        oProcesser->createProcess();
        if(oProcesser->localPort > 0 && oProcesser->localPort < USHRT_MAX)
        {
            oProcesser->createSocket();
        }
        if(oProcesser->waitFirstMillisecond > 0)
        {
            Sleep(oProcesser->waitFirstMillisecond);
            cout << "Process: "<< oProcesser->exeFileName << " WaitTime: " << oProcesser->waitFirstMillisecond << endl;
        }
    }

    refresh_num = 1;
    msepoch_t dtPreNow = GlobalFunction::fn_currentMsepoch();
    while(1)
    {
        msepoch_t dtNow = GlobalFunction::fn_currentMsepoch();
        unsigned int dtSpan = dtNow - dtPreNow;   //持续时间
        for(size_t i = 0; i < processes.size(); ++i)
        {
            Processer *oProcesser = processes.at(i);
            //程序退出 或 接收数据超时
            if(oProcesser->isThreadWaitFinish || ( oProcesser->heartBuffer.size() > 0 && ( oProcesser->notReceiveLong > oProcesser->timeOut_receive ) ))
            {
                refresh_num = 0;
                oProcesser->rerunProcess();
                refresh_num = 1;
                continue;
            }
            else
            {
                unsigned int notReceiveTime = dtSpan + oProcesser->notReceiveLong;
                oProcesser->notReceiveLong = notReceiveTime;
            }
            //socket发生错误重启进程
            if ( oProcesser->isThreadReceiveError )
            {
                oProcesser->restartSocket();
                continue;
            }


        }
        dtPreNow = dtNow;
    }

    WSACleanup();

    return 0;
}


void MainWindow::startMainProcess()
{
    DWORD launchThreadId;
    startAction->setEnabled(false);
    HANDLE launchThread = ::CreateThread(NULL, 0, launchProcess, this, 0, &launchThreadId);
    if(!launchThread)
    {
        cout << "startMainProcess : CreateThread failed" << endl;
    }

}

void MainWindow::createActions()
{
    startAction = new QAction(trUtf8("启动"), this);
    startAction->setStatusTip(tr("Start process"));
    startAction->setEnabled(false);
   // startAction->setIcon(QIcon(":/image/start.bmp"));
    connect(startAction, SIGNAL(triggered()), this, SLOT(startMainProcess()));

}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(trUtf8("文件"));

    fileMenu->addAction(startAction);
    fileMenu->addSeparator();
    fileMenu->addAction(trUtf8("退出"));

    editMenu = menuBar()->addMenu(tr("&Edit"));
    //menuBar()->addSeparator();
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("about process"));
}

void MainWindow::createToolBars()
{

    fileToolBar = addToolBar(trUtf8("启动"));
    fileToolBar->addAction(startAction);
    editToolBar = addToolBar(tr("Edit"));
    
}

int &MainWindow::getValue()
{
    return refresh_num;
}


