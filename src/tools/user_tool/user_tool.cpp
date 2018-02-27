#include "user_tool.h"
#include "ui_user_tool.h"


#include <ccxx/qtcommon.h>


using namespace std;

user_tool::user_tool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::user_tool)
{
    ui->setupUi(this);
}

user_tool::~user_tool()
{
    delete ui;
}

void user_tool::interinfo_out(const std::string &sInfo, const std::string &sTitle, int type, int reason, int source, int target, int iTag)
{
    stringstream sOutInfo;
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

    outInfo(sOutInfo.str());
}

void user_tool::outInfo(const QString &sInfo)
{
    static int iCount = 0;
    if (iCount % 300 == 0) ui->outInfoEd->clear();
    ui->outInfoEd->append(QString::number(++iCount) + "   " +QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->outInfoEd->append(sInfo);
}

void user_tool::outInfo(const std::string &sInfo)
{
    outInfo(QtCommonString::gbkToQString(sInfo));
}

void user_tool::on_hexSaveBn_clicked()
{
    string s = QtCommonString::gbkToStdString(ui->plainTextEdit->toPlainText());

    vector<char> cs = CxString::hexToData(s);

    QString sFileName;
    if (QtCommonDialog::DialogSave(sFileName))
    {
        CxFile::save(QtCommonString::gbkToStdString(sFileName), string(cs.data(), cs.size()));
    }
}

void user_tool::on_json2cstrBn_clicked()
{
    string sDoc = QtCommonString::gbkToStdString(ui->json2cstrEd->toPlainText());

    sDoc = CxString::replace(sDoc, "\"%d\"", "%d");
    sDoc = CxString::replace(sDoc, "\"%lld\"", "%lld");
    sDoc = CxString::replace(sDoc, "\"", "\\\"");

    vector<string> sLines = CxString::split(sDoc, '\n');
    for (size_t i = 0; i < sLines.size(); ++i)
    {
        string & sLine = sLines.at(i);
        if (i==0)
        {
            sLine = "\"" + sLine + "\"";
        }
        else
        {
            sLine = "\"\\r\\n" + sLine + "\"";
        }
    }

    ui->json2cstrRd->setPlainText(QtCommonString::gbkToQString(CxString::join(sLines, "\r\n")));
}
