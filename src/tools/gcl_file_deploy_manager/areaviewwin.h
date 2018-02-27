#ifndef AREAVIEWWIN_H
#define AREAVIEWWIN_H

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

class QDragEnterEvent;
class QDropEvent;


namespace Ui {
class areaviewwin;
}

class AreaViewWin : public QWidget
{
    Q_OBJECT

public:
    explicit AreaViewWin(QWidget *parent = 0);
    ~AreaViewWin();

    void drawLineTest();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::areaviewwin *ui;
};

#endif // AREAVIEWWIN_H
