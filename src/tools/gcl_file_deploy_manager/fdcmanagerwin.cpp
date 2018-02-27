#include "fdcmanagerwin.h"
#include "ui_fdcmanagerwin.h"

#include <QFileInfo>
#include <QUrl>
#include <QProcess>

#include <ccxx/qtcommon.h>

#include "pra.h"
#include "dialog_fdc_quick_input.h"

using namespace std;

#define fdc_win_state_none (0)
#define fdc_win_state_view (1)
#define fdc_win_state_edit (2)




//*fdc--------------------------------------

void fn_fdcHeadInit(QTableWidget * oTableWidget)
{
    //量的相关数据
    QTableWidget * oGrid = oTableWidget;
    //首次刷新UI数据
    oGrid->clear();
    QStringList sHeaders;
    sHeaders.append(QString::fromUtf8("产品名称"));
    sHeaders.append(QString::fromUtf8("相对目录"));
    sHeaders.append(QString::fromUtf8("配置命令"));
    sHeaders.append(QString::fromUtf8("标识"));
    sHeaders.append(QString::fromUtf8("变量名S"));
    sHeaders.append(QString::fromUtf8("变量参数S"));
    sHeaders.append(QString::fromUtf8("修改时间"));
    sHeaders.append(QString::fromUtf8("修改内容"));
    sHeaders.append(QString::fromUtf8("修改人"));
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    oGrid->setColumnWidth(0, 100);
    oGrid->setColumnWidth(1, 150);
}

void fn_fdcFill(QTableWidget * oTableWidget, int iRowIndex, const Fdc & fdc)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem* oItem;
    //产品名称
    oItem = oGrid->item(iRowIndex, 0);
    QtCommonWidget::setItemValue(oItem, fdc.name);
    //相对目录
    oItem = oGrid->item(iRowIndex, 1);
    QtCommonWidget::setItemValue(oItem, fdc.rpath);
    //配置命令
    oItem = oGrid->item(iRowIndex, 2);
    QtCommonWidget::setItemValue(oItem, fdc.configcmd);
    //标识
    oItem = oGrid->item(iRowIndex, 3);
    QtCommonWidget::setItemValue(oItem, fdc.title);
    //模块名
    oItem = oGrid->item(iRowIndex, 4);
    QtCommonWidget::setItemValue(oItem, fdc.varName);
    //描述
    oItem = oGrid->item(iRowIndex, 5);
    QtCommonWidget::setItemValue(oItem, fdc.varParams);
    //修改时间
    oItem = oGrid->item(iRowIndex, 6);
    QtCommonWidget::setItemValueDateTime(oItem, fdc.modifyTime);
    //修改内容
    oItem = oGrid->item(iRowIndex, 7);
    QtCommonWidget::setItemValue(oItem, fdc.modifyContent);
    //修改人
    oItem = oGrid->item(iRowIndex, 8);
    QtCommonWidget::setItemValue(oItem, fdc.modifier);
}

void fn_fdcInsert(QTableWidget * oTableWidget, const Fdc & fdc)
{
    int iRowIndex = 0;
    QTableWidget * oGrid = oTableWidget;
    oGrid->insertRow(iRowIndex);
    QTableWidgetItem* oItem;
    //产品名称
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 0, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.name);
    //相对目录
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 1, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.rpath);
    //配置命令
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 2, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.configcmd);
    //标识
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 3, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.title);
    //模块名
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 4, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.varName);
    //描述
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 5, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.varParams);
    //修改时间
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 6, oItem);
    QtCommonWidget::setItemValueDateTime(oItem, fdc.modifyTime);
    //修改内容
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 7, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.modifyContent);
    //修改人
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 8, oItem);
    QtCommonWidget::setItemValue(oItem, fdc.modifier);
}





//*fdc--------------------------------------
void fn_fdcItemHeadInit(QTableWidget * oTableWidget)
{
    /*
    std::string path;
    std::string pathType;
    std::string version;
    */
    //量的相关数据
    QTableWidget * oGrid = oTableWidget;
    //首次刷新UI数据
    oGrid->clear();
    QStringList sHeaders;
    sHeaders.append(QString::fromUtf8("文件路径"));
    sHeaders.append(QString::fromUtf8("需要更新"));
    sHeaders.append(QString::fromUtf8("版本"));
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    oGrid->setColumnWidth(0, 180);
    oGrid->setColumnWidth(1, 60);
    oGrid->setColumnWidth(2, 80);
}

void fn_fdcItemFill(QTableWidget * oTableWidget, int iRowIndex, const FdcItem & fdcItem)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem* oItem;
    //相对目录
    oItem = oGrid->item(iRowIndex, 0);
    QtCommonWidget::setItemValue(oItem, fdcItem.rfilePath);
    //
    oItem = oGrid->item(iRowIndex, 1);
    QtCommonWidget::setItemValue(oItem, fdcItem.beupdate);
    //
    oItem = oGrid->item(iRowIndex, 2);
    QtCommonWidget::setItemValue(oItem, fdcItem.version);
}

void fn_fdcItemInsert(QTableWidget * oTableWidget, const FdcItem & fdcItem)
{
    int iRowIndex = 0;
    QTableWidget * oGrid = oTableWidget;
    oGrid->insertRow(iRowIndex);
    QTableWidgetItem* oItem;
    //相对目录
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 0, oItem);
    QtCommonWidget::setItemValue(oItem, fdcItem.rfilePath);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 1, oItem);
    QtCommonWidget::setItemValue(oItem, fdcItem.beupdate);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 2, oItem);
    QtCommonWidget::setItemValue(oItem, fdcItem.version);
}




void fn_fdcsRefresh(QTableWidget * oTableWidget, const vector<Fdc> & fdcs)
{
    //量的相关数据
    int iRowCount = fdcs.size();
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = 0;

    //首次刷新UI数据
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(0);
        for (size_t i = 0; i < fdcs.size(); ++i)
        {
            const Fdc & fdc = fdcs.at(i);

            fn_fdcInsert(oGrid, fdc);
        }
    }
    else
    {
        if (fdcs.empty()) return;
        for (size_t i = fdcs.size()-1; i >= 0; --i)
        {
            const Fdc & fdc = fdcs.at(i);

            fn_fdcFill(oGrid, iRowIndex, fdc);

            iRowIndex++;
        }
    }
}

FdcManagerWin::FdcManagerWin(QWidget *oParent) :
    QWidget(oParent),
    ui(new Ui::FdcManagerWin)
{
    _currentRow = -1;
    _currentState = fdc_win_state_none;

    ui->setupUi(this);

    fn_fdcHeadInit(ui->fdcsTw);
    fn_fdcItemHeadInit(ui->fdcItemsTw);
    ui->fdcsTw->setSelectionBehavior ( QAbstractItemView::SelectRows ); //设置选择行为，以行为单位
    ui->fdcsTw->setSelectionMode ( QAbstractItemView::SingleSelection ); //设置选择模式，选择单行
    fn_fdcsRefresh(ui->fdcsTw, FdcManager::getFdcs());

    refreshFdcsTitle();

    this->setAcceptDrops(true);
    ui->fdcItemsTw->setAcceptDrops(true);

    ui->fdcsTw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fdcsTw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    _tableMenu = new QMenu(oParent);

    _test1Action = new QAction("test 1 ...", oParent);
    connect(_test1Action, SIGNAL(triggered()), this, SLOT(test1Slot()));

    _test2Action = new QAction("test 2 ...", oParent);
    connect(_test2Action, SIGNAL(triggered()), this, SLOT(test2Slot()));

    _test3Action = new QAction("test 3 ...", oParent);
    connect(_test3Action, SIGNAL(triggered()), this, SLOT(test3Slot()));

    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 5);

    ui->fdcsTw->installEventFilter(this);
    ui->fdcItemsTw->installEventFilter(this);
    this->installEventFilter(this);

    if (ui->fdcsTw->rowCount()>0)
    {
        ui->fdcsTw->selectRow(0);
    }
}

FdcManagerWin::~FdcManagerWin()
{
    delete ui;
}

std::vector<string> FdcManagerWin::selectFdcs(int iMaxSelectCount)
{
    std::vector<string> r;

    QDialog mDialog;

    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelBtn;
    QPushButton *okBtn;

    if (mDialog.objectName().isEmpty())
        mDialog.setObjectName(QString::fromUtf8("SelectFdcsWin"));
    mDialog.resize(585, 405);
    verticalLayout = new QVBoxLayout(&mDialog);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    tableWidget = new QTableWidget(&mDialog);
    tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

    verticalLayout->addWidget(tableWidget);

    widget = new QWidget(&mDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    horizontalLayout = new QHBoxLayout(widget);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    cancelBtn = new QPushButton(widget);
    cancelBtn->setText(QString::fromUtf8("取消"));
    cancelBtn->setMinimumSize(QSize(100, 32));

    horizontalLayout->addWidget(cancelBtn);

    okBtn = new QPushButton(widget);
    okBtn->setText(QString::fromUtf8("选择"));
    okBtn->setMinimumSize(QSize(100, 32));

    horizontalLayout->addWidget(okBtn);

    verticalLayout->addWidget(widget);

    mDialog.connect( okBtn, SIGNAL(clicked()), &mDialog, SLOT(accept()) );
    mDialog.connect( cancelBtn, SIGNAL(clicked()), &mDialog, SLOT(reject()) );

    mDialog.setWindowTitle( QString::fromUtf8("产品选择") );
    //
    fn_fdcHeadInit(tableWidget);
    fn_fdcsRefresh(tableWidget, FdcManager::getFdcs());
    tableWidget->setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    if (iMaxSelectCount==1)
    {
        tableWidget->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    }
    if ( mDialog.exec() == QDialog::Accepted )
    {
        QItemSelectionModel *selections = tableWidget->selectionModel();
        QModelIndexList selected = selections->selectedIndexes();
        int iRow;
        foreach (QModelIndex index, selected)
        {
            iRow = index.row();
            QTableWidgetItem * oItem = tableWidget->item(iRow, 0);
            if (oItem)
            {
                string sName = QtCommonWidget::getItemValue(oItem, string());
                if (!CxContainer::contain(r, sName))
                {
                    r.push_back(sName);
                }
            }
        }
    }

    return r;
}

void FdcManagerWin::dragEnterEvent(QDragEnterEvent *event)
{
//    setText(tr("<drop content>"));
//    setBackgroundRole(QPalette::Highlight);
    event->acceptProposedAction();
}

void FdcManagerWin::dragMoveEvent(QDragMoveEvent *event)
{
    if (_currentState == fdc_win_state_edit)
        event->acceptProposedAction();
    else
        event->ignore();
}

void FdcManagerWin::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasImage()) {
//        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    } else if (mimeData->hasHtml()) {
//        setText(mimeData->html());
//        setTextFormat(Qt::RichText);
    } else if (mimeData->hasText()) {
//        setText(mimeData->text());
//        setTextFormat(Qt::PlainText);
    } else if (mimeData->hasUrls()) {
        QList<QUrl> urlList(mimeData->urls());
        QFileInfo fileInfo;
        std::vector<string> fileInfos;
        for (int i = 0; i < urlList.size(); ++i)
        {
            fileInfo.setFile(urlList.at(i).toLocalFile());
            if(fileInfo.isFile() || fileInfo.isDir())
            {
//                    && FORMAT_LIST.contains(fileInfo.suffix().toLower())){
                fileInfos.push_back(QtCommonString::gbkToStdString(fileInfo.filePath()));
            }
        }
        if (fileInfos.size()>0)
        {
            addFileInfo(fileInfos);
            event->acceptProposedAction();
        }
    } else {
//        setText(tr("Cannot display data"));
    }
    //    event->acceptProposedAction();
}

bool FdcManagerWin::eventFilter(QObject * oObject, QEvent * oEvent)
{
    if (oEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);

        if (keyEvent->modifiers() & Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_E)
            {
                setCurrentState(fdc_win_state_edit);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_S)
            {
                if (_currentState == fdc_win_state_edit)
                {
                    string sName = getNameByRow(_currentRow);
                    editViewIn(sName);
                    setCurrentState(fdc_win_state_view);
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_R)
            {
                if (_currentState==fdc_win_state_edit)
                {
                    string sName = getNameByRow(_currentRow);
                    if ( editViewOut(sName) )
                    {
                        setCurrentState(fdc_win_state_view);
                    }
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_N)
            {
                newProduct();
                return true;
            }
        }
    }

    if (oObject == ui->fdcsTw)
    {
        if (oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                deleteFdcsBySelectRows();
                return true;
            }
        }
    }

    if (oObject == ui->fdcItemsTw)
    {
        if (_currentState == fdc_win_state_edit && oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                deleteFdcItemsBySelectRows();
                return true;
            }
        }
    }

    // pass the event on to the parent class
    return QWidget::eventFilter(oObject, oEvent);
}

void FdcManagerWin::closeEvent(QCloseEvent *)
{
    if (_currentState == fdc_win_state_edit)
    {
        string sName = getNameByRow(_currentRow);
        editViewIn(sName);
        setCurrentState(fdc_win_state_view);
    }
}
void FdcManagerWin::dragLeaveEvent(QDragLeaveEvent *event)
{
//    clear();
    event->accept();
}

void FdcManagerWin::on_fdcsTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    setCurrentRow(ui->fdcsTw->currentRow());
}

void FdcManagerWin::customContextMenu(const QPoint &pos)
{
    QModelIndex posModelIndex = ui->fdcsTw->indexAt(pos);
    //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    if(! posModelIndex.isValid())
    {
        return;
    }
    _tableMenu->clear();
    _tableMenu->addAction(_test1Action);
    _tableMenu->addAction(_test2Action);
    _tableMenu->addAction(_test3Action);
    _tableMenu->exec(QCursor::pos());
}

void FdcManagerWin::test1Slot()
{
    for (size_t i = 0; i < 100; ++i)
    {
        Fdc fdc;
        fdc.name = CxString::toString(int(i));
        FdcManager::add(fdc);
    }
    fn_fdcsRefresh(ui->fdcsTw, FdcManager::getFdcs());
}

void FdcManagerWin::test2Slot()
{
    QTableWidget * oGrid = ui->fdcsTw;

    oGrid->insertRow(1);
    oGrid->insertRow(2);
}

void FdcManagerWin::test3Slot()
{

}

void FdcManagerWin::deleteFdcsBySelectRows()
{
    QTableWidget *oGrid = ui->fdcsTw;
    QString sRows;
    vector<int> iRows = QtCommonWidget::getSelectRows(oGrid, &sRows);

    if (iRows.size()>0)
    {
        QString sMsg = QString::fromUtf8("? 确认删除 行 :\n  %1").arg(sRows);
        if (QtCommonDialog::ShowQuery(sMsg))
        {
            Pra * oPra = NULL;
            string sFdc;
            for (size_t i = 0; i < iRows.size(); ++i)
            {
                int iRow = iRows.at(i);
                sFdc = getNameByRow(iRow);
                oPra = PraManager::findByFdc(sFdc);
                if (oPra) break;
            }
            if (! oPra)
            {
                for (size_t i = 0; i < iRows.size(); ++i)
                {
                    int iRow = iRows.at(i);
                    FdcManager::removeByName(getNameByRow(iRow));
                    oGrid->removeRow(iRow);
                }
                if (oGrid->rowCount()<=0)
                {
                    _currentRow = -1;
                    editViewOut("");
                }
                refreshFdcsTitle();
            }
            else
            {
                QString sPraName = QtCommonString::gbkToQString(oPra->name);
                QString sFdcName = QtCommonString::gbkToQString(sFdc);
                QtCommonDialog::ShowWarning(QString::fromUtf8("不好意思地告诉您：产品 %1 已经在区域 %2 内使用，请从区域中删除产品后再删除此产品。\n\n给你带来不便之处请谅解，谢谢！").arg(sPraName).arg(sFdcName));
            }
        }
    }
}

void FdcManagerWin::deleteFdcItemsBySelectRows()
{
    QtCommonWidget::deleteSelectRows(ui->fdcItemsTw);
}

void FdcManagerWin::setCurrentRow(int iRow)
{
    if (_currentRow != iRow)
    {
        if (_currentState == fdc_win_state_edit)
        {
            string sName = getNameByRow(_currentRow);
            editViewIn(sName);
        }

        string sName = getNameByRow(iRow);
        if ( editViewOut(sName) )
        {
            _currentRow = iRow;
            setCurrentState(fdc_win_state_view);
            refreshFdcsTitle();
        }
    }
}

void FdcManagerWin::setCurrentState(int iState)
{
    if (_currentState != iState)
    {
        if (iState == fdc_win_state_view)
        {
            ui->editPn2->setEnabled(false);
            ui->newBn->setEnabled(true);
            ui->editBn->setEnabled(_currentRow>=0);
            ui->saveBn->setEnabled(false);
            ui->restoreBn->setEnabled(false);
            ui->quickBn->setEnabled(false);

            ui->fdcItemsTw->setSelectionBehavior ( QAbstractItemView::SelectRows ); //设置选择行为，以行为单位
//            ui->fdcsTw->setSelectionMode ( QAbstractItemView::SingleSelection ); //设置选择模式，选择单行
//            ui->fdcItemsTw->setEnabled(false);
        }
        else
        {
            ui->editPn2->setEnabled(true);
            ui->nameEd->setEnabled(false);
            ui->fileNameEd->setEnabled(false);
            ui->newBn->setEnabled(false);
            ui->editBn->setEnabled(false);
            ui->saveBn->setEnabled(true);
            ui->restoreBn->setEnabled(true);
            ui->quickBn->setEnabled(true);
//            ui->fdcItemsTw->setEnabled(true);
            ui->fdcItemsTw->setSelectionBehavior ( QAbstractItemView::SelectItems ); //设置选择行为，以行为单位
            ui->configCmdEd->setFocus();
        }
        _currentState = iState;
    }
}

void FdcManagerWin::newProduct()
{
    if (_currentState==fdc_win_state_none || _currentState==fdc_win_state_view)
    {
        QString sRootDir = QtCommonString::gbkToQString(FdcManager::getProductRootPath());
        QString sDir = QtCommonDialog::DialogDir(sRootDir, QString::fromUtf8("请选择产品所在的文件夹："));
        if (sDir.size()>0 && sDir != sRootDir)
        {
            addProduct(QtCommonString::gbkToStdString(sDir));
        }
    }
}

void FdcManagerWin::addProduct(const string &sPath)
{
    CxFilePathInfo filePathInfo = CxFileSystem::getPathInfo(sPath);
    if (filePathInfo.pathType == CxFileSystem::PathTypeDir)
    {
        string sName = CxFileSystem::extractFileName(sPath);
        bool bOk;
        string sRpath = CxFileSystem::relativeFilePath(FdcManager::getProductRootPath(), sPath, &bOk);
        if (sName.size()>0 && bOk && sRpath.size()>0)
        {
            QString sProductName = QtCommonString::gbkToQString(sName);
            if (QtCommonDialog::DialogInput(QString::fromUtf8("请输入产品名称："), sProductName))
            {
                sName = QtCommonString::gbkToStdString(sProductName);
                if (sName.size()>0)
                {
                    if (! FdcManager::findByName(sName))
                    {
                        Fdc fdc;
                        fdc.name = sName;
                        fdc.rpath = sRpath;
                        fdc.modifyTime = filePathInfo.lastWriteTime;
                        if (FdcManager::add(fdc)>0)
                        {
                            fn_fdcInsert(ui->fdcsTw, fdc);
                            _currentRow++;
                            ui->fdcsTw->setCurrentCell(0, 0);
                            setCurrentState(fdc_win_state_edit);
                            refreshFdcsTitle();
                            QProcess::execute(QString::fromUtf8("Explorer /root,%1").arg(QtCommonString::gbkToQString(sPath)));
                            if (_currentRow==0)
                            {
                                editViewOut(sName);
                            }
                        }
                    }
                    else
                    {
                        cxPrompt() << sPath << " 's name is exist!!!";
                    }
                }
                else
                {
                    cxPrompt() << sPath << " 's name length must greater than 0!!!";
                }
            }
        }
        else
        {
            cxPrompt() << sPath << CxString::format(" 's name is empty or rpath don't exist rootpath[%s]!!!", FdcManager::getProductRootPath().c_str());
        }
    }
    else
    {
        cxPrompt() << sPath << " is empty or is not vaild directory!!!";
    }
}

void FdcManagerWin::addFileInfo(const std::vector<string> &sPaths)
{
    Fdc * oFdc = getFdcByRow(_currentRow);
    if (! oFdc) return;
    string sFdcPath = CxFileSystem::mergeFilePath(FdcManager::getProductRootPath(), oFdc->rpath);
    for (size_t i = 0; i < sPaths.size(); ++i)
    {
        const string & sPath = sPaths.at(i);
        CxFilePathInfo filePathInfo = CxFileSystem::getPathInfo(sPath);
        if (filePathInfo.pathType == CxFileSystem::PathTypeFile || filePathInfo.pathType == CxFileSystem::PathTypeDir)
        {
            bool bOk;
            string sRfilePath = CxFileSystem::relativeFilePath(sFdcPath, sPath, &bOk);
            if (sRfilePath.size()>0 && bOk)
            {
                if (rowByfdcItemPath(sPath)<0)
                {
                    FdcItem fdcItem;
                    fdcItem.rfilePath = sRfilePath;
                    fdcItem.beupdate = FdcManager::isBeupdate(sRfilePath);
                    fdcItem.version = "";
                    fn_fdcItemInsert(ui->fdcItemsTw, fdcItem);
                }
                else
                {
                    cxPrompt() << sPath << " 's name is exist!!!";
                    continue;
                }
            }
            else
            {
                cxPrompt() << sPath << CxString::format("rpath don't exist fdcPath[%s]!!!", sFdcPath.c_str());
                continue;
            }
        }
        else
        {
            cxPrompt() << sPath << " is empty or is not vaild!!!";
            continue;
        }
    }
}

string FdcManagerWin::getNameByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->fdcsTw->item(iRow, 0);
    if (oItem)
    {
        string sName = QtCommonWidget::getItemValue(oItem, string());
        return sName;
    }
    return string();
}

string FdcManagerWin::getRpathByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->fdcsTw->item(iRow, 1);
    if (oItem)
    {
        string sFileName = QtCommonWidget::getItemValue(oItem, string());
        return sFileName;
    }
    return string();
}

Fdc *FdcManagerWin::getFdcByRow(int iRow)
{
    return FdcManager::findByName(getNameByRow(iRow));
}

void FdcManagerWin::refreshFdcsTitle()
{
    ui->fdcsTitleLb->setText(QString::fromUtf8("总数量：%1 [%2] | 当前 : %3行 [%4] %5").arg(ui->fdcsTw->rowCount()).arg(FdcManager::getFdcs().size()).arg(_currentRow).arg(ui->fdcsTw->currentRow()).arg(QtCommonString::gbkToQString(getNameByRow(_currentRow))));
}

int FdcManagerWin::editViewIn(const string &sName)
{
    Fdc * oFdc = FdcManager::findByName(sName);
    if (oFdc)
    {
        //oFdc->name = QtCommonString::gbkToStdString( ui->nameEd->text() );
        //oFdc->rpath = QtCommonString::gbkToStdString( ui->fileNameEd->text() );
        oFdc->configcmd = QtCommonString::gbkToStdString( ui->configCmdEd->text() );
        oFdc->title = QtCommonString::gbkToStdString( ui->titleEd->text() );
        oFdc->varName = QtCommonString::gbkToStdString( ui->varNameEd->text() );
        oFdc->varParams = QtCommonString::gbkToStdString( ui->descEd->toPlainText() );
        oFdc->modifyTime = ui->modifyTimeEd->dateTime().toMSecsSinceEpoch();
        oFdc->modifyContent = QtCommonString::gbkToStdString( ui->modifyContentEd->toPlainText() );
        oFdc->modifier = QtCommonString::gbkToStdString( ui->modifierEd->text() );

        QTableWidget * oGrid = ui->fdcItemsTw;
        std::vector<FdcItem> & fdcItems = oFdc->items;
        fdcItems.clear();
        QTableWidgetItem * oItem;
        for (int i = 0; i < oGrid->rowCount(); ++i)
        {
            FdcItem fdcItem;
            fdcItem.fdc = oFdc->name;
            //path
            oItem = oGrid->item(i, 0);
            fdcItem.rfilePath = QtCommonWidget::getItemValue(oItem, string());
            //pathType
            oItem = oGrid->item(i, 1);
            fdcItem.beupdate = QtCommonWidget::getItemValue(oItem, GM_INT_MAX);
            oItem = oGrid->item(i, 2);
            fdcItem.version = QtCommonWidget::getItemValue(oItem, string());
            fdcItems.push_back(fdcItem);
        }

        fn_fdcFill(ui->fdcsTw, _currentRow, * oFdc);
        return TRUE;
    }
    return FALSE;
}

int FdcManagerWin::editViewOut(const string &sName)
{
    Fdc * oFdc = FdcManager::findByName(sName);
    if (oFdc)
    {
//            std::string name;
//            std::string fileName;
//            std::string version;
//            std::string title;
//            std::string moduleName;
//            std::string desc;
//            msepoch_t   modifyTime;
//            std::string modifyContent;
//            std::string modifier;

        ui->nameEd->setText( QtCommonString::gbkToQString( oFdc->name ) );
        ui->fileNameEd->setText( QtCommonString::gbkToQString( oFdc->rpath ) );
        ui->configCmdEd->setText( QtCommonString::gbkToQString( oFdc->configcmd ) );
        ui->titleEd->setText( QtCommonString::gbkToQString( oFdc->title ) );
        ui->varNameEd->setText( QtCommonString::gbkToQString( oFdc->varName ) );
        ui->descEd->setPlainText( QtCommonString::gbkToQString( oFdc->varParams ) );
        ui->modifyTimeEd->setDateTime( QDateTime::fromMSecsSinceEpoch(oFdc->modifyTime) );
        ui->modifyContentEd->setPlainText( QtCommonString::gbkToQString( oFdc->modifyContent ) );
        ui->modifierEd->setText( QtCommonString::gbkToQString( oFdc->modifier ) );

        QTableWidget * oGrid = ui->fdcItemsTw;
        oGrid->setRowCount(0);
        std::vector<FdcItem> & fdcItems = oFdc->items;
        for (size_t i = 0; i < fdcItems.size(); ++i)
        {
            const FdcItem & fdcItem = fdcItems.at(i);
            fn_fdcItemInsert(oGrid, fdcItem);
        }
        return TRUE;
    }
    else
    {
        ui->nameEd->setText( QString() );
        ui->fileNameEd->setText( QString() );
        ui->configCmdEd->setText( QString() );
        ui->titleEd->setText( QString() );
        ui->varNameEd->setText( QString() );
        ui->descEd->setPlainText( QString() );
//        ui->modifyTimeEd->setDateTime( QDateTime::fromMSecsSinceEpoch(oFdc->modifyTime) );
        ui->modifyContentEd->setPlainText( QString() );
        ui->modifierEd->setText( QString() );

        QTableWidget * oGrid = ui->fdcItemsTw;
        oGrid->setRowCount(0);
    }
    return FALSE;
}

int FdcManagerWin::rowByfdcItemPath(const string &sPath)
{
    QTableWidget * oGrid = ui->fdcItemsTw;
    QTableWidgetItem * oItem;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        oItem = oGrid->item(i, 0);
        if (QtCommonWidget::getItemValue(oItem, string()) == sPath)
        {
            return i;
        }
    }
    return -1;
}

void FdcManagerWin::on_newBn_clicked()
{
    newProduct();
}

void FdcManagerWin::on_editBn_clicked()
{
    setCurrentState(fdc_win_state_edit);
}

void FdcManagerWin::on_saveBn_clicked()
{
    string sName = getNameByRow(_currentRow);
    editViewIn(sName);
    setCurrentState(fdc_win_state_view);
}

void FdcManagerWin::on_restoreBn_clicked()
{
    if (_currentState==fdc_win_state_edit)
    {
        string sName = getNameByRow(_currentRow);
        if ( editViewOut(sName) )
        {
            setCurrentState(fdc_win_state_view);
        }
    }
}


void FdcManagerWin::on_quickBn_clicked()
{
    if (_currentState==fdc_win_state_edit)
    {
        std::string sVdiConfigFilePath;
        std::string sDescribeProjectFileName = DialogFdcQuickInput::getFdcQuickInput(getNameByRow(_currentRow), sVdiConfigFilePath);
        if (sDescribeProjectFileName.size()>0)
        {
#ifdef GM_OS_WIN
            string sConfigCmd = "./bin_qt/gcl_gcl_config_tool.exe -pi SettingPlugin=;";
#else
            string sConfigCmd = "./bin_qt/gcl_gcl_config_tool -pi SettingPlugin=;";
#endif
            if (sVdiConfigFilePath.size()>0)
            {
                sConfigCmd += "TerminalPlugin=;";
            }
            sConfigCmd += CxString::format(" -pf %s", sDescribeProjectFileName.c_str());
            string sVarName;
            if (sVdiConfigFilePath.size()>0)
            {
                sConfigCmd += CxString::format(" -tf %s", sVdiConfigFilePath.c_str());
                sVarName = CxString::format("terminal=%s", sVdiConfigFilePath.c_str());
            }
            ui->configCmdEd->setText(QtCommonString::gbkToQString(sConfigCmd));
            ui->varNameEd->setText(QtCommonString::gbkToQString(sVarName));
        }
    }
}
