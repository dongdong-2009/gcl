#include <QtCore>
#include <QApplication>
#include <QDesktopWidget>
#include "myinputpanelcontext.h"



MyInputPanelContext::MyInputPanelContext()
{
    m_pInputPanel = new MyInputPanel;
    connect(m_pInputPanel, SIGNAL(characterGenerated(int)), SLOT(sendCharacter(int)));
}

MyInputPanelContext::~MyInputPanelContext()
{
    delete m_pInputPanel;
}

bool MyInputPanelContext::filterEvent(const QEvent* event)
{
    if (event->type() == QEvent::RequestSoftwareInputPanel)
    {
        updatePosition();
        m_pInputPanel->show();
        return true;
    }
    else if (event->type() == QEvent::CloseSoftwareInputPanel)
    {
        m_pInputPanel->hide();
        return true;
    }
    return false;
}

QString MyInputPanelContext::identifierName()
{
    return "MyInputPanelContext";
}

void MyInputPanelContext::reset()
{
}

bool MyInputPanelContext::isComposing() const
{
    return false;
}

QString MyInputPanelContext::language()
{
    return "en_US";
}

void MyInputPanelContext::sendCharacter(int character)
{
    QPointer<QWidget> w = focusWidget();
    if (!w)
    {
        return;
    }

    QKeyEvent keyPress(QEvent::KeyPress, /*character.unicode()*/character, Qt::NoModifier, QString(character));
    QApplication::sendEvent(w, &keyPress);

    if (!w)
    {
        return;
    }

    QKeyEvent keyRelease(QEvent::KeyPress,/* character.unicode()*/character, Qt::NoModifier, QString());
    QApplication::sendEvent(w, &keyRelease);
}

void MyInputPanelContext::updatePosition()
{
    QWidget *widget = focusWidget();
    if (!widget)
    {
        return;
    }

    QRect widgetRect = widget->rect();
    QRect screenWidgetRect = qApp->desktop()->availableGeometry();
    QPoint panelPos = QPoint(widgetRect.left(), widgetRect.bottom() + 2);
    panelPos = widget->mapToGlobal(panelPos);
    panelPos.setX(screenWidgetRect.left() + 2);
    m_pInputPanel->move(panelPos);
}
