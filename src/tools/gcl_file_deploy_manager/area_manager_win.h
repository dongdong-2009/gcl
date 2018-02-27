#ifndef AREA_MANAGER_WIN_H
#define AREA_MANAGER_WIN_H

#include <QWidget>


class QDragEnterEvent;
class QDropEvent;


namespace Ui {
class area_manager_win;
}

class area_manager_win : public QWidget
{
    Q_OBJECT

public:
    explicit area_manager_win(QWidget *parent = 0);
    ~area_manager_win();

    void drawLineTest();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::area_manager_win *ui;
};

#endif // AREA_MANAGER_WIN_H
