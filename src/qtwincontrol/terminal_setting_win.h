#ifndef TERMINAL_SETTING_WIN_H
#define TERMINAL_SETTING_WIN_H

#include <QDialog>
#include <QStringList>
#include <QHash>

#include <vdi/terminal_manager.h>

#include "simple_win.h"

namespace Ui {
    class TerminalSettingWin;
}

class TerminalSettingWin : public QDialog
{
    Q_OBJECT

public:
    explicit TerminalSettingWin(QWidget *parent = 0);
    ~TerminalSettingWin();

    //result
    inline int ActionResult() const { return _actionResult; }

    inline const QHash<QString, QString>& ProtocolChanged() { return _protocolChanged; }

    inline const QHash<QString, QString>& ChannelChanged() { return _channelChanged; }

    ///main in
    void LoadWin(TerminalBase* terminal);

private slots:
    void on_SaveBn_clicked();

    void on_DefaultBn_clicked();

    void on_CloseBn_clicked();

private:
    Ui::TerminalSettingWin *ui;

    SimpleWin* _protocolWin;
    QHash<QString, QString> _protocolChanged;
    SimpleWin* _channelWin;
    QHash<QString, QString> _channelChanged;
    int _actionResult;

    QString SaveAll();

    void RestoreAll();

};

#endif // TERMINAL_SETTING_WIN_H
