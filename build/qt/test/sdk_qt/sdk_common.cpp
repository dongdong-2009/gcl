#include "sdk_common.h"

#include <QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>

using namespace std;

const char f_hexChar16[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


vector<yx_t> f_yxs;

vector<yc_t> f_ycs;

vector<yw_t> f_yws;


QString SdkCommon::gbkToQString(const std::string &s)
{
    return QString::fromLocal8Bit(s.data(), s.size());
}

QStringList SdkCommon::gbkToQStrings(const std::vector<std::string> &ss)
{
    QStringList r;
    for (size_t i = 0; i < ss.size(); ++i)
    {
        const std::string & s = ss.at(i);
        r.append(QString::fromLocal8Bit(s.data(), s.size()));
    }
    return r;
}

QStringList SdkCommon::gbkToQStrings(const std::string *oSs, int iCount)
{
    QStringList r;
    for (size_t i = 0; i < iCount; ++i)
    {
        const std::string & s = oSs[i];
        r.append(QString::fromLocal8Bit(s.data(), s.size()));
    }
    return r;
}

std::vector<std::string> SdkCommon::gbkToStdStrings(const QStringList &ss)
{
    std::vector<std::string> r;
    for (size_t i = 0; i < ss.count(); ++i)
    {
        const QString & s = ss[i];
        r.push_back(gbkToStdString(s));
    }
    return r;
}

std::string SdkCommon::gbkToStdString(const QString &s)
{
    QByteArray data = s.toLocal8Bit();
    return std::string( data.data(), data.size() );
}

bool SdkCommon::DialogInput2(const QString &sTitle, const QString &sValue1Title, const QString &sValue2Title, QString &sValue1, QString &sValue2, int iValue1EchoMode, int iValue2EchoMode)
{
    QDialog dialog;

    QLabel* value1Label = new QLabel( sValue1Title );
    QLabel* value2Label = new QLabel( sValue2Title );
    QLineEdit* value1Ed = new QLineEdit;
    value1Ed->setEchoMode( (QLineEdit::EchoMode)iValue1EchoMode );
    QLineEdit* value2Ed = new QLineEdit;
    value2Ed->setEchoMode( (QLineEdit::EchoMode)iValue2EchoMode );

    value1Ed->setText(sValue1);

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget( value1Label, 0, 0, 1, 1);
    gridLayout->addWidget( value1Ed, 0, 1, 1, 3 );
    gridLayout->addWidget( value2Label, 1, 0, 1, 1 );
    gridLayout->addWidget( value2Ed, 1, 1, 1, 3 );

    QPushButton* okBtn = new QPushButton( QObject::tr("确定") );
    QPushButton* cancelBtn =new QPushButton  ( QObject::tr("取消") );
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setSpacing( 60 );
    btnLayout->addWidget( okBtn );
    btnLayout->addWidget( cancelBtn );

    QVBoxLayout* dlgLayout = new QVBoxLayout;
    dlgLayout->setMargin( 40 );
    dlgLayout->addLayout( gridLayout );
    dlgLayout->addStretch( 40 );
    dlgLayout->addLayout( btnLayout );
    dialog.setLayout( dlgLayout );

    dialog.connect( okBtn, SIGNAL(clicked()), &dialog, SLOT(accept()) );
    dialog.connect( cancelBtn, SIGNAL(clicked()), &dialog, SLOT(reject()) );

    dialog.setWindowTitle( sTitle );
    dialog.resize( 380, 220 );
    if ( dialog.exec() == QDialog::Accepted )
    {
        sValue1 = value1Ed->text();
        sValue2 = value2Ed->text();
        return true;
    }
    else
    {
        return false;
    }
}

bool SdkCommon::DialogInput3(const QString &sTitle, const QString &sValue1Title, const QString &sValue2Title, const QString &sValue3Title, QString &sValue1, QString &sValue2, QString &sValue3, int iValue1EchoMode, int iValue2EchoMode, int iValue3EchoMode)
{
    QDialog dialog;

    QLabel* value1Label = new QLabel( sValue1Title );
    QLabel* value2Label = new QLabel( sValue2Title );
    QLabel* value3Label = new QLabel( sValue3Title );
    QLineEdit* value1Ed = new QLineEdit;
    value1Ed->setMaxLength(13);
    value1Ed->setEchoMode( (QLineEdit::EchoMode)iValue1EchoMode );
    QLineEdit* value2Ed = new QLineEdit;
    value2Ed->setMaxLength(13);
    value2Ed->setEchoMode( (QLineEdit::EchoMode)iValue2EchoMode );
    QLineEdit* value3Ed = new QLineEdit;
    value3Ed->setMaxLength(13);
    value3Ed->setEchoMode( (QLineEdit::EchoMode)iValue3EchoMode );

    value1Ed->setText(sValue1);
    value2Ed->setText(sValue2);
    value3Ed->setText(sValue3);

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget( value1Label, 0, 0, 1, 1);
    gridLayout->addWidget( value1Ed, 0, 1, 1, 3 );
    gridLayout->addWidget( value2Label, 1, 0, 1, 1 );
    gridLayout->addWidget( value2Ed, 1, 1, 1, 3 );
    gridLayout->addWidget( value3Label, 2, 0, 1, 1 );
    gridLayout->addWidget( value3Ed, 2, 1, 1, 3 );

    QPushButton* okBtn = new QPushButton( QPushButton::tr("确定") );
    QPushButton* cancelBtn =new QPushButton  ( QPushButton::tr("取消") );
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setSpacing( 60 );
    btnLayout->addWidget( okBtn );
    btnLayout->addWidget( cancelBtn );

    QVBoxLayout* dlgLayout = new QVBoxLayout;
    dlgLayout->setMargin( 40 );
    dlgLayout->addLayout( gridLayout );
    dlgLayout->addStretch( 40 );
    dlgLayout->addLayout( btnLayout );
    dialog.setLayout( dlgLayout );

    dialog.connect( okBtn, SIGNAL(clicked()), &dialog, SLOT(accept()) );
    dialog.connect( cancelBtn, SIGNAL(clicked()), &dialog, SLOT(reject()) );

    dialog.setWindowTitle( sTitle );
    dialog.resize( 380, 220 );
    if ( dialog.exec() == QDialog::Accepted )
    {
        sValue1 = value1Ed->text();
        sValue2 = value2Ed->text();
        sValue3 = value3Ed->text();
        return true;
    }
    else
    {
        return false;
    }
}

const std::vector<yx_t> &SdkCommon::yxs()
{
    return f_yxs;
}

yx_t *SdkCommon::getYx(int iAddress)
{
    for (size_t i = 0; i < f_yxs.size(); ++i)
    {
        yx_t & yx = f_yxs.at(i);
        if (yx.address == iAddress)
            return & yx;
    }
    return NULL;
}

void SdkCommon::setYx(int address, int quality, int state, long long datetime)
{
    yx_t * oYx = getYx(address);
    if (oYx)
    {
        oYx->quality = quality;
        oYx->state = state;
        oYx->datetime = datetime;
    }
    else
    {
        yx_t yx;
        yx.address = address;
        yx.quality = quality;
        yx.state = state;
        yx.res = 0;
        yx.datetime = datetime;
        f_yxs.push_back(yx);
    }
}

const std::vector<yc_t> &SdkCommon::ycs()
{
    return f_ycs;
}

yc_t *SdkCommon::getYc(int iAddress)
{
    for (size_t i = 0; i < f_ycs.size(); ++i)
    {
        yc_t & yc = f_ycs.at(i);
        if (yc.address == iAddress)
            return & yc;
    }
    return NULL;
}

void SdkCommon::setYc(int address, int quality, double value, long long datetime)
{
    yc_t * oYc = getYc(address);
    if (oYc)
    {
        oYc->quality = quality;
        oYc->value = value;
        oYc->datetime = datetime;
    }
    else
    {
        yc_t yc;
        yc.address = address;
        yc.quality = quality;
        yc.value = value;
        yc.datetime = datetime;
        f_ycs.push_back(yc);
    }
}

const std::vector<yw_t> &SdkCommon::yws()
{
    return f_yws;
}

yw_t *SdkCommon::getYw(int iAddress)
{
    for (size_t i = 0; i < f_yws.size(); ++i)
    {
        yw_t & yw = f_yws.at(i);
        if (yw.address == iAddress)
            return & yw;
    }
    return NULL;
}

void SdkCommon::setYw(int address, int quality, const char *value, long long datetime)
{
    yw_t * oYw = getYw(address);
    if (oYw)
    {
        oYw->quality = quality;
        CxStringC::set(oYw->value, sizeof(oYw->value), value);
        oYw->datetime = datetime;
    }
    else
    {
        yw_t yw;
        yw.address = address;
        yw.quality = quality;
        CxStringC::set(yw.value, sizeof(yw.value), value);
        yw.datetime = datetime;
        f_yws.push_back(yw);
    }
}

vector<int> SdkCommon::dealYx_01(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    vector<int> addresses;
    if (iParamType == gcd_address_int32_vqt_t::getType())
    {
        if (iDataLength < sizeof(gcd_address_int32_vqt_t) * iParamCount) return addresses;
        gcd_address_int32_vqt_t * oValue = (gcd_address_int32_vqt_t *)pData;
        for (size_t i = 0; i < iParamCount; ++i)
        {
            addresses.push_back(oValue->address);
            int iValue = oValue->value;
            SdkCommon::setYx(oValue->address, oValue->quality, iValue, oValue->datetime);
            oValue ++;
        }
    }
    else if (iParamType == gcd_address_vqt_intText1_t::getType())
    {
        if (iDataLength < sizeof(gcd_address_vqt_intText1_t) * iParamCount) return addresses;
        gcd_address_vqt_intText1_t * oValue = (gcd_address_vqt_intText1_t *)pData;
        for (size_t i = 0; i < iParamCount; ++i)
        {
            addresses.push_back(oValue->address);
            int iValue = (oValue->value[0] - 0x30);
            SdkCommon::setYx(oValue->address, oValue->quality, iValue, oValue->datetime);
            oValue ++;
        }
    }
    return addresses;
}

vector<int> SdkCommon::dealYc_01(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    vector<int> addresses;
    if (iParamType == gcd_address_double_vqt_t::getType())
    {
        if (iDataLength < sizeof(gcd_address_double_vqt_t) * iParamCount) return addresses;
        gcd_address_double_vqt_t * oValue = (gcd_address_double_vqt_t *)pData;
        for (size_t i = 0; i < iParamCount; ++i)
        {
            addresses.push_back(oValue->address);
            double dValue = oValue->value;
            setYc(oValue->address, oValue->quality, dValue, oValue->datetime);
            oValue ++;
        }
    }
    else if (iParamType == gcd_address_vqt_realText15_t::getType())
    {
        if (iDataLength < sizeof(gcd_address_vqt_realText15_t) * iParamCount) return addresses;
        gcd_address_vqt_realText15_t * oValue = (gcd_address_vqt_realText15_t *)pData;
        for (size_t i = 0; i < iParamCount; ++i)
        {
            addresses.push_back(oValue->address);
            double dValue = atof(oValue->value);
            setYc(oValue->address, oValue->quality, dValue, oValue->datetime);
            oValue ++;
        }
    }
    return addresses;
}

std::vector<int> SdkCommon::dealYw_01(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    vector<int> addresses;
    if (iParamType == gcd_address_vqt_string128_t::getType())
    {
        if (iDataLength < sizeof(gcd_address_vqt_string128_t) * iParamCount) return addresses;
        gcd_address_vqt_string128_t * oValue = (gcd_address_vqt_string128_t *)pData;
        for (size_t i = 0; i < iParamCount; ++i)
        {
            addresses.push_back(oValue->address);
            setYw(oValue->address, oValue->quality, oValue->value, oValue->datetime);
            oValue ++;
        }
    }
    return addresses;
}

bool SdkCommon::dialogInput(const QString &sPrompt, QString &sVaule)
{
    bool ok;
    QString text = QInputDialog::getText(0, QObject::tr("输入框"),
                                         sPrompt, QLineEdit::Normal,
                                         sVaule, &ok);
    if (ok){
        sVaule = text;
        return true;
    } else
        return false;
}

