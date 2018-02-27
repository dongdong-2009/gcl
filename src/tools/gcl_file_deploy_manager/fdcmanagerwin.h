#ifndef FDCMANAGERWIN_H
#define FDCMANAGERWIN_H

#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>
#include <QDialog>
#include <QLabel>

#include "fdc.h"

namespace Ui {
class FdcManagerWin;
}

class FdcManagerWin : public QWidget
{
    Q_OBJECT

public:
    explicit FdcManagerWin(QWidget *parent = 0);
    ~FdcManagerWin();

    static std::vector<std::string> selectFdcs(int iMaxSelectCount = GM_INT_MAX);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    bool eventFilter(QObject *, QEvent *);

    void closeEvent(QCloseEvent *);

private:
    Ui::FdcManagerWin *ui;

private slots:
    void on_fdcsTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void customContextMenu(const QPoint &pos);

    void test1Slot();

    void test2Slot();

    void test3Slot();

    void on_newBn_clicked();

    void on_editBn_clicked();

    void on_saveBn_clicked();

    void on_restoreBn_clicked();

    void on_quickBn_clicked();

private:
    QMenu* _tableMenu;
    QAction* _test1Action;
    QAction* _test2Action;
    QAction* _test3Action;

    int _currentRow;
    int _currentState;

public:

protected:
    void deleteFdcsBySelectRows();

    void deleteFdcItemsBySelectRows();

private:
    void setCurrentRow(int iRow);

    void setCurrentState(int iState);

    void newProduct();

    void addProduct(const std::string & sPath);

    void addFileInfo(const std::vector<std::string> & sPaths);

    std::string getNameByRow(int iRow);

    std::string getRpathByRow(int iRow);

    Fdc * getFdcByRow(int iRow);

    void refreshFdcsTitle();

    int editViewIn(const std::string & sName);

    int editViewOut(const std::string & sName);

    int rowByfdcItemPath(const std::string & sPath);

};

#endif // FDCMANAGERWIN_H
