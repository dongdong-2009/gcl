#include "applicationwidget.h"
#include <QtGui>

#define MAX_BUF_SIZE 512

ShareMemory sm;
Processer *oProcesser;
string text = "";
SOCKET socketId;
SOCKADDR_IN recvAddr;
SOCKADDR_IN sendAddr;

struct processer
{
    int s_proNum;
    int s_unusalCloseTime;
    DWORD s_dwprocessId;
    HANDLE s_hProcess;
    char s_exeName[32];
    int s_startTimes;
    msepoch_t s_startTime;
    char s_commandLine[128];
    char s_exeFilePath[128];
};

struct processer *map_lp;
struct processer pro[12];


ApplicationsTab::ApplicationsTab(QWidget *parent)
    : QWidget(parent)
{

    tableHeaders << "进程名" << "进程ID"  << "线程数" << "工作设置内存" <<
                    "网络状态"  << "运行时长" << "IO读取" << "IO写入" <<
                    "启动次数" << "优先级" << "异常关闭次数" ;
    //processtable->setItemDelegate(new NoFocusDelegate());             //虚框不显示

    processTable = new QTableWidget(0, tableHeaders.size());
    processTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    QHeaderView* vheaderView = processTable->verticalHeader();        //行头不显示

    vheaderView->setHidden(true);
    processTable->setShowGrid(false);
    processTable->setHorizontalHeaderLabels(tableHeaders);

    //logwidget = new LogWidget();
    //logwidget->appendMessage("ssssss");

    m_thread = new MyThread(800);
    checkbox = new QCheckBox("定时刷新");
    checkbox->setChecked(false);    
    killProcessButton = new QPushButton("结束进程");

    rightMenu = new QMenu(processTable);
    action_choseTableTitle = new QAction("选择列", this);
    action_createProcess = new QAction("启动进程", this);
    action_killProcess = new QAction("结束进程", this);
    action_show = new QAction("显示", this);
    action_hide = new QAction("隐藏", this);
    action_showMax = new QAction("最大化", this);
    action_showMin = new QAction("最小化", this);

    rightMenu->addAction(action_show);
    rightMenu->addAction(action_hide);
    rightMenu->addSeparator();
    rightMenu->addAction(action_choseTableTitle);
    rightMenu->addSeparator();
    rightMenu->addAction(action_createProcess);
    rightMenu->addAction(action_killProcess);
    rightMenu->addSeparator();
    rightMenu->addAction(action_showMax);
    rightMenu->addAction(action_showMin);


    processTable->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(checkbox);
    hlayout->addStretch(20);
    hlayout->addWidget(killProcessButton);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(processTable);
    vlayout->addLayout(hlayout);
    setLayout(vlayout);

    connect(action_showMax, SIGNAL(triggered()), this, SLOT(slot_showMax()));
    connect(action_showMin, SIGNAL(triggered()), this, SLOT(slot_showMin()));
    connect(action_choseTableTitle, SIGNAL(triggered()), this, SLOT(slot_choseTableTitle()));
    connect(action_createProcess, SIGNAL(triggered()), this, SLOT(slot_createProcess()));
    connect(action_killProcess, SIGNAL(triggered()), this, SLOT(slot_killProcess()));
    connect(action_show, SIGNAL(triggered()), this, SLOT(slot_show()));
    connect(action_hide, SIGNAL(triggered()), this, SLOT(slot_hide()));

    connect(checkbox, SIGNAL(clicked()), this, SLOT(checkChange()));
    connect(m_thread, SIGNAL(sendMessage(QString)), this, SLOT(showMessage(QString)));
    connect(killProcessButton, SIGNAL(clicked()), this, SLOT(slot_killProcess()));
    connect(processTable, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_customContextMenuRequested(QPoint)));

    createShareMemoryThread();
    createSocketThread();
    //exeNameItem();
}



static DWORD WINAPI fn_threadSend(LPVOID lpParam)
{
    WSADATA wsd;
    if(WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        cxDebug() << "WSAStartup初始化套接字动态库失败" << cxEndLine;
        ::system("pause");
        return false;
    }

    if(LOBYTE(wsd.wVersion) != 2 || HIBYTE(wsd.wVersion) != 2)
    {
        cxDebug() << "wVersion套接字库版本错误" << cxEndLine;
        WSACleanup();
        return false;
    }

    socketId = socket(AF_INET, SOCK_DGRAM, 0);
    if(INVALID_SOCKET == socketId)
    {
        cxDebug() << "socket创建套接字失败, 失败原因 ： " << WSAGetLastError() << cxEndLine;
        return false;
    }

    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons((short)4001);
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons((short)4000);
    sendAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    //clientAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(socketId, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR)
    {
        cout << "bind绑定套接字失败， 失败原因： " << WSAGetLastError() << endl;
        return false;
    }

    int recvAddrLen = sizeof(recvAddr);
    int sendAddrLen = sizeof(sendAddr);
    char sendBuf[MAX_BUF_SIZE] = "x";
    int iSendSize = sendto(socketId, sendBuf, MAX_BUF_SIZE, 0, (SOCKADDR*)&sendAddr, sendAddrLen);
    if(iSendSize == SOCKET_ERROR)
    {
        cout << "sendto failed: " << WSAGetLastError() << endl;
        return false;
    }
    char recvBuf[MAX_BUF_SIZE];
    ZeroMemory(recvBuf, MAX_BUF_SIZE);
    while(1)
    {
        int iReceivesize = recvfrom(socketId, recvBuf, MAX_BUF_SIZE, 0, (SOCKADDR*)&recvAddr, &recvAddrLen);
        char sign = recvBuf[0];
        cout << "iReceivesize: " << iReceivesize << "recvBuf: " << recvBuf << endl;
        if(iSendSize == SOCKET_ERROR)
        {
            cout << "recvfrom failed: " << WSAGetLastError() << endl;
            return false;
        }

        switch(sign)
        {
            case 'u':
            {
                text = (recvBuf+1);
                text.append(" 异常关闭，程序自动重启! ");
                cout << recvBuf+1 << endl;
                break;
            }
            case 't':
            {
                text = (recvBuf+1);
                text.append(" 连接超时，程序自动重启! ");
                cout << recvBuf+1 << endl;
                break;
            }
            case 'a':
            {
                text = (recvBuf+1);
                text.append(" 程序手动启动! ");
                cout << recvBuf+1 << endl;
                break;
            }
            case 'b':
            {
                text = (recvBuf+1);
                text.append(" 程序手动关闭! ");
                cout << recvBuf+1 << endl;
                break;
            }
            default:
            {
                cout << "收到的命令出错" << endl;
                break;
            }
        }
    }

    closesocket(socketId);
    WSACleanup();
    return 0;
}

string ApplicationsTab::getMessage()
{
    return text;
}

void ApplicationsTab::clearText()
{
    text.clear();
}

bool ApplicationsTab::createSocketThread()
{
    DWORD dwThreadId;
    HANDLE socketThread = CreateThread(NULL, 0, fn_threadSend, NULL, 0, &dwThreadId);
    return socketThread != NULL;
}

static DWORD WINAPI createShareMemory(PVOID pParam)
{
    HANDLE hMutex = CreateMutex(NULL, FALSE, "SM_Mutex");
    if (NULL == hMutex)
    {
        if (ERROR_FILE_NOT_FOUND == GetLastError())
        {
            cout << "OpenMutex fail: file not found!" << endl;
            return -1;
        }
        else
        {
            cout << "OpenMutex fail:" << GetLastError() << endl;
            return -1;
        }
    }

    //hMutex 一旦互斥对象处于有信号状态，则该函数返回
    if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0)
    {
        cout << "WaitForSingleObject error: " << GetLastError() << endl;
       if (NULL != hMutex)				ReleaseMutex(hMutex);
        return -1;
    }

    bool bOk = sm.create("ShareMemory", 1024*1024);
    if(bOk)
    {
        map_lp = (processer *)sm.getmp();
    }
    else
    {
        ReleaseMutex(hMutex);
    }

    HANDLE hServerWriteOver = CreateEvent(NULL, TRUE, FALSE, "ServerWriteOver");
    HANDLE hClientReadOver = CreateEvent(NULL, TRUE, FALSE, "ClientReadOver");
    if ( NULL == hServerWriteOver || NULL == hClientReadOver )
    {
        cout << "CreateEvent" << GetLastError() << endl;
        ReleaseMutex(hMutex);
        sm.realese();
    }
    while(1)
    {
        if (!SetEvent(hClientReadOver)) //把指定的事件对象设置为有信号状态
        {
            sm.realese();
            if (NULL != hServerWriteOver)	CloseHandle(hServerWriteOver);
            if (NULL != hClientReadOver)	CloseHandle(hClientReadOver);
            if (NULL != hMutex)				ReleaseMutex(hMutex);
            cout << "!SetEvent(hClientReadOver)" << endl;
            return -1;
        }

        if (WaitForSingleObject(hServerWriteOver, INFINITE) != WAIT_OBJECT_0)
        {
            sm.realese();
            if (NULL != hServerWriteOver)	CloseHandle(hServerWriteOver);
            if (NULL != hClientReadOver)	CloseHandle(hClientReadOver);
            if (NULL != hMutex)				ReleaseMutex(hMutex);
            cout << "WaitForSingleObject(hServerWriteOver, INFINITE)"  << endl;
            return -1;
        }

        memcpy( &pro[0], map_lp, sizeof(processer));

        for(int i = 1; i < pro[0].s_proNum; ++i)
        {
            memcpy( &pro[i], (map_lp+i), sizeof(processer));
        }
        if (!ResetEvent(hServerWriteOver))
        {
            if (NULL != hServerWriteOver)	CloseHandle(hServerWriteOver);
            if (NULL != hClientReadOver)	CloseHandle(hClientReadOver);
            sm.realese();
            if (NULL != hMutex)				ReleaseMutex(hMutex);
            cout << "!ResetEvent(hServerWriteOver)" << endl;
            return -1;
        }

        if (NULL != hMutex) ReleaseMutex(hMutex);
        Sleep(1000);
    }

    SetEvent(hClientReadOver);
    if (NULL != hMutex) ReleaseMutex(hMutex);
    return 0;
}

void ApplicationsTab::createShareMemoryThread()
{
    DWORD launchThreadId;
    HANDLE launchThread = ::CreateThread(NULL, 0, createShareMemory, this, 0, &launchThreadId);
    if(!launchThread)
    {
        cout << "createShareMemoryThread : CreateThread failed" << endl;
    }
}


void ApplicationsTab::showMessage(QString)
{

        for(int j = 0; j < pro[0].s_proNum; ++j)
        {
            std::map<std::string, std::string> proMsg;
            proMsg = oProcesser->getProcessMessage(pro[j].s_exeName, pro[j].s_hProcess, pro[j].s_commandLine, pro[j].s_exeFilePath,
                                                   pro[j].s_dwprocessId, pro[j].s_startTime, pro[j].s_startTimes, pro[j].s_unusalCloseTime);
            if( (pro[0].s_proNum) > (processTable->rowCount())  && (j >= processTable->rowCount()) && (processTable->rowCount() != (pro[0].s_proNum)) )
            {
                processTable->insertRow(j);
            }

            for(int i = 0; i < tableHeaders.size(); ++i)
            {
                std::map<std::string, std::string>::const_iterator iter = proMsg.find((tableHeaders.at(i).toStdString()));
                if(iter != proMsg.end())
                {
                    processTable->setItem(j, i, new QTableWidgetItem(QString::fromStdString(iter->second)));
                }
            }
        }
}

void ApplicationsTab::checkChange()
{
    bool isCheck = checkbox->isChecked();
    if(isCheck)
    {
        m_thread->start();
    }
    else
    {
        m_thread->stop();
    }
}

void ApplicationsTab::slot_choseTableTitle()
{
    ChoseDialog *choseDialog = new ChoseDialog;
    for(int i = 1; i < tableHeaders.size(); ++i)
    {
        if(tableHeaders.at(i) == "进程ID")
        choseDialog->idCheckbox->setChecked(true);
        if(tableHeaders.at(i) == "线程数")
            choseDialog->threadsCheckbox->setChecked(true);
        if(tableHeaders.at(i) == "工作设置内存")
            choseDialog->memoryCheckbox->setChecked(true);
        if(tableHeaders.at(i) == "网络状态")
            choseDialog->networkCheckbox->setChecked(true);
        if(tableHeaders.at(i) == "运行时长")
            choseDialog->runtimeCheckbox->setChecked(true);
        if(tableHeaders.at(i) == "IO读取")
            choseDialog->ioReadCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "IO写入")
            choseDialog->ioWriteCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "启动次数")
            choseDialog->startTimeCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "优先级")
            choseDialog->priorityCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "异常关闭次数")
            choseDialog->unusalCloseCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "IO读取字节")
            choseDialog->ioReadByteCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "IO写入字节")
            choseDialog->ioWriteByteCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "峰值工作设置")
            choseDialog->PeakWorkingCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "专用工作集")
            choseDialog->privateMemoryCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "提交大小")
            choseDialog->pageFileUseCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "程序路径")
            choseDialog->filePathCheckBox->setChecked(true);
        if(tableHeaders.at(i) == "命令行")
            choseDialog->CommandLineCheckBox->setChecked(true);
    }

    if(choseDialog->exec())
    {
        tableHeaders.clear();
        tableHeaders.append("进程名");

        if(choseDialog->idCheckbox->isChecked())
        {
            tableHeaders.append("进程ID");
        }

        if(choseDialog->threadsCheckbox->isChecked())
        {
            tableHeaders.append("线程数");
        }

        if(choseDialog->memoryCheckbox->isChecked())
        {
            tableHeaders.append("工作设置内存");
        }

        if(choseDialog->networkCheckbox->isChecked())
        {
            tableHeaders.append("网络状态");
        }

        if(choseDialog->runtimeCheckbox->isChecked())
        {
            tableHeaders.append("运行时长");
        }

        if(choseDialog->ioReadCheckBox->isChecked())
        {
            tableHeaders.append("IO读取");
        }

        if(choseDialog->ioWriteCheckBox->isChecked())
        {
            tableHeaders.append("IO写入");
        }
        if(choseDialog->startTimeCheckBox->isChecked())
        {
            tableHeaders.append("启动次数");
        }

        if(choseDialog->priorityCheckBox->isChecked())
        {
            tableHeaders.append("优先级");
        }

        if(choseDialog->unusalCloseCheckBox->isChecked())
        {
            tableHeaders.append("异常关闭次数");
        }

        if(choseDialog->ioReadByteCheckBox->isChecked())
        {
            tableHeaders.append("IO读取字节");
        }

        if(choseDialog->ioWriteByteCheckBox->isChecked())
        {
            tableHeaders.append("IO写入字节");
        }

        if(choseDialog->PeakWorkingCheckBox->isChecked())
        {
            tableHeaders.append("峰值工作设置");
        }
        if(choseDialog->privateMemoryCheckBox->isChecked())
        {
            tableHeaders.append("专用工作集");
        }

        if(choseDialog->pageFileUseCheckBox->isChecked())
        {
            tableHeaders.append("提交大小");
        }

        if(choseDialog->filePathCheckBox->isChecked())
        {
            tableHeaders.append("程序路径");
        }

        if(choseDialog->CommandLineCheckBox->isChecked())
        {
            tableHeaders.append("命令行");
        }

        processTable->setColumnCount(tableHeaders.size());
        processTable->setHorizontalHeaderLabels(tableHeaders);
    }
}


void ApplicationsTab::on_customContextMenuRequested(QPoint pos)
{
    curItem = processTable->itemAt(pos);
    QList<QTableWidgetItem *> list = processTable->selectedItems();

    if(list.empty())
        return;

    if( list.size() < 2 )
    {
        action_createProcess->setEnabled(true);
        action_killProcess->setEnabled(false);
        action_showMax->setEnabled(false);
        action_showMin->setEnabled(false);
        action_hide->setEnabled(false);
        action_show->setEnabled(false);
    }
    else
    {
        action_createProcess->setEnabled(false);
        action_killProcess->setEnabled(true);
        action_showMax->setEnabled(true);
        action_showMin->setEnabled(true);
        action_hide->setEnabled(true);
        action_show->setEnabled(true);
    }
    rightMenu->exec(QCursor::pos());                //弹出右键菜单
}


void ApplicationsTab::slot_killProcess()
{
    bool ok;
    QString str = QInputDialog::getText(this, "密码核对", "请输入密码：", QLineEdit::Password, "", &ok);
    if(ok)
    {
        if(str == QString("ygct"))
        {
            char sendBuf[MAX_BUF_SIZE] = "b";
            int currentRow = processTable->currentRow();

            std::string processName = (processTable->item(currentRow, 0)->text()).toStdString();
            strcpy(sendBuf+1, (GlobalFunction::fn_toString(currentRow).c_str()) );
            strcpy(sendBuf+2, processName.c_str());

            int iSendSize = 0;
            int sendAddrLen = sizeof(SOCKADDR);

            iSendSize = sendto(socketId, sendBuf, MAX_BUF_SIZE, 0, (SOCKADDR*)&sendAddr, sendAddrLen);
            if(iSendSize == SOCKET_ERROR)
            {
                cout << "sendto error: " << WSAGetLastError() << endl;
                closesocket(socketId);
                WSACleanup();
            }

            cout << "kill " << processName << endl;
            processTable->clearContents();
        }
        else
        {
            int iRet = QMessageBox::information( NULL, "警告", "密码错误", QMessageBox::Yes|QMessageBox::No );
            switch(iRet)
            {
                case QMessageBox::Yes:
                case QMessageBox::No:
                default:
                    break;
            }
            this->show();
        }
    }
}


void ApplicationsTab::slot_createProcess()
{
    char sendBuf[MAX_BUF_SIZE] = "a";
    int currentRow = processTable->currentRow();
    std::string processName = (processTable->item(currentRow, 0)->text()).toStdString();
    strcpy(sendBuf+1, (GlobalFunction::fn_toString(currentRow).c_str()) );
    strcpy(sendBuf+2, processName.c_str());

    int iSendSize = 0;
    int sendAddrLen = sizeof(SOCKADDR);

    iSendSize = sendto(socketId, sendBuf, MAX_BUF_SIZE, 0, (SOCKADDR*)&sendAddr, sendAddrLen);
    if(iSendSize == SOCKET_ERROR)
    {
        cout << "sendto error: " << WSAGetLastError() << endl;
        closesocket(socketId);
        WSACleanup();
    }
}


std::string ApplicationsTab::getProcessName()
{
    int currentRow = processTable->currentRow();
    std::string windowName = pro[currentRow].s_exeFilePath;
    return  windowName;
}


void ApplicationsTab::slot_showMax()
{
    HWND hWnd = FindWindow(NULL, getProcessName().data());
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);
}

void ApplicationsTab::slot_showMin()
{
    HWND hWnd = FindWindow(NULL, getProcessName().data());
    ShowWindow(hWnd, SW_MINIMIZE);
}

void ApplicationsTab::slot_hide()
{
    HWND hWnd = FindWindow(NULL, getProcessName().data());
    ShowWindow(hWnd, SW_HIDE);
}

void ApplicationsTab::slot_show()
{
    HWND hWnd = FindWindow(NULL, getProcessName().data());
    if(!hWnd)
    {
        HWND hWnd1;
        hWnd1 = CreateWindow(NULL, getProcessName().data(), WS_SYSMENU|WS_VISIBLE, 0, 0, 400, 300, NULL, NULL, 0, NULL);

        if (hWnd1 == NULL){
            cout << "窗口创建失败" << endl;
        }
    }
    else
    {
        ShowWindow(hWnd, SW_SHOWNORMAL);
    }
}
