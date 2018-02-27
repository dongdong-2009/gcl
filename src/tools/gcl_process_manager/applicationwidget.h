#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H
#include "global.h"
#include "processer.h"
#include "qthread.h"
#include "revisedialog.h"
#include "sharememory.h"
#include <QWidget>
#include <QTableWidget>
#include <QDialog>

class QPushButton;
class QCheckBox;


class ApplicationsTab : public QWidget
{
    Q_OBJECT

public slots:
    void checkChange();
    void showMessage(QString);

    void slot_showMax();
    void slot_showMin();
    void slot_hide();
    void slot_show();
    void slot_killProcess();
    void slot_choseTableTitle();
    void slot_createProcess();

    void on_customContextMenuRequested(QPoint pos);

public:
    ApplicationsTab(QWidget *parent = 0);

    void exeNameItem();
    std::string getProcessName();
    void createShareMemoryThread();
    bool createSocketThread();

    QTableWidget *processTable;
    QPushButton *killProcessButton;

    static string getMessage();
    static void clearText();



private:
    QCheckBox *checkbox;
    QTableWidgetItem *curItem;
    MyThread *m_thread;
    QMenu *rightMenu;
    QAction *action_choseTableTitle;
    QAction *action_createProcess;
    QAction *action_killProcess;
    QAction *action_showMax;
    QAction *action_showMin;
    QAction *action_hide;
    QAction *action_show;
    QStringList tableHeaders;
};



#endif // APPLICATIONWIDGET_H
