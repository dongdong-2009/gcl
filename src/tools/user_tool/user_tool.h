#ifndef USER_TOOL_H
#define USER_TOOL_H

#include <QMainWindow>

#include <ccxx/ccxx.h>

namespace Ui {
class user_tool;
}

class user_tool : public QMainWindow
{
    Q_OBJECT

public:
    explicit user_tool(QWidget *parent = 0);
    ~user_tool();

    void interinfo_out( const std::string& sInfo, const std::string& sTitle, int type = 0, int reason = 0, int source = 0, int target = 0, int iTag = 0 );

    CxInterinfo::PlatformEnum platformValue() { return CxInterinfo::Platform_Desktop; }

    void outInfo(const QString& sInfo);

    void outInfo(const std::string& sInfo);


private slots:
    void on_hexSaveBn_clicked();

    void on_json2cstrBn_clicked();

private:
    Ui::user_tool *ui;
};

#endif // USER_TOOL_H
