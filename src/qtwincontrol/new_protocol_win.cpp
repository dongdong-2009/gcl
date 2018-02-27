#include "new_protocol_win.h"
#include "ui_new_protocol_win.h"
#include <QRegExp>
#include <QRegExpValidator>

#include <ccxx/qtcommon.h>
#include <vdi/protocol_base.h>

NewProtocolWin::NewProtocolWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProtocolWin)
{
    ui->setupUi(this);

    QRegExp regExp(QtCommonString::gbkToQString( GclConfig::regstringName()) );
    QRegExpValidator * regExpValidator = new QRegExpValidator(regExp, this);
    ui->NewTerminalNameEd->setValidator(regExpValidator);

    initControls();

    setWindowTitle("New Terminal");
}

NewProtocolWin::~NewProtocolWin()
{
    delete ui;
}

void NewProtocolWin::setTerminalName(const QString &terminalName)
{
    _terminalName = terminalName;
}

QString NewProtocolWin::getTerminalName() const
{
    return _terminalName;
}

void NewProtocolWin::setProtocolName(const QString &protoclName)
{
    _protocolName = protoclName;
}

QString NewProtocolWin::getProtocolName() const
{
    return _protocolName;
}

void NewProtocolWin::on_NewTerminalBn_clicked()
{
    QString sProtocolSimpleName = ui->NewProtocolSimpleNameCb->currentText();
    QString sTerminalName = ui->NewTerminalNameEd->text().trimmed();
    if(!sTerminalName.isEmpty()){
        setTerminalName(sTerminalName);
        setProtocolName(sProtocolSimpleName);
        this->close();
    }
}

void NewProtocolWin::initControls()
{
    QStringList sProtocolSimpleNames = QtCommonString::gbkToQStrings( ProtocolFactoryManager::factoryNames() );
    ui->NewProtocolSimpleNameCb->addItems(sProtocolSimpleNames);
}
