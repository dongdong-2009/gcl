#ifndef MYINPUTPANEL_H
#define MYINPUTPANEL_H

#include <QWidget>

namespace Ui {
class MyInputPanel;
}

class MyInputPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MyInputPanel();
    ~MyInputPanel();

signals:
    void characterGenerated(int character);

protected:
    bool event(QEvent *e);

private slots:
    void saveFocusWidget(QWidget *oldFocus, QWidget *newFocus);
    void panelButtonClicded();

private:
    void setKeyState();

private:
    Ui::MyInputPanel *ui;
    bool m_bShiftPressed;
    QWidget *m_pLastFocusedWidget;
};

#endif // MYINPUTPANEL_H
