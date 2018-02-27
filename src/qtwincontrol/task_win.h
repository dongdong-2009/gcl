#ifndef TASK_WIN_H
#define TASK_WIN_H

#include <QString>
#include <QList>
#include <QTableWidget>


class TaskWin
{
public:
    static void reportMonsbOperateManager(QTableWidget *oTableWidget);

    static void reportMonsbControlTaskManager(QTableWidget *oTableWidget);

};
#endif // TASK_WIN_H
