#ifndef MYINPUTPANELCONTEXT_H
#define MYINPUTPANELCONTEXT_H


#include <QtGui/QInputContext>
#include "myinputpanel.h"

class MyInputPanel;
class MyInputPanelContext : public QInputContext
{
    Q_OBJECT

public:
    MyInputPanelContext();
    ~MyInputPanelContext();

public:
    bool filterEvent(const QEvent* event);
    QString identifierName();
    QString language();
    bool isComposing() const;
    void reset();

private slots:
    void sendCharacter(int character);

private:
    void updatePosition();

private:
    MyInputPanel *m_pInputPanel;
};

//! [0]

#endif // MYINPUTPANELCONTEXT_H
