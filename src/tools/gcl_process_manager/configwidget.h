#ifndef TABDIALOG_H
#define TABDIALOG_H
#include "global.h"
#include <QDialog>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include "revisedialog.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QPushButton;
class QLabel;
class ReviseDialog;
QT_END_NAMESPACE

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = 0);
private:
public slots:
};


class ConfigTab : public QWidget
{
    Q_OBJECT

public:
    ConfigTab(QWidget *parent = 0);
    static int num;
    static QString qstrname;
    QTableWidget *process_declareTable;
    QTableWidget *process_paramTable;
    QTableWidgetItem *curItem;
    void CreateTree();
    void ProHeadMessagePage();
    void ProMessagePage();
    std::string getNodeAttribute();


public slots:
    void showSelectedMessage(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_customContextMenuRequested(QPoint pos);
    void slot_revise();
    void slot_delete();
    void slot_addnode();
    //void showMeaasge();


private:
    int processSignNum;
    QMenu *rightClickMenu;
    QAction *action_revise;
    QAction *action_delete;
    QAction *action_addnode;
    QTreeWidget *treeWidget;
    QStackedWidget *pagesWidget;

};

#endif // TABDIALOG_H
