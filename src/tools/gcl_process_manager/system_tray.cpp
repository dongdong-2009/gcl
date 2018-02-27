#include "system_tray.h"

SystemTray::SystemTray(QWidget *parent)
  : QSystemTrayIcon(parent)
{
  createActions();
}

SystemTray::~SystemTray()
{

}

void SystemTray::createActions()
{
    setToolTip("管理器");
    action_logout = new QAction(this);
    action_open = new QAction(this);

    action_logout->setText("退出");
    action_open->setText("前端显示");

    tray_menu = new QMenu();
    tray_menu->addAction(action_logout);
    tray_menu->addAction(action_open);

    //connect(action_logout, SIGNAL(triggered()), qApp, SLOT(quit()));
   // connect(action_open, SIGNAL(triggered()), this, SIGNAL(showWidget()));
    setContextMenu(tray_menu);
}
