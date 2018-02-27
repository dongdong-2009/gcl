#include "messageboxwin.h"
#include "ui_messageboxwin.h"

using namespace std;

MessageBoxWin::MessageBoxWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageBoxWin)
{
    ui->setupUi(this);

    CxInterinfoOut::addObserver(this);
}

MessageBoxWin::~MessageBoxWin()
{
    CxInterinfoOut::removeObserver(this);
    delete ui;
}

void MessageBoxWin::interinfo_out(const std::string &sInfo, const std::string &sTitle, int type, int reason, int source, int target, int iTag)
{
    stringstream sOutInfo;

    sOutInfo << CxTime::currentMsepochString() << " ";

    if (sTitle.size()>0)
        sOutInfo << "[title]=" << sTitle;
    if (type)
        sOutInfo << "[type]=" << type;
    if (reason)
        sOutInfo << "[reason]=" << reason;
    if (source)
        sOutInfo << "[source]=" << source;
    if (target)
        sOutInfo << "[target]=" << target;
    if (iTag)
        sOutInfo << "[tag]=" << iTag;
    if (sOutInfo.tellp() > 0)
        sOutInfo << CxGlobal::lineString;
    sOutInfo << sInfo;
    ui->infoEd->append(QtCommonString::gbkToQString( sOutInfo.str()) );
}
