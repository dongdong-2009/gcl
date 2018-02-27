#include "table_win.h"

#include "describe_win.h"

#include <ccxx/cxfile.h>
#include <ccxx/cxprofile.h>
#include <ccxx/cxinterinfo.h>
#include <ccxx/qtcommon.h>

#include <QComboBox>
#include <QKeyEvent>
#include <QHeaderView>

using namespace std;


void fn_setItemValue(QStandardItem *oItem, const std::string &sValue, const CxDescribeBase * oDescribe)
{
    QString qsValue;
    bool bOk;

    switch (oDescribe->valueType())
    {
    case CxValueType::ValueType_Integer :
    {
        const CxDescribeInteger* oIntegerDescribe = reinterpret_cast<const CxDescribeInteger*>(oDescribe);
        if (oIntegerDescribe->IsHexDisplay())
        {
            int iValue = CxString::toInt32(sValue, & bOk);
            if (bOk)
                qsValue = QString("0x")+QString::number(iValue, 16);
        }
    }
        break;
    case CxValueType::ValueType_Enum :
    {
    }
        break;
    case CxValueType::ValueType_Boolean :
    {
        const CxDescribeBoolean* oBooleanDescribe = reinterpret_cast<const CxDescribeBoolean*>(oDescribe);
    }
        break;
    case CxValueType::ValueType_Double :
    {
        const CxDescribeDouble* oDoubleDescribe = reinterpret_cast<const CxDescribeDouble*>(oDescribe);
    }
        break;
    case CxValueType::ValueType_DateTime :
    {
        const CxDescribeDateTime* oDateTimeDescribe = reinterpret_cast<const CxDescribeDateTime*>(oDescribe);
    }
        break;
    case CxValueType::ValueType_String :
    {
        const CxDescribeString* oStringDescribe = reinterpret_cast<const CxDescribeString*>(oDescribe);
    }
        break;
    }

    if (qsValue.isEmpty()) qsValue = QtCommonString::gbkToQString(sValue);
    QtCommonWidget::setItemValueEditRole(oItem, qsValue);
}


TableItemDelegate::TableItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    _describeUnit = NULL;
}

QWidget * TableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (_describeUnit && index.column() < _describeUnit->describes().size())
    {
        CxDescribeBase* oDescribe = _describeUnit->describe(index.column());
        if (oDescribe)
        {
            if (oDescribe->name() == _describeUnit->localCaptionName())
            {
                QComboBox* oComboBox = new QComboBox(parent);
                const std::map<std::string, std::string>& foreignKeyValues = _describeUnit->foreignKeyValues();
                for (std::map<std::string, std::string>::const_iterator it = foreignKeyValues.begin(); it != foreignKeyValues.end(); ++it)
                {
                    QString sKey = QtCommonString::gbkToQString( it->first );
                    QString sValue = QtCommonString::gbkToQString( it->second );
                    oComboBox->addItem(sValue, sKey);
                }
                return oComboBox;
            }
            else
            {
                QWidget* oWidget = DescribeWin::createControl(oDescribe, parent);
                if (oWidget)
                {
                    return oWidget;
                }
            }
        }
    }
    return QItemDelegate::createEditor(parent, option, index);
}

void TableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (_describeUnit && index.column() < _describeUnit->describes().size())
    {
        CxDescribeBase* oDescribe = _describeUnit->describe(index.column());
        if (oDescribe)
        {
            if (oDescribe->name() == _describeUnit->localCaptionName())
            {
                QComboBox* oComboBox = reinterpret_cast<QComboBox*>(editor);
                int iIndex = oComboBox->findText(index.model()->data(index, Qt::EditRole).toString());
                oComboBox->setCurrentIndex(iIndex);
            } else {
                oDescribe->valueFromString(QtCommonString::gbkToStdString( index.model()->data(index, Qt::EditRole).toString()) );
                DescribeWin::controlViewOut(oDescribe, editor);
            }
        }
    }
    else
        return QItemDelegate::setEditorData(editor, index);
}

void TableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStandardItemModel * oModel = (QStandardItemModel*)model;
    if (_describeUnit && index.column() < _describeUnit->describes().size())
    {
        CxDescribeBase* oDescribe = _describeUnit->describe(index.column());
        if (oDescribe)
        {
            if (oDescribe->name() == _describeUnit->localCaptionName())
            {
                QComboBox* oComboBox = reinterpret_cast<QComboBox*>(editor);
                QString sKey;
                QString sValue;
                int iCurrentIndex = oComboBox->currentIndex();
                if (iCurrentIndex>=0)
                {
                    sValue = oComboBox->itemText(iCurrentIndex);
                    sKey = oComboBox->itemData(iCurrentIndex).toString();
                }

                fn_setItemValue(oModel->itemFromIndex(index), QtCommonString::gbkToStdString(sValue), oDescribe);
//                model->setData(index, sValue);
                CxDescribeBase* oDescribeKey = _describeUnit->describe(_describeUnit->localKeyName());
                if (oDescribeKey)
                {
                    QModelIndex modeIndex = model->index(index.row(), oDescribeKey->columnIndex());
                    fn_setItemValue(oModel->itemFromIndex(modeIndex), QtCommonString::gbkToStdString(sKey), oDescribeKey);
//                    model->setData(modeIndex, sKey);
                }
            }
            else
            {
                DescribeWin::controlViewIn(oDescribe, editor);
                fn_setItemValue(oModel->itemFromIndex(index), oDescribe->valueToString(), oDescribe);
//                QtCommonWidget::setItemValueEditRole(oModel->itemFromIndex(index), QtCommonString::gbkToQString(oDescribe->valueToString()));
//                model->setData( index, QtCommonString::gbkToQString(oDescribe->valueToString()) );
            }
        }
    }
    else
        return QItemDelegate::setModelData(editor, model, index);
}

void TableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

TableWin::TableWin()
    : QObject(0)
{
    _describeUnit = new CxDescribeUnit();
    _isSetDescribeUnit = false;
    _parent = NULL;
    _tableView = NULL;
}

TableWin::~TableWin()
{
    if (! _isSetDescribeUnit && _describeUnit)
        delete _describeUnit;
}

void TableWin::loadWin(QWidget *oParent, const std::string & sDataFilePath, const std::string & sUiFilePath, const std::string& sDataType, const std::string& sDataParam)
{
    initUi(oParent);

    loadData(sDataFilePath, sDataType, sDataParam);
}

void TableWin::initUi(QWidget *oParent)
{
    GM_INVALID_RETURE(oParent);
    GM_INVALID_RETURE(_describeUnit);

    QString sObjectName = QtCommonString::gbkToQString( _describeUnit->unitName() + "Ed" );
    QWidget* oTableEd = oParent->findChild<QWidget*>(sObjectName);

    if (oTableEd && oTableEd->metaObject()->className() == QTableView::staticMetaObject.className())
        return;

    const std::map<std::string, std::string> & foreignKeyValues = _describeUnit->foreignKeyValues();

    _layout = new QHBoxLayout(oParent);
    _tableView = new QTableView(oParent);
    _layout->addWidget(_tableView);
    _layout->setStretch(0, 10);
    _tableView->setObjectName(sObjectName);
    _tableView->installEventFilter(this);
    _tableItemModel = new QStandardItemModel(_tableView);
    _tableView->setModel(_tableItemModel);
    _tableItemDelegate = new TableItemDelegate(_tableView);
    _tableItemDelegate->setDescribeUnit(_describeUnit);
    _tableView->setItemDelegate(_tableItemDelegate);
    _listWidget = new QListWidget(oParent);
    _listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    if (foreignKeyValues.size()>0)
    {
        _layout->addWidget(_listWidget);
        _layout->setStretch(1,2);
    }
    else
    {
        _listWidget->setVisible(false);
    }
    _listWidget->installEventFilter(this);
    _listWidget->setSortingEnabled(true);
    //sort
    QHeaderView *oHeaderView = _tableView->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器
    oHeaderView->setSortIndicator(0, Qt::AscendingOrder);
    oHeaderView->setSortIndicatorShown(true);
#if (QT_VERSION < QT_VERSION_CHECK(4, 8, 9))
    oHeaderView->setClickable(true);
#else
    oHeaderView->setSectionsClickable(true);
#endif
    connect(oHeaderView, SIGNAL(sectionClicked(int)), _tableView, SLOT (sortByColumn(int)));
    //action
    _tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    _tableMenu = new QMenu(oParent);

    _setSameValueAction = new QAction("Set Same Value...", oParent);
    connect(_setSameValueAction, SIGNAL(triggered()), this, SLOT(setSameValueSlot()));

    _setIncreaseValueAction = new QAction("Set Increase Value...", oParent);
    connect(_setIncreaseValueAction, SIGNAL(triggered()), this, SLOT(setIncreaseValueSlot()));

    _exportCaptionsAction = new QAction(QtCommonString::gbkToQString("Export " + _describeUnit->localCaptionName() + " and selected column to file..."), oParent);
    connect(_exportCaptionsAction, SIGNAL(triggered()), this, SLOT(exportCaptionsSlot()));

    _importCaptionsAction = new QAction(QtCommonString::gbkToQString("Import selected column base on " + _describeUnit->localCaptionName() + " from file..."), oParent);
    connect(_importCaptionsAction, SIGNAL(triggered()), this, SLOT(importCaptionsSlot()));

    _batchAddRowsAction = new QAction("Batch Add Rows ...", oParent);
    connect(_batchAddRowsAction, SIGNAL(triggered()), this, SLOT(batchAddRowsSlot()));

    for (std::map<std::string, std::string>::const_iterator it = foreignKeyValues.begin(); it != foreignKeyValues.end(); ++it)
    {
        QString sKey =   QtCommonString::gbkToQString(it->first);
        QString sValue = QtCommonString::gbkToQString(it->second);
        QListWidgetItem* oItem = new QListWidgetItem(sValue, _listWidget);
        oItem->setData(Qt::UserRole, sKey);
    }
    const std::vector<CxDescribeBase*> & describes = _describeUnit->describes();
    _tableItemModel->setColumnCount( describes.size() );
    for (size_t iColumnIndex = 0; iColumnIndex < describes.size(); ++iColumnIndex)
    {
        CxDescribeBase* oDescribe = describes.at(iColumnIndex);
        _tableItemModel->setHeaderData(iColumnIndex, Qt::Horizontal, QtCommonString::gbkToQString(oDescribe->title()));
        oDescribe->setColumnIndex(iColumnIndex);
    }
}

void TableWin::clearData()
{
    _tableItemModel->setRowCount(0);
}

void TableWin::loadData(const std::string & sDataFilePath, const std::string & sDataType, const std::string & sDataParam)
{
    GM_INVALID_RETURE(_describeUnit);
    GM_INVALID_RETURE(_tableView);
    GM_INVALID_RETURE(_tableItemModel);
    _tableItemModel->setRowCount(0);
    const std::vector<CxDescribeBase*> & describes = _describeUnit->describes();
    CxSkverBase * oProfile = CxSkverManager::beginUse(sDataFilePath, sDataType, sDataParam);
    if (oProfile)
    {
        int iRowIndex = 0;
        std::vector<std::string> sSections = oProfile->sectionNames(1);
        for (size_t i = 0; i < sSections.size(); ++i)
        {
            std::string sSection = sSections.at(i);
            oProfile->prepareGet(sSection);
            std::string sValue;
            //has filter
            if (_describeUnit->filterName().size()>0)
            {
                sValue = oProfile->getValue(_describeUnit->filterName(), sValue);
                if (sValue != _describeUnit->filterValue())
                {
                    continue;
                }
            }
            int iValueCount = 0;
            for (size_t iColumnIndex = 0; iColumnIndex < describes.size(); ++iColumnIndex)
            {
                CxDescribeBase* oDescribe = describes.at(iColumnIndex);
                sValue = oProfile->getValue(oDescribe->name(), std::string());
                if (sValue.empty())
                    sValue = oDescribe->defaultValueToString();
                else
                    ++iValueCount;
                QStandardItem* oItem = new QStandardItem();
                fn_setItemValue(oItem, sValue, oDescribe);
                _tableItemModel->setItem(iRowIndex,iColumnIndex,oItem);
            }
            if (iValueCount == 0)
            {
                _tableItemModel->removeRow(iRowIndex);
            }
            else
            {
                ++iRowIndex;
            }
        }
        CxSkverManager::endUse(oProfile);
    }
    _tableView->showGrid();
}

void TableWin::redefault()
{

}

int TableWin::saveWin(const std::string &sDataFilePath, const std::string &sDataType, const std::string &sDataParam)
{
    if (! _tableView || ! _describeUnit)
        return -1;
    CxSkverBase * oProfile = CxSkverManager::beginUse(sDataFilePath, sDataType, sDataParam, false);
    if (oProfile)
    {
        int iRowCount = _tableItemModel->rowCount();
        for (int i = 0; i < iRowCount; i++)
        {
            oProfile->prepareSet(CxString::format("%06d",i));
            const std::vector<CxDescribeBase*> & describes = _describeUnit->describes();
            for (size_t j = 0; j < describes.size(); ++j)
            {
                const CxDescribeBase* oDescribe = describes.at(j);
                QStandardItem* oItem = _tableItemModel->item(i, j);
                std::string sValue = QtCommonString::gbkToStdString( oItem->text() );
                oProfile->setValue(oDescribe->name(), sValue);
            }
        }
        return CxSkverManager::endUse(oProfile, true);
    }
    return -1;
}

void TableWin::setDescribeUnit(CxDescribeUnit *value)
{
    if (! _isSetDescribeUnit && _describeUnit)
        delete _describeUnit;
    _describeUnit = value;
    _isSetDescribeUnit = true;
}

bool TableWin::eventFilter(QObject* oObject, QEvent* oEvent)
{
    if (oObject == _listWidget)
    {
        if (oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Return && (keyEvent->modifiers() & Qt::ControlModifier))
            {
                insertTableRowsByList();
                return true;
            }
            else if(keyEvent->key() == Qt::Key_F && (keyEvent->modifiers() & Qt::ControlModifier))
            {
                searchListText();
                return true;
            }
        }
    }
    else if (oObject == _tableView)
    {
        if (oEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(oEvent);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                deleteTableSelectRows();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Insert)
            {
                insertTableRow();
                return true;
            }
            else if(keyEvent->key() == Qt::Key_F && (keyEvent->modifiers() & Qt::ControlModifier))
            {
                searchTableText();
                return true;
            }
        }
    }
    // pass the event on to the parent class
    return QObject::eventFilter(oObject, oEvent);
}

void TableWin::deleteTableSelectRows()
{
    QItemSelectionModel *selections = _tableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    QHash<int, int> rowMap;
    foreach (QModelIndex index, selected)
    {
        rowMap.insert(index.row(), 0);
    }
    int rowToDel;
    QHashIterator<int, int> rowMapIterator(rowMap);
    rowMapIterator.toBack();
    while (rowMapIterator.hasPrevious())
    {
        rowMapIterator.previous();
        rowToDel = rowMapIterator.key();
        _tableItemModel->removeRow(rowToDel);
    }
}

void TableWin::insertTableRow()
{
    if (! _describeUnit)
        return;
    QTableView* oTableView = _tableView;
    QModelIndex selected = oTableView->currentIndex();
    int iRow = selected.row();
    iRow = iRow < 0 ? 0 : iRow;
    _tableItemModel->insertRow(iRow);
    const std::vector<CxDescribeBase*> & describes = _describeUnit->describes();
    for (size_t iColumnIndex = 0; iColumnIndex < describes.size(); ++iColumnIndex)
    {
        const CxDescribeBase* oDescribe = describes.at(iColumnIndex);
        std::string sValue = _describeUnit->describeDefaultValue(_describeUnit, oDescribe, iRow);
        if (sValue.size()<=0)
            sValue = oDescribe->defaultValueToString();
        _tableItemModel->setItem(iRow,iColumnIndex,new QStandardItem(QtCommonString::gbkToQString( sValue )));
    }
}

void TableWin::searchTableText()
{
    QTableView* oTableView = _tableView;
    QStandardItemModel* oTableModel = _tableItemModel;
    if ( oTableModel->rowCount() <= 0 )
        return;
    QModelIndex selected = oTableView->currentIndex();
    int iSelectColumn = selected.column();
    QString sValueSearched;
    if ( ! QtCommonDialog::DialogInput("Value Serched", sValueSearched) )
        return;
    if ( sValueSearched.isEmpty() )
        return;
    for (int i = 0; i < oTableModel->rowCount(); ++i)
    {
        QStandardItem* oItem = oTableModel->item(i,iSelectColumn);
        QString sValue = oItem->data(Qt::DisplayRole).toString();
        int iIndex = sValue.indexOf(sValueSearched);
        if ( iIndex >= 0 )
        {
            oTableView->selectRow(i);
            return;
        }
    }
}

void TableWin::searchListText()
{
    QListWidget* oListWidget = _listWidget;
    QString sValueSearched;
    if ( ! QtCommonDialog::DialogInput("Value Serched", sValueSearched) )
        return;
    if ( sValueSearched.isEmpty() )
        return;
    for (int i = 0; i < oListWidget->count(); ++i)
    {
        QListWidgetItem* oItem = oListWidget->item(i);
        QString sValue = oItem->data(Qt::DisplayRole).toString();
        int iIndex = sValue.indexOf(sValueSearched);
        if ( iIndex >= 0 )
        {
            oListWidget->setCurrentRow(i);
            return;
        }
    }
}

void TableWin::insertTableRowsByList()
{
    if (! _describeUnit)
        return;
    QListWidget* oListWidget = _listWidget;
    QList<QListWidgetItem*> selectedItems = oListWidget->selectedItems();
    QTableView* oTableView = _tableView;
    QModelIndex selected = oTableView->currentIndex();
    int iRow = selected.row();
    iRow = (iRow > -1) ? iRow : 0;
    _tableItemModel->insertRows(iRow, selectedItems.count());
    foreach (QListWidgetItem* oItem, selectedItems) {
        QString sKey = oItem->data(Qt::UserRole).toString();
        QString sValue = oItem->text();
        const std::vector<CxDescribeBase*> & describes = _describeUnit->describes();
        for (size_t iColumnIndex = 0; iColumnIndex < describes.size(); ++iColumnIndex)
        {
            const CxDescribeBase* oDescribe = describes.at(iColumnIndex);
            if (oDescribe->name() == _describeUnit->localCaptionName())
            {
                _tableItemModel->setItem(iRow,oDescribe->columnIndex(),new QStandardItem(sValue));
            }
            else if (oDescribe->name() == _describeUnit->localKeyName())
            {
                _tableItemModel->setItem(iRow,oDescribe->columnIndex(),new QStandardItem(sKey));
            }
            else
            {
                std::string sValue = _describeUnit->describeDefaultValue(_describeUnit, oDescribe, iRow);
                if (sValue.size()<=0)
                    sValue = oDescribe->defaultValueToString();
                _tableItemModel->setItem(iRow,iColumnIndex,new QStandardItem(QtCommonString::gbkToQString( sValue )));
            }
        }
        ++iRow;
    }
}

void TableWin::customContextMenu(const QPoint &pos)
{
    if (! _describeUnit)
        return;
    QModelIndex posModelIndex = _tableView->indexAt(pos);
    //获取当前被点击的节点
    QStandardItem * posItem = _tableItemModel->itemFromIndex(posModelIndex);
    //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    if(! posItem)
    {
        return;
    }
    _tableMenu->clear();
    if (_describeUnit && posModelIndex.column() < _describeUnit->describes().size())
    {
        CxDescribeBase* oDescribe = _describeUnit->describe(posModelIndex.column());
        if (oDescribe)
        {
            if ( (! oDescribe->isReadOnly() && oDescribe->isVisible() && oDescribe->name() != _describeUnit->localCaptionName() ) )
            {
                _tableMenu->addAction(_setSameValueAction);
                if ( oDescribe->typeSimpleName() == CxDescribeInteger::TYPESIMPLENAME() || oDescribe->typeSimpleName() == CxDescribeDouble::TYPESIMPLENAME())
                {
                    _tableMenu->addAction(_setIncreaseValueAction);
                }
                _tableMenu->addAction(_exportCaptionsAction);
                _tableMenu->addAction(_importCaptionsAction);
                if ( _describeUnit->foreignKeyValues().size() < 1)
                    _tableMenu->addAction(_batchAddRowsAction);
            }
        }
    }
    _tableMenu->exec(QCursor::pos());
}

void TableWin::setSameValueSlot()
{
    if (! _describeUnit)
        return;
    QString sValue;
    if (! QtCommonDialog::DialogInput("Input The Set Same Value", sValue))
    {
        return;
    }
    QStandardItemModel * oTableItemModel = _tableItemModel;
    QItemSelectionModel * selections = _tableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    foreach (QModelIndex index, selected)
    {
        QStandardItem * oItem = oTableItemModel->itemFromIndex(index);
        CxDescribeBase* oDescribe = _describeUnit->describe(index.column());
        if (oDescribe && oItem)
        {
            string sValue2 = QtCommonString::gbkToStdString( sValue );
            if (oDescribe->isValidValue(sValue2))
            {
                fn_setItemValue(oItem, sValue2, oDescribe);
//                oItem->setText(sValue);
            } else
            {
                cxPrompt() << "IsValidValue : " << QtCommonString::gbkToStdString( sValue );
            }
        }
        else
        {
            cxPrompt() << "oDescribe || oItem is NULL";
        }
    }
}

void TableWin::setIncreaseValueSlot()
{
    if (! _describeUnit)
        return;
    QString sValue;
    if (! QtCommonDialog::DialogInput("Input The Set Increase Value", sValue)) {
        return;
    }
    int iValue = 0;
    bool bOk;
    iValue = QtCommonString::FromString(sValue, iValue, &bOk);
    if (! bOk)
    {
        return;
    }
    QStandardItemModel * oTableItemModel = _tableItemModel;
    QItemSelectionModel * selections = _tableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    foreach (QModelIndex index, selected)
    {
        QStandardItem * oItem = oTableItemModel->itemFromIndex(index);
        CxDescribeBase* oDescribe = _describeUnit->describe(index.column());
        std::string sValue2 = CxString::toString(iValue);
        ++iValue;
        if (oDescribe && oItem)
        {
            if (oDescribe->isValidValue(sValue2))
            {
                fn_setItemValue(oItem, sValue2, oDescribe);
//                oItem->setData(iValue, Qt::DisplayRole);
            }
            else
            {
                cxPrompt() << "IsValidValue : " << sValue2;
            }
        }
        else
        {
            cxPrompt() << "oDescribe || oItem is NULL";
        }
    }
}

void TableWin::exportCaptionsSlot()
{
    if (! _describeUnit)
        return;
    const CxDescribeBase * oDescribeCaption = _describeUnit->describe(_describeUnit->localCaptionName());
    if ( ! oDescribeCaption )
        return;
    QTableView* oTableView = _tableView;
    QStandardItemModel * oTableItemModel = _tableItemModel;
    QModelIndex selected = oTableView->currentIndex();
    int iSelectColumn = selected.column();
    if ( iSelectColumn < 0 || iSelectColumn >= oTableItemModel->columnCount() )
        return;
    std::vector<std::string> sRows/* = CommonFile::FileLoadToStringList(sFilePath)*/;
    for (int i = 0; i < oTableItemModel->rowCount(); i++)
    {
        QStandardItem* oItem = oTableItemModel->item(i, oDescribeCaption->columnIndex());
        std::string sCaption = QtCommonString::gbkToStdString(oItem->text());
        oItem = oTableItemModel->item(i, iSelectColumn);
        std::string sSelect = QtCommonString::gbkToStdString(oItem->text());
        sRows.push_back(sSelect + ',' + sCaption);
    }
    if (sRows.size() > 0)
    {
        QString sFilePath;
        if (QtCommonDialog::DialogSave(sFilePath, "*.txt"))
        {
            if ( CxFile::save(QtCommonString::gbkToStdString( sFilePath ), sRows) )
                QtCommonDialog::ShowPrompt("Save Success.");
            else
                QtCommonDialog::ShowPrompt("Save Fail.");
        }
    }
}

void TableWin::importCaptionsSlot()
{
    if (! _describeUnit)
        return;
    const CxDescribeBase * oDescribeCaption = _describeUnit->describe(_describeUnit->localCaptionName());
    if ( ! oDescribeCaption )
        return;
    QTableView* oTableView = _tableView;
    QStandardItemModel * oTableItemModel = _tableItemModel;
    QModelIndex selected = oTableView->currentIndex();
    int iSelectColumn = selected.column();
    if ( iSelectColumn < 0 || iSelectColumn >= oTableItemModel->columnCount() )
        return;
    QString sFilePath;
    if (! QtCommonDialog::DialogOpen(sFilePath, "*.txt"))
        return;
    std::vector<std::string> sRows;
    CxFile::load(QtCommonString::gbkToStdString(sFilePath), sRows);
    if (sRows.size() <= 0)
        return;
    QHash<QString, QString> sCaptions;
    for (size_t i = 0; i < sRows.size(); ++i)
    {
        QString sCaption = QtCommonString::gbkToQString( sRows[i] );
        QString sSelect= QtCommonString::FirstToken(sCaption, ',');
        if (! sCaption.isEmpty())
            sCaptions[sCaption] = sSelect;
    }
    for (int i = 0; i < oTableItemModel->rowCount(); i++)
    {
        QStandardItem* oItem = oTableItemModel->item(i, oDescribeCaption->columnIndex());
        QString sCaption = oItem->text();
        if (sCaptions.contains(sCaption))
        {
            oItem = oTableItemModel->item(i, iSelectColumn);
            string sValue = QtCommonString::gbkToStdString(sCaptions[sCaption]);
            fn_setItemValue(oItem, sValue, oDescribeCaption);
//            oItem->setText( sCaptions[sCaption] );
        }
    }
}

void TableWin::batchAddRowsSlot()
{
    QString sValue;
    if (! QtCommonDialog::DialogInput("Input The Set Batch Value", sValue))
    {
        return;
    }
    bool bOk;
    int iValue = sValue.toInt(& bOk);
    if (bOk && iValue > 0 && iValue < 20000)
    {
        for (int i = 0; i < iValue; ++i)
        {
            insertTableRow();
        }
    }
}

