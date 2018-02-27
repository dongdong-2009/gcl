#include "myinputpanel.h"
#include "ui_myinputpanel.h"
#include <QDebug>

MyInputPanel::MyInputPanel() :
    QWidget(0, Qt::Tool | Qt::WindowStaysOnTopHint),
    ui(new Ui::MyInputPanel),
    m_bShiftPressed(false),
    m_pLastFocusedWidget(NULL)
{
    ui->setupUi(this);
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(saveFocusWidget(QWidget*,QWidget*)));
    QList<QPushButton *> buttonList = this->findChildren<QPushButton *>();
    foreach(QPushButton *button, buttonList)
    {
        connect(button, SIGNAL(clicked()), this, SLOT(panelButtonClicded()));
    }
}

MyInputPanel::~MyInputPanel()
{
    delete ui;
}

bool MyInputPanel::event(QEvent *e)
{
    switch (e->type())
    {
//! [1]
    case QEvent::WindowActivate:
        if (m_pLastFocusedWidget != NULL)
        {
            m_pLastFocusedWidget->activateWindow();
        }
        break;
//! [1]
    default:
        break;
    }

    return QWidget::event(e);
}

void MyInputPanel::saveFocusWidget(QWidget *oldFocus, QWidget *newFocus)
{
    if (newFocus != 0 && !this->isAncestorOf(newFocus))
    {
        m_pLastFocusedWidget = newFocus;
    }
}

void MyInputPanel::panelButtonClicded()
{
    QPushButton *curButton = qobject_cast<QPushButton *> (sender());
    QString text = curButton->text();
    if(text.isEmpty())
    {
        this->close();
        return;
    }
    else if(text == "←")
    {
        emit characterGenerated(Qt::Key_Backspace);
    }
    else if(text == "Shift")
    {
        m_bShiftPressed = !m_bShiftPressed;
        setKeyState();
    }
    else
    {
        QChar chr = text.at(0);
        char ascCode = chr.toLatin1();
//        char ascCode = chr.toAscii();
        emit characterGenerated(ascCode);
    }
}

void MyInputPanel::setKeyState()
{
    if(m_bShiftPressed)
    {
        //按下shift当前为小写，需要改为大写
        QList<QPushButton *> buttonList = this->findChildren<QPushButton *>();
        foreach(QPushButton *button, buttonList)
        {
            QString text = button->text();
            if(text.isEmpty() || text == "←" || text == "Shift")
            {
                continue;
            }
            QChar chr = text.at(0);
            char ascCode = chr.toLatin1();
            switch(ascCode)
            {
            case '~':
                button->setText("`");
                break;
            case '1':
                button->setText("!");
                break;
            case '2':
                button->setText("@");
                break;
            case '3':
                button->setText("#");
                break;
            case '4':
                button->setText("$");
                break;
            case '5':
                button->setText("%");
                break;
            case '6':
                button->setText("^");
                break;
            case '7':
                button->setText("&&");
                break;
            case '8':
                button->setText("*");
                break;
            case '9':
                button->setText("(");
                break;
            case '0':
                button->setText(")");
                break;
            case '-':
                button->setText("_");
                break;
            case '=':
                button->setText("+");
                break;
            case '[':
                button->setText("{");
                break;
            case ']':
                button->setText("}");
                break;
            case '\\':
                button->setText("|");
                break;
            case ';':
                button->setText(":");
                break;
            case '\'':
                button->setText("\"");
                break;
            case ',':
                button->setText("<");
                break;
            case '.':
                button->setText(">");
                break;
            case '/':
                button->setText("?");
                break;
            default:
                //字母，小写变大写，减去asc码的差值32
            {
                ascCode -= 32;
                QChar newChr(ascCode);
                button->setText(QString(newChr));
            }
                break;
            }
        }
    }
    else
    {
        //再次按下shift，当前为大写，需要改为小写
        QList<QPushButton *> buttonList = this->findChildren<QPushButton *>();
        foreach(QPushButton *button, buttonList)
        {
            QString text = button->text();
            if(text.isEmpty() || text == "←" || text == "Shift")
            {
                continue;
            }
            QChar chr = text.at(0);
            char ascCode = chr.toLatin1();
            switch(ascCode)
            {
            case '`':
                button->setText("~");
                break;
            case '!':
                button->setText("1");
                break;
            case '@':
                button->setText("2");
                break;
            case '#':
                button->setText("3");
                break;
            case '$':
                button->setText("4");
                break;
            case '%':
                button->setText("5");
                break;
            case '^':
                button->setText("6");
                break;
            case '&':
                button->setText("7");
                break;
            case '*':
                button->setText("8");
                break;
            case '(':
                button->setText("9");
                break;
            case ')':
                button->setText("0");
                break;
            case '_':
                button->setText("-");
                break;
            case '+':
                button->setText("=");
                break;
            case '{':
                button->setText("[");
                break;
            case '}':
                button->setText("]");
                break;
            case '|':
                button->setText("\\");
                break;
            case ':':
                button->setText(";");
                break;
            case '\"':
                button->setText("\'");
                break;
            case '<':
                button->setText(",");
                break;
            case '>':
                button->setText(".");
                break;
            case '?':
                button->setText("/");
                break;
            default:
                //字母，大写变小写，加上asc码的差值32
            {
                ascCode += 32;
                QChar newChr(ascCode);
                button->setText(QString(newChr));
            }
                break;
            }
        }
    }
}
