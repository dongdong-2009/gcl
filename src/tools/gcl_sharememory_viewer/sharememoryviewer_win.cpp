#include "sharememoryviewer_win.h"
#include "ui_sharememoryviewer_win.h"


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

void fn_setItemValue(QTableWidgetItem* oItem, const double & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toDouble() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QString::number(value, 'f', 3));
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
        oItem->setText(QtCommonString::ToString(value));
    }
}

void fn_setItemValueDateTime(QTableWidgetItem* oItem, const long long & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toLongLong() != value)
    {
        oItem->setData(Qt::UserRole, value);
        oItem->setText(QtCommonString::ToString(value));
    }
}


void fn_setMeasureValue(QTableWidgetItem* oItem, const int & value)
{
    GM_INVALID_RETURE(oItem);
    QVariant data = oItem->data(Qt::UserRole);
    if (data.type() == QVariant::Invalid || data.toInt() != value)
    {
        switch (value)
        {
        case 1:
            //分
            oItem->setBackgroundColor( QColor(Qt::green) );
            break;
        case 2:
            //合上
            oItem->setBackgroundColor( QColor(Qt::red) );
            break;
        case 3:
            //异常
            oItem->setBackgroundColor( QColor(Qt::yellow) );
            break;
        case 4:
            //故障
            oItem->setBackgroundColor( QColor(Qt::blue) );
            break;
        default:
            oItem->setBackgroundColor( QColor(Qt::gray) );
            break;
        }
        fn_setItemValue(oItem, value);
    }
}

void fn_setMeasureValue(QTableWidgetItem* oItem, const double & value)
{
    GM_INVALID_RETURE(oItem);
    fn_setItemValue(oItem, value);
}

void fn_setMeasureValue(QTableWidgetItem* oItem, const StrawValue & value)
{
    GM_INVALID_RETURE(oItem);
    fn_setItemValue(oItem, value.value);
}



template<typename T>
void fn_refreshMeasures(typename MeasureShareMemory<T>::MemoryManagerBase * oMemoryManager, QTableWidget * oTableWidget, int iMeasureId1 = -1, int iMeasureId2 = -1)
{
    GM_INVALID_RETURE(oMemoryManager->isValid());

    /*
        int measureId;
        T value; // 量值
        int quality; // 值的品质
        msepoch_t refreshTime; // 8
        msepoch_t changedTime; // 8
        int refreshSourceId;
        int changedSourceId; // 值的修改时间
        int refreshReasonId;
        int changedReasonId; // 修改值的原因
        int outLineInterval;
        int res;
     */
    //量的相关数据
    int iRowCount;
    size_t * oMeasuresIndex;
    size_t * oMeasuresIndexEnd;

    if (iMeasureId1 != -1 && iMeasureId2 != -1)
    {
        size_t iIndex1 = iMeasureId1 - oMemoryManager->getMinId();
        size_t iIndex2 = iMeasureId2 - oMemoryManager->getMinId();
        GM_INVALID_RETURE(iIndex1 <= iIndex2);
        GM_INVALID_RETURE(iIndex1 < oMemoryManager->measureCount());
        if (iIndex2 >= oMemoryManager->measureCount()) iIndex2 = oMemoryManager->measureCount();
        oMeasuresIndex = oMemoryManager->measuresIndexBegin()+iIndex1;
        oMeasuresIndexEnd = oMemoryManager->measuresIndexBegin()+iIndex2;
        iRowCount = iIndex2-iIndex1;
    }
    else
    {
        oMeasuresIndex = oMemoryManager->measuresIndexBegin();
        oMeasuresIndexEnd = oMemoryManager->measuresIndexEnd();
        iRowCount = oMemoryManager->measureCount();
    }
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = 0;
    typename MeasureShareMemory<T>::MeasureData * oMeasureDataBegin = oMemoryManager->measuresDataBegin();
    typename MeasureShareMemory<T>::MeasureData measureData;
    //首次刷新UI数据
    QString sText;
    QTableWidgetItem* oItem;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->clear();
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("MeasureId");
        sHeaders.append("值");
        sHeaders.append("品质");
        sHeaders.append("修改时间");
        sHeaders.append("修改源ID");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(2, 50);
        oGrid->setColumnWidth(3, 130);
        while (oMeasuresIndex < oMeasuresIndexEnd)
        {
            measureData = *(oMeasureDataBegin + (*oMeasuresIndex));
            //MeasureId
            oItem = new QTableWidgetItem(sText);
            oGrid->setItem(iRowIndex, 0, oItem);
            fn_setItemValue16(oItem, measureData.measureId);
            //值
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 1, oItem);
            fn_setMeasureValue(oItem, measureData.value);
            //品质
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 2, oItem);
            fn_setItemValue(oItem, measureData.quality);
            //修改时间
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 3, oItem);
            fn_setItemValueDateTime(oItem, measureData.changedTime);
            //修改源ID
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 4, oItem);
            fn_setItemValue16(oItem, measureData.changedSourceId);

            oMeasuresIndex++;
            iRowIndex++;
        }
        oMeasuresIndex = oMemoryManager->measuresIndexBegin();
        iRowIndex = 0;
    }
    while (oMeasuresIndex < oMeasuresIndexEnd)
    {
        measureData = *(oMeasureDataBegin + (*oMeasuresIndex));
        //MeasureId
        oItem = oGrid->item(iRowIndex, 0);
        fn_setItemValue16(oItem, measureData.measureId);
        //值
        oItem = oGrid->item(iRowIndex, 1);
        fn_setMeasureValue(oItem, measureData.value);
        //品质
        oItem = oGrid->item(iRowIndex, 2);
        fn_setItemValue(oItem, measureData.quality);
        //修改时间
        oItem = oGrid->item(iRowIndex, 3);
        fn_setItemValueDateTime(oItem, measureData.changedTime);
        //修改源ID
        oItem = oGrid->item(iRowIndex, 4);
        fn_setItemValue16(oItem, measureData.changedSourceId);
        oMeasuresIndex++;
        iRowIndex++;
    }
}

template<typename T>
void fn_refreshChangeds(typename MeasureShareMemory<T>::MemoryManagerBase * oMemoryManager, QTableWidget * oTableWidget, size_t iIndex1 = UINT_MAX, size_t iIndex2 = UINT_MAX)
{
    GM_INVALID_RETURE(oMemoryManager->isValid());

    /*
        int changedId; // 变化ID : 1,2,3...0x7fffffff
        int measureId; // 量ID
        T value; // 量值
        int quality; // 值的品质
        msepoch_t changedTime; // 值的修改时间
        int changedSourceId; // 修改值的源ID
        int changedReasonId; // 修改值的原因
        T oldValue; // 修改前的值
     */
    //量的相关数据
    typename MeasureShareMemory<T>::ChangedData * oChangedData;
    typename MeasureShareMemory<T>::ChangedData * oChangedDataEnd;
    int iRowCount;

    if (iIndex1 != UINT_MAX && iIndex2 != UINT_MAX)
    {
        GM_INVALID_RETURE(iIndex1<=iIndex2);
        GM_INVALID_RETURE(iIndex1<oMemoryManager->changedCount());
        if (iIndex2>=oMemoryManager->changedCount()) iIndex2 = oMemoryManager->changedCount()-1;
        oChangedData = oMemoryManager->changedDataBegin() + iIndex1;
        oChangedDataEnd = oMemoryManager->changedDataBegin() + iIndex2;
        iRowCount = iIndex2-iIndex1;
    }
    else
    {
        oChangedData = oMemoryManager->changedDataBegin();
        oChangedDataEnd = oMemoryManager->changedDataEnd();
        iRowCount = oMemoryManager->changedCount();
    }
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = 0;
    //首次刷新UI数据
    QString sText;
    QTableWidgetItem* oItem;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->clear();
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("changedId");
        sHeaders.append("MeasureId");
        sHeaders.append("值");
        sHeaders.append("品质");
        sHeaders.append("修改时间");
        sHeaders.append("修改源ID");
        sHeaders.append("旧值");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        while (oChangedData < oChangedDataEnd)
        {
            //changedId
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 0, oItem);
            fn_setItemValue(oItem, oChangedData->changedId);
            //MeasureId
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 1, oItem);
            fn_setItemValue16(oItem, oChangedData->measureId);
            //值
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 2, oItem);
            fn_setMeasureValue(oItem, oChangedData->value);
            //品质
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 3, oItem);
            fn_setItemValue(oItem, oChangedData->quality);
            //修改时间
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 4, oItem);
            fn_setItemValueDateTime(oItem, oChangedData->changedTime);
            //修改源ID
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 5, oItem);
            fn_setItemValue16(oItem, oChangedData->changedSourceId);
            //旧值
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 6, oItem);
            fn_setMeasureValue(oItem, oChangedData->oldValue);

            oChangedData++;
            iRowIndex++;
        }
        oChangedData = oMemoryManager->changedDataBegin();
        iRowIndex = 0;
    }
    while (oChangedData < oChangedDataEnd)
    {
        //changedId
        oItem = oGrid->item(iRowIndex, 0);
        fn_setItemValue(oItem, oChangedData->changedId);
        //MeasureId
        oItem = oGrid->item(iRowIndex, 1);
        fn_setItemValue16(oItem, oChangedData->measureId);
        //值
        oItem = oGrid->item(iRowIndex, 2);
        fn_setMeasureValue(oItem, oChangedData->value);
        //品质
        oItem = oGrid->item(iRowIndex, 3);
        fn_setItemValue(oItem, oChangedData->quality);
        //修改时间
        oItem = oGrid->item(iRowIndex, 4);
        fn_setItemValueDateTime(oItem, oChangedData->changedTime);
        //修改源ID
        oItem = oGrid->item(iRowIndex, 5);
        fn_setItemValue16(oItem, oChangedData->changedSourceId);
        //旧值
        oItem = oGrid->item(iRowIndex, 6);
        fn_setMeasureValue(oItem, oChangedData->oldValue);
        oChangedData++;
        iRowIndex++;
    }
}

template<typename T>
void fn_refreshControls(typename MeasureShareMemory<T>::MemoryManagerBase * oMemoryManager, QTableWidget * oTableWidget)
{
    GM_INVALID_RETURE(oMemoryManager->isValid());

    /*
        int controlId; // 变化ID : 1,2,3...0x7fffffff
        int measureId; // 量ID
        T value; // 控制值
        msepoch_t controlTime; // 值的控制时间
        int controlSourceId; // 控制的源ID
        int controlReasonId; // 控制的原因
        int controlStrategyId; // 控制的策略
        int controlMethodId; // 控制的方法
        char params[512];
        int res; // 保留
     */
    //量的相关数据
    typename MeasureShareMemory<T>::ControlData * oControlDataEnd = oMemoryManager->controlDataEnd();
    typename MeasureShareMemory<T>::ControlData * oControlData = oMemoryManager->controlDataBegin();
    QTableWidget * oGrid = oTableWidget;
    int iRowCount = oMemoryManager->controlCount();
    int iRowIndex = 0;
    //首次刷新UI数据
    QString sText;
    QTableWidgetItem* oItem;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->clear();
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("controlId");
        sHeaders.append("MeasureId");
        sHeaders.append("控制值");
        sHeaders.append("控制时间");
        sHeaders.append("源ID");
        sHeaders.append("转发数量");
        sHeaders.append("参数");
        sHeaders.append("原因");
        sHeaders.append("策略");
        sHeaders.append("方法");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        oGrid->setColumnWidth(0, 70);
        oGrid->setColumnWidth(1, 70);
        oGrid->setColumnWidth(2, 100);
        oGrid->setColumnWidth(3, 125);
        oGrid->setColumnWidth(4, 60);
        oGrid->setColumnWidth(5, 60);
        oGrid->setColumnWidth(6, 80);
        oGrid->setColumnWidth(7, 60);
        oGrid->setColumnWidth(8, 60);
        oGrid->setColumnWidth(9, 60);
        while (oControlData < oControlDataEnd)
        {
            //controlId
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 0, oItem);
            fn_setItemValue(oItem, oControlData->controlId);
            //MeasureId
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 1, oItem);
            fn_setItemValue16(oItem, oControlData->measureId);
            //控制值
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 2, oItem);
            fn_setMeasureValue(oItem, oControlData->value);
            //控制时间
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 3, oItem);
            fn_setItemValueDateTime(oItem, oControlData->controlTime);
            //源ID
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 4, oItem);
            fn_setItemValue16(oItem, oControlData->controlSourceId);
            //转发数量
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 5, oItem);
            fn_setItemValue(oItem, oControlData->transferCount);
            //参数
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 6, oItem);
            fn_setItemValue(oItem, oControlData->params);
            //原因
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 7, oItem);
            fn_setMeasureValue(oItem, oControlData->controlReasonId);
            //策略
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 8, oItem);
            fn_setMeasureValue(oItem, oControlData->controlStrategyId);
            //方法
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 9, oItem);
            fn_setMeasureValue(oItem, oControlData->controlMethodId);

            oControlData++;
            iRowIndex++;
        }
        oControlData = oMemoryManager->controlDataBegin();
        iRowIndex = 0;
    }
    while (oControlData < oControlDataEnd)
    {
        //controlId
        oItem = oGrid->item(iRowIndex, 0);
        fn_setItemValue(oItem, oControlData->controlId);
        //MeasureId
        oItem = oGrid->item(iRowIndex, 1);
        fn_setItemValue16(oItem, oControlData->measureId);
        //控制值
        oItem = oGrid->item(iRowIndex, 2);
        fn_setMeasureValue(oItem, oControlData->value);
        //控制时间
        oItem = oGrid->item(iRowIndex, 3);
        fn_setItemValueDateTime(oItem, oControlData->controlTime);
        //源ID
        oItem = oGrid->item(iRowIndex, 4);
        fn_setItemValue16(oItem, oControlData->controlSourceId);
        //源ID
        oItem = oGrid->item(iRowIndex, 5);
        fn_setItemValue(oItem, oControlData->transferCount);
        //参数
        oItem = oGrid->item(iRowIndex, 6);
        fn_setMeasureValue(oItem, oControlData->params);
        //原因
        oItem = oGrid->item(iRowIndex, 7);
        fn_setMeasureValue(oItem, oControlData->controlReasonId);
        //策略
        oItem = oGrid->item(iRowIndex, 8);
        fn_setMeasureValue(oItem, oControlData->controlStrategyId);
        //方法
        oItem = oGrid->item(iRowIndex, 9);
        fn_setMeasureValue(oItem, oControlData->controlMethodId);

        oControlData++;
        iRowIndex++;
    }
}

void fn_refreshFileInfos(FileInfoShareMemory * oFileInfoShareMemory, QTableWidget * oTableWidget, size_t iIndex1 = UINT_MAX, size_t iIndex2 = UINT_MAX)
{
    GM_INVALID_RETURE(oFileInfoShareMemory->isValid());

    /*
        int changedId; // 变化ID : 1,2,3...0x7fffffff
        int fileType;  //文件类型
        int64 fileSize;
        msepoch_t creationTime;
        msepoch_t lastWriteTime;
        int sourceId;
        int targetId;
        int saveStatus;
        int res[6];
        char filepath[128];
        char fileShell[192];
     */
    //量的相关数据
    FileInfoChangedData * oChangedData;
    FileInfoChangedData * oChangedDataEnd;
    int iRowCount;

    if (iIndex1 != UINT_MAX && iIndex2 != UINT_MAX)
    {
        GM_INVALID_RETURE(iIndex1<=iIndex2);
        GM_INVALID_RETURE(iIndex1<oFileInfoShareMemory->changedCount());
        if (iIndex2>=oFileInfoShareMemory->changedCount()) iIndex2 = oFileInfoShareMemory->changedCount()-1;
        oChangedData = oFileInfoShareMemory->changedDataBegin() + iIndex1;
        oChangedDataEnd = oFileInfoShareMemory->changedDataBegin() + iIndex2;
        iRowCount = iIndex2-iIndex1;
    }
    else
    {
        oChangedData = oFileInfoShareMemory->changedDataBegin();
        oChangedDataEnd = oFileInfoShareMemory->changedDataEnd();
        iRowCount = oFileInfoShareMemory->changedCount();
    }
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = 0;
    //首次刷新UI数据
    QString sText;
    QTableWidgetItem* oItem;
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->clear();
        oGrid->setRowCount(iRowCount);
        QStringList sHeaders;
        sHeaders.append("changedId");
        sHeaders.append("fileType");
        sHeaders.append("fileSize");
        sHeaders.append("创建时间");
        sHeaders.append("修改时间");
        sHeaders.append("源ID");
        sHeaders.append("目标ID");
        sHeaders.append("保存状态");
        sHeaders.append("filepath");
        sHeaders.append("fileShell");
        int iColumnCount = sHeaders.count();
        oGrid->setColumnCount(iColumnCount);
        oGrid->setHorizontalHeaderLabels(sHeaders);
        while (oChangedData < oChangedDataEnd)
        {
            //changedId
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 0, oItem);
            fn_setItemValue(oItem, oChangedData->changedId);
            //fileType
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 1, oItem);
            fn_setItemValue(oItem, oChangedData->fileType);
            //fileSize
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 2, oItem);
            fn_setItemValue(oItem, oChangedData->fileSize);
            //创建时间
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 3, oItem);
            fn_setItemValueDateTime(oItem, oChangedData->creationTime);
            //修改时间
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 4, oItem);
            fn_setItemValueDateTime(oItem, oChangedData->lastWriteTime);
            //源ID
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 5, oItem);
            fn_setItemValue16(oItem, oChangedData->sourceId);
            //目标ID
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 6, oItem);
            fn_setItemValue16(oItem, oChangedData->targetId);
            //保存状态
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 7, oItem);
            fn_setItemValue(oItem, oChangedData->saveStatus);
            //filepath
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 8, oItem);
            fn_setItemValue(oItem, oChangedData->filepath);
            //filepath
            oItem = new QTableWidgetItem();
            oGrid->setItem(iRowIndex, 9, oItem);
            fn_setItemValue(oItem, oChangedData->fileShell);

            oChangedData++;
            iRowIndex++;
        }
        oChangedData = oFileInfoShareMemory->changedDataBegin();
        iRowIndex = 0;
    }
    while (oChangedData < oChangedDataEnd)
    {
        //changedId
        oItem = oGrid->item(iRowIndex, 0);
        fn_setItemValue(oItem, oChangedData->changedId);
        //fileType
        oItem = oGrid->item(iRowIndex, 1);
        fn_setItemValue(oItem, oChangedData->fileType);
        //fileSize
        oItem = oGrid->item(iRowIndex, 2);
        fn_setItemValue(oItem, oChangedData->fileSize);
        //创建时间
        oItem = oGrid->item(iRowIndex, 3);
        fn_setItemValueDateTime(oItem, oChangedData->creationTime);
        //修改时间
        oItem = oGrid->item(iRowIndex, 4);
        fn_setItemValueDateTime(oItem, oChangedData->lastWriteTime);
        //源ID
        oItem = oGrid->item(iRowIndex, 5);
        fn_setItemValue16(oItem, oChangedData->sourceId);
        //目标ID
        oItem = oGrid->item(iRowIndex, 6);
        fn_setItemValue16(oItem, oChangedData->targetId);
        //保存状态
        oItem = oGrid->item(iRowIndex, 7);
        fn_setItemValue(oItem, oChangedData->saveStatus);
        //filepath
        oItem = oGrid->item(iRowIndex, 8);
        fn_setItemValue(oItem, oChangedData->filepath);
        //filepath
        oItem = oGrid->item(iRowIndex, 9);
        fn_setItemValue(oItem, oChangedData->fileShell);

        oChangedData++;
        iRowIndex++;
    }
}



ShareMemoryViewer_win::ShareMemoryViewer_win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShareMemoryViewer_win)
{
    ui->setupUi(this);

    _timerTimes = 0;
    _timer.init(this, 250);
    _timer.start();

    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 4);
    ui->splitter_2->setStretchFactor(0, 3);
    ui->splitter_2->setStretchFactor(1, 4);
    ui->splitter_3->setStretchFactor(0, 3);
    ui->splitter_3->setStretchFactor(1, 4);

    ui->tabWidget->setCurrentIndex(0);
}

ShareMemoryViewer_win::~ShareMemoryViewer_win()
{
    _timer.stop();
    delete ui;
}

void ShareMemoryViewer_win::timer_timeOut(const CxTimer *oTimer)
{
    int iOrder = _timerTimes % 5;
    switch (iOrder)
    {
    case 1:
    {
        refreshMonsbs();
    }
        break;
    case 2:
    {
        refreshYcadds();
    }
        break;
    case 3:
    {
        refreshStraws();
    }
        break;
    case 4:
    {
        refreshFileInfos();
    }
        break;
    default:
        break;
    }

    _timerTimes++;
}

void ShareMemoryViewer_win::refreshMonsbs()
{
    if (ui->yxFilterCb->isChecked())
        fn_refreshMeasures<int>(MonsbShareMemory::memoryManager(), ui->monsbDataTw, ui->yxFilterEd1->value(), ui->yxFilterEd2->value());
    else
        fn_refreshMeasures<int>(MonsbShareMemory::memoryManager(), ui->monsbDataTw);

    if (ui->yxFilterCb_2->isChecked())
        fn_refreshChangeds<int>(MonsbShareMemory::memoryManager(), ui->monsbChangedTw, ui->yxFilterEd1_2->value(), ui->yxFilterEd2_2->value());
    else
        fn_refreshChangeds<int>(MonsbShareMemory::memoryManager(), ui->monsbChangedTw);

    fn_refreshControls<int>(MonsbShareMemory::memoryManager(), ui->ykTw);
}

void ShareMemoryViewer_win::refreshYcadds()
{
    if (ui->ycFilterCb->isChecked())
        fn_refreshMeasures<double>(YcaddShareMemory::memoryManager(), ui->ycaddDataTw, ui->ycFilterEd1->value(), ui->ycFilterEd2->value());
    else
        fn_refreshMeasures<double>(YcaddShareMemory::memoryManager(), ui->ycaddDataTw);

    if (ui->ycFilterCb_2->isChecked())
        fn_refreshChangeds<double>(YcaddShareMemory::memoryManager(), ui->ycaddChangedTw, ui->ycFilterEd1_2->value(), ui->ycFilterEd2_2->value());
    else
        fn_refreshChangeds<double>(YcaddShareMemory::memoryManager(), ui->ycaddChangedTw);

    fn_refreshControls<double>(YcaddShareMemory::memoryManager(), ui->ytTw);
}

void ShareMemoryViewer_win::refreshStraws()
{
    if (ui->ywFilterCb->isChecked())
        fn_refreshMeasures<StrawValue>(StrawShareMemory::memoryManager(), ui->strawDataTw, ui->ywFilterEd1->value(), ui->ywFilterEd2->value());
    else
        fn_refreshMeasures<StrawValue>(StrawShareMemory::memoryManager(), ui->strawDataTw);

    if (ui->ywFilterCb_2->isChecked())
        fn_refreshChangeds<StrawValue>(StrawShareMemory::memoryManager(), ui->strawChangedTw, ui->ywFilterEd1_2->value(), ui->ywFilterEd2_2->value());
    else
        fn_refreshChangeds<StrawValue>(StrawShareMemory::memoryManager(), ui->strawChangedTw);

    fn_refreshControls<StrawValue>(StrawShareMemory::memoryManager(), ui->ywTw);
}

void ShareMemoryViewer_win::refreshFileInfos()
{
    if (ui->fileInfoFilterCb->isChecked())
        fn_refreshFileInfos(FileInfoShareMemory::memoryManager(), ui->fileInfoTw, ui->fileInfoFilterEd1_2->value(), ui->fileInfoFilterEd2_2->value());
    else
        fn_refreshFileInfos(FileInfoShareMemory::memoryManager(), ui->fileInfoTw);
}

void ShareMemoryViewer_win::on_yxFilterEd1_valueChanged(int arg1)
{
    ui->yxFilterCb->setChecked(false);
}

void ShareMemoryViewer_win::on_yxFilterEd2_valueChanged(int arg1)
{
    ui->yxFilterCb->setChecked(false);
}

void ShareMemoryViewer_win::on_yxFilterEd1_2_valueChanged(int arg1)
{
    ui->yxFilterCb_2->setChecked(false);
}

void ShareMemoryViewer_win::on_yxFilterEd2_2_valueChanged(int arg1)
{
    ui->yxFilterCb_2->setChecked(false);
}

void ShareMemoryViewer_win::on_ycFilterEd1_valueChanged(int arg1)
{
    ui->ycFilterCb->setChecked(false);
}

void ShareMemoryViewer_win::on_ycFilterEd2_valueChanged(int arg1)
{
    ui->ycFilterCb->setChecked(false);
}

void ShareMemoryViewer_win::on_ycFilterEd1_2_valueChanged(int arg1)
{
    ui->ycFilterCb_2->setChecked(false);
}

void ShareMemoryViewer_win::on_ycFilterEd2_2_valueChanged(int arg1)
{
    ui->ycFilterCb_2->setChecked(false);
}

void ShareMemoryViewer_win::on_ywFilterEd1_valueChanged(int arg1)
{
    ui->ywFilterCb->setChecked(false);
}

void ShareMemoryViewer_win::on_ywFilterEd2_valueChanged(int arg1)
{
    ui->ywFilterCb->setChecked(false);
}

void ShareMemoryViewer_win::on_ywFilterEd1_2_valueChanged(int arg1)
{
    ui->ywFilterCb_2->setChecked(false);
}

void ShareMemoryViewer_win::on_ywFilterEd2_2_valueChanged(int arg1)
{
    ui->ywFilterCb_2->setChecked(false);
}

void ShareMemoryViewer_win::on_yxFilterEd5_editTextChanged(const QString &arg1)
{
    outInfo(arg1);
}

void ShareMemoryViewer_win::outInfo(const std::string &sInfo)
{
    ui->plainTextEdit->appendPlainText(QtCommonString::gbkToQString(sInfo));
}

void ShareMemoryViewer_win::outInfo(const QString &sInfo)
{
    ui->plainTextEdit->appendPlainText((sInfo));
}

void ShareMemoryViewer_win::on_yxFilterEd5_currentTextChanged(const QString &arg1)
{
    outInfo(arg1);
}

void ShareMemoryViewer_win::on_yxFilterEd6_currentTextChanged(const QString &arg1)
{
    outInfo(arg1);
}
