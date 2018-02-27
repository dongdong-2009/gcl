#ifndef MESSAGEBOXWIN_H
#define MESSAGEBOXWIN_H

#include <QWidget>

#include <ccxx/ccxx.h>
#include <ccxx/qtcommon.h>

namespace Ui {
class MessageBoxWin;
}

class MessageBoxWin : public QWidget, public CxInterinfoOut_I
{
    Q_OBJECT

public:
    explicit MessageBoxWin(QWidget *parent = 0);
    ~MessageBoxWin();

protected:
    void interinfo_out( const std::string& sInfo, const std::string& sTitle, int type = 0, int reason = 0, int source = 0, int target = 0, int iTag = 0 );

    inline CxInterinfo::PlatformEnum platformValue() { return CxInterinfo::Platform_Desktop; }

private:
    Ui::MessageBoxWin *ui;
};

#endif // MESSAGEBOXWIN_H
