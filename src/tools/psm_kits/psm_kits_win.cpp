#include "psm_kits_win.h"
#include "ui_psm_kits_win.h"

PsmKitsWin::PsmKitsWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PsmKitsWin)
{
    ui->setupUi(this);
}

PsmKitsWin::~PsmKitsWin()
{
    delete ui;
}
