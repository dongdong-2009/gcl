#ifndef MEASUREWIN_H
#define MEASUREWIN_H


#include "sdk_common.h"

#include <QString>
#include <QList>
#include <QTableWidget>


class MeasureWin
{
public:
    static void reportMonsbManager(QTableWidget* oTableWidget, const std::vector<yx_t> & yxs);

    static void reportYcaddManager(QTableWidget* oTableWidget, const std::vector<yc_t> & ycs);

    static void reportStrawManager(QTableWidget* oTableWidget, const std::vector<yw_t> & yws);

};


void fn_setItemValue16(QTableWidgetItem* oItem, const int & value);

void fn_setItemValue(QTableWidgetItem* oItem, const int & value);

void fn_setItemValue(QTableWidgetItem* oItem, const uint & value);

void fn_setItemValue(QTableWidgetItem* oItem, const double & value);

void fn_setItemValue(QTableWidgetItem* oItem, const long long & value);

void fn_setItemValue(QTableWidgetItem* oItem, const char * value);

void fn_setItemValue(QTableWidgetItem* oItem, const QString & value);

void fn_setItemValueDateTime(QTableWidgetItem* oItem, const long long & value);



#endif // MEASUREWIN_H
