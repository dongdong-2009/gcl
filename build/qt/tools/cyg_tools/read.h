#ifndef READ
#define READ
#include <QObject>
#include <QDebug>

class Reader : public QObject
{
    Q_OBJECT
public:
    Reader(){}

    void receiveNewspaper(const QString & name)
    {
        qDebug() << "Receive NewsPaper : " << name;
    }
};

#endif // READ

