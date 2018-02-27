#include "revisedialog.h"
#include <QLayout>

ReviseDialog::ReviseDialog(QWidget *parent)
    : QDialog(parent)
{
    okButton = new QPushButton(trUtf8("确定"));
    okButton->setEnabled(false);
    canleButton = new QPushButton(trUtf8("取消"));

    nameLabel = new QLabel(trUtf8("参数名称"));
    dataLabel = new QLabel(trUtf8("参数数值"));

    nameEdit = new QLineEdit;
    dataEdit = new QLineEdit;

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(okButton);
    hlayout->addStretch(2);
    hlayout->addWidget(canleButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
    layout->addWidget(dataLabel);
    layout->addWidget(dataEdit);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addLayout(layout);
    mainlayout->addLayout(hlayout);
    setLayout(mainlayout);
    setWindowTitle("参数编辑");

    connect(canleButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ReviseDialog::enabledOkButton(const QString &)
{
    okButton->setEnabled(true);
}

ChoseDialog::ChoseDialog(QWidget *parent)
    : QDialog(parent)
{
    scroll = new QScrollArea();
    label = new QLabel("请选择将显示在管理器的进程页上的列");
    frame = new QFrame(scroll);
    scroll->setWidget(frame);
    scroll->setWidgetResizable(true);

    //exeCheckBox           = new QCheckBox("进程名");
    idCheckbox            = new QCheckBox("进程ID");
    threadsCheckbox       = new QCheckBox("线程数");
    memoryCheckbox        = new QCheckBox("工作设置（内存）");       //内存-工作设置
    networkCheckbox       = new QCheckBox("网络状态");
    runtimeCheckbox       = new QCheckBox("运行时间");
    ioReadCheckBox        = new QCheckBox("IO读取");
    ioWriteCheckBox       = new QCheckBox("IO写入");
    startTimeCheckBox     = new QCheckBox("启动次数");
    priorityCheckBox      = new QCheckBox("优先级");
    unusalCloseCheckBox   = new QCheckBox("异常关闭次数");
    ioReadByteCheckBox    = new QCheckBox("IO读取字节");
    ioWriteByteCheckBox   = new QCheckBox("IO写入字节");
    PeakWorkingCheckBox   = new QCheckBox("峰值工作设置");            //内存-峰值工作设置
    privateMemoryCheckBox = new QCheckBox("专用工作集");             //内存-专用工作集
    pageFileUseCheckBox   = new QCheckBox("提交大小");               //内存-提交大小
    filePathCheckBox      = new QCheckBox("程序路径");
    CommandLineCheckBox   = new QCheckBox("命令行");

   QVBoxLayout *vLayout = new QVBoxLayout(frame);
   frame->setLayout(vLayout);
   //vLayout->addWidget(exeCheckBox);
   vLayout->addWidget(idCheckbox);
   vLayout->addWidget(threadsCheckbox);
   vLayout->addWidget(memoryCheckbox);
   vLayout->addWidget(networkCheckbox);
   vLayout->addWidget(runtimeCheckbox);
   vLayout->addWidget(ioReadCheckBox);
   vLayout->addWidget(ioWriteCheckBox);
   vLayout->addWidget(startTimeCheckBox);
   vLayout->addWidget(priorityCheckBox);
   vLayout->addWidget(unusalCloseCheckBox);
   vLayout->addWidget(ioReadByteCheckBox);
   vLayout->addWidget(ioWriteByteCheckBox);
   vLayout->addWidget(PeakWorkingCheckBox);
   vLayout->addWidget(privateMemoryCheckBox);
   vLayout->addWidget(pageFileUseCheckBox);
   vLayout->addWidget(filePathCheckBox);
   vLayout->addWidget(CommandLineCheckBox);

   scroll->show();
   okButton = new QPushButton("确定");
   cancleButton = new QPushButton("取消");
   QHBoxLayout *hLayout = new QHBoxLayout;
   hLayout->addStretch(10);
   hLayout->addWidget(okButton);
   hLayout->addWidget(cancleButton);

   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addWidget(label);
   mainLayout->addWidget(scroll);
   mainLayout->addLayout(hLayout);

   setLayout(mainLayout);
   setFixedSize(250,270);
   setWindowTitle("选择进程页列");
   connect(cancleButton, SIGNAL(clicked()), this, SLOT(reject()));
   connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}


