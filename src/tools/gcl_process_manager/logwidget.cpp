
#include "logwidget.h"
#include <QCoreApplication>


LogWidget::LogWidget(QWidget *parent)
 : QWidget(parent)
{
    m_thread = new MyThread(1000);
    is_finished = false;

    planTextEdit = new QPlainTextEdit();
    planTextEdit->setReadOnly(true);

    clear_button = new QPushButton();
    clear_button->setText("clear");

    checkbox = new QCheckBox("定时刷新");
    //checkbox->setChecked(true);

    QHBoxLayout *button_layout = new QHBoxLayout;
    button_layout->addStretch();
    button_layout->addWidget(checkbox);
    button_layout->addWidget(clear_button);
    button_layout->setSpacing(10);
    button_layout->setContentsMargins(0, 0, 10, 10);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(planTextEdit);
    main_layout->addLayout(button_layout);
    main_layout->setSpacing(10);
    main_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(main_layout);

    m_thread->start();

    //connect(checkbox, SIGNAL(clicked()), this, SLOT(checkChange()));
    connect(m_thread, SIGNAL(sendMessage(QString)), this, SLOT(showlog(QString)));
    connect(clear_button, SIGNAL(clicked()), this, SLOT(clearAction()));

}


LogWidget::~LogWidget()
{

}


void LogWidget::showlog(QString msg)
{
    //string text = ApplicationsTab::getMessage();
    QString text = QString::fromStdString(ApplicationsTab::getMessage());
    msg.append(" ");
    msg.append(text);
    msg.append("\n\n");
    if(text.isEmpty())
        ApplicationsTab::clearText();
    else
    {
        appendMessage(msg);
        ApplicationsTab::clearText();
    }
}


void LogWidget::checkChange()
{
    bool isCheck = checkbox->isChecked();
    if(isCheck)
    {
        m_thread->start();
    }
    else
    {
        m_thread->stop();
    }
}


void LogWidget::clearAction()
{
    planTextEdit->clear();
}


void LogWidget::appendMessage(const QString& text)
{
    planTextEdit->appendPlainText(text); // Adds the message to the widget
}



