#include "mainwindow.h"
#include <QApplication>
#include <string>
#include <iostream>
#include <QPushButton>
#include <QDebug>
#include <QObject>

#include "newspaper.h"
#include "read.h"

using namespace std;

void testPrint()
{
    qDebug() << "HHH";
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QPushButton button("Quit");
//    //QObject::connect(&button, &QPushButton::clicked, this, &QApplication::quit);
//    //QObject::connect(&button, &QPushButton::clicked, testPrint);
//    QObject::connect(&button, &QPushButton::clicked, [](bool) {
//        qDebug() << "You clicked me!";
//    });
//    button.show();

//    Newspaper newspaper("Newspaper A");
//    Reader reader;
//    QObject::connect(&newspaper, &Newspaper::newPaper,
//                     &reader, &Reader::receiveNewspaper);

//    newspaper.send();

    MainWindow mainwindow;

    mainwindow.show();

    return a.exec();
}
