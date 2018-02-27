#include "terminal_setting_win.h"
#include "ui_terminal_setting_win.h"
#include <ccxx/qtcommon.h>


TerminalSettingWin::TerminalSettingWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TerminalSettingWin)
{
    ui->setupUi(this);
    _protocolWin = new SimpleWin();
    _channelWin = new SimpleWin();
    _actionResult = -1;
}

TerminalSettingWin::~TerminalSettingWin()
{
    delete _protocolWin;
    delete _channelWin;
    delete ui;
}

void TerminalSettingWin::LoadWin(TerminalBase* oTerminal)
{
    const ProtocolBase* oProtocol = oTerminal->protocol();
    if (oProtocol)
    {
        std::string sUiFilePathProtocol = GclConfig::protocolUiFilePath((oProtocol->typeSimpleName()));
        QWidget* oProtocolPn = ui->ProtocolTab;
        CxDescribeUnit* oDescribeUnit = _protocolWin->describeUnit();
        oDescribeUnit->setUnitName(oProtocol->protocolName());

//        std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), oTerminal->terminalName() + ".ini");
//        oDescribeUnit->setDataFilePath(terminalFilePath);
        oProtocol->describeSelf(oDescribeUnit);
        _protocolWin->loadWin(oProtocolPn, sUiFilePathProtocol);
        _protocolWin->controlsViewOut();
    }
    if (oProtocol && oProtocol->channel())
    {
        const CxChannelBase* oChannel = oProtocol->channel();
        std::string sUiFilePathChannel = GclConfig::channelUiFilePath((oChannel->typeSimpleName()));
        QWidget* oChannelPn = ui->ChannelTab;
        CxDescribeUnit* oDescribeUnit = _channelWin->describeUnit();
        oDescribeUnit->setUnitName(oChannel->channelName());

//        std::string terminalFilePath = CxFileSystem::mergeFilePath(GclConfig::terminalPath(), oTerminal->terminalName() + ".ini");
//        oDescribeUnit->setDataFilePath(terminalFilePath);
        oChannel->describeSelf(*oDescribeUnit);
        _channelWin->loadWin(oChannelPn, sUiFilePathChannel);
        _channelWin->controlsViewOut();
    }
}

void TerminalSettingWin::on_SaveBn_clicked()
{
    QString sMsg = SaveAll();
    if (sMsg.length())
        QtCommonDialog::ShowWarning(QString("Save False ：%1").arg(sMsg));
    else {
        _actionResult = 1;
        this->close();
    }
}

void TerminalSettingWin::on_DefaultBn_clicked()
{
    RestoreAll();
}

void TerminalSettingWin::on_CloseBn_clicked()
{
    _actionResult = 0;
    close();
}

QString TerminalSettingWin::SaveAll()
{
    if (_protocolWin) {
        QString sMsg = _protocolWin->controlsValueCheck();
        if (sMsg.size()>0)
            return sMsg;
        _protocolWin->controlsViewIn();
        _protocolChanged = _protocolWin->describeNameValuesChanged();
    }
    if (_channelWin) {
        QString sMsg = _channelWin->controlsValueCheck();
        if (sMsg.size()>0)
            return sMsg;
        _channelWin->controlsViewIn();
        _channelChanged = _channelWin->describeNameValuesChanged();
    }
    return "";
}

void TerminalSettingWin::RestoreAll()
{
    if (_protocolWin) {
        _protocolWin->controlsViewOut(true);
    }
    if (_channelWin) {
        _channelWin->controlsViewOut(true);
    }
}
