#include "sdk_measure_win.h"

void fn_setItemValue16(QTableWidgetItem* oItem, const int & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toInt() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QString::number(value, 16));
    }
}

void fn_setItemValue(QTableWidgetItem* oItem, const int & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toInt() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QString::number(value));
    }
}

void fn_setItemValue(QTableWidgetItem* oItem, const uint & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toInt() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QString::number(value));
    }
}

void fn_setItemValue(QTableWidgetItem* oItem, const double & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toDouble() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QString::number(value));
    }
}

void fn_setItemValue(QTableWidgetItem* oItem, const long long & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toLongLong() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QString::number(value));
    }
}

void fn_setItemValue(QTableWidgetItem* oItem, const char * value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toByteArray() != value)
    {
        QByteArray pData(value);
        oItem->setData(Qt::UserRole, pData);
        oItem->setText(SdkCommon::gbkToQString(value));
    }
}

void fn_setItemValue(QTableWidgetItem* oItem, const QString & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toString() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(value);
    }
}

void fn_setItemValueDateTime(QTableWidgetItem* oItem, const long long & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toLongLong() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QDateTime::fromMSecsSinceEpoch(value).toString(cs_datetime_format_));
    }
}

void MeasureWin::reportMonsbManager(QTableWidget *oTableWidget, const std::vector<yx_t> & yxs)
{
    QTableWidget* oGrid = oTableWidget;
    int iRowCount = yxs.size();
    if (iRowCount>512) iRowCount = 512;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("No");
        sHeaders.append("MeasureId");
        sHeaders.append("Value");
        sHeaders.append("Quality");
        sHeaders.append("Time");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
        for (size_t i = 0; i < yxs.size(); ++i)
        {
            if (i>iRowCount) break;
            const yx_t & yx = yxs.at(i);
            int iRow = i;
            QTableWidgetItem* oItem;
            //row
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 0, oItem);
            fn_setItemValue(oItem, i+1);
            //address
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 1, oItem);
            fn_setItemValue(oItem, yx.address);
            //state
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 2, oItem);
            fn_setItemValue(oItem, yx.state);
            //quality
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 3, oItem);
            fn_setItemValue(oItem, yx.quality);
            //datetime
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 4, oItem);
            fn_setItemValue(oItem, QDateTime::fromMSecsSinceEpoch(yx.datetime).toString(cs_datetime_format_));
        }
    }
    for (size_t i = 0; i < yxs.size(); ++i)
    {
        if (i>iRowCount) break;
        const yx_t & yx = yxs.at(i);
        QTableWidgetItem* oItem;

        oItem = oGrid->item(i, 1);
        fn_setItemValue(oItem, yx.address);

        oItem = oGrid->item(i, 2);
        fn_setItemValue(oItem, yx.state);

        oItem = oGrid->item(i, 3);
        fn_setItemValue(oItem, yx.quality);

        oItem = oGrid->item(i, 4);
        fn_setItemValue(oItem, QDateTime::fromMSecsSinceEpoch(yx.datetime).toString(cs_datetime_format_));
    }
}

void MeasureWin::reportYcaddManager(QTableWidget *oTableWidget, const std::vector<yc_t> & ycs)
{
    QTableWidget* oGrid = oTableWidget;
    int iRowCount = ycs.size();
    if (iRowCount>512) iRowCount = 512;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
		sHeaders.append("No");
		sHeaders.append("MeasureId");
		sHeaders.append("Value");
		sHeaders.append("Quality");
		sHeaders.append("Time");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
        for (size_t i = 0; i < ycs.size(); ++i)
        {
            if (i>iRowCount) break;
            const yc_t & yc = ycs.at(i);
            int iRow = i;
            QTableWidgetItem* oItem;
            //row
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 0, oItem);
            fn_setItemValue(oItem, i+1);
            //address
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 1, oItem);
            fn_setItemValue(oItem, yc.address);
            //state
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 2, oItem);
            fn_setItemValue(oItem, yc.value);
            //quality
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 3, oItem);
            fn_setItemValue(oItem, yc.quality);
            //datetime
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 4, oItem);
            fn_setItemValue(oItem, QDateTime::fromMSecsSinceEpoch(yc.datetime).toString(cs_datetime_format_));
        }
    }
    for (size_t i = 0; i < ycs.size(); ++i)
    {
        if (i>iRowCount) break;
        const yc_t & yc = ycs.at(i);
        QTableWidgetItem* oItem;

        oItem = oGrid->item(i, 1);
        fn_setItemValue(oItem, yc.address);

        oItem = oGrid->item(i, 2);
        fn_setItemValue(oItem, yc.value);

        oItem = oGrid->item(i, 3);
        fn_setItemValue(oItem, yc.quality);

        oItem = oGrid->item(i, 4);
        fn_setItemValue(oItem, QDateTime::fromMSecsSinceEpoch(yc.datetime).toString(cs_datetime_format_));
    }
}

void MeasureWin::reportStrawManager(QTableWidget *oTableWidget, const std::vector<yw_t> & yws)
{
    QTableWidget* oGrid = oTableWidget;
    int iRowCount = yws.size();
    if (iRowCount>512) iRowCount = 512;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
		sHeaders.append("No");
		sHeaders.append("MeasureId");
		sHeaders.append("Value");
		sHeaders.append("Quality");
		sHeaders.append("Time");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(4, 150);
        for (size_t i = 0; i < yws.size(); ++i)
        {
            if (i>iRowCount) break;
            const yw_t & yw = yws.at(i);
            int iRow = i;
            QTableWidgetItem* oItem;
            //row
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 0, oItem);
            fn_setItemValue(oItem, i+1);
            //address
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 1, oItem);
            fn_setItemValue(oItem, yw.address);
            //state
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 2, oItem);
            fn_setItemValue(oItem, yw.value);
            //quality
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 3, oItem);
            fn_setItemValue(oItem, yw.quality);
            //datetime
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRow, 4, oItem);
            fn_setItemValue(oItem, QDateTime::fromMSecsSinceEpoch(yw.datetime).toString(cs_datetime_format_));
        }
    }
    for (size_t i = 0; i < yws.size(); ++i)
    {
        if (i>iRowCount) break;
        const yw_t & yw = yws.at(i);
        QTableWidgetItem* oItem;

        oItem = oGrid->item(i, 1);
        fn_setItemValue(oItem, yw.address);

        oItem = oGrid->item(i, 2);
        fn_setItemValue(oItem, yw.value);

        oItem = oGrid->item(i, 3);
        fn_setItemValue(oItem, yw.quality);

        oItem = oGrid->item(i, 4);
        fn_setItemValue(oItem, QDateTime::fromMSecsSinceEpoch(yw.datetime).toString(cs_datetime_format_));
    }
}

