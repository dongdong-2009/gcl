#ifndef SIMPLE_WIN_H
#define SIMPLE_WIN_H

#include <QString>
#include <QHash>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <ccxx/cxdescribe.h>

class SimpleWin
{
public:
    SimpleWin();

    ~SimpleWin();

    QWidget* loadWin(QWidget* oParent, const std::string & sUiFilePath);

    //data to ui
    void controlsViewOut(bool bIsDefault = false);

    void controlsViewOutCalc();

    //ui's control's value check
    QString controlsValueCheck();

    //ui to data
    void controlsViewIn();

    inline CxDescribeUnit* describeUnit() const { return _describeUnit; }

    inline const QHash<QString, QString> & describeNameValuesChanged () const { return _describeNameValuesChanged; }

private:
    CxDescribeUnit* _describeUnit;
    QWidget* _panel;
    QHash<QString, QString> _describeNameValuesChanged;

};

#endif // SIMPLE_WIN_H
