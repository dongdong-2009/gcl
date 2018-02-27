#include "hex_spin_box.h"

THexSpinBox::THexSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
    validator = new QRegExpValidator(QRegExp("0x[0-9A-Fa-f]{1,8}"), this);
    setRange(0, 255);
    setPrefix("0x");
}

QValidator::State THexSpinBox::validate(QString &text, int &pos) const
{
    return validator->validate(text, pos);
}

QString THexSpinBox::textFromValue(int value) const
{
    return QString::number(value, 16).toUpper();
}

int THexSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toInt(&ok, 16);
}
