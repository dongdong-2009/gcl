#include "describe_win.h"

#include <ccxx/cxcontainer.h>

#include <ccxx/qtcommon.h>

#include "hex_spin_box.h"

/*
Integer QSpinBox
bool QCheckBox
enum QComboBox
double QDoubleSpinBox
datetime QDateTimeEdit
string QLineEdit
*/

using namespace std;

QString ObjectNameEd(const QString& sName){
    return sName + "Ed";
}

QString ObjectNameLb(const QString& sName){
    return sName + "Lb";
}

QWidget * DescribeWin::createControl(const CxDescribeBase* oDescribe, QWidget* oParent, bool bCreateLabel)
{
    QString     describeName              = QtCommonString::gbkToQString( oDescribe->name             () );
    QString     describeTitle             = QtCommonString::gbkToQString( oDescribe->title            () );
    QString     describeDesc              = QtCommonString::gbkToQString( oDescribe->desc             () );
    bool        describeIsVisible         = oDescribe->isVisible        ();
    bool        describeIsReadOnly        = oDescribe->isReadOnly       ();
    int         describeMaxLength         = oDescribe->maxLength        ();
//    int         describeMinLength         = oDescribe->minLength        ();
    QString     describeScheduleExpression = QtCommonString::gbkToQString( oDescribe->regularExpression() );
    QStringList describeValueScopes       = QtCommonString::gbkToQStrings(oDescribe->valueScopes      () );

    if (! describeIsVisible)
        return NULL;
    QWidget* oWidget = NULL;
    if (describeValueScopes.count()>0)
    {
        QComboBox* oComboBox = new QComboBox(oParent);
        oComboBox->insertItems(0, describeValueScopes);
        oWidget = oComboBox;
    }
    else
    {
        switch (oDescribe->valueType())
        {
        case CxValueType::ValueType_Integer :
        {
            const CxDescribeInteger* oIntegerDescribe = reinterpret_cast<const CxDescribeInteger*>(oDescribe);
            QSpinBox* oSpinBox;
            if (oIntegerDescribe->IsHexDisplay()) {
                oSpinBox = new THexSpinBox(oParent);
            } else {
                oSpinBox = new QSpinBox(oParent);
            }
            oSpinBox->setMinimum(oIntegerDescribe->minValue());
            oSpinBox->setMaximum(oIntegerDescribe->maxValue());
            oWidget = oSpinBox;
        }
            break;
        case CxValueType::ValueType_Enum :
        {
//            const CxDescribeEnum* oEnumDescribe = reinterpret_cast<const CxDescribeEnum*>(oDescribe);
        }
            break;
        case CxValueType::ValueType_Boolean :
        {
            const CxDescribeBoolean* oBooleanDescribe = reinterpret_cast<const CxDescribeBoolean*>(oDescribe);
            QCheckBox* oCheckBox = new QCheckBox(oParent);
            oCheckBox->setText( QtCommonString::gbkToQString( oBooleanDescribe->title()) );
            oWidget = oCheckBox;
        }
            break;
        case CxValueType::ValueType_Double :
        {
            const CxDescribeDouble* oDoubleDescribe = reinterpret_cast<const CxDescribeDouble*>(oDescribe);
            QDoubleSpinBox* oDoubleSpinBox = new QDoubleSpinBox(oParent);
            oDoubleSpinBox->setMinimum(oDoubleDescribe->minValue());
            oDoubleSpinBox->setMaximum(oDoubleDescribe->maxValue());
            oWidget = oDoubleSpinBox;
        }
            break;
        case CxValueType::ValueType_DateTime :
        {
            const CxDescribeDateTime* oDateTimeDescribe = reinterpret_cast<const CxDescribeDateTime*>(oDescribe);
            QDateTimeEdit* oDateTimeEdit = new QDateTimeEdit(oParent);
            QDateTime dtMinValue = QDateTime::fromMSecsSinceEpoch(oDateTimeDescribe->minValue());
            QDateTime dtMaxValue = QDateTime::fromMSecsSinceEpoch(oDateTimeDescribe->maxValue());
            oDateTimeEdit->setMinimumDateTime(dtMinValue);
            oDateTimeEdit->setMaximumDateTime(dtMaxValue);
            oWidget = oDateTimeEdit;
        }
            break;
        case CxValueType::ValueType_String :
        {
//            const CxDescribeString* oStringDescribe = reinterpret_cast<const CxDescribeString*>(oDescribe);
            QLineEdit* oLineEdit =  new QLineEdit(oParent);
            oLineEdit->setMaxLength(describeMaxLength);
            if (describeScheduleExpression.size()>0)
                oLineEdit->setValidator(new QRegExpValidator(QRegExp(describeScheduleExpression),oLineEdit));
            oWidget = oLineEdit;
        }
            break;
        }
    }
    if (oWidget)
    {
        oWidget->setObjectName(ObjectNameEd(describeName));
        oWidget->setToolTip(describeDesc);
        oWidget->setStatusTip(describeDesc);
        oWidget->setEnabled(! describeIsReadOnly);
//        oWidget->setMinimumWidth(200);
//        oWidget->setMinimumHeight(32);

        if (bCreateLabel)
        {
            QLabel* oLabel = new QLabel(oParent);
            oLabel->setObjectName(ObjectNameLb(describeName));
            oLabel->setText(describeTitle);
//            oLabel->setMinimumWidth(100);
//            oLabel->setMinimumHeight(32);
        }
    }
    return oWidget;
}

QWidget *DescribeWin::createControl(const string &sTypeSimpleName)
{
    QWidget* oWidget = NULL;
    if (sTypeSimpleName == CxValueType::simpleName<int>())
    {
        QSpinBox * oSpinBox = new QSpinBox();
        oWidget = oSpinBox;
    }
    else if (sTypeSimpleName == CxDescribeStrings::enumString())
    {
        QComboBox * oComboBox = new QComboBox();
        oWidget = oComboBox;
    }
    else if (sTypeSimpleName == CxValueType::simpleName<bool>())
    {
        QCheckBox* oCheckBox = new QCheckBox();
        oWidget = oCheckBox;
    }
    else if (sTypeSimpleName == CxValueType::simpleName<double>())
    {
        QDoubleSpinBox* oDoubleSpinBox = new QDoubleSpinBox();
        oWidget = oDoubleSpinBox;
    }
    else if (sTypeSimpleName == CxValueType::simpleName<msepoch_t>())
    {
        QDateTimeEdit* oDateTimeEdit = new QDateTimeEdit();
        oWidget = oDateTimeEdit;
    }
    else if (sTypeSimpleName == CxValueType::simpleName<string>())
    {
        QLineEdit* oLineEdit =  new QLineEdit();
        oWidget = oLineEdit;
    }

    return oWidget;
}

QString DescribeWin::controlValueCheck(const CxDescribeBase* oDescribe, QWidget *oWidget)
{
    QString sMsgError1 = QString("%1 - internal Error, className is different!").arg(QtCommonString::gbkToQString( oDescribe->name()) );
    QString sMsgError2 = QString("%1 - Error").arg(QtCommonString::gbkToQString( oDescribe->name()) );
    if (oDescribe->valueScopes().size()>0){
        if (oWidget->metaObject()->className() != QComboBox::staticMetaObject.className())
            return sMsgError1;
//        QComboBox* oComboBox = reinterpret_cast<QComboBox*>(oWidget);
    } else {
        switch (oDescribe->valueType()) {
        case CxValueType::ValueType_Integer : {
            const CxDescribeInteger* oIntegerDescribe = reinterpret_cast<const CxDescribeInteger*>(oDescribe);
            QSpinBox* oSpinBox = NULL;
            if (oWidget->metaObject()->className() == QSpinBox::staticMetaObject.className())
                oSpinBox = reinterpret_cast<QSpinBox*>(oWidget);
            else if (oWidget->metaObject()->className() == THexSpinBox::staticMetaObject.className()) {
                oSpinBox = reinterpret_cast<THexSpinBox*>(oWidget);
            } else {
                return sMsgError1;
            }
            if (oSpinBox->value() < oIntegerDescribe->minValue() || oSpinBox->value() > oIntegerDescribe->maxValue()){
                return sMsgError2;
            }
        }
            break;
        case CxValueType::ValueType_Enum : {
//            const CxDescribeEnum* oEnumDescribe = reinterpret_cast<const CxDescribeEnum*>(oDescribe);
        }
            break;
        case CxValueType::ValueType_Boolean : {
//            const CxDescribeBoolean* oBooleanDescribe = reinterpret_cast<const CxDescribeBoolean*>(oDescribe);
            if (oWidget->metaObject()->className() != QCheckBox::staticMetaObject.className())
                return sMsgError1;
//            QCheckBox* oCheckBox = reinterpret_cast<QCheckBox*>(oWidget);
        }
            break;
        case CxValueType::ValueType_Double : {
            const CxDescribeDouble* oDoubleDescribe = reinterpret_cast<const CxDescribeDouble*>(oDescribe);
            if (oWidget->metaObject()->className() != QDoubleSpinBox::staticMetaObject.className())
                return sMsgError1;
            QDoubleSpinBox* oDoubleSpinBox = reinterpret_cast<QDoubleSpinBox*>(oWidget);
            if (oDoubleSpinBox->value() < oDoubleDescribe->minValue() || oDoubleSpinBox->value() > oDoubleDescribe->maxValue()){
                return sMsgError2;
            }
        }
            break;
        case CxValueType::ValueType_DateTime : {
            const CxDescribeDateTime* oDateTimeDescribe = reinterpret_cast<const CxDescribeDateTime*>(oDescribe);
            if (oWidget->metaObject()->className() != QDateTimeEdit::staticMetaObject.className())
                return sMsgError1;
            QDateTimeEdit* oDateTimeEdit = reinterpret_cast<QDateTimeEdit*>(oWidget);
            QDateTime dtMinValue = QDateTime::fromMSecsSinceEpoch(oDateTimeDescribe->minValue());
            QDateTime dtMaxValue = QDateTime::fromMSecsSinceEpoch(oDateTimeDescribe->maxValue());
            if (oDateTimeEdit->dateTime() < dtMinValue || oDateTimeEdit->dateTime() > dtMaxValue){
                return sMsgError2;
            }
        }
            break;
        case CxValueType::ValueType_String : {
            const CxDescribeString* oStringDescribe = reinterpret_cast<const CxDescribeString*>(oDescribe);
            if (oWidget->metaObject()->className() != QLineEdit::staticMetaObject.className())
                return sMsgError1;
            QLineEdit* oLineEdit = reinterpret_cast<QLineEdit*>(oWidget);
            if (oLineEdit->text().size() < oStringDescribe->minLength() || oLineEdit->text().size() > oStringDescribe->maxLength()){
                return sMsgError2;
            }
        }
            break;
        }
    }
    return "";
}

void DescribeWin::controlViewIn(CxDescribeBase* oDescribe, QWidget* oWidget, bool* bChanged)
{
    bool bChanged2 = false;
    if (oDescribe->valueScopes().size()>0)
    {
        if (oWidget->metaObject()->className() != QComboBox::staticMetaObject.className())
            return;
        QComboBox* oComboBox = reinterpret_cast<QComboBox*>(oWidget);
        if (oDescribe->valueType() == CxValueType::ValueType_Enum)
        {
            CxDescribeEnum* oEnumDescribe = reinterpret_cast< CxDescribeEnum*>(oDescribe);
            if (oEnumDescribe->value() != oComboBox->currentIndex())
            {
                bChanged2 = true;
                oEnumDescribe->setValue(oComboBox->currentIndex());
            }
        }
        else
        {
            QStringList oValueScopes = QtCommonString::gbkToQStrings( oDescribe->valueScopes() );
            QString sValue;
            if (oValueScopes.count()>0 && oComboBox->currentIndex() > -1 && oComboBox->currentIndex() < oValueScopes.count())
                sValue = oValueScopes[oComboBox->currentIndex()];
            else
                sValue = oComboBox->currentText();
            std::string stdValue = QtCommonString::gbkToStdString( sValue );
            if (oDescribe->valueToString() != stdValue)
            {
                bChanged2 = true;
                oDescribe->valueFromString( stdValue );
            }
        }
    }
    else
    {
        switch (oDescribe->valueType())
        {
        case CxValueType::ValueType_Integer :
        {
            CxDescribeInteger* oIntegerDescribe = reinterpret_cast< CxDescribeInteger*>(oDescribe);
            QSpinBox* oSpinBox = NULL;
            if (oWidget->metaObject()->className() == QSpinBox::staticMetaObject.className())
                oSpinBox = reinterpret_cast<QSpinBox*>(oWidget);
            else if (oWidget->metaObject()->className() == THexSpinBox::staticMetaObject.className())
            {
                oSpinBox = reinterpret_cast<THexSpinBox*>(oWidget);
            } else
            {
                return;
            }
            if (oSpinBox->value() != oIntegerDescribe->value())
            {
                bChanged2 = true;
                oIntegerDescribe->setValue(oSpinBox->value());
            }
        }
            break;
        case CxValueType::ValueType_Enum :
        {
//            CxDescribeEnum* oEnumDescribe = reinterpret_cast< CxDescribeEnum*>(oDescribe);
        }
            break;
        case CxValueType::ValueType_Boolean :
        {
            CxDescribeBoolean* oBooleanDescribe = reinterpret_cast< CxDescribeBoolean*>(oDescribe);
            if (oWidget->metaObject()->className() != QCheckBox::staticMetaObject.className())
                return;
            QCheckBox* oCheckBox = reinterpret_cast<QCheckBox*>(oWidget);
            bool bValue = oCheckBox->checkState() == Qt::Checked;
            if (bValue != oBooleanDescribe->value())
            {
                bChanged2 = true;
                oBooleanDescribe->setValue(bValue);
            }
        }
            break;
        case CxValueType::ValueType_Double :
        {
            CxDescribeDouble* oDoubleDescribe = reinterpret_cast< CxDescribeDouble*>(oDescribe);
            if (oWidget->metaObject()->className() != QDoubleSpinBox::staticMetaObject.className())
                return;
            QDoubleSpinBox* oDoubleSpinBox = reinterpret_cast<QDoubleSpinBox*>(oWidget);
            if (oDoubleSpinBox->value() != oDoubleDescribe->value())
            {
                bChanged2 = true;
                oDoubleDescribe->setValue(oDoubleSpinBox->value());
            }
        }
            break;
        case CxValueType::ValueType_DateTime :
        {
            CxDescribeDateTime* oDateTimeDescribe = reinterpret_cast< CxDescribeDateTime*>(oDescribe);
            if (oWidget->metaObject()->className() != QDateTimeEdit::staticMetaObject.className())
                return;
            QDateTimeEdit* oDateTimeEdit = reinterpret_cast<QDateTimeEdit*>(oWidget);
            qlonglong dtValue = oDateTimeEdit->dateTime().toMSecsSinceEpoch();
            if (dtValue != oDateTimeDescribe->value())
            {
                bChanged2 = true;
                oDateTimeDescribe->setValue(dtValue);
            }
        }
            break;
        case CxValueType::ValueType_String :
        {
            CxDescribeString* oStringDescribe = reinterpret_cast< CxDescribeString*>(oDescribe);
            if (oWidget->metaObject()->className() != QLineEdit::staticMetaObject.className())
                return;
            QLineEdit* oLineEdit = reinterpret_cast<QLineEdit*>(oWidget);
            if (oLineEdit->text() != QtCommonString::gbkToQString( oStringDescribe->value() ))
            {
                bChanged2 = true;
                oStringDescribe->setValue( QtCommonString::gbkToStdString( oLineEdit->text()) );
            }
        }
            break;
        }
    }
    if (bChanged)
    {
        *bChanged = bChanged2;
    }
}

QVariant DescribeWin::controlViewIn(QWidget *oWidget)
{
    QVariant r;
    GM_INVALID_RETURE_(oWidget, r);
    if (oWidget->metaObject()->className() == QSpinBox::staticMetaObject.className())
    {
        QSpinBox * oEd = reinterpret_cast<QSpinBox*>(oWidget);
        return QVariant(oEd->value());
    }
    if (oWidget->metaObject()->className() == QComboBox::staticMetaObject.className())
    {
        QComboBox * oEd = reinterpret_cast<QComboBox*>(oWidget);
        return QVariant(oEd->currentText());
    }
    if (oWidget->metaObject()->className() == QCheckBox::staticMetaObject.className())
    {
        QCheckBox * oEd = reinterpret_cast<QCheckBox*>(oWidget);
        return QVariant(oEd->isChecked());
    }
    if (oWidget->metaObject()->className() == QDoubleSpinBox::staticMetaObject.className())
    {
        QDoubleSpinBox * oEd = reinterpret_cast<QDoubleSpinBox*>(oWidget);
        return QVariant(oEd->value());
    }
    if (oWidget->metaObject()->className() == QDateTimeEdit::staticMetaObject.className())
    {
        QDateTimeEdit * oEd = reinterpret_cast<QDateTimeEdit*>(oWidget);
        return QVariant(oEd->dateTime());
    }
    if (oWidget->metaObject()->className() == QLineEdit::staticMetaObject.className())
    {
        QLineEdit * oEd = reinterpret_cast<QLineEdit*>(oWidget);
        return QVariant(oEd->text());
    }
    return r;
}

void DescribeWin::controlViewOut(const CxDescribeBase* oDescribe, QWidget *oWidget, bool bIsDefault)
{
    if (oDescribe->valueScopes().size()>0)
    {
        if (oWidget->metaObject()->className() != QComboBox::staticMetaObject.className())
        {
            return;
        }
        QComboBox* oComboBox = reinterpret_cast<QComboBox*>(oWidget);
        QStringList oValueScopes = QtCommonString::gbkToQStrings( oDescribe->valueScopes() );
        if (oComboBox->count()<=0)
        {
            oComboBox->insertItems(0, oValueScopes);
        }
        if (oDescribe->valueType() == CxValueType::ValueType_Enum)
        {
            const CxDescribeEnum* oEnumDescribe = reinterpret_cast<const CxDescribeEnum*>(oDescribe);
            oComboBox->setCurrentIndex(oEnumDescribe->value());
        }
        else
        {
            QString sValue = (bIsDefault) ? QtCommonString::gbkToQString( oDescribe->defaultValueToString()) : QtCommonString::gbkToQString(oDescribe->valueToString() );
            int iIndexOf = oValueScopes.indexOf(sValue);
            if (oValueScopes.count()>0 && iIndexOf>=0 )
            {
                oComboBox->setCurrentIndex(iIndexOf);
            }
            else
            {
                oComboBox->setCurrentIndex(oComboBox->findText(sValue));
            }
        }
    }
    else
    {
        switch (oDescribe->valueType())
        {
        case CxValueType::ValueType_Integer :
        {
            const CxDescribeInteger* oIntegerDescribe = reinterpret_cast<const CxDescribeInteger*>(oDescribe);
            QSpinBox* oSpinBox = NULL;
            if (oWidget->metaObject()->className() == QSpinBox::staticMetaObject.className())
            {
                oSpinBox = reinterpret_cast<QSpinBox*>(oWidget);
            }
            else if (oWidget->metaObject()->className() == THexSpinBox::staticMetaObject.className())
            {
                oSpinBox = reinterpret_cast<THexSpinBox*>(oWidget);
            }
            else
            {
                return;
            }
            int iValue = (bIsDefault) ? oIntegerDescribe->defaultValue() : oIntegerDescribe->value();
            oSpinBox->setValue(iValue);
        }
            break;
        case CxValueType::ValueType_Enum :
        {
//            const CxDescribeEnum* oEnumDescribe = reinterpret_cast<const CxDescribeEnum*>(oDescribe);
        }
            break;
        case CxValueType::ValueType_Boolean :
        {
            const CxDescribeBoolean* oBooleanDescribe = reinterpret_cast<const CxDescribeBoolean*>(oDescribe);
            if (oWidget->metaObject()->className() != QCheckBox::staticMetaObject.className())
            {
                return;
            }
            QCheckBox* oCheckBox = reinterpret_cast<QCheckBox*>(oWidget);
            bool bValue = (bIsDefault) ? oBooleanDescribe->defaultValue() : oBooleanDescribe->value();
            Qt::CheckState eValue = (bValue) ? Qt::Checked : Qt::Unchecked;
            oCheckBox->setCheckState(eValue);
        }
            break;
        case CxValueType::ValueType_Double :
        {
            const CxDescribeDouble* oDoubleDescribe = reinterpret_cast<const CxDescribeDouble*>(oDescribe);
            if (oWidget->metaObject()->className() != QDoubleSpinBox::staticMetaObject.className())
            {
                return;
            }
            QDoubleSpinBox* oDoubleSpinBox = reinterpret_cast<QDoubleSpinBox*>(oWidget);
            double dValue = (bIsDefault) ? oDoubleDescribe->defaultValue() : oDoubleDescribe->value();
            oDoubleSpinBox->setValue(dValue);
        }
            break;
        case CxValueType::ValueType_DateTime :
        {
            const CxDescribeDateTime* oDateTimeDescribe = reinterpret_cast<const CxDescribeDateTime*>(oDescribe);
            if (oWidget->metaObject()->className() != QDateTimeEdit::staticMetaObject.className())
            {
                return;
            }
            QDateTimeEdit* oDateTimeEdit = reinterpret_cast<QDateTimeEdit*>(oWidget);
            qlonglong dtValue = (bIsDefault) ? oDateTimeDescribe->defaultValue() : oDateTimeDescribe->value();
            oDateTimeEdit->setDateTime(QDateTime::fromMSecsSinceEpoch( dtValue ));
        }
            break;
        case CxValueType::ValueType_String : {
            const CxDescribeString* oStringDescribe = reinterpret_cast<const CxDescribeString*>(oDescribe);
            if (oWidget->metaObject()->className() != QLineEdit::staticMetaObject.className())
            {
                return;
            }
            QLineEdit* oLineEdit = reinterpret_cast<QLineEdit*>(oWidget);
            QString sValue = (bIsDefault) ? QtCommonString::gbkToQString(oStringDescribe->defaultValue()) : QtCommonString::gbkToQString(oStringDescribe->value());
            oLineEdit->setText(sValue);
        }
            break;
        }
    }
    if (oWidget)
    {
        oWidget->setToolTip("");
        oWidget->setStatusTip("");
    }
}

void DescribeWin::controlViewOut(QWidget *oWidget, const string &sValue)
{
    GM_INVALID_RETURE(oWidget);
    if (oWidget->metaObject()->className() == QSpinBox::staticMetaObject.className())
    {
        QSpinBox * oEd = reinterpret_cast<QSpinBox*>(oWidget);
        oEd->setValue(CxString::toInt32(sValue));
    }
    if (oWidget->metaObject()->className() == QComboBox::staticMetaObject.className())
    {
        QComboBox * oEd = reinterpret_cast<QComboBox*>(oWidget);
        oEd->setEditText(QtCommonString::gbkToQString(sValue));
    }
    if (oWidget->metaObject()->className() == QCheckBox::staticMetaObject.className())
    {
        QCheckBox * oEd = reinterpret_cast<QCheckBox*>(oWidget);
        oEd->setChecked(CxString::toBoolean(sValue));
    }
    if (oWidget->metaObject()->className() == QDoubleSpinBox::staticMetaObject.className())
    {
        QDoubleSpinBox * oEd = reinterpret_cast<QDoubleSpinBox*>(oWidget);
        oEd->setValue(CxString::toDouble(sValue));
    }
    if (oWidget->metaObject()->className() == QDateTimeEdit::staticMetaObject.className())
    {
        QDateTimeEdit * oEd = reinterpret_cast<QDateTimeEdit*>(oWidget);
        oEd->setDateTime( QDateTime::fromString(QtCommonString::gbkToQString(sValue)) );
    }
    if (oWidget->metaObject()->className() == QLineEdit::staticMetaObject.className())
    {
        QLineEdit * oEd = reinterpret_cast<QLineEdit*>(oWidget);
        oEd->setText(QtCommonString::gbkToQString(sValue));
    }
}

int DescribeWin::createControls(const CxDescribeUnit* oDescribeUnit, QWidget *oParent)
{
    int iNewControl = 0;
    const std::vector<CxDescribeBase*> & describes = oDescribeUnit->describes();
    for (size_t i = 0; i < describes.size(); ++i)
    {
        CxDescribeBase* oDescribe = describes.at(i);
        QWidget * oWidget = findControl(oParent, QtCommonString::gbkToQString(oDescribe->name()));
        if ( ! oWidget )
        {
            if (createControl(oDescribe, oParent))
                ++iNewControl;
        }
//        else
//        {
//            if (oWidget->metaObject()->className() != QComboBox::staticMetaObject.className())
//            {
//                QComboBox* oComboBox = reinterpret_cast<QComboBox*>(oWidget);
//                if (oDescribe->valueScopes().size()>0 && oComboBox->size())
//            }
//        }
    }
    return iNewControl;
}

void DescribeWin::saveUiFile(const CxDescribeUnit* oDescribeUnit, const QString &sFilePath)
{
    QWidget* oParent = new QWidget();
    oParent->setObjectName(QtCommonString::gbkToQString(oDescribeUnit->unitName()));
    oParent->setGeometry(200, 200, 800, 600);
    createControls(oDescribeUnit, oParent);
    saveControls(oParent, sFilePath);
    delete oParent;
}

class QFormBuilder1 : public QFormBuilder
{
public:

protected:

    QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name){
        if (widgetName == THexSpinBox::staticMetaObject.className()){
            QWidget* o = new THexSpinBox(parentWidget);
            o->setObjectName(name);
            return o;
        }
        return QFormBuilder::createWidget(widgetName, parentWidget, name);
    }


};

void DescribeWin::saveControls(QWidget *oWidget, const QString &sFilePath)
{
    QFormBuilder1 builder;
    QFile file(sFilePath);
    if (file.open(QFile::WriteOnly)){
        oWidget->setVisible(true);
        builder.save(&file, oWidget);
        file.close();
    }
}

QWidget* DescribeWin::loadControls(const QString &sUiFilePath, QWidget* oParent)
{
    QFormBuilder1 builder;
    QFile file(sUiFilePath);
    if (file.open(QFile::ReadOnly)) {
        QWidget* oWidget = builder.load(&file, oParent);
        file.close();
        return oWidget;
    }
    return NULL;
}

QWidget * DescribeWin::findControl(QWidget *oParent, const QString &sDescribeName)
{
    QString sObjectName = ObjectNameEd(sDescribeName);
    QWidget* oResult = NULL;
    if (oParent){
        oResult = oParent->findChild<QWidget*>(sObjectName);
    }
    return oResult;
}

void DescribeWin::controlsViewOut(const CxDescribeUnit* oDescribeUnit, QWidget *oParent, bool bIsDefault)
{
    GM_INVALID_RETURE(oDescribeUnit);
    const std::vector<CxDescribeBase*> & describes = oDescribeUnit->describes();
    for (size_t i = 0; i < describes.size(); ++i)
    {
        CxDescribeBase* oDescribe = describes.at(i);
        QWidget* oWidget = findControl(oParent, QtCommonString::gbkToQString(oDescribe->name()));
        if (oWidget)
        {
            controlViewOut(oDescribe, oWidget, bIsDefault);
        }
    }
}

QString DescribeWin::controlsValueCheck(const CxDescribeUnit* oDescribeUnit, QWidget *oParent)
{
    const std::vector<CxDescribeBase*> & describes = oDescribeUnit->describes();
    for (size_t i = 0; i < describes.size(); ++i)
    {
        CxDescribeBase* oDescribe = describes.at(i);
        QWidget* oWidget = findControl(oParent, QtCommonString::gbkToQString(oDescribe->name()));
        if (oWidget)
        {
            QString sMsg = controlValueCheck(oDescribe, oWidget);
            if (sMsg.size()>0)
            {
                return sMsg;
            }
        }
    }
    return "";
}

QHash<QString, QString> DescribeWin::controlsViewIn(CxDescribeUnit* oDescribeUnit, QWidget *oParent)
{
    QHash<QString, QString> changedNameValues;
    const std::vector<CxDescribeBase*> & describes = oDescribeUnit->describes();
    for (size_t i = 0; i < describes.size(); ++i) {
        CxDescribeBase* oDescribe = describes.at(i);
        if (oDescribe->isReadOnly())
            continue;
        QWidget* oWidget = findControl(oParent, QtCommonString::gbkToQString(oDescribe->name()));
        if (oWidget){
            bool bChanged;
            controlViewIn(oDescribe, oWidget, &bChanged);
            if (bChanged){
                changedNameValues[QtCommonString::gbkToQString(oDescribe->name())] = QtCommonString::gbkToQString(oDescribe->valueToString());
            }
        }
    }
    return changedNameValues;
}

QWidget *DescribeWin::loadControls(CxDescribeUnit *oUnit)
{
    GM_INVALID_RETURE_(oUnit, NULL);
    QGroupBox * tabContainer;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *verticalSpacer;

    tabContainer = new QGroupBox();
//    tabContainer->setObjectName(QString::fromUtf8("tab"));
    tabContainer->setTitle(QString::fromUtf8("配置数据"));
    verticalLayout_2 = new QVBoxLayout(tabContainer);
    verticalLayout_2->setSpacing(6);
    verticalLayout_2->setContentsMargins(11, 11, 11, 11);
//    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    scrollArea = new QScrollArea(tabContainer);
//    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
//    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 632, 232));
    verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
    verticalLayout_3->setSpacing(6);
    verticalLayout_3->setContentsMargins(11, 11, 11, 11);
//    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));

    const std::vector<CxDescribeBase*> & describes = oUnit->describes();
    QString sTitle;
    for (size_t i = 0; i < describes.size(); ++i)
    {
        CxDescribeBase * oDescribe = describes.at(i);
        QWidget * oEd = createControl(oDescribe, tabContainer, false);
        if (! oEd) continue;
        oEd->setMinimumHeight(30);
        sTitle = QtCommonString::gbkToQString(oDescribe->title());
//        sName = "ed_" + QString::number(i);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
//        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(scrollAreaWidgetContents);
//        label->setObjectName(QString::fromUtf8("label"));
        label->setText(sTitle);
        label->setMinimumWidth(120);
        label->setMinimumHeight(30);

        horizontalLayout->addWidget(label);
        horizontalLayout->addWidget(oEd);
        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 10);

        verticalLayout_3->addLayout(horizontalLayout);
    }

    verticalSpacer = new QSpacerItem(20, 183, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_3->addItem(verticalSpacer);

    scrollArea->setWidget(scrollAreaWidgetContents);

    verticalLayout_2->addWidget(scrollArea);

    return tabContainer;
}

class _EditorInfo_
{
public:
    string name;
    string type;
    string value;

    _EditorInfo_(string sName, string sType, string sValue) :
        name(sName), type(sType), value(sValue)
    {
    }

};

QWidget *DescribeWin::loadControls(CxUnitAttr * oUnitAttr)
{
    GM_INVALID_RETURE_(oUnitAttr, NULL);

    QGroupBox *tabContainer;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *reloadDataNameBn;
    QPushButton *saveDataNameBn;
    QPushButton *redefaultDataNamBn;

    tabContainer = new QGroupBox();
//    tabContainer->setObjectName(QString::fromUtf8("tab"));
    tabContainer->setTitle(QString::fromUtf8("数据存放"));
    verticalLayout_2 = new QVBoxLayout(tabContainer);
    verticalLayout_2->setSpacing(6);
    verticalLayout_2->setContentsMargins(11, 11, 11, 11);
//    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    scrollArea = new QScrollArea(tabContainer);
//    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
//    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 632, 232));
    verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
    verticalLayout_3->setSpacing(6);
    verticalLayout_3->setContentsMargins(11, 11, 11, 11);
//    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));

    vector<_EditorInfo_> editorInfos;
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::dataType(), CxValueType::simpleName<string>(), oUnitAttr->dataType));
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::dataPath(), CxValueType::simpleName<string>(), oUnitAttr->dataPath));
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::dataSection(), CxValueType::simpleName<string>(), oUnitAttr->dataSection));
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::dataName(), CxValueType::simpleName<string>(), oUnitAttr->dataName));
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::dataParam(), CxValueType::simpleName<string>(), oUnitAttr->dataParam));
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::dataRewrite(), CxValueType::simpleName<bool>(), CxString::toString(oUnitAttr->dataRewrite)));
    editorInfos.push_back(_EditorInfo_(CxDescribeStrings::describeFilePath(), CxValueType::simpleName<string>(), oUnitAttr->describeFilePath));

    QString sTitle;
    QString sName;
    for (size_t i = 0; i < editorInfos.size(); ++i)
    {
        const _EditorInfo_ & editorInfo = editorInfos.at(i);
        QWidget * oEd = createControl(editorInfo.type);
        if (! oEd) continue;
        controlViewOut(oEd, editorInfo.value);
        oEd->setMinimumHeight(30);
        sName = ObjectNameEd(QtCommonString::gbkToQString(editorInfo.name));
        oEd->setObjectName(sName);
        if (editorInfo.name == CxDescribeStrings::describeFilePath())
        {
            oEd->setEnabled(false);
        }
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
//        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        sTitle = QtCommonString::gbkToQString(editorInfo.name);
        label = new QLabel(scrollAreaWidgetContents);
//        label->setObjectName(QString::fromUtf8("label"));
        label->setText(sTitle);
        label->setMinimumWidth(80);
        label->setMinimumHeight(30);

        horizontalLayout->addWidget(label);
        horizontalLayout->addWidget(oEd);
        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 10);

        verticalLayout_3->addLayout(horizontalLayout);
    }

    verticalSpacer = new QSpacerItem(20, 183, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_3->addItem(verticalSpacer);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(6);
//    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    reloadDataNameBn = new QPushButton(tabContainer);
    reloadDataNameBn->setObjectName(ObjectNameEd("reloadDataNameBn"));
    reloadDataNameBn->setText(QString::fromUtf8("重载DataName"));
    reloadDataNameBn->setMinimumSize(QSize(0, 30));

    horizontalLayout_2->addWidget(reloadDataNameBn);

    saveDataNameBn = new QPushButton(tabContainer);
    saveDataNameBn->setObjectName(ObjectNameEd("saveDataNameBn"));
    saveDataNameBn->setText(QString::fromUtf8("保存DataName"));
    saveDataNameBn->setMinimumSize(QSize(0, 30));

    horizontalLayout_2->addWidget(saveDataNameBn);

    verticalLayout_3->addLayout(horizontalLayout_2);

    redefaultDataNamBn = new QPushButton(tabContainer);
    redefaultDataNamBn->setObjectName(ObjectNameEd("redefaultDataNamBn"));
    redefaultDataNamBn->setText(QString::fromUtf8("还原默认值"));
    redefaultDataNamBn->setMinimumSize(QSize(0, 30));

    verticalLayout_3->addWidget(redefaultDataNamBn);

    scrollArea->setWidget(scrollAreaWidgetContents);

    verticalLayout_2->addWidget(scrollArea);

    return tabContainer;
}

void DescribeWin::controlsViewIn(QWidget *oParent, CxUnitAttr *oUnitAttr)
{
    GM_INVALID_RETURE(oParent);
    GM_INVALID_RETURE(oUnitAttr);
    QVariant rValue;
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::dataType())));
    if (rValue.type() == QVariant::String)
    {
        oUnitAttr->dataType = QtCommonString::gbkToStdString(rValue.toString());
    }
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::dataPath())));
    if (rValue.type() == QVariant::String)
    {
        oUnitAttr->dataPath = QtCommonString::gbkToStdString(rValue.toString());
    }
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::dataSection())));
    if (rValue.type() == QVariant::String)
    {
        oUnitAttr->dataSection = QtCommonString::gbkToStdString(rValue.toString());
    }
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::dataName())));
    if (rValue.type() == QVariant::String)
    {
        oUnitAttr->dataName = QtCommonString::gbkToStdString(rValue.toString());
    }
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::dataParam())));
    if (rValue.type() == QVariant::String)
    {
        oUnitAttr->dataParam = QtCommonString::gbkToStdString(rValue.toString());
    }
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::dataRewrite())));
    if (rValue.type() == QVariant::Bool)
    {
        oUnitAttr->dataRewrite = rValue.toBool();
    }
    rValue = controlViewIn(findControl(oParent, QtCommonString::gbkToQString(CxDescribeStrings::describeFilePath())));
    if (rValue.type() == QVariant::String)
    {
        oUnitAttr->describeFilePath = QtCommonString::gbkToStdString(rValue.toString());
    }
}
