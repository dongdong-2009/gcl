#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include "applicationwidget.h"
#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QLayout>


class LogWidget : public QWidget
{
    Q_OBJECT
public:
    LogWidget(QWidget *parent = 0);

    ~LogWidget();

    void outPutLog(QtMsgType type, const QString &msg);
    void appendMessage(const QString& text);


public slots:
    void clearAction();
    void showlog(QString msg);
    void checkChange();

private:
    bool is_finished;
    QFile m_logFile;
    MyThread *m_thread;
    //QTextBrowser *browser;
    QCheckBox *checkbox;
    QPushButton *clear_button;
    QPlainTextEdit *planTextEdit;

};

#endif // LOGWIDGET_H
