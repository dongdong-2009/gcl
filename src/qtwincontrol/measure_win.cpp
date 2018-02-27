#include "measure_win.h"


#include <ccxx/qtcommon.h>


using namespace std;


void MeasureWin::reportMonsbManager(QTableWidget *oTableWidget)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<MonsbMeasure*>* oMonsbs = MonsbManager::measures();
    int iRowCount = oMonsbs->size();
    if (oGrid->rowCount() != iRowCount){
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("MonsbType");
        sHeaders.append("MeasureId");
        sHeaders.append("MeasureName");
        sHeaders.append("value");
        sHeaders.append("minValue");
        sHeaders.append("maxValue");
        sHeaders.append("DefaultValue");
        sHeaders.append("OriginalValue");
        sHeaders.append("NewValue");
        sHeaders.append("OldValue");
        sHeaders.append("LastChangeSourceId");
        sHeaders.append("LastChangeReasonId");
        sHeaders.append("LastRefreshTime");
        sHeaders.append("LastChangeTime");
        sHeaders.append("OutLineInterval");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < oMonsbs->size(); ++i)
        {
            MonsbMeasure* oMonsb = oMonsbs->at(i);
            int iRow = i;
            QString sText;
            QTableWidgetItem* oItem;
            //MonsbType
            sText = QtCommonString::gbkToQString( CS_MonsbTypeNames[oMonsb->monsbType()] );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 0, oItem);
            //MeasureId
            sText = QtCommonString::ToString(oMonsb->measureId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 1, oItem);
            //MeasureName
            sText = QtCommonString::ToString(oMonsb->measureName());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //MonsbValue
            sText = QtCommonString::ToString(oMonsb->value());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 3, oItem);
            //MonsbMinValue
            sText = QtCommonString::ToString(oMonsb->minValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 4, oItem);
            //MonsbMaxValue
            sText = QtCommonString::ToString(oMonsb->maxValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 5, oItem);
            //MonsbDefaultValue
            sText = QtCommonString::ToString(oMonsb->defaultValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
            //MonsbOriginalValue
            sText = QtCommonString::ToString(oMonsb->originalValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 7, oItem);
            //MonsbNewValue
            sText = QtCommonString::ToString(oMonsb->newValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 8, oItem);
            //MonsbOldValue
            sText = QtCommonString::ToString(oMonsb->oldValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 9, oItem);
            //MonsbLastChangeSourceId
            sText = QtCommonString::ToString(oMonsb->lastChangeSourceId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 10, oItem);
            //MonsbLastChangeReasonId
            sText = QtCommonString::ToString(oMonsb->lastChangeReasonId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 11, oItem);
            //MonsbLastRefreshTime
            sText = QtCommonString::ToString(oMonsb->lastRefreshTime());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 12, oItem);
            //MonsbLastChangeTime
            sText = QtCommonString::ToString(oMonsb->lastChangeTime());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 13, oItem);
            //MonsbOutLineInterval
            sText = QtCommonString::ToString(oMonsb->outLineInterval());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 14, oItem);
        }
    }
    for (int i = 0; i < oMonsbs->size(); ++i)
    {
        MonsbMeasure* oMonsb = oMonsbs->at(i);
        QTableWidgetItem* oItemMeasureName = oGrid->item(i,2);
        QTableWidgetItem* oItemMeasureValue = oGrid->item(i,3);
        if (oMonsb->value() == 1) {
            oItemMeasureName->setBackgroundColor( QColor(Qt::red) );
            oItemMeasureValue->setText("1");
        } else if (oMonsb->value() == 0) {
            oItemMeasureName->setBackgroundColor( QColor(Qt::green) );
            oItemMeasureValue->setText("0");
        }else if (oMonsb->value() == 2) {//未知
            oItemMeasureName->setBackgroundColor( QColor(Qt::gray) );
            oItemMeasureValue->setText("2");
        }else if (oMonsb->value() == 3) {//异常
            oItemMeasureName->setBackgroundColor( QColor(Qt::blue) );
            oItemMeasureValue->setText("3");
        }else if (oMonsb->value() == 4) {//故障
            oItemMeasureName->setBackgroundColor( QColor(Qt::yellow) );
            oItemMeasureValue->setText("4");
        }
        else {
            oItemMeasureName->setBackgroundColor( QColor(Qt::gray) );
            oItemMeasureValue->setText("other");
        }
    }
}

void MeasureWin::reportYcaddManager(QTableWidget *oTableWidget)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<YcaddMeasure*>* oYcadds = YcaddManager::measures();
    int iRowCount = oYcadds->size();
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("YcaddType");
        sHeaders.append("MeasureId");
        sHeaders.append("MeasureName");
        sHeaders.append("value");
        sHeaders.append("minValue");
        sHeaders.append("maxValue");
        sHeaders.append("DefaultValue");
        sHeaders.append("OriginalValue");
        sHeaders.append("NewValue");
        sHeaders.append("OldValue");
        sHeaders.append("LastChangeSourceId");
        sHeaders.append("LastChangeReasonId");
        sHeaders.append("LastRefreshTime");
        sHeaders.append("LastChangeTime");
        sHeaders.append("OutLineInterval");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < oYcadds->size(); ++i)
        {
            YcaddMeasure* oYcadd = oYcadds->at(i);
            int iRow = i;
            QString sText;
            QTableWidgetItem* oItem;
            //YcaddType
            sText = QtCommonString::gbkToQString( CS_YcaddTypeNames[oYcadd->ycaddType()] );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 0, oItem);
            //MeasureId
            sText = QtCommonString::ToString(oYcadd->measureId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 1, oItem);
            //MeasureName
            sText = QtCommonString::ToString(oYcadd->measureName());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //YcaddValue
            sText = QtCommonString::ToString(oYcadd->value());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 3, oItem);
            //YcaddMinValue
            sText = QtCommonString::ToString(oYcadd->minValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 4, oItem);
            //YcaddMaxValue
            sText = QtCommonString::ToString(oYcadd->maxValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 5, oItem);
            //YcaddDefaultValue
            sText = QtCommonString::ToString(oYcadd->defaultValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
            //YcaddOriginalValue
            sText = QtCommonString::ToString(oYcadd->originalValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 7, oItem);
            //YcaddNewValue
            sText = QtCommonString::ToString(oYcadd->newValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 8, oItem);
            //YcaddOldValue
            sText = QtCommonString::ToString(oYcadd->oldValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 9, oItem);
            //YcaddLastChangeSourceId
            sText = QtCommonString::ToString(oYcadd->lastChangeSourceId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 10, oItem);
            //YcaddLastChangeReasonId
            sText = QtCommonString::ToString(oYcadd->lastChangeReasonId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 11, oItem);
            //YcaddLastRefreshTime
            sText = QtCommonString::ToString(oYcadd->lastRefreshTime());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 12, oItem);
            //YcaddLastChangeTime
            sText = QtCommonString::ToString(oYcadd->lastChangeTime());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 13, oItem);
            //YcaddOutLineInterval
            sText = QtCommonString::ToString(oYcadd->outLineInterval());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 14, oItem);
        }
    }
    for (int i = 0; i < oYcadds->size(); ++i)
    {
        YcaddMeasure* oYcadd = oYcadds->at(i);
        QTableWidgetItem* oItemMeasureValue = oGrid->item(i, 3);
        QString sText = QtCommonString::ToString(oYcadd->value());
        oItemMeasureValue->setText(sText);

    }
}

void MeasureWin::reportStrawManager(QTableWidget *oTableWidget)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<StrawMeasure*>* oStraws = StrawManager::measures();
    int iRowCount = oStraws->size();
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("StrawType");
        sHeaders.append("MeasureId");
        sHeaders.append("MeasureName");
        sHeaders.append("value");
        sHeaders.append("minValue");
        sHeaders.append("maxValue");
        sHeaders.append("DefaultValue");
        sHeaders.append("OriginalValue");
        sHeaders.append("NewValue");
        sHeaders.append("OldValue");
        sHeaders.append("LastChangeSourceId");
        sHeaders.append("LastChangeReasonId");
        sHeaders.append("LastRefreshTime");
        sHeaders.append("LastChangeTime");
        sHeaders.append("OutLineInterval");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < oStraws->size(); ++i)
        {
            StrawMeasure* oStraw = oStraws->at(i);
            int iRow = i;
            QString sText;
            QTableWidgetItem* oItem;
            //StrawType
            sText = QtCommonString::gbkToQString( CS_StrawTypeNames[oStraw->strawType()] );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 0, oItem);
            //MeasureId
            sText = QtCommonString::ToString(oStraw->measureId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 1, oItem);
            //MeasureName
            sText = QtCommonString::ToString(oStraw->measureName());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //StrawValue
            sText = QtCommonString::ToString(oStraw->value());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 3, oItem);
            //StrawMinValue
            sText = QtCommonString::ToString(oStraw->minValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 4, oItem);
            //StrawMaxValue
            sText = QtCommonString::ToString(oStraw->maxValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 5, oItem);
            //StrawDefaultValue
            sText = QtCommonString::ToString(oStraw->defaultValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
            //StrawOriginalValue
            sText = QtCommonString::ToString(oStraw->originalValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 7, oItem);
            //StrawNewValue
            sText = QtCommonString::ToString(oStraw->newValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 8, oItem);
            //StrawOldValue
            sText = QtCommonString::ToString(oStraw->oldValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 9, oItem);
            //StrawLastChangeSourceId
            sText = QtCommonString::ToString(oStraw->lastChangeSourceId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 10, oItem);
            //StrawLastChangeReasonId
            sText = QtCommonString::ToString(oStraw->lastChangeReasonId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 11, oItem);
            //StrawLastRefreshTime
            sText = QtCommonString::ToString(oStraw->lastRefreshTime());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 12, oItem);
            //StrawLastChangeTime
            sText = QtCommonString::ToString(oStraw->lastChangeTime());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 13, oItem);
            //StrawOutLineInterval
            sText = QtCommonString::ToString(oStraw->outLineInterval());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 14, oItem);
        }
    }
    for (int i = 0; i < oStraws->size(); ++i)
    {
        StrawMeasure* oStraw = oStraws->at(i);
        QTableWidgetItem* oItemMeasureValue = oGrid->item(i, 3);
        QString sText = QtCommonString::ToString(oStraw->value());
        oItemMeasureValue->setText(sText);

    }
}

void MeasureWin::reportMonsbWorkerManager(QTableWidget *oTableWidget, const MonsbAddressWorkerTable *oWorkerTable)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<MonsbAddressWorker*>* oWorkers = oWorkerTable->workers();
    int iRowCount = oWorkers->size();
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("WorkerName");
        sHeaders.append("WorkerCode");
        sHeaders.append("Address");
        sHeaders.append("State");
        sHeaders.append("OldState");
        sHeaders.append("DateTime");
        sHeaders.append("StationId");
        sHeaders.append("StationName");
        sHeaders.append("Port");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < oWorkers->size(); ++i)
        {
            MonsbAddressWorker * oWorker = oWorkers->at(i);
            QString sValue;
            QString sOldValue;
            QString sDateTime;
            const MonsbMeasure * oMonsb = oWorker->measure();
            if (oMonsb)
            {
                sValue = QtCommonString::ToString(oMonsb->value());
                sOldValue = QtCommonString::ToString(oMonsb->oldValue());
                sDateTime = QtCommonString::ToString(oMonsb->lastRefreshTime());
            }
            int iRow = i;
            QString sText;
            QTableWidgetItem* oItem;
            //WorkerName
            sText = QtCommonString::gbkToQString( oWorker->workerName() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 0, oItem);
            //WorkerCode
            sText = QtCommonString::gbkToQString( oWorker->workerCode() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 1, oItem);
            //Address
            sText = QtCommonString::ToString(oWorker->address());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //sValue
            oItem = new QTableWidgetItem(sValue);
            oGrid->setItem(iRow, 3, oItem);
            //sOldValue
            oItem = new QTableWidgetItem(sOldValue);
            oGrid->setItem(iRow, 4, oItem);
            //sDateTime
            oItem = new QTableWidgetItem(sDateTime);
            oGrid->setItem(iRow, 5, oItem);
            //StationId
            sText = QtCommonString::ToString(oWorker->stationId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
            //StationName
            oItem = new QTableWidgetItem(QtCommonString::gbkToQString( oWorker->stationName()) );
            oGrid->setItem(iRow, 7, oItem);
            //Port
            sText = QtCommonString::ToString(oWorker->port());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 8, oItem);
        }
    }
    for (int i = 0; i < oWorkers->size(); ++i)
    {
        MonsbAddressWorker * oWorker = oWorkers->at(i);
        QString sValue;
        QString sOldValue;
        QString sDateTime;
        const MonsbMeasure * oMonsb = oWorker->measure();
        if (oMonsb)
        {
            sValue = QtCommonString::ToString(oMonsb->value());
            sOldValue = QtCommonString::ToString(oMonsb->oldValue());
            sDateTime = QtCommonString::ToString(oMonsb->lastRefreshTime());
        }
        QTableWidgetItem* oItemValue = oGrid->item(i,3);
        oItemValue->setText(sValue);
        QTableWidgetItem* oItemOldValue = oGrid->item(i,4);
        oItemOldValue->setText(sOldValue);
        QTableWidgetItem* oItemDateTime = oGrid->item(i,5);
        oItemDateTime->setText(sDateTime);
    }
}

void MeasureWin::reportYcaddWorkerManager(QTableWidget *oTableWidget, const YcaddAddressWorkerTable *oWorkerTable)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<YcaddAddressWorker*>* oWorkers = oWorkerTable->workers();
    int iRowCount = oWorkers->size();
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("WorkerName");
        sHeaders.append("WorkerCode");
        sHeaders.append("Address");
        sHeaders.append("value");
        sHeaders.append("OldValue");
        sHeaders.append("DateTime");
        sHeaders.append("StationId");
        sHeaders.append("StationName");
        sHeaders.append("Port");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < oWorkers->size(); ++i)
        {
            YcaddAddressWorker * oWorker = oWorkers->at(i);
            QString sValue;
            QString sOldValue;
            QString sDateTime;
            const YcaddMeasure * oYcadd = oWorker->measure();
            if (oYcadd)
            {
                sValue = QtCommonString::ToString(oYcadd->value());
                sOldValue = QtCommonString::ToString(oYcadd->oldValue());
                sDateTime = QtCommonString::ToString(oYcadd->lastRefreshTime());
            }
            int iRow = i;
            QString sText;
            QTableWidgetItem* oItem;
            //WorkerName
            sText = QtCommonString::gbkToQString( oWorker->workerName() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 0, oItem);
            //WorkerCode
            sText = QtCommonString::gbkToQString( oWorker->workerCode() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 1, oItem);
            //Address
            sText = QtCommonString::ToString(oWorker->address());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //sValue
            oItem = new QTableWidgetItem(sValue);
            oGrid->setItem(iRow, 3, oItem);
            //sOldValue
            oItem = new QTableWidgetItem(sOldValue);
            oGrid->setItem(iRow, 4, oItem);
            //sDateTime
            oItem = new QTableWidgetItem(sDateTime);
            oGrid->setItem(iRow, 5, oItem);
            //StationId
            sText = QtCommonString::ToString(oWorker->stationId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
            //StationName
            oItem = new QTableWidgetItem(QtCommonString::gbkToQString( oWorker->stationName() ));
            oGrid->setItem(iRow, 7, oItem);
            //Port
            sText = QtCommonString::ToString(oWorker->port());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 8, oItem);
        }
    }
    for (int i = 0; i < oWorkers->size(); ++i)
    {
        YcaddAddressWorker * oWorker = oWorkers->at(i);
        QString sValue;
        QString sOldValue;
        QString sDateTime;
        const YcaddMeasure * oYcadd = oWorker->measure();
        if (oYcadd)
        {
            sValue = QtCommonString::ToString(oYcadd->value());
            sOldValue = QtCommonString::ToString(oYcadd->oldValue());
            sDateTime = QtCommonString::ToString(oYcadd->lastRefreshTime());
        }
        QTableWidgetItem* oItemValue = oGrid->item(i,3);
        oItemValue->setText(sValue);
        QTableWidgetItem* oItemOldValue = oGrid->item(i,4);
        oItemOldValue->setText(sOldValue);
        QTableWidgetItem* oItemDateTime = oGrid->item(i,5);
        oItemDateTime->setText(sDateTime);
    }
}

void MeasureWin::reportStrawWorkerManager(QTableWidget *oTableWidget, const StrawAddressWorkerTable *oWorkerTable)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<StrawAddressWorker*>* oWorkers = oWorkerTable->workers();
    int iRowCount = oWorkers->size();
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("WorkerName");
        sHeaders.append("WorkerCode");
        sHeaders.append("Address");
        sHeaders.append("value");
        sHeaders.append("OldValue");
        sHeaders.append("DateTime");
        sHeaders.append("StationId");
        sHeaders.append("StationName");
        sHeaders.append("Port");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < oWorkers->size(); ++i)
        {
            StrawAddressWorker * oWorker = oWorkers->at(i);
            QString sValue;
            QString sOldValue;
            QString sDateTime;
            const StrawMeasure * oStraw = oWorker->measure();
            if (oStraw)
            {
                sValue = QtCommonString::ToString(oStraw->value());
                sOldValue = QtCommonString::ToString(oStraw->oldValue());
                sDateTime = QtCommonString::ToString(oStraw->lastRefreshTime());
            }
            int iRow = i;
            QString sText;
            QTableWidgetItem* oItem;
            //WorkerName
            sText = QtCommonString::gbkToQString( oWorker->workerName() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 0, oItem);
            //WorkerCode
            sText = QtCommonString::gbkToQString( oWorker->workerCode() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 1, oItem);
            //Address
            sText = QtCommonString::ToString(oWorker->address());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //sValue
            oItem = new QTableWidgetItem(sValue);
            oGrid->setItem(iRow, 3, oItem);
            //sOldValue
            oItem = new QTableWidgetItem(sOldValue);
            oGrid->setItem(iRow, 4, oItem);
            //sDateTime
            oItem = new QTableWidgetItem(sDateTime);
            oGrid->setItem(iRow, 5, oItem);
            //StationId
            sText = QtCommonString::ToString(oWorker->stationId());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
            //StationName
            oItem = new QTableWidgetItem(QtCommonString::gbkToQString( oWorker->stationName() ));
            oGrid->setItem(iRow, 7, oItem);
            //Port
            sText = QtCommonString::ToString(oWorker->port());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 8, oItem);
        }
    }
    for (int i = 0; i < oWorkers->size(); ++i)
    {
        StrawAddressWorker * oWorker = oWorkers->at(i);
        QString sValue;
        QString sOldValue;
        QString sDateTime;
        const StrawMeasure * oStraw = oWorker->measure();
        if (oStraw)
        {
            sValue = QtCommonString::ToString(oStraw->value());
            sOldValue = QtCommonString::ToString(oStraw->oldValue());
            sDateTime = QtCommonString::ToString(oStraw->lastRefreshTime());
        }
        QTableWidgetItem* oItemValue = oGrid->item(i,3);
        oItemValue->setText(sValue);
        QTableWidgetItem* oItemOldValue = oGrid->item(i,4);
        oItemOldValue->setText(sOldValue);
        QTableWidgetItem* oItemDateTime = oGrid->item(i,5);
        oItemDateTime->setText(sDateTime);
    }
}

