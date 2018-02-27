#ifndef DIALOG_FDC_QUICK_INPUT_H
#define DIALOG_FDC_QUICK_INPUT_H

#include <QDialog>

namespace Ui {
class DialogFdcQuickInput;
}

class DialogFdcQuickInput : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFdcQuickInput(QWidget *parent = 0);
    ~DialogFdcQuickInput();

    static std::string getFdcQuickInput(const std::string & sDescribeProjectFileName, std::string &sVdiConfigFilePath);

private slots:
    void on_yesBn_clicked();

    void on_cancelBn_clicked();

    void on_hasVdiEd_clicked();

private:
    Ui::DialogFdcQuickInput *ui;

    std::string _describeProjectFileName;
    std::string _vdiConfigFileName;

};

#endif // DIALOG_FDC_QUICK_INPUT_H
