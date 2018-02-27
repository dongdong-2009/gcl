#ifndef NEW_PROTOCOL_WIN_H
#define NEW_PROTOCOL_WIN_H

#include <QDialog>
#include <vdi/terminal_manager.h>

namespace Ui {
class NewProtocolWin;
}

class NewProtocolWin : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewProtocolWin(QWidget *parent = 0);
    ~NewProtocolWin();

    void setTerminalName(const QString& terminalName);
    QString getTerminalName() const;

    void setProtocolName(const QString& protoclName);
    QString getProtocolName() const;

private slots:
    void on_NewTerminalBn_clicked();

private:
    Ui::NewProtocolWin *ui;

    QString _terminalName;
    QString _protocolName;

    void initControls();
};

#endif // NEW_PROTOCOL_WIN_H
