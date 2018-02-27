#ifndef SYSTEM_TRAY_H
#define SYSTEM_TRAY_H
#include <QWidget>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    SystemTray(QWidget *parent = 0);
    ~SystemTray();
public:
    QAction *action_logout;
    QAction *action_open;

private:
    void createActions();

private:
    QMenu *tray_menu; //托盘菜单


};

#endif // SYSTEM_TRAY_H
