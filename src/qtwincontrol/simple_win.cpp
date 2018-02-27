#include "simple_win.h"

#include <ccxx/qtcommon.h>

#include "describe_win.h"

SimpleWin::SimpleWin()
{
    _describeUnit = new CxDescribeUnit();
    _panel = NULL;
}

SimpleWin::~SimpleWin()
{
    delete _describeUnit;
}

QWidget* SimpleWin::loadWin(QWidget* oParent, const std::string & sUiFilePath)
{
    QString sFilePath = QtCommonString::gbkToQString(sUiFilePath);
    QWidget * oPanel = DescribeWin::loadControls(sFilePath, oParent);
    if (oPanel){
        if ( DescribeWin::createControls(_describeUnit, oPanel) > 0 ) {
        }
    } else {
        DescribeWin::saveUiFile(_describeUnit, sFilePath);
        oPanel = DescribeWin::loadControls(sFilePath, oParent);
    }
    _panel = oPanel;
    return oPanel;
}

void SimpleWin::controlsViewIn()
{
    if (_panel)
        _describeNameValuesChanged = DescribeWin::controlsViewIn(_describeUnit, _panel);
}

void SimpleWin::controlsViewOut(bool bIsDefault)
{
    if (_panel)
        DescribeWin::controlsViewOut(_describeUnit, _panel, bIsDefault);
}

void SimpleWin::controlsViewOutCalc()
{
    if (_panel) {
        if ( _describeUnit->describeValuesCalc(_describeUnit) <= 0) {
            _describeUnit->restoreDefaultValues();
        }
        DescribeWin::controlsViewOut(_describeUnit, _panel);
    }
}

QString SimpleWin::controlsValueCheck()
{
    if (_panel)
        return DescribeWin::controlsValueCheck(_describeUnit, _panel);
    return "";
}

