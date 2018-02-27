#ifndef REVISEDIALOG_H
#define REVISEDIALOG_H
#include "global.h"

class ReviseDialog : public QDialog
{
    Q_OBJECT

public:
    ReviseDialog(QWidget *parent = 0);
    QPushButton *okButton;
    QPushButton *canleButton;
    QLineEdit *nameEdit;
    QLineEdit *dataEdit;
    QLineEdit *decribeEdit;

public slots:
    void enabledOkButton(const QString &);

private:
    QLabel *nameLabel;
    QLabel *dataLabel;

};

class ChoseDialog : public QDialog
{
    Q_OBJECT
public:
    ChoseDialog(QWidget *parent = 0);

public:
    QCheckBox* exeCheckBox;
    QCheckBox* idCheckbox;
    QCheckBox* threadsCheckbox;
    QCheckBox* memoryCheckbox;      //内存-工作设置
    QCheckBox* networkCheckbox;
    QCheckBox* runtimeCheckbox;
    QCheckBox* ioReadCheckBox;
    QCheckBox* ioWriteCheckBox;
    QCheckBox* startTimeCheckBox;
    QCheckBox* priorityCheckBox;
    QCheckBox* unusalCloseCheckBox;
    QCheckBox* ioReadByteCheckBox;
    QCheckBox* ioWriteByteCheckBox;
    QCheckBox* PeakWorkingCheckBox;  //内存-峰值工作设置
    QCheckBox* privateMemoryCheckBox; //内存-专用工作集
    QCheckBox* pageFileUseCheckBox;  //内存-提交大小
    QCheckBox* filePathCheckBox;
    QCheckBox* CommandLineCheckBox;


private:
    QScrollArea* scroll;
    QPushButton* okButton;
    QPushButton* cancleButton;
    QLabel* label;
    QFrame* frame;

};

#endif // REVISEDIALOG_H
