#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include "global.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread(int time);
    void setMessage(QString message);
    void stop();
signals:
    void sendMessage(QString);
protected:
    void run();
    void printMessage();

private:
    QString messageStr;
    volatile bool stopped;
    int sleepTime;

};

#endif // MYTHREAD_H
