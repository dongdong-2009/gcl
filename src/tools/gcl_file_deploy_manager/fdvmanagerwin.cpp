#include "fdvmanagerwin.h"
#include "ui_fdvmanagerwin.h"

#include <QFileInfo>
#include <QUrl>

#include <ccxx/qtcommon.h>

using namespace std;

#define fdv_win_state_none (0)
#define fdv_win_state_view (1)
#define fdv_win_state_edit (2)


void fn_fdvFill(QTableWidget * oTableWidget, int iRowIndex, const Fdv & fdv)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem* oItem;
    //md5
    oItem = oGrid->item(iRowIndex, 0);
    QtCommonWidget::setItemValue(oItem, fdv.md5);
    //文件名
    oItem = oGrid->item(iRowIndex, 1);
    QtCommonWidget::setItemValue(oItem, fdv.fileName);
    //版本
    oItem = oGrid->item(iRowIndex, 2);
    QtCommonWidget::setItemValue(oItem, fdv.version);
    //标识
    oItem = oGrid->item(iRowIndex, 3);
    QtCommonWidget::setItemValue(oItem, fdv.title);
    //模块名
    oItem = oGrid->item(iRowIndex, 4);
    QtCommonWidget::setItemValue(oItem, fdv.moduleName);
    //描述
    oItem = oGrid->item(iRowIndex, 5);
    QtCommonWidget::setItemValue(oItem, fdv.desc);
    //修改时间
    oItem = oGrid->item(iRowIndex, 6);
    QtCommonWidget::setItemValueDateTime(oItem, fdv.modifyTime);
    //修改内容
    oItem = oGrid->item(iRowIndex, 7);
    QtCommonWidget::setItemValue(oItem, fdv.modifyContent);
    //修改人
    oItem = oGrid->item(iRowIndex, 8);
    QtCommonWidget::setItemValue(oItem, fdv.modifier);
}

void fn_fdvInsert(QTableWidget * oTableWidget, const Fdv & fdv)
{
    int iRowIndex = 0;
    QTableWidget * oGrid = oTableWidget;
    oGrid->insertRow(iRowIndex);
    QTableWidgetItem* oItem;
    //md5
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 0, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.md5);
    //文件名
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 1, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.fileName);
    //版本
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 2, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.version);
    //标识
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 3, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.title);
    //模块名
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 4, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.moduleName);
    //描述
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 5, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.desc);
    //修改时间
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 6, oItem);
    QtCommonWidget::setItemValueDateTime(oItem, fdv.modifyTime);
    //修改内容
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 7, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.modifyContent);
    //修改人
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 8, oItem);
    QtCommonWidget::setItemValue(oItem, fdv.modifier);
}


void fn_fdvHeadInit(QTableWidget * oTableWidget)
{
    /*
    std::string md5;
    std::string fileName;
    std::string title;
    std::string moduleName;
    std::string desc;
    msepoch_t   modifyTime;
    std::string modifyContent;
    std::string modifier;
    */
    //量的相关数据
    QTableWidget * oGrid = oTableWidget;
    //首次刷新UI数据
    oGrid->clear();
    QStringList sHeaders;
    sHeaders.append(QString::fromUtf8("md5"));
    sHeaders.append(QString::fromUtf8("文件名"));
    sHeaders.append(QString::fromUtf8("版本"));
    sHeaders.append(QString::fromUtf8("标识"));
    sHeaders.append(QString::fromUtf8("模块名"));
    sHeaders.append(QString::fromUtf8("描述"));
    sHeaders.append(QString::fromUtf8("修改时间"));
    sHeaders.append(QString::fromUtf8("修改内容"));
    sHeaders.append(QString::fromUtf8("修改人"));
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    oGrid->setColumnWidth(0, 250);
    oGrid->setColumnWidth(1, 150);
}

void fn_fdvsRefresh(QTableWidget * oTableWidget, const vector<Fdv> & fdvs)
{
    /*
    std::string md5;
    std::string fileName;
    std::string title;
    std::string moduleName;
    std::string desc;
    msepoch_t   modifyTime;
    std::string modifyContent;
    std::string modifier;
    */
    //量的相关数据
    int iRowCount = fdvs.size();
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = 0;

    //首次刷新UI数据
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(0);
        for (size_t i = 0; i < fdvs.size(); ++i)
        {
            const Fdv & fdv = fdvs.at(i);

            fn_fdvInsert(oGrid, fdv);
        }
    }
    else
    {
        if (fdvs.empty()) return;
        for (size_t i = fdvs.size()-1; i >= 0; --i)
        {
            const Fdv & fdv = fdvs.at(i);

            fn_fdvFill(oGrid, iRowIndex, fdv);

            iRowIndex++;
        }
    }
}

FdvManagerWin::FdvManagerWin(QWidget *oParent) :
    QWidget(oParent),
    ui(new Ui::FdvManagerWin)
{
    _currentRow = -1;
    _currentState = fdv_win_state_none;

    ui->setupUi(this);

    fn_fdvHeadInit(ui->fdvsTw);

    fn_fdvsRefresh(ui->fdvsTw, FdvManager::getFdvs());

    refreshFdvsTitle();

    this->setAcceptDrops(true);
//    ui->fdvsTw->setAcceptDrops(true);

    ui->fdvsTw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fdvsTw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    _tableMenu = new QMenu(oParent);

    _test1Action = new QAction("test 1 ...", oParent);
    connect(_test1Action, SIGNAL(triggered()), this, SLOT(test1Slot()));

    _test2Action = new QAction("test 2 ...", oParent);
    connect(_test2Action, SIGNAL(triggered()), this, SLOT(test2Slot()));

    _test3Action = new QAction("test 3 ...", oParent);
    connect(_test3Action, SIGNAL(triggered()), this, SLOT(test3Slot()));

    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 2);

    ui->fdvsTw->installEventFilter(this);
    this->installEventFilter(this);
}

FdvManagerWin::~FdvManagerWin()
{
    delete ui;
}

void FdvManagerWin::dragEnterEvent(QDragEnterEvent *event)
{
//    setText(tr("<drop content>"));
//    setBackgroundRole(QPalette::Highlight);
    event->acceptProposedAction();
}
void FdvManagerWin::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}
void FdvManagerWin::dropEvent(QDropEvent *event)
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
            if(fileInfo.isFile())
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

bool FdvManagerWin::eventFilter(QObject * oObject, QEvent * oEvent)
{
    if (oEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);

        if (keyEvent->modifiers() & Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_E)
            {
                setCurrentState(fdv_win_state_edit);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_S)
            {
                if (_currentState == fdv_win_state_edit)
                {
                    string sMd5 = getMd5ByRow(_currentRow);
                    editViewIn(sMd5);
                    setCurrentState(fdv_win_state_view);
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_R)
            {
                if (_currentState==fdv_win_state_edit)
                {
                    string sMd5 = getMd5ByRow(_currentRow);
                    editViewOut(sMd5);
                    return true;
                }
            }
        }
    }

    if (oObject == ui->fdvsTw)
    {
        if (oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                deleteTableSelectRows();
                return true;
            }
        }
    }

    // pass the event on to the parent class
    return QWidget::eventFilter(oObject, oEvent);
}

void FdvManagerWin::closeEvent(QCloseEvent *)
{
    if (_currentState == fdv_win_state_edit)
    {
        string sMd5 = getMd5ByRow(_currentRow);
        editViewIn(sMd5);
        setCurrentState(fdv_win_state_view);
    }
}
void FdvManagerWin::dragLeaveEvent(QDragLeaveEvent *event)
{
//    clear();
    event->accept();
}

void FdvManagerWin::on_fdvsTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    setCurrentRow(ui->fdvsTw->currentRow());
}

void FdvManagerWin::customContextMenu(const QPoint &pos)
{
    QModelIndex posModelIndex = ui->fdvsTw->indexAt(pos);
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

void FdvManagerWin::test1Slot()
{
    for (size_t i = 0; i < 100; ++i)
    {
        Fdv fdv;
        fdv.md5 = CxString::toString(int(i));
        FdvManager::add(fdv);
    }
    fn_fdvsRefresh(ui->fdvsTw, FdvManager::getFdvs());
}

void FdvManagerWin::test2Slot()
{
    QTableWidget * oGrid = ui->fdvsTw;

    oGrid->insertRow(1);
    oGrid->insertRow(2);
}

void FdvManagerWin::test3Slot()
{

}

void FdvManagerWin::deleteTableSelectRows()
{
    QTableWidget *oGrid = ui->fdvsTw;
    QString sRows;
    vector<int> iRows = QtCommonWidget::getSelectRows(oGrid, &sRows);

    if (iRows.size()>0)
    {
        QString sMsg = QString::fromUtf8("? 确认删除 行 :\n  %1").arg(sRows);
        if (QtCommonDialog::ShowQuery(sMsg))
        {
            for (size_t i = 0; i < iRows.size(); ++i)
            {
                int iRow = iRows.at(i);
                FdvManager::removeByMd5(getMd5ByRow(iRow));
                oGrid->removeRow(iRow);
            }
            refreshFdvsTitle();
        }
    }
}

void FdvManagerWin::setCurrentRow(int iRow)
{
    if (_currentRow != iRow)
    {
        if (_currentState == fdv_win_state_edit)
        {
            string sMd5 = getMd5ByRow(_currentRow);
            editViewIn(sMd5);
        }

        string sMd5 = getMd5ByRow(iRow);
        if ( editViewOut(sMd5) )
        {
            _currentRow = iRow;
            refreshFdvsTitle();
        }
    }
}

void FdvManagerWin::setCurrentState(int iState)
{
    if (_currentState != iState)
    {
        if (iState == fdv_win_state_view)
        {
            ui->editPn2->setEnabled(false);
            ui->editBn->setEnabled(true);
            ui->saveBn->setEnabled(false);
            ui->restoreBn->setEnabled(false);
        }
        else
        {
            ui->editPn2->setEnabled(true);
            ui->md5Ed->setEnabled(false);
            ui->fileNameEd->setEnabled(false);
            ui->editBn->setEnabled(false);
            ui->saveBn->setEnabled(true);
            ui->restoreBn->setEnabled(true);
            ui->versionEd->setFocus();
        }
        _currentState = iState;
    }
}

void FdvManagerWin::addFileInfo(const std::vector<string> &sFilePaths)
{
    for (size_t i = 0; i < sFilePaths.size(); ++i)
    {
        const string & sFilePath = sFilePaths.at(i);
        CxFilePathInfo filePathInfo = CxFileSystem::getPathInfo(sFilePath);
        if (filePathInfo.fileSize > 0)
        {
            string sMd5 = CxCrc::file2md5(sFilePath);
            if (sMd5.size()>0)
            {
                if (! FdvManager::findByMd5(sMd5))
                {
                    Fdv fdv;
                    fdv.md5 = sMd5;
                    fdv.fileName = CxFileSystem::extractFileName(sFilePath);
                    fdv.modifyTime = filePathInfo.lastWriteTime;
                    if (FdvManager::add(fdv)>0)
                    {
                        fn_fdvInsert(ui->fdvsTw, fdv);
                        _currentRow++;
                    }
//                    ui->fdvsTw->insertRow();
                }
                else
                {
                    cxPrompt() << sFilePath << " 's md5 is exist!!!";
                    continue;
                }
            }
            else
            {
                cxPrompt() << sFilePath << " 's md5 is empty !!!";
                continue;
            }
        }
        else
        {
            cxPrompt() << sFilePath << " is empty or is not vaild!!!";
            continue;
        }
    }

    refreshFdvsTitle();
}

string FdvManagerWin::getMd5ByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->fdvsTw->item(iRow, 0);
    if (oItem)
    {
        string sMd5 = QtCommonWidget::getItemValue(oItem, string());
        return sMd5;
    }
    return string();
}

string FdvManagerWin::getFileNameByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->fdvsTw->item(iRow, 1);
    if (oItem)
    {
        string sFileName = QtCommonWidget::getItemValue(oItem, string());
        return sFileName;
    }
    return string();
}

Fdv *FdvManagerWin::getFdvByRow(int iRow)
{
    return FdvManager::findByMd5(getMd5ByRow(iRow));
}

void FdvManagerWin::refreshFdvsTitle()
{
    ui->fdvsTitleLb->setText(QString::fromUtf8("总数量：%1 [%2] | 当前 : %3行 [%4] %5").arg(ui->fdvsTw->rowCount()).arg(FdvManager::getFdvs().size()).arg(_currentRow).arg(ui->fdvsTw->currentRow()).arg(QtCommonString::gbkToQString(getFileNameByRow(_currentRow))));
}

int FdvManagerWin::editViewIn(const string &sMd5)
{
    Fdv * oFdv = FdvManager::findByMd5(sMd5);
    if (oFdv)
    {
        //oFdv->md5 = QtCommonString::gbkToStdString( ui->md5Ed->text() );
        oFdv->fileName = QtCommonString::gbkToStdString( ui->fileNameEd->text() );
        oFdv->version = QtCommonString::gbkToStdString( ui->versionEd->text() );
        oFdv->title = QtCommonString::gbkToStdString( ui->titleEd->text() );
        oFdv->moduleName = QtCommonString::gbkToStdString( ui->moduleNameEd->text() );
        oFdv->desc = QtCommonString::gbkToStdString( ui->descEd->toPlainText() );
        oFdv->modifyTime = ui->modifyTimeEd->dateTime().toMSecsSinceEpoch();
        oFdv->modifyContent = QtCommonString::gbkToStdString( ui->modifyContentEd->toPlainText() );
        oFdv->modifier = QtCommonString::gbkToStdString( ui->modifierEd->text() );

        fn_fdvFill(ui->fdvsTw, _currentRow, * oFdv);
        return TRUE;
    }
    return FALSE;
}

int FdvManagerWin::editViewOut(const string &sMd5)
{
    Fdv * oFdv = FdvManager::findByMd5(sMd5);
    if (oFdv)
    {
//            std::string md5;
//            std::string fileName;
//            std::string version;
//            std::string title;
//            std::string moduleName;
//            std::string desc;
//            msepoch_t   modifyTime;
//            std::string modifyContent;
//            std::string modifier;

        ui->md5Ed->setText( QtCommonString::gbkToQString( oFdv->md5 ) );
        ui->fileNameEd->setText( QtCommonString::gbkToQString( oFdv->fileName ) );
        ui->versionEd->setText( QtCommonString::gbkToQString( oFdv->version ) );
        ui->titleEd->setText( QtCommonString::gbkToQString( oFdv->title ) );
        ui->moduleNameEd->setText( QtCommonString::gbkToQString( oFdv->moduleName ) );
        ui->descEd->setPlainText( QtCommonString::gbkToQString( oFdv->desc ) );
        ui->modifyTimeEd->setDateTime( QDateTime::fromMSecsSinceEpoch(oFdv->modifyTime) );
        ui->modifyContentEd->setPlainText( QtCommonString::gbkToQString( oFdv->modifyContent ) );
        ui->modifierEd->setText( QtCommonString::gbkToQString( oFdv->modifier ) );

        setCurrentState(fdv_win_state_view);
        return TRUE;
    }
    return FALSE;
}

void FdvManagerWin::on_editBn_clicked()
{
    setCurrentState(fdv_win_state_edit);
}

void FdvManagerWin::on_saveBn_clicked()
{
    string sMd5 = getMd5ByRow(_currentRow);
    editViewIn(sMd5);
    setCurrentState(fdv_win_state_view);
}

void FdvManagerWin::on_restoreBn_clicked()
{
    if (_currentState==fdv_win_state_edit)
    {
        string sMd5 = getMd5ByRow(_currentRow);
        editViewOut(sMd5);
    }
}
