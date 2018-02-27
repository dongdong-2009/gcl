#include "pramanagerwin.h"
#include "ui_pramanagerwin.h"

#include "fdcmanagerwin.h"

#include "praui.h"

using namespace std;

PraManagerWin::PraManagerWin(QWidget *oParent) :
    QWidget(oParent),
    ui(new Ui::PraManagerWin)
{
    _currentRow = -1;
    _currentState = pra_win_state_none;

    ui->setupUi(this);

    std::vector<std::string> sLocalIps = CxNetwork::getLocalIps();
    for (size_t i = 0; i < sLocalIps.size(); ++i)
    {
        string sIp = sLocalIps.at(i);
        if (CxString::isValidIp(sIp))
        {
            if (_defaultIp.empty()) _defaultIp = sIp;
            ui->localIpEd->addItem(QtCommonString::gbkToQString(sIp));
        }
    }

    PraUi::praHeadInit(ui->prasTw);
    PraUi::praItemHeadInit(ui->praItemsTw);
    PraUi::prasRefresh(ui->prasTw, PraManager::getPras());
    ui->prasTw->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->prasTw->setSelectionMode ( QAbstractItemView::SingleSelection);

    refreshPrasTitle();

    this->setAcceptDrops(true);
    ui->praItemsTw->setAcceptDrops(true);

    ui->prasTw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->prasTw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    _tableMenu = new QMenu(oParent);

    _test1Action = new QAction("test 1 ...", oParent);
    connect(_test1Action, SIGNAL(triggered()), this, SLOT(test1Slot()));

    _test2Action = new QAction("test 2 ...", oParent);
    connect(_test2Action, SIGNAL(triggered()), this, SLOT(test2Slot()));

    _test3Action = new QAction("test 3 ...", oParent);
    connect(_test3Action, SIGNAL(triggered()), this, SLOT(test3Slot()));

    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 5);

    ui->prasTw->installEventFilter(this);
    ui->praItemsTw->installEventFilter(this);
    this->installEventFilter(this);
}

PraManagerWin::~PraManagerWin()
{
    delete ui;
}

bool PraManagerWin::eventFilter(QObject * oObject, QEvent * oEvent)
{
    if (oEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);

        if (keyEvent->modifiers() & Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_E)
            {
                setCurrentState(pra_win_state_edit);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_S)
            {
                if (_currentState == pra_win_state_edit)
                {
                    string sMd5 = getAreaNameByRow(_currentRow);
                    editViewIn(sMd5);
                    setCurrentState(pra_win_state_view);
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_R)
            {
                if (_currentState==pra_win_state_edit)
                {
                    string sName = getAreaNameByRow(_currentRow);
                    if ( editViewOut(sName) )
                    {
                        setCurrentState(pra_win_state_view);
                    }
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_N)
            {
                newArea();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_I)
            {
                addFdcsToCurrentProduct();
                return true;
            }
        }
    }

    if (oObject == ui->prasTw)
    {
        if (oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                deletePrasBySelectRows();
                return true;
            }
        }
    }

    if (oObject == ui->praItemsTw)
    {
        if (oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                deletePraItemsBySelectRows();
                return true;
            }
        }
    }

    // pass the event on to the parent class
    return QWidget::eventFilter(oObject, oEvent);
}

void PraManagerWin::closeEvent(QCloseEvent *)
{
    if (_currentState == pra_win_state_edit)
    {
        string sMd5 = getAreaNameByRow(_currentRow);
        editViewIn(sMd5);
        setCurrentState(pra_win_state_view);
    }
}

void PraManagerWin::on_prasTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    setCurrentRow(ui->prasTw->currentRow());
}

void PraManagerWin::customContextMenu(const QPoint &pos)
{
    QModelIndex posModelIndex = ui->prasTw->indexAt(pos);
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

void PraManagerWin::test1Slot()
{
    for (size_t i = 0; i < 100; ++i)
    {
        Pra pra;
        pra.name = CxString::toString(int(i));
        PraManager::add(pra);
    }
    PraUi::prasRefresh(ui->prasTw, PraManager::getPras());
}

void PraManagerWin::test2Slot()
{
    QTableWidget * oGrid = ui->prasTw;

    oGrid->insertRow(1);
    oGrid->insertRow(2);
}

void PraManagerWin::test3Slot()
{

}

void PraManagerWin::deletePrasBySelectRows()
{
    QTableWidget *oGrid = ui->prasTw;
    QString sRows;
    vector<int> iRows = QtCommonWidget::getSelectRows(oGrid, &sRows);

    if (iRows.size()>0)
    {
        QString sMsg = QString::fromUtf8("? 确认删除选择区域 行 :\n  %1").arg(sRows);
        if (QtCommonDialog::ShowQuery(sMsg))
        {
            for (size_t i = 0; i < iRows.size(); ++i)
            {
                int iRow = iRows.at(i);
                PraManager::removeByName(getAreaNameByRow(iRow));
                oGrid->removeRow(iRow);
            }
            if (oGrid->rowCount()<=0)
            {
                _currentRow = -1;
                editViewOut("");
            }
            refreshPrasTitle();
        }
    }
}

void PraManagerWin::deletePraItemsBySelectRows()
{
//    Pra * oPra = getPraByRow(_currentRow);
//    if (oPra)
//    {
//        QTableWidget *oGrid = ui->praItemsTw;
//        QString sRows;
//        vector<int> iRows = QtCommonWidget::getSelectRows(oGrid, &sRows);

//        if (iRows.size()>0)
//        {
//            QString sMsg = QString::fromUtf8("? 确认删除区域内产品 行 :\n  %1").arg(sRows);
//            if (QtCommonDialog::ShowQuery(sMsg))
//            {
//                for (size_t i = 0; i < iRows.size(); ++i)
//                {
//                    int iRow = iRows.at(i);

////                    string sPath = getFdcAreaPath(oPra, getFdcNameByRow(iRow));

//                    oGrid->removeRow(iRow);
//                }
//                if (oGrid->rowCount()<=0)
//                {
//                    _currentRow = -1;
//                    editViewOut("");
//                }
//                refreshPrasTitle();
//            }
//        }
//    }
    QtCommonWidget::deleteSelectRows(ui->praItemsTw);
}

void PraManagerWin::setCurrentRow(int iRow)
{
    if (_currentRow != iRow)
    {
        if (_currentState == pra_win_state_edit)
        {
            string sName = getAreaNameByRow(_currentRow);
            editViewIn(sName);
        }

        string sName = getAreaNameByRow(iRow);
        if ( editViewOut(sName) )
        {
            _currentRow = iRow;
            setCurrentState(pra_win_state_view);
            refreshPrasTitle();
        }
    }
}

void PraManagerWin::setCurrentState(int iState)
{
    if (_currentState != iState)
    {
        ui->localPortEd->setEnabled(false);
        if (iState == pra_win_state_view)
        {
            ui->editPn2->setEnabled(false);
            ui->newBn->setEnabled(true);
            ui->editBn->setEnabled(_currentRow>=0);
            ui->saveBn->setEnabled(false);
            ui->restoreBn->setEnabled(false);
            ui->praItemsTw->setEnabled(false);
        }
        else
        {
            ui->editPn2->setEnabled(true);
            ui->md5Ed->setEnabled(false);
            ui->fileNameEd->setEnabled(false);
            ui->newBn->setEnabled(false);
            ui->editBn->setEnabled(false);
            ui->saveBn->setEnabled(true);
            ui->restoreBn->setEnabled(true);
            ui->praItemsTw->setEnabled(true);
            ui->versionEd->setFocus();
        }
        _currentState = iState;
    }
}

void PraManagerWin::newArea()
{
    if (_currentState==pra_win_state_none || _currentState==pra_win_state_view)
    {
        string sAreaName;
        QString qsAreaName;
        if (QtCommonDialog::DialogInput(QString::fromUtf8("请输入区域英文名（作为区域文件夹名用），不能用空格与中文："), qsAreaName))
        {
            sAreaName = QtCommonString::gbkToStdString(qsAreaName);
            if (sAreaName.empty() || sAreaName.size() > 64 || ! CxString::isValidCodeName(sAreaName) )
            {
                QtCommonDialog::ShowPrompt(QString::fromUtf8("请输入区域英文名（作为区域文件夹名用），不能用空格与中文："));
                sAreaName.clear();
            }
        }
        string sDir = CxFileSystem::mergeFilePath(PraManager::getProjectRootPath(), sAreaName);
        if (! CxFileSystem::isExist(sDir))
        {
            CxFileSystem::createDir(sDir);
            const std::vector<string> & sDirs = FdcManager::getProductLevel1Paths();
            for (size_t i = 0; i < sDirs.size(); ++i)
            {
                CxFileSystem::createDir(CxFileSystem::mergeFilePath(sDir, sDirs.at(i)));
            }
        }
        if (! CxFileSystem::isExist(sDir))
        {
            QtCommonDialog::ShowPrompt(QString::fromUtf8("区域文件夹创建失败！！！"));
            sAreaName.clear();
        }
        if (sAreaName.size()>0)
        {
            addArea(sAreaName);
        }
    }
}

void PraManagerWin::addArea(const string &sAreaName)
{
    if (sAreaName.size()>0 && ! PraManager::findByName(sAreaName))
    {
        Pra pra;
        pra.name = sAreaName;
        pra.rpath = sAreaName;
        pra.ip_r = string();
        pra.port_r = 7987;
        pra.ip_l = _defaultIp;
        pra.port_l = 7921;
        if (PraManager::add(pra)>0)
        {
            PraUi::praInsert(ui->prasTw, pra);
            _currentRow++;
            ui->prasTw->setCurrentCell(0, 0);
            setCurrentState(pra_win_state_edit);
            refreshPrasTitle();
            if (_currentRow==0)
            {
                editViewOut(sAreaName);
            }
        }
    }
    else
    {
        cxPrompt() << sAreaName << " 's name is exist!!!";
    }
}

void PraManagerWin::addFdcsToCurrentProduct()
{
    Pra * oPra = getPraByRow(_currentRow);
    if (_currentState==pra_win_state_edit && oPra)
    {
        vector<string> fdcs = FdcManagerWin::selectFdcs(1);
        if (fdcs.size()>0)
        {
            QTableWidget * oGrid = ui->praItemsTw;
            std::vector<PraItem> praItemsWin = getPraItems(oGrid);
            bool bHas;
            for (size_t j = 0; j < fdcs.size(); ++j)
            {
                const string & sFdc = fdcs.at(j);
                bHas = false;
                for (size_t i = 0; i < praItemsWin.size(); ++i)
                {
                    const PraItem & praItem = praItemsWin.at(i);
                    if (praItem.fdc == sFdc)
                    {
                        bHas = true;
                        break;
                    }
                }
                if (! bHas)
                {
                    if (addFdcToProduct(sFdc, oPra))
                    {
                        PraItem praItem;
                        praItem.fdc = sFdc;
                        PraUi::praItemInsert(ui->praItemsTw, praItem);
                        cxPrompt() << "add product success.";
                    }
                }
            }
        }
    }
}

bool PraManagerWin::addFdcToProduct(const string &sFdc, const Pra *oPra)
{
    GM_INVALID_RETURE_(oPra, false);
    bool r = false;
    Fdc * oFdc = FdcManager::findByName(sFdc);
    if (oFdc)
    {
        string sProductPath = oPra->getProductPath(oFdc);
        if (CxString::isValidFullPath(sProductPath))
        {
            CxFileSystem::createDirMultiLevel(sProductPath);
            CxFilePathInfo filePathInfo = CxFileSystem::getPathInfo(sProductPath);
            if (filePathInfo.pathType == CxFileSystem::PathTypeDir)
            {
                bool bOk;
                string sRpath = CxFileSystem::relativeFilePath(PraManager::getProjectRootPath(), sProductPath, &bOk);
                if (bOk && sRpath.size()>0)
                {
                    this->setEnabled(false);
                    const vector<FdcItem> & fdcItems = oFdc->items;
                    cxPrompt() << "add product begin copy files to '"  << sProductPath << "' . fileCount=" << fdcItems.size();

                    bool bCancel = false;
                    QProgressDialog progressDlg(this)/* = new QProgressDialog(this)*/;
                    QFont font("ZYSong18030",12);
                    progressDlg.setFont(font);
                    progressDlg.setWindowModality(Qt::WindowModal);
                    progressDlg.setMinimumDuration(5);
                    progressDlg.setWindowTitle(QString::fromUtf8("please wait"));
                    progressDlg.setLabelText(QString::fromUtf8("正在复制......      "));
                    progressDlg.setCancelButtonText(QString::fromUtf8("取消"));
                    progressDlg.setRange(0,fdcItems.size());
                    progressDlg.setMinimumWidth(this->width() / 3);

                    int iFailCount = 0;
                    for (size_t i = 0; i < fdcItems.size(); ++i)
                    {
                        const FdcItem & fdcItem = fdcItems.at(i);
                        //产品文件路径
                        string sFilePath = oFdc->getProductFilepath(fdcItem);
                        //区域文件路径
                        string sToFilePath = oPra->getProductFilepath(oFdc, fdcItem);
                        int iSourceFileSize;
                        CxFileSystem::createDirMultiLevel(CxFileSystem::parentPath(sToFilePath));
                        if (fdcItem.beupdate || ! CxFileSystem::isExist(sToFilePath))
                        {
                            if (! CxCrc::isSameMd5FileData(sFilePath, sToFilePath))
                            {
                                int iCopySize = CxFileSystem::copyFile(sFilePath, sToFilePath, true, &iSourceFileSize);
                                string sResult = iCopySize == iSourceFileSize ? "success" : "fail";
                                cxPrompt() << "add product copy " << sResult << " from '" << sFilePath << "' [size=" << iSourceFileSize << "] to '" << sToFilePath << "' , has copySize=" << iCopySize;
                                if (iSourceFileSize != iCopySize)
                                {
                                    ++iFailCount;
                                }
                            }
                            else
                            {
                                cxPrompt() << "add product copy ignore. from '" << sFilePath << "' to '" << sToFilePath << "' , ignore for same file data!!!";
                            }
                        }
                        else
                        {
                            cxPrompt() << "add product copy ignore. from '" << sFilePath << "' to '" << sToFilePath << "' , ignore for is beupdate file!!!";
                        }

                        progressDlg.setValue(i);
                        if(progressDlg.wasCanceled())
                        {
                            bCancel = true;
                            break;
                        }
                    }
                    cxPrompt() << "add product end. failCount" << iFailCount;

                    if (iFailCount==0 && ! bCancel)
                    {
                        r = true;
                    }
                    this->setEnabled(true);
                }
                else
                {
                    cxPrompt() << "add product to area fail. '" << sProductPath << CxString::format("' 's name is empty or rpath don't exist rootpath[%s]!!!", FdcManager::getProductRootPath().c_str());
                }
            }
            else
            {
                cxPrompt() << "add product to area fail. '" << sProductPath << "' is empty or is not vaild directory!!!";
            }
        }
        else
        {
            cxPrompt() << "add product to area fail. '" << sProductPath << "' is empty or is not vaild directory!!!";
        }
    }
    else
    {
        cxPrompt() << "add product to area fail. product can not find!!!";
    }
    return r;
}

void PraManagerWin::addFileInfo(const std::vector<string> &sPaths)
{
    Pra * oPra = getPraByRow(_currentRow);
    if (! oPra) return;
    string sPraPath = CxFileSystem::mergeFilePath(PraManager::getProjectRootPath(), oPra->rpath);
    for (size_t i = 0; i < sPaths.size(); ++i)
    {
        const string & sPath = sPaths.at(i);
        CxFilePathInfo filePathInfo = CxFileSystem::getPathInfo(sPath);
        if (filePathInfo.pathType == CxFileSystem::PathTypeFile || filePathInfo.pathType == CxFileSystem::PathTypeDir)
        {
            bool bOk;
            string sRpath = CxFileSystem::relativeFilePath(sPraPath, sPath, &bOk);
            if (sRpath.size()>0 && bOk)
            {
                if (rowByPraItemPath(sPath)<0)
                {
                    PraItem praItem;
                    praItem.res1 = sRpath;
                    praItem.desc = "";
                    PraUi::praItemInsert(ui->praItemsTw, praItem);
                }
                else
                {
                    cxPrompt() << sPath << " 's name is exist!!!";
                    continue;
                }
            }
            else
            {
                cxPrompt() << sPath << CxString::format("rpath don't exist praPath[%s]!!!", sPraPath.c_str());
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

string PraManagerWin::getAreaNameByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->prasTw->item(iRow, 0);
    if (oItem)
    {
        string sName = QtCommonWidget::getItemValue(oItem, string());
        return sName;
    }
    return string();
}

string PraManagerWin::getAreaRpathByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->prasTw->item(iRow, 1);
    if (oItem)
    {
        string sFileName = QtCommonWidget::getItemValue(oItem, string());
        return sFileName;
    }
    return string();
}

Pra *PraManagerWin::getPraByRow(int iRow)
{
    return PraManager::findByName(getAreaNameByRow(iRow));
}

string PraManagerWin::getFdcNameByRow(int iRow)
{
    QTableWidgetItem * oItem = ui->praItemsTw->item(iRow, 0);
    if (oItem)
    {
        string sName = QtCommonWidget::getItemValue(oItem, string());
        return sName;
    }
    return string();
}

std::vector<PraItem> PraManagerWin::getPraItems(QTableWidget *oGrid)
{
    std::vector<PraItem> r;
    QTableWidgetItem * oItem;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        PraItem praItem;
//        praItem.pra = oPra->name;
        //fdc
        oItem = oGrid->item(i, 0);
        praItem.fdc = QtCommonWidget::getItemValue(oItem, string());
        //path
        oItem = oGrid->item(i, 1);
        praItem.res1 = QtCommonWidget::getItemValue(oItem, string());
        //version
        oItem = oGrid->item(i, 2);
        praItem.desc = QtCommonWidget::getItemValue(oItem, string());
        if (FdcManager::findByName(praItem.fdc))
        {
            r.push_back(praItem);
        }
        else
        {
            cxWarning() << "PraManagerWin::editViewIn fdc=" << praItem.fdc << "don't exist!!!";
            continue;
        }
    }
    return r;
}

void PraManagerWin::refreshPrasTitle()
{
    ui->prasTitleLb->setText(QString::fromUtf8("总数量：%1 [%2] | 当前 : %3行 [%4] %5").arg(ui->prasTw->rowCount()).arg(PraManager::getPras().size()).arg(_currentRow).arg(ui->prasTw->currentRow()).arg(QtCommonString::gbkToQString(getAreaNameByRow(_currentRow))));
}

int PraManagerWin::editViewIn(const string &sName)
{
    Pra * oPra = PraManager::findByName(sName);
    if (oPra)
    {
        //oPra->md5 = QtCommonString::gbkToStdString( ui->md5Ed->text() );
        oPra->rpath = QtCommonString::gbkToStdString( ui->fileNameEd->text() );
        oPra->res1 = QtCommonString::gbkToStdString( ui->versionEd->text() );
        oPra->title = QtCommonString::gbkToStdString( ui->titleEd->text() );
        oPra->ip_r = QtCommonString::gbkToStdString( ui->moduleNameEd->text() );
        oPra->port_r =  ui->portEd->value();
        oPra->ip_l = QtCommonString::gbkToStdString( ui->localIpEd->currentText() );
        oPra->port_l = ui->localPortEd->value();
        oPra->desc = QtCommonString::gbkToStdString( ui->descEd->toPlainText() );
        oPra->remark = QtCommonString::gbkToStdString( ui->modifyContentEd->toPlainText() );

        QTableWidget * oGrid = ui->praItemsTw;
        std::vector<PraItem> & praItems = oPra->items;
        praItems.clear();
        std::vector<PraItem> praItemsWin = getPraItems(oGrid);
        for (size_t i = 0; i < praItemsWin.size(); ++i)
        {
            PraItem praItem = praItemsWin.at(i);
            praItem.pra = oPra->name;
            praItems.push_back(praItem);
        }

        PraUi::praFill(ui->prasTw, _currentRow, * oPra);
        return TRUE;
    }
    return FALSE;
}

int PraManagerWin::editViewOut(const string &sName)
{
    Pra * oPra = PraManager::findByName(sName);
    if (oPra)
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

        ui->md5Ed->setText( QtCommonString::gbkToQString( oPra->name ) );
        ui->fileNameEd->setText( QtCommonString::gbkToQString( oPra->rpath ) );
        ui->moduleNameEd->setText( QtCommonString::gbkToQString( oPra->ip_r ) );
        ui->portEd->setValue( oPra->port_r );
        ui->localIpEd->setEditText( QtCommonString::gbkToQString( oPra->ip_l ) );
        ui->localPortEd->setValue( oPra->port_l );
        ui->titleEd->setText( QtCommonString::gbkToQString( oPra->title ) );
        ui->descEd->setPlainText( QtCommonString::gbkToQString( oPra->desc ) );
        ui->modifyContentEd->setPlainText( QtCommonString::gbkToQString( oPra->remark ) );
        ui->versionEd->setText( QtCommonString::gbkToQString( oPra->res1 ) );

        QTableWidget * oGrid = ui->praItemsTw;
        oGrid->setRowCount(0);
        std::vector<PraItem> & praItems = oPra->items;
        for (size_t i = 0; i < praItems.size(); ++i)
        {
            const PraItem & praItem = praItems.at(i);
            PraUi::praItemInsert(oGrid, praItem);
        }
        return TRUE;
    }
    else
    {
        ui->md5Ed->setText( QString() );
        ui->fileNameEd->setText( QString() );
        ui->moduleNameEd->setText( QString() );
        ui->portEd->setValue(7921);
        if (ui->localIpEd->count()>0)
            ui->localIpEd->setCurrentIndex(0);
        else
            ui->localIpEd->setEditText(QString());
        ui->localPortEd->setValue(7987);
        ui->titleEd->setText( QString() );
        ui->descEd->setPlainText( QString() );
        ui->modifyContentEd->setPlainText( QString() );
        ui->versionEd->setText( QString() );

        QTableWidget * oGrid = ui->praItemsTw;
        oGrid->setRowCount(0);
    }
    return FALSE;
}

int PraManagerWin::rowByPraItemPath(const string &sPath)
{
    QTableWidget * oGrid = ui->praItemsTw;
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

void PraManagerWin::on_newBn_clicked()
{
    newArea();
}

void PraManagerWin::on_editBn_clicked()
{
    setCurrentState(pra_win_state_edit);
}

void PraManagerWin::on_saveBn_clicked()
{
    string sName = getAreaNameByRow(_currentRow);
    editViewIn(sName);
    setCurrentState(pra_win_state_view);
}

void PraManagerWin::on_restoreBn_clicked()
{
    if (_currentState==pra_win_state_edit)
    {
        string sName = getAreaNameByRow(_currentRow);
        if ( editViewOut(sName) )
        {
            setCurrentState(pra_win_state_view);
        }
    }
}

void PraManagerWin::on_insertFdcBn_clicked()
{
    addFdcsToCurrentProduct();
}
