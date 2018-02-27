#include "dialog_fdc_quick_input.h"
#include "ui_dialog_fdc_quick_input.h"

#include <ccxx/qtcommon.h>
#include <ccxx/cxstring.h>


using namespace std;


DialogFdcQuickInput::DialogFdcQuickInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFdcQuickInput)
{
    ui->setupUi(this);
}

DialogFdcQuickInput::~DialogFdcQuickInput()
{
    delete ui;
}

string DialogFdcQuickInput::getFdcQuickInput(const std::string & sDescribeProjectFileName, std::string &sVdiConfigFilePath)
{
    string r;
    if (1>0)
    {
        DialogFdcQuickInput dg;
        dg._describeProjectFileName = CxString::format("./describe/%s.project.ini", sDescribeProjectFileName.c_str());
        dg._vdiConfigFileName = CxString::format("./config/%s.config.ini", sDescribeProjectFileName.c_str());
        dg.ui->describeProjectFileNameEd->setText( QtCommonString::gbkToQString(dg._describeProjectFileName) );
        if (dg.exec() == QDialog::Accepted)
        {
            sVdiConfigFilePath = dg._vdiConfigFileName;
            r = dg._describeProjectFileName;
        }
    }
    return r;
}

void DialogFdcQuickInput::on_yesBn_clicked()
{
    if (ui->describeProjectFileNameEd->text().isEmpty())
    {
        ui->outInfoEd->setText(ui->describeProjectFileNameLb->text()+"，不能为空。");
        return;
    }
    if (ui->hasVdiEd->isChecked() && ui->vdiConfigFileNameEd->text().isEmpty())
    {
        ui->outInfoEd->setText(ui->vdiConfigFileNameLb->text()+"，不能为空。");
        return;
    }

    _describeProjectFileName = QtCommonString::gbkToStdString(ui->describeProjectFileNameEd->text());
    _vdiConfigFileName = QtCommonString::gbkToStdString(ui->vdiConfigFileNameEd->text());
    this->accept();
}

void DialogFdcQuickInput::on_cancelBn_clicked()
{
    this->reject();
}

void DialogFdcQuickInput::on_hasVdiEd_clicked()
{
    if (ui->hasVdiEd->isChecked())
    {
        ui->vdiConfigFileNameEd->setText( QtCommonString::gbkToQString(_vdiConfigFileName) );
    }
    else
    {
        ui->vdiConfigFileNameEd->setText("");
    }
}
