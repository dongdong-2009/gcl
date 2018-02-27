#include "qthread.h"
#include <QDebug>
#include <QWidget>
#include <QTextEdit>
#include <QDateTime>

MyThread::MyThread(int time)
{
    stopped = false;
    sleepTime = time;
}


void MyThread::run()
{
    while(!stopped)
    {
        emit sendMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"));
        msleep(sleepTime);
    }
    stopped = false;
}

void MyThread::stop()
{
    stopped = true;
}

void MyThread::setMessage(QString message)
{
    messageStr = message;
}

void MyThread::printMessage()
{
    qDebug() << messageStr;
    sleep(1);
}



