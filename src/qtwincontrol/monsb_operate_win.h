#ifndef MONSB_OPERATE_WIN_H
#define MONSB_OPERATE_WIN_H

#include <QDialog>
#include <QTableWidget>

#include <vdi/monsb_control.h>

namespace Ui {
    class MonsbOperateWin;
}

class MonsbOperateWin : public QDialog
{
    Q_OBJECT

public:
    explicit MonsbOperateWin(QWidget *parent = 0);
    ~MonsbOperateWin();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MonsbOperateWin *ui;

    static void refreshMonsbOperates(QTableWidget *oTableWidget, const std::vector<MonsbOperate*>& oMeasureOperates);

    static void refreshMeasureControls(QTableWidget* oTableWidget, const std::vector<MonsbControl >& controls);

    static void refreshMeasureControl(QTableWidget* oTableWidget, const MonsbControl& control, int iRow);

    static std::vector<MonsbControl > getControls(QTableWidget* oTableWidget);

    static void refreshMonsbSelects(QTableWidget* oTableWidget, const std::vector<MonsbMeasure*>& monsbs);

    static void insertMonsbControls(QTableWidget* oSelectsTv, QTableWidget* oControlsTv);

    static void appendMonsbControls(QTableWidget* oSelectsTv, QTableWidget* oControlsTv);

    static void insertMonsbControls(QTableWidget* oSelectsTv, QTableWidget* oControlsTv, int iRow);

    static void insertPairControls(QTableWidget* oSelectsTv, QTableWidget* oControlsTv);

    const MonsbMeasure* monsbOperateData(QTableWidget* oTableWidget, int row, int& iFromValue, int& iToValue);

    void saveCurrentMonsbOperate();

private slots:
    void monsbOperatesCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_MonsbTypeOperatesCb_currentIndexChanged(int index);

    void on_MonsbTypeSelectsCb_currentIndexChanged(int index);

    void on_ControlsDefaultBn_clicked();

    void on_ControlsInsertMultiBn_clicked();

    void on_ControlsAddMultiBn_clicked();

    void on_ControlsAddPairBn_clicked();

    void on_CancelsDefaultBn_clicked();

    void on_CancelsInsertMultiBn_clicked();

    void on_CancelsAddMultiBn_clicked();

    void on_CancelsAddPairBn_clicked();

};

#endif // MONSB_OPERATE_WIN_H
