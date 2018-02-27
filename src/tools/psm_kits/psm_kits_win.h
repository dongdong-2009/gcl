#ifndef PSM_KITS_WIN_H
#define PSM_KITS_WIN_H

#include <QMainWindow>

namespace Ui {
class PsmKitsWin;
}

class PsmKitsWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit PsmKitsWin(QWidget *parent = 0);
    ~PsmKitsWin();

private:
    Ui::PsmKitsWin *ui;
};

#endif // PSM_KITS_WIN_H
