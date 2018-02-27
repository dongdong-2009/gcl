#include "monsb_operate_win.h"
#include "ui_monsb_operate_win.h"

#include <ccxx/qtcommon.h>
#include <QCloseEvent>

bool bCreatinga = false;
int _currentRow = -1;
const MonsbMeasure* _currentMonsb = NULL;
int _fromValue = -1;
int _toValue = -1;


MonsbOperateWin::MonsbOperateWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonsbOperateWin)
{
    ui->setupUi(this);

    bCreatinga = true;
    QStringList sMonsbTypeNames = QtCommonString::gbkToQStrings(CS_MonsbTypeNames, sizeof(CS_MonsbTypeNames) / sizeof(CS_MonsbTypeNames[0]));
    ui->MonsbTypeOperatesCb->addItems(sMonsbTypeNames);
    ui->MonsbTypeSelectsCb->addItems(sMonsbTypeNames);
    bCreatinga = false;

    QTableWidget* oMonsbOperatesTv = ui->MonsbOperatesTv;
    connect(oMonsbOperatesTv, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(monsbOperatesCurrentCellChanged(int,int,int,int)));

    //sort
    QHeaderView *oHeaderView;
    oHeaderView = oMonsbOperatesTv->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器
    oHeaderView->setSortIndicator(0, Qt::AscendingOrder);
    oHeaderView->setSortIndicatorShown(true);
    oHeaderView->setClickable(true);
    connect(oHeaderView, SIGNAL(sectionClicked(int)), oMonsbOperatesTv, SLOT (sortByColumn(int)));

    on_MonsbTypeOperatesCb_currentIndexChanged(ui->MonsbTypeOperatesCb->currentIndex());

    QTableWidget* oMonsbSelectsTv = ui->MonsbSelectsTv;
    refreshMonsbSelects(oMonsbSelectsTv, * (MonsbManager::measures()));

    //sort
    oHeaderView = oMonsbSelectsTv->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器
    oHeaderView->setSortIndicator(0, Qt::AscendingOrder);
    oHeaderView->setSortIndicatorShown(true);
    oHeaderView->setClickable(true);
    connect(oHeaderView, SIGNAL(sectionClicked(int)), oMonsbSelectsTv, SLOT (sortByColumn(int)));

    on_MonsbTypeSelectsCb_currentIndexChanged(ui->MonsbTypeSelectsCb->currentIndex());

    ui->ControlsGb->setEnabled(_currentMonsb);
    ui->CancelsGb->setEnabled(_currentMonsb);
}

MonsbOperateWin::~MonsbOperateWin()
{
    delete ui;
}

void MonsbOperateWin::refreshMonsbOperates(QTableWidget *oTableWidget, const std::vector<MonsbOperate*>& oMeasureOperates)
{
    int iRowCount = oMeasureOperates.size();
    oTableWidget->setRowCount(iRowCount);
    QStringList sHeaders;
    sHeaders.append("MeasureName");
    sHeaders.append("FromValue");
    sHeaders.append("ToValue");
    sHeaders.append("MeasureId");
    int iColumnCount = sHeaders.count();
    oTableWidget->setColumnCount(iColumnCount);
    oTableWidget->setHorizontalHeaderLabels(sHeaders);
    for (int i = 0; i < oMeasureOperates.size(); ++i) {
        MonsbOperate* oMonsbOperate = oMeasureOperates.at(i);
        const MonsbMeasure* oMonsb = oMonsbOperate->measure();
        int iRow = i;
        QString sText;
        QTableWidgetItem* oItem;
        //MeasureName
        sText = QtCommonString::gbkToQString(oMonsb->measureName());
        oItem = new QTableWidgetItem(sText);
        oTableWidget->setItem(iRow, 0, oItem);
        //FromValue
        sText = QtCommonString::ToString(oMonsbOperate->fromValue());
        oItem = new QTableWidgetItem(sText);
        oTableWidget->setItem(iRow, 1, oItem);
        //ToValue
        sText = QtCommonString::ToString(oMonsbOperate->toValue());
        oItem = new QTableWidgetItem(sText);
        oTableWidget->setItem(iRow, 2, oItem);
        //MeasureId
        sText = QtCommonString::ToString(oMonsb->measureId());
        oItem = new QTableWidgetItem(sText);
        oTableWidget->setItem(iRow, 3, oItem);
    }
    oTableWidget->sortByColumn(0);
}

void MonsbOperateWin::monsbOperatesCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if (_currentRow == currentRow)
        return;
    if (_currentMonsb) {
        saveCurrentMonsbOperate();
    }
    int iFromValue;
    int iToValue;
    const MonsbMeasure* oMonsb = monsbOperateData(ui->MonsbOperatesTv, currentRow, iFromValue, iToValue);
    if (oMonsb) {
        const MonsbOperate* oMonsbOperate = MonsbOperateManager::measureOperate(oMonsb, iFromValue, iToValue);
        if (oMonsbOperate) {
            refreshMeasureControls(ui->ControlsTv, oMonsbOperate->controls());
            refreshMeasureControls(ui->CancelsTv, oMonsbOperate->cancels());
        }
    }
    _currentMonsb = oMonsb;
    _fromValue = iFromValue;
    _toValue = iToValue;
    _currentRow = currentRow;
    ui->ControlsGb->setEnabled(_currentMonsb);
    ui->CancelsGb->setEnabled(_currentMonsb);
    QString sMeasureName = QtCommonString::gbkToQString( (_currentMonsb) ? _currentMonsb->measureName() : "" );
    ui->CurrentMonsbOperateLb->setText(QString("%1 : %2 - %3").arg(sMeasureName).arg(_fromValue).arg(_toValue));
}

std::vector<MonsbControl > MonsbOperateWin::getControls(QTableWidget* oTableWidget)
{
    std::vector<MonsbControl > measureControls;
    for (int iRowIndex = 0; iRowIndex < oTableWidget->rowCount(); ++iRowIndex)
    {
        QTableWidgetItem* oItem;
        oItem = oTableWidget->item(iRowIndex, 1);
        QString sFromValue = oItem->text();
        oItem = oTableWidget->item(iRowIndex, 2);
        QString sToValue = oItem->text();
        oItem = oTableWidget->item(iRowIndex, 3);
        QString sMeasureId = oItem->text();
        int iMeasureId = QtCommonString::FromString(sMeasureId, (int)-1);
        int iFromValue = QtCommonString::FromString(sFromValue, (int)-1);
        int iToValue =   QtCommonString::FromString(sToValue, (int)-1);
        if (iMeasureId != -1 || iFromValue != -1 || iToValue != -1)
        {
            const MonsbMeasure* oMonsb = MonsbManager::measure(iMeasureId);
            if (oMonsb)
            {
                measureControls.push_back(MonsbControl(oMonsb, iFromValue, iToValue));
            }
        }
    }
    return measureControls;
}

void MonsbOperateWin::refreshMeasureControls(QTableWidget* oTableWidget, const std::vector<MonsbControl >& controls)
{
    oTableWidget->clear();
    int iRowCount = controls.size();
    oTableWidget->setRowCount(iRowCount);
    QStringList sHeaders;
    sHeaders.append("MeasureName");
    sHeaders.append("FromValue");
    sHeaders.append("ToValue");
    sHeaders.append("MeasureId");
    int iColumnCount = sHeaders.count();
    oTableWidget->setColumnCount(iColumnCount);
    oTableWidget->setHorizontalHeaderLabels(sHeaders);
    for (int i = 0; i < controls.size(); ++i) {
        const MonsbControl& control = controls[i];
        refreshMeasureControl(oTableWidget, control, i);
    }
}

void MonsbOperateWin::refreshMeasureControl(QTableWidget *oTableWidget, const MonsbControl &control, int iRow)
{
    const MonsbMeasure* oMonsb = control.measure;
    int iFromValue = control.fromValue;
    int iToValue = control.toValue;
    QString sMeasureId = QtCommonString::ToString(oMonsb->measureId());
    QString sMeasureName = QtCommonString::gbkToQString( oMonsb->measureName() );
    QString sFromValue = QtCommonString::ToString(iFromValue);
    QString sToValue = QtCommonString::ToString(iToValue);
    QTableWidgetItem* oItem;
    //sMeasureName
    oItem = new QTableWidgetItem(sMeasureName);
    oItem->setFlags(Qt::ItemIsEditable);
    oTableWidget->setItem(iRow, 0, oItem);
    //sFromValue
    oItem = new QTableWidgetItem(sFromValue);
    oTableWidget->setItem(iRow, 1, oItem);
    //sToValue
    oItem = new QTableWidgetItem(sToValue);
    oTableWidget->setItem(iRow, 2, oItem);
    //sMeasureId
    oItem = new QTableWidgetItem(sMeasureId);
    oItem->setFlags(Qt::ItemIsEditable);
    oTableWidget->setItem(iRow, 3, oItem);
}

void MonsbOperateWin::refreshMonsbSelects(QTableWidget* oTableWidget, const std::vector<MonsbMeasure*>& monsbs)
{
    int iRowCount = monsbs.size();
    oTableWidget->setRowCount(iRowCount);
    QStringList sHeaders;
    sHeaders.append("MeasureName");
    sHeaders.append("MeasureId");
    int iColumnCount = sHeaders.count();
    oTableWidget->setColumnCount(iColumnCount);
    oTableWidget->setHorizontalHeaderLabels(sHeaders);
    for (int i = 0; i < monsbs.size(); ++i) {
        MonsbMeasure* oMonsb = monsbs[i];
        QString sText;
        QTableWidgetItem* oItem;
        //MeasureName
        sText = QtCommonString::gbkToQString(oMonsb->measureName());
        oItem = new QTableWidgetItem(sText);
        oTableWidget->setItem(i, 0, oItem);
        //MeasureId
        sText = QtCommonString::ToString(oMonsb->measureId());
        oItem = new QTableWidgetItem(sText);
        oTableWidget->setItem(i, 1, oItem);
    }
    oTableWidget->sortByColumn(0);
}

const MonsbMeasure * MonsbOperateWin::monsbOperateData(QTableWidget* oTableWidget, int iRow, int &iFromValue, int &iToValue)
{
    if (iRow > oTableWidget->rowCount() || iRow < 0)
        return NULL;
    QTableWidgetItem* oItemId = oTableWidget->item(iRow, 3);
    int iMeasureId = QtCommonString::FromString(oItemId->text(), -1);
    const MonsbMeasure* oMonsb = MonsbManager::measure(iMeasureId);
    QTableWidgetItem* oItemFromValue = oTableWidget->item(iRow, 1);
    iFromValue = QtCommonString::FromString(oItemFromValue->text(), -1);
    QTableWidgetItem* oItemToValue = oTableWidget->item(iRow, 2);
    iToValue = QtCommonString::FromString(oItemToValue->text(), -1);
    return oMonsb;
}

void MonsbOperateWin::on_MonsbTypeOperatesCb_currentIndexChanged(int index)
{
    if (bCreatinga) return;
    if (index == MonsbType_0)
    {
        const std::vector<MonsbOperate*>& monsbOperates = *( MonsbOperateManager::measureOperates());
        refreshMonsbOperates(ui->MonsbOperatesTv, monsbOperates);
    } else {
        std::vector<MonsbOperate*> monsbOperates = MonsbOperateManager::measureOperates(index);
        refreshMonsbOperates(ui->MonsbOperatesTv, monsbOperates);
    }
}

void MonsbOperateWin::on_MonsbTypeSelectsCb_currentIndexChanged(int index)
{
    if (bCreatinga) return;
    if (index == MonsbType_0)
    {
        refreshMonsbSelects(ui->MonsbSelectsTv, * (MonsbManager::measures()));
    } else {
        std::vector<MonsbMeasure*> monsbs = MonsbManager::measures(index);
        refreshMonsbSelects(ui->MonsbSelectsTv, monsbs);
    }
}

void MonsbOperateWin::on_ControlsDefaultBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oTableWidget = ui->ControlsTv;
    oTableWidget->setRowCount(1);
    MonsbControl control(_currentMonsb, _fromValue, _toValue);
    refreshMeasureControl(oTableWidget, control, 0);
}

void MonsbOperateWin::on_ControlsAddMultiBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oSelectsTv = ui->MonsbSelectsTv;
    QTableWidget* oControlsTv = ui->ControlsTv;
    appendMonsbControls(oSelectsTv, oControlsTv);
}

void MonsbOperateWin::on_ControlsInsertMultiBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oSelectsTv = ui->MonsbSelectsTv;
    QTableWidget* oControlsTv = ui->ControlsTv;
    insertMonsbControls(oSelectsTv, oControlsTv);
}

void MonsbOperateWin::on_ControlsAddPairBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oSelectsTv = ui->MonsbSelectsTv;
    QTableWidget* oControlsTv = ui->ControlsTv;
    insertPairControls(oSelectsTv, oControlsTv);
}

void MonsbOperateWin::on_CancelsDefaultBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oTableWidget = ui->CancelsTv;
    oTableWidget->setRowCount(0);
}

void MonsbOperateWin::on_CancelsInsertMultiBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oSelectsTv = ui->MonsbSelectsTv;
    QTableWidget* oControlsTv = ui->CancelsTv;
    insertMonsbControls(oSelectsTv, oControlsTv);
}

void MonsbOperateWin::on_CancelsAddMultiBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oSelectsTv = ui->MonsbSelectsTv;
    QTableWidget* oControlsTv = ui->CancelsTv;
    appendMonsbControls(oSelectsTv, oControlsTv);
}

void MonsbOperateWin::on_CancelsAddPairBn_clicked()
{
    if (! _currentMonsb)
        return;
    QTableWidget* oSelectsTv = ui->MonsbSelectsTv;
    QTableWidget* oControlsTv = ui->CancelsTv;
    insertPairControls(oSelectsTv, oControlsTv);
}

void MonsbOperateWin::closeEvent(QCloseEvent * oEvent)
{
    if (QtCommonDialog::ShowQuery(QString("Save All?")))
    {
        saveCurrentMonsbOperate();
        MonsbOperateManager::saveToFile(CxFileSystem::mergeFilePath( GclConfig::measurePath(), CS_FileNameMonsbOperates));
    }
    oEvent->accept();
    QDialog::closeEvent(oEvent);
}

void MonsbOperateWin::saveCurrentMonsbOperate()
{
    int iFromValue;
    int iToValue;
    const MonsbMeasure* oMonsb = monsbOperateData(ui->MonsbOperatesTv, _currentRow, iFromValue, iToValue);
    if (oMonsb && oMonsb->isValidValue(iFromValue) && oMonsb->isValidValue(iToValue)) {
        std::vector<MonsbControl > controls = getControls(ui->ControlsTv);
        std::vector<MonsbControl > cancels = getControls(ui->CancelsTv);
        MonsbOperateManager::setControls(_currentMonsb, iFromValue, iToValue, controls);
        MonsbOperateManager::setCancels(_currentMonsb, iFromValue, iToValue, cancels);
    }
}

void MonsbOperateWin::insertMonsbControls(QTableWidget *oSelectsTv, QTableWidget *oControlsTv)
{
    insertMonsbControls(oSelectsTv, oControlsTv, oControlsTv->currentRow());
}

void MonsbOperateWin::appendMonsbControls(QTableWidget *oSelectsTv, QTableWidget *oControlsTv)
{
    insertMonsbControls(oSelectsTv, oControlsTv, oControlsTv->rowCount());
}

void MonsbOperateWin::insertMonsbControls(QTableWidget *oSelectsTv, QTableWidget *oControlsTv, int iRow)
{
    QHash<const MonsbMeasure*, int> monsbs;
    QList<QTableWidgetItem*> selectedItems = oSelectsTv->selectedItems();
    for (int i = 0; i < selectedItems.count(); ++i) {
        QTableWidgetItem* item = selectedItems.at(i);
        int iRow2 = item->row();
        QTableWidgetItem* itemMeasureId = oSelectsTv->item(iRow2, 1);
        int iMeasureId = QtCommonString::FromString(itemMeasureId->text(), -1);
        const MonsbMeasure* oMonsb = MonsbManager::measure(iMeasureId);
        if (oMonsb){
            monsbs[oMonsb] = _toValue;
        }
    }
    if (monsbs.count()>0) {
        int iRow2 = iRow;
        if (iRow2 <= 0)
            iRow2 = 0;
        QHash<const MonsbMeasure*, int>::const_iterator it = monsbs.constBegin();
        while (it != monsbs.constEnd()) {
            const MonsbMeasure* oMonsb = it.key();
            MonsbControl control(oMonsb, _fromValue, _toValue);
            oControlsTv->insertRow(iRow);
            refreshMeasureControl(oControlsTv, control, iRow2);
            ++iRow;
            ++it;
        }
    }
}

void MonsbOperateWin::insertPairControls(QTableWidget *oSelectsTv, QTableWidget *oControlsTv)
{
    int iRow = oSelectsTv->currentRow();
    if (iRow < 0) {
        return;
    }
    QTableWidgetItem* itemMeasureId = oSelectsTv->item(iRow, 1);
    int iMeasureId = QtCommonString::FromString(itemMeasureId->text(), -1);
    const MonsbMeasure* oMonsb = MonsbManager::measure(iMeasureId);
    if (oMonsb) {
        int iRow2 = 0;
        oControlsTv->insertRow(iRow2);
        MonsbControl controlFirst(oMonsb, 0, 1);
        refreshMeasureControl(oControlsTv, controlFirst, iRow2);

        iRow2 = oControlsTv->rowCount();
        oControlsTv->insertRow(iRow2);
        MonsbControl controlLast(oMonsb, 1, 0);
        refreshMeasureControl(oControlsTv, controlLast, iRow2);
    }
}


