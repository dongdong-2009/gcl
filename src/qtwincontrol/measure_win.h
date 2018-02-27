#ifndef MEASURE_WIN_H
#define MEASURE_WIN_H


#include <QString>
#include <QList>
#include <QTableWidget>

#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>
#include <vdi/straw_address_worker.h>


class MeasureWin
{
public:
    static void reportMonsbManager(QTableWidget* oTableWidget);

    static void reportYcaddManager(QTableWidget* oTableWidget);

    static void reportStrawManager(QTableWidget* oTableWidget);

    static void reportMonsbWorkerManager(QTableWidget* oTableWidget, const MonsbAddressWorkerTable * oWorkerTable);

    static void reportYcaddWorkerManager(QTableWidget* oTableWidget, const YcaddAddressWorkerTable * oWorkerTable);

    static void reportStrawWorkerManager(QTableWidget* oTableWidget, const StrawAddressWorkerTable * oWorkerTable);

};

#endif // MEASURE_WIN_H
