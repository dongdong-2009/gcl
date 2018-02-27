#include <ccxx/cxapplication.h>
#include <ccxx/cxtimer.h>
#include <common/gcl_config.h>
#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>
#include "calc_measure.h"
#include <vdi/common_service.h>

//#include "math_parser.h"

using namespace std;


extern map<int, int> g_mMonsbOldValue;

extern map<int, bool> g_mYcaddWarnHigh;
extern map<int, bool> g_mYcaddWarnLow;

extern map<int, bool> g_mCalcWarnHigh;
extern map<int, bool> g_mCalcWarnLow;


int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    cout << "calculate process start." << endl;

    GclConfig::load();

    CommonService::init();

    MonsbManager::loadFromFile( GclConfig::monsbMeasurePath() );
    YcaddManager::loadFromFile( GclConfig::ycaddMeasurePath() );
    StrawManager::loadFromFile( GclConfig::strawMeasurePath() );
    CalcManager::loadFromFile( GclConfig::calcMeasurePath() );

    cout << "yx count : " << MonsbManager::measures()->size() << endl;
    cout << "yc count : " << YcaddManager::measures()->size() << endl;
    cout << "yw count : " << StrawManager::measures()->size() << endl;
    cout << "js count : " << CalcManager::measures()->size() << endl;

    for (size_t i = 0; i < MonsbManager::measures()->size(); i++)
    {
        int nMeasureId = 0x01000000+i;
        MonsbMeasure* pMonsb = MonsbManager::measure(nMeasureId);
        if (pMonsb != NULL)
        {
            g_mMonsbOldValue[nMeasureId] = pMonsb->value();
        }
    }

    for (size_t i = 0; i < YcaddManager::measures()->size(); i++)
    {
        int nMeasureId = 0x02000000+i;
        YcaddMeasure* pYcadd = YcaddManager::measure(nMeasureId);
        if (pYcadd != NULL)
        {
            g_mYcaddWarnHigh[nMeasureId] = false;
            g_mYcaddWarnLow[nMeasureId] = false;
        }
    }

    for (size_t i = 0; i < CalcManager::measures()->size(); i++)
    {
        int nMeasureId = 0x05000000+i;
        CalcMeasure* pCalc = CalcManager::measure(nMeasureId);
        if (pCalc != NULL)
        {
            g_mCalcWarnHigh[nMeasureId] = false;
            g_mCalcWarnLow[nMeasureId] = false;
        }
    }

/*
    cout<<endl;
    string szExpression;

    do {
        cout<<"Please input calculate expression: ";
        cin>>szExpression;

        CMathParser mp(szExpression);
        if (szExpression == "quit")
        {
            cout<<"quit calculate!";
            break;
        }

        double dValue = mp.parse();
        if (mp.error())
        {
            cout<<"calculate expression error!"<<endl;
        }
        else
        {
            cout<<"calculate result: "<<szExpression<<" = "<<dValue<<endl;
        }

        cout<<endl;
    }while (true);
*/
    int iResult = CxApplication::exec();

    return iResult;
}

