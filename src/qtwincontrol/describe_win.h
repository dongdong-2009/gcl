#ifndef DESCRIBE_WIN_H
#define DESCRIBE_WIN_H


#include <QString>
#include <QHash>
#include <QDateTime>

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QFormBuilder>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>

#include <ccxx/cxdescribe.h>
#include <ccxx/cxtime.h>

class DescribeWin
{
public:
    static QWidget* createControl(const CxDescribeBase* oDescribe, QWidget* oParent, bool bCreateLabel=true);

    static QWidget* createControl(const std::string & sTypeSimpleName);

    static void controlViewOut(const CxDescribeBase* oDescribe, QWidget* oWidget, bool bIsDefault = false);

    static void controlViewOut(QWidget* oWidget, const std::string & sValue);

    static QString controlValueCheck(const CxDescribeBase* oDescribe, QWidget* oWidget);

    static void controlViewIn(CxDescribeBase* oDescribe, QWidget* oWidget, bool* bChanged = NULL);

    static QVariant controlViewIn(QWidget* oWidget);

    static int createControls(const CxDescribeUnit* oDescribeUnit, QWidget* oParent);

    static void saveUiFile(const CxDescribeUnit* oDescribeUnit, const QString& sFilePath);

    static void saveControls(QWidget* oWidget, const QString& sFilePath);

    static QWidget* loadControls(const QString& sUiFilePath, QWidget* oParent);

    static QWidget* findControl(QWidget *oParent, const QString &sDescribeName);

    static void controlsViewOut(const CxDescribeUnit* oDescribeUnit, QWidget* oParent, bool bIsDefault = false);

    static QString controlsValueCheck(const CxDescribeUnit* oDescribeUnit, QWidget* oParent);

    static QHash<QString, QString> controlsViewIn(CxDescribeUnit* oDescribeUnit, QWidget* oParent);

    static QWidget* loadControls(CxDescribeUnit * oUnit);

    static QWidget* loadControls(CxUnitAttr * oUnitAttr);

    static void controlsViewIn(QWidget* oParent, CxUnitAttr * oUnitAttr);

};

#endif // DESCRIBE_WIN_H
