#ifndef FDVMANAGERWIN_H
#define FDVMANAGERWIN_H

#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>

#include "fdv.h"

namespace Ui {
class FdvManagerWin;
}

class FdvManagerWin : public QWidget
{
    Q_OBJECT

public:
    explicit FdvManagerWin(QWidget *parent = 0);
    ~FdvManagerWin();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    bool eventFilter(QObject *, QEvent *);

    void closeEvent(QCloseEvent *);

private:
    Ui::FdvManagerWin *ui;

private slots:
    void on_fdvsTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void customContextMenu(const QPoint &pos);

    void test1Slot();

    void test2Slot();

    void test3Slot();

    void on_editBn_clicked();

    void on_saveBn_clicked();

    void on_restoreBn_clicked();

private:
    QMenu* _tableMenu;
    QAction* _test1Action;
    QAction* _test2Action;
    QAction* _test3Action;

    int _currentRow;
    int _currentState;

public:

protected:
    void deleteTableSelectRows();

private:
    void setCurrentRow(int iRow);

    void setCurrentState(int iState);

    void addFileInfo(const std::vector<std::string> & sFilePaths);

    std::string getMd5ByRow(int iRow);

    std::string getFileNameByRow(int iRow);

    Fdv * getFdvByRow(int iRow);

    void refreshFdvsTitle();

    int editViewIn(const std::string & sMd5);

    int editViewOut(const std::string & sMd5);

};

#endif // FDVMANAGERWIN_H
