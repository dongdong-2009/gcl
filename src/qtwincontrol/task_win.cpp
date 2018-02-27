#include "task_win.h"

#include <ccxx/qtcommon.h>

#include <vdi/monsb_control_task.h>


using namespace std;


void TaskWin::reportMonsbOperateManager(QTableWidget *oTableWidget)
{
    QTableWidget* oGrid = oTableWidget;
    const std::vector<MonsbOperate*> * measureOperates = MonsbOperateManager::measureOperates();
    int iRowCount = measureOperates->size();
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("MonsbType");
        sHeaders.append("MeasureId");
        sHeaders.append("MeasureName");
        sHeaders.append("FromValue");
        sHeaders.append("ToValue");
        sHeaders.append("Controls");
        sHeaders.append("Cancels");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        for (int i = 0; i < measureOperates->size(); ++i)
        {
            const MonsbOperate* oMeasureOperate = measureOperates->at(i);
            const MonsbMeasure* oMonsb = oMeasureOperate->measure();
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
            sText = QtCommonString::gbkToQString( oMonsb->measureName() );
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 2, oItem);
            //FromValue
            sText = QtCommonString::ToString(oMeasureOperate->fromValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 3, oItem);
            //ToValue
            sText = QtCommonString::ToString(oMeasureOperate->toValue());
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 4, oItem);
            //Controls
            const std::vector<MonsbControl > & controls = oMeasureOperate->controls();
            sText = QString("%1:").arg(controls.size());
            for (int i = 0; i < controls.size(); ++i)
            {
                const MonsbControl& control = controls[i];
                const MonsbMeasure* oMonsb = control.measure;
                QString sMeasureName = QtCommonString::gbkToQString( oMonsb->measureName() );
                int iFromValue = control.fromValue;
                int iToValue = control.toValue;
                sText += QString("(%1=%2 - %3)").arg(sMeasureName).arg(iFromValue).arg(iToValue);
            }
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 5, oItem);
            //Cancels
            const std::vector<MonsbControl > & cancels = oMeasureOperate->controls();
            sText = QString("%1:").arg(cancels.size());
            for (int i = 0; i < cancels.size(); ++i)
            {
                const MonsbControl& cancel = cancels[i];
                const MonsbMeasure* oMonsb = cancel.measure;
                QString sMeasureName = QtCommonString::gbkToQString( oMonsb->measureName() );
                int iFromValue = cancel.fromValue;
                int iToValue = cancel.toValue;
                sText += QString("(%1=%2 - %3)").arg(sMeasureName).arg(iFromValue).arg(iToValue);
            }
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRow, 6, oItem);
        }
    }
}

void TaskWin::reportMonsbControlTaskManager(QTableWidget *oTableWidget)
{
    QTableWidget* oGrid = oTableWidget;
    const vector<MonsbControlTask*>* oTasks = MonsbControlTaskManager::tasks();
    int iRowCount = oTasks->size();
    oGrid->setRowCount(iRowCount);
    QStringList sHeaders;
    sHeaders.append("MonsbType");
    sHeaders.append("MeasureId");
    sHeaders.append("MeasureName");
    sHeaders.append("FromValue");
    sHeaders.append("ToValue");
    sHeaders.append("Controls");
    sHeaders.append("Cancels");
    sHeaders.append("RunMonsb");
    sHeaders.append("RunToValue");
    sHeaders.append("RunIndex");
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    for (int i = 0; i < oTasks->size(); ++i)
    {
        MonsbControlTask* oTask = oTasks->at(i);
        const MonsbMeasure* oMonsb = oTask->measure();
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
        //FromValue
        sText = QtCommonString::ToString(oTask->fromValue());
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 3, oItem);
        //ToValue
        sText = QtCommonString::ToString(oTask->toValue());
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 4, oItem);
        //s control
        const vector<MonsbControl >& controls = oTask->controls();
        sText = QString("%1:").arg(controls.size());
        for (int i = 0; i < controls.size(); ++i)
        {
            const MonsbControl& control = controls[i];
            const MonsbMeasure* oMonsb = control.measure;
            QString sMeasureName = QtCommonString::gbkToQString( oMonsb->measureName() );
            int iFromValue = control.fromValue;
            int iToValue = control.toValue;
            sText += QString("(%1=%2 - %3)").arg(sMeasureName).arg(iFromValue).arg(iToValue);
        }
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 5, oItem);
        //s cancel
        const vector<MonsbControl >& cancels = oTask->cancels();
        sText = QString("%1:").arg(cancels.size());
        for (int i = 0; i < cancels.size(); ++i)
        {
            const MonsbControl& cancel = cancels[i];
            const MonsbMeasure* oMonsb = cancel.measure;
            QString sMeasureName = QtCommonString::gbkToQString( oMonsb->measureName() );
            int iFromValue = cancel.fromValue;
            int iToValue = cancel.toValue;
            sText += QString("(%1=%2 - %3)").arg(sMeasureName).arg(iFromValue).arg(iToValue);
        }
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 6, oItem);
        //RunMonsb
        const MonsbMeasure* oCurrentMonsb = oTask->currentRunMeasure();
        sText = QtCommonString::gbkToQString( (oCurrentMonsb) ? oCurrentMonsb->measureName() : "NULL" );
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 7, oItem);
        //RunToValue
        sText = QtCommonString::ToString(oTask->currentRunToValue());
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 8, oItem);
        //RunIndex
        sText = QtCommonString::ToString(oTask->controlIndex());
        oItem = new QTableWidgetItem(sText);
        oGrid->setItem(iRow, 9, oItem);
    }
}
