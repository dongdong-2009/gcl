#ifndef PRAMANAGERWIN_H
#define PRAMANAGERWIN_H

#include <QtGui>
#include <QMdiArea>
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
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QProgressDialog>

#include "pra.h"

namespace Ui {
class PraManagerWin;
}

class PraManagerWin : public QWidget
{
    Q_OBJECT

public:
    explicit PraManagerWin(QWidget *parent = 0);
    ~PraManagerWin();

protected:
    bool eventFilter(QObject *, QEvent *);

    void closeEvent(QCloseEvent *);

private:
    Ui::PraManagerWin *ui;

private slots:
    void on_prasTw_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void customContextMenu(const QPoint &pos);

    void test1Slot();

    void test2Slot();

    void test3Slot();

    void on_newBn_clicked();

    void on_editBn_clicked();

    void on_saveBn_clicked();

    void on_restoreBn_clicked();

    void on_insertFdcBn_clicked();

private:
    QMenu* _tableMenu;
    QAction* _test1Action;
    QAction* _test2Action;
    QAction* _test3Action;

    int _currentRow;
    int _currentState;

    std::string _defaultIp;

public:

protected:
    void deletePrasBySelectRows();

    void deletePraItemsBySelectRows();

private:
    void setCurrentRow(int iRow);

    void setCurrentState(int iState);

    void newArea();

    void addArea(const std::string & sAreaName);

    void addFdcsToCurrentProduct();

    bool addFdcToProduct(const std::string &sFdc, const Pra * oPra);

    void addFileInfo(const std::vector<std::string> & sPaths);

    std::string getAreaNameByRow(int iRow);

    std::string getAreaRpathByRow(int iRow);

    Pra * getPraByRow(int iRow);

    std::string getFdcNameByRow(int iRow);

    std::vector<PraItem> getPraItems(QTableWidget * oGrid);

    void refreshPrasTitle();

    int editViewIn(const std::string & sName);

    int editViewOut(const std::string & sName);

    int rowByPraItemPath(const std::string & sPath);

};

#endif // PRAMANAGERWIN_H
