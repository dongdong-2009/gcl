#include "mainwindow.h"
#include "configwidget.h"
#include "no_focus_delegate.h"
#include "revisedialog.h"
#include <QtGui>
#include <QFile>
#include <QDir>
#include <QTreeWidget>

std::string qCurrentItemStr = "";

TabWidget::TabWidget(QWidget *parent)
    : QWidget(parent)
{

}

ConfigTab::ConfigTab( QWidget *parent)
    : QWidget(parent)
{
    treeWidget = new QTreeWidget;
    treeWidget->setMouseTracking(true);
    //treeWidget->setColumnCount(1);
    treeWidget->setStyleSheet("QTreeWidget::item{height:25px}"); //设置字体大小
    //treeWidget->setColumnWidth(0, 2);
    CreateTree();

    process_declareTable = new QTableWidget;
    ProHeadMessagePage();
    process_paramTable = new QTableWidget;
    ProMessagePage();

    rightClickMenu = new QMenu(process_paramTable);
    action_revise = new QAction("修改", this);
    action_delete = new QAction("删除", this);
    action_addnode = new QAction("增加", this);
    rightClickMenu->addAction(action_revise);
    rightClickMenu->addSeparator();
    rightClickMenu->addAction(action_delete);
    rightClickMenu->addSeparator();
    rightClickMenu->addAction(action_addnode);
    process_paramTable->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(process_paramTable, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_customContextMenuRequested(QPoint)));
    connect(action_delete, SIGNAL(triggered()), this, SLOT(slot_delete()));
    connect(action_revise, SIGNAL(triggered()), this, SLOT(slot_revise()));
    connect(action_addnode, SIGNAL(triggered()), this, SLOT(slot_addnode()));

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(process_declareTable);
    pagesWidget->addWidget(process_paramTable);

    connect(treeWidget, SIGNAL(currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(showSelectedMessage( QTreeWidgetItem*, QTreeWidgetItem* )));

    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
    treeWidget->setSizePolicy(sizePolicy);
    treeWidget->setMinimumSize(QSize(80, 0));

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(3);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(pagesWidget->sizePolicy().hasHeightForWidth());
    pagesWidget->setSizePolicy(sizePolicy1);

    QHBoxLayout *main_layout = new QHBoxLayout();
    this->setLayout(main_layout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(treeWidget);
    splitter->addWidget(pagesWidget);

    main_layout->addWidget(splitter);

}

void ConfigTab::CreateTree()
{
    std::vector<std::map<std::string, std::string> > xmlvector1 = Processer::analyseXml(GlobalFunction::getApplicationConfigFilePath());
    if( xmlvector1.size() > 0 )
    {
        treeWidget->setHeaderLabel( "程序信息管理" );
        QTreeWidgetItem *processParamHead = new QTreeWidgetItem( treeWidget, QStringList("启动参数配置"));
        QTreeWidgetItem *executableFileHead = new QTreeWidgetItem( treeWidget, QStringList("程序文件信息"));

        for(std::size_t i = 0; i < xmlvector1.size(); ++i)
        {
            std::map<std::string, std::string> xecutableMap = xmlvector1.at(i);
            std::map<std::string, std::string>::const_iterator iter = xecutableMap.find("pf");
            if(iter != xecutableMap.end())
            {
                std::string str = iter->second;
                std::size_t signNum = str.find_last_of(".");
                if(signNum != std::string::npos)
                {
                    //启动参数配置树
                    std::string processName = str.substr(0, signNum);
                    QString qProcessName = QString::fromStdString(processName);
                    QTreeWidgetItem *processItem = new QTreeWidgetItem( processParamHead, QStringList(qProcessName));
                    processParamHead->addChild( processItem );
                }

                //程序文件信息树
                QString qstr = QString::fromStdString(iter->second);
                QTreeWidgetItem *executableFileItem = new QTreeWidgetItem( executableFileHead, QStringList(qstr));
                executableFileHead->addChild( executableFileItem );
            }
        }
        treeWidget->setCurrentItem( processParamHead );
    }
}


void ConfigTab::ProHeadMessagePage()
{
    process_declareTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    process_declareTable->horizontalHeader()->setHighlightSections(false);
    process_declareTable->horizontalHeader()->setDefaultSectionSize(350);
    //process_table->resizeColumnsToContents();
    //process_table->resizeRowsToContents();
    process_declareTable->setColumnCount(1);                             //行 列数
    process_declareTable->setRowCount(8);
    process_declareTable->setShowGrid(false);
    QHeaderView* vheaderView = process_declareTable->verticalHeader();   //行头不显示
    vheaderView->setHidden(true);                                        //边框不显示
    QHeaderView* hheaderView = process_declareTable->horizontalHeader(); //设置默认列表头对齐格式
    hheaderView->setDefaultAlignment(Qt::AlignLeft);
    QStringList header;
    header << "配置名称说明";
    process_declareTable->setHorizontalHeaderLabels(header);

    process_declareTable->setItem(0, 0, new QTableWidgetItem("pf [process file]是应用程序文件，可填写相对路径或绝对路径"));
    process_declareTable->setItem(1, 0, new QTableWidgetItem("pt [process type]是应用程序类型32位中最低位表示是否核心程序，如果是核心，此程序一旦over就必须全部应用程序重新启动"));
    process_declareTable->setItem(2, 0, new QTableWidgetItem("pw [process wait]是应用程序首次启动等待多长时间再启动下一个 --{默认为启动进程后不等待"));
    process_declareTable->setItem(3, 0, new QTableWidgetItem("nt [network timeout]是网络超时，没有收到数据的超时 --{默认为5000"));
    process_declareTable->setItem(4, 0, new QTableWidgetItem("nh [network heart]是网络心跳报文 --{默认为a55aa55a"));
    process_declareTable->setItem(5, 0, new QTableWidgetItem("pp [process param]进程的参数"));
    process_declareTable->setItem(6, 0, new QTableWidgetItem("pm [process mode]进程模式 0 无窗口 1 控制台"));
    process_declareTable->setItem(7, 0, new QTableWidgetItem("np [network port]是网络监听端口号列表，监听 默认为空,不启动网络判断"));
}

void ConfigTab::ProMessagePage()
{
    //std::vector<std::map<std::string, std::string> > xmlvector1 = Processer::analyseXml(GlobalFunction::getApplicationConfigFilePath());

    process_paramTable->setItemDelegate(new NoFocusDelegate());             //虚框不显示
    process_paramTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    process_paramTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    process_paramTable->horizontalHeader()->setVisible(true);
    process_paramTable->resizeColumnsToContents();
    process_paramTable->resizeRowsToContents();

    process_paramTable->setShowGrid(false);
    QHeaderView* headerView = process_paramTable->verticalHeader();
    headerView->setHidden(true);                                          //边框不显示
    QStringList header;
    header << "参数名称" << "参数数值";
    process_paramTable->setHorizontalHeaderLabels(header);

}


void ConfigTab::showSelectedMessage(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    std::vector<std::map<std::string, std::string> > xmlvector1 = Processer::analyseXml(GlobalFunction::getApplicationConfigFilePath());
    process_paramTable->setColumnCount(2);                                   //行 列数
    process_paramTable->clear();
    QStringList header;
    header << "参数名称" << "参数数值";
    process_paramTable->setHorizontalHeaderLabels(header);
    if (!current)
        current = previous;

    std::string str;
    QTreeWidgetItem *phItem = current->parent();    //获取当前item的父item
    if(!phItem)
    {                                                //是根节点
        QString  qstr = current->text(0);
        str =  qstr.toStdString();
    }
    while (phItem)
    {
        QString  qstr = current->text(0);
        str =  qstr.toStdString();
        phItem = phItem->parent();
    }

    qCurrentItemStr = str;
    qCurrentItemStr.append(".exe");

    for(int i = 0; i < xmlvector1.size(); ++i)
    {
        std::map<std::string, std::string> xml = xmlvector1.at(i);
        if(qCurrentItemStr == xml["pf"])
        {
            processSignNum = i;
            pagesWidget->setCurrentIndex(1);
            process_paramTable->setRowCount(xml.size());
            break;
        }
        else
        {
            pagesWidget->setCurrentIndex(0);
            continue;
        }
    }

    if(processSignNum > 12)
        return;

    int i = 0;
    std::map<std::string, std::string> xml = xmlvector1.at(processSignNum);
    for(std::map<std::string, std::string>::iterator iter = xml.begin(); iter != xml.end(); ++iter)
    {
        QString strfirst = QString::fromStdString(iter->first);
        process_paramTable->setItem(i, 0, new QTableWidgetItem(strfirst));
        QString strsecond = QString::fromStdString(iter->second);
        process_paramTable->setItem(i, 1, new QTableWidgetItem(strsecond));
        std::cout << iter->first << " *** "<< iter->second << std::endl;
        ++i;
    }

}


void ConfigTab::on_customContextMenuRequested(QPoint pos)
{
    curItem = process_paramTable->itemAt(pos);

    if(curItem == NULL)
        return;
    rightClickMenu->exec(QCursor::pos());                //弹出右键菜单

}


void ConfigTab::slot_revise()
{
    QString name = process_paramTable->item(curItem->row(), 0)->text();
    QString data = process_paramTable->item(curItem->row(), 1)->text();

    ReviseDialog *revisedialog = new ReviseDialog;
    revisedialog->nameEdit->setText(name);
    revisedialog->dataEdit->setText(data);

    revisedialog->nameEdit->setEnabled(false);
    connect(revisedialog->dataEdit, SIGNAL(textChanged(const QString &)), revisedialog, SLOT(enabledOkButton(const QString &)));

    if (revisedialog->exec())
    {
        QString new_data = revisedialog->dataEdit->text();

        if (new_data != data )
        {
            int num =  curItem->row();
            std::cout << num << std::endl;

            std::string  namestr = name.toStdString();
            std::string  datastr = new_data.toStdString();
            bool modifOK;

            if(namestr != "desc")
            {
                std::string strMainAttributeName = getNodeAttribute();
                modifOK = Processer::ModifyNode_Text(GlobalFunction::getApplicationConfigFilePath(), "process", strMainAttributeName, namestr, datastr);
                if(modifOK)
                {
                    std::cout << "<<<<<<<<<<<<<<<<<<<success" << std::endl;
                }
            }
            else
            {
                //修改desc  ModifyNode_Attribute 未完成 need to do
                std::cout << "not do" << std::endl;
            }

            process_paramTable->setItem(curItem->row(), 1, new QTableWidgetItem(new_data));

            std::cout << "doTextChange" << namestr << datastr << std::endl;
        }
    }
}



void ConfigTab::slot_delete()
{   
    int iRet = QMessageBox::information( NULL, "警告", "是否删除所选内容", QMessageBox::Yes|QMessageBox::No );
    switch(iRet)
    {
    case QMessageBox::Yes:
    {
        QTableWidgetItem * item = process_paramTable->currentItem();
        if(item == NULL) return;
        QString data = process_paramTable->item(curItem->row(), 0)->text();
        std::string strNode = data.toStdString();

        std::string strMainAttributeName = getNodeAttribute();

        bool delOK = Processer::DelNode(GlobalFunction::getApplicationConfigFilePath(), "process", strMainAttributeName, strNode);
        if(delOK)
        {
            process_paramTable->removeRow(item->row());
        }
        break;
    }
    case QMessageBox::No:
        break;
    default:
        break;
    }

}


std::string ConfigTab::getNodeAttribute()
{
    std::vector<std::map<std::string, std::string> > xmlvector1 = Processer::analyseXml(GlobalFunction::getApplicationConfigFilePath());

    std::string strMainAttributeName = "";
    for(size_t i = 0; i < xmlvector1.size(); ++i)
    {
        std::map<std::string, std::string> xmlMap = xmlvector1.at(i);
        std::map<std::string, std::string>::const_iterator iter = xmlMap.find("pf");
        if(iter != xmlMap.end())
        {
            if(iter->second == qCurrentItemStr)
            {
                std::map<std::string, std::string>::const_iterator iter1 = xmlMap.find("desc");
                if(iter1 != xmlMap.end())
                {
                    strMainAttributeName = iter1->second;
                }
            }
        }
    }
    return strMainAttributeName;
}


void ConfigTab::slot_addnode()
{

    ReviseDialog *addnodedialog = new ReviseDialog;

    connect(addnodedialog->dataEdit, SIGNAL(textChanged(const QString &)), addnodedialog, SLOT(enabledOkButton(const QString &)));
    connect(addnodedialog->nameEdit, SIGNAL(textChanged(const QString &)), addnodedialog, SLOT(enabledOkButton(const QString &)));

    if (addnodedialog->exec())
    {
        QString new_data = addnodedialog->dataEdit->text();
        QString new_name = addnodedialog->nameEdit->text();

        std::string  namestr = new_name.toStdString();
        std::string  datastr = new_data.toStdString();

        if ((namestr.size() > 0) && (datastr.size() > 0))
        {
            std::string strMainAttributeName = getNodeAttribute();
            Processer::AddNode(GlobalFunction::getApplicationConfigFilePath(), "process", strMainAttributeName, namestr, datastr);
        }
    }
}

