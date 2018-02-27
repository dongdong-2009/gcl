#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::outInfo(const std::string &sInfo)
{
    ui->plainTextEdit->appendPlainText(QtCommonString::gbkToQString(sInfo));
}

void MainWindow::outInfo(const QString &sInfo)
{
    ui->plainTextEdit->appendPlainText((sInfo));
}
