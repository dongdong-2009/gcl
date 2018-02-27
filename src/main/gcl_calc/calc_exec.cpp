#include "calc_exec.h"
#include "math_parser.h"

#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include "calc_measure.h"
#include <psm/general_access.h>


#define WT_NO_WARN          0
#define WT_CHANGE_WARN      1
#define WT_0TO1_WARN        2
#define WT_1TO0_WARN        3
#define WT_ORER_WARN        4
#define WT_HIGH_WARN        5
#define WT_LOW_WARN         6

//#define QV_NORMAL         0
#define QV_HIGH             1
#define QV_LOW              2
#define QV_0TO1             3
#define QV_1TO0             4


using namespace std;

map<int, int> g_mMonsbOldValue;

map<int, bool> g_mYcaddWarnHigh;
map<int, bool> g_mYcaddWarnLow;

map<int, bool> g_mCalcWarnHigh;
map<int, bool> g_mCalcWarnLow;


//静态初始化
CalcExec * fn_oCalcExec()
{
    CxApplication::registStartFunction(CalcExec::start);
    CxApplication::registStopFunction(CalcExec::stop);

    static CalcExec m;
    return &m;
}


static CalcExec * f_oCalcExec = fn_oCalcExec();


void CalcExec::start()
{
    CxTimerManager::startTimer(CalcExec::timerTimeout, 1000);
    GeneralAccess::start();
}

void CalcExec::stop()
{
    CxTimerManager::stopTimer(CalcExec::timerTimeout);
    GeneralAccess::stop();
}

void CalcExec::timerTimeout(int iInterval)
{
    cout << "calculate process runing." << endl;

    CalcProcess();

    WarnProcess();
}

void CalcExec::CalcProcess()
{
    int iSourceId = 0;
    CalcMeasure::beginSetValues(0, iSourceId);

    for (size_t i = 0; i < CalcManager::measures()->size(); i++)
    {
        int nMeasureId = 0x05000000+i;
        CalcMeasure* pCalc = CalcManager::measure(nMeasureId);
        if (pCalc != NULL)
        {
            CMathParser mp(pCalc->getExpression());
            double dValue = mp.parse();

            if (!mp.error())
            {
                pCalc->setValue(dValue);
            }
            else
            {
                cout<<"calculate expression error!"<<endl;
            }
        }
    }

    CalcMeasure::endSetValues();
}

void CalcExec::WarnProcess()
{
    MonsbWarnProc();
    YcaddWarnProc();
    CalcWarnProc();
}

void CalcExec::MonsbWarnProc()
{
    for (size_t i = 0; i < MonsbManager::measures()->size(); i++)
    {
        int nMeasureId = 0x01000000+i;
        MonsbMeasure* pMonsb = MonsbManager::measure(nMeasureId);
        if (pMonsb == NULL)
            continue;

        int nWarnType = pMonsb->getWarnType();
        if (nWarnType == WT_NO_WARN)
            continue;

        map<int, int>::iterator itOldValue = g_mMonsbOldValue.find(nMeasureId);
        if (itOldValue == g_mMonsbOldValue.end())
            continue;

        int nValue = pMonsb->value();
        if (nValue == itOldValue->second)
            continue;

        if (nValue == 1)
        {
            if ((nWarnType == WT_CHANGE_WARN) || (nWarnType == WT_0TO1_WARN))
            {
                pMonsb->setQuality(QV_0TO1);
            }
        }
        else if (nValue == 0)
        {
            if ((nWarnType == WT_CHANGE_WARN) || (nWarnType == WT_1TO0_WARN))
            {
                pMonsb->setQuality(QV_1TO0);
            }
        }

        itOldValue->second = nValue;
    }
}

void CalcExec::YcaddWarnProc()
{
    for (size_t i = 0; i < YcaddManager::measures()->size(); i++)
    {
        int nMeasureId = 0x02000000+i;
        YcaddMeasure* pYcadd = YcaddManager::measure(nMeasureId);
        if (pYcadd == NULL)
            continue;

        int nWarnType = pYcadd->getWarnType();
        if (nWarnType == WT_NO_WARN)
            continue;

        map<int, bool>::iterator itWarnHigh = g_mYcaddWarnHigh.find(nMeasureId);
        map<int, bool>::iterator itWarnLow = g_mYcaddWarnLow.find(nMeasureId);
        if ((itWarnHigh == g_mYcaddWarnHigh.end()) || (itWarnLow == g_mYcaddWarnLow.end()))
            continue;

        double dValue = pYcadd->value();
        if (dValue > pYcadd->getRunHigh())
        {
            if ((nWarnType == WT_ORER_WARN) || (nWarnType == WT_HIGH_WARN))
            {
                if (!itWarnHigh->second)
                {
                    itWarnHigh->second = true;
                    pYcadd->setQuality(QV_HIGH);
                //  cout << "MeasureId:" << nMeasureId << " over run high; value = " << pCalc->value() << endl;
                }
            }
        }
        else if (dValue < pYcadd->getRunLow())
        {
            if ((nWarnType == WT_ORER_WARN) || (nWarnType == WT_LOW_WARN))
            {
                if (!itWarnLow->second)
                {
                    itWarnLow->second = true;
                    pYcadd->setQuality(QV_LOW);
                //  cout << "MeasureId:" << nMeasureId << " over run high; value = " << pCalc->value() << endl;
                }
            }
        }
        else
        {
            itWarnHigh->second = false;
            itWarnLow->second = false;

        //  pYcadd->setQuality(QV_NORMAL);
        }
    }
}

void CalcExec::CalcWarnProc()
{
    for (size_t i = 0; i < CalcManager::measures()->size(); i++)
    {
        int nMeasureId = 0x05000000+i;
        CalcMeasure* pCalc = CalcManager::measure(nMeasureId);
        if (pCalc == NULL)
            continue;

        int nWarnType = pCalc->getWarnType();
        if (nWarnType == WT_NO_WARN)
            continue;

        map<int, bool>::iterator itWarnHigh = g_mCalcWarnHigh.find(nMeasureId);
        map<int, bool>::iterator itWarnLow = g_mCalcWarnLow.find(nMeasureId);
        if ((itWarnHigh == g_mCalcWarnHigh.end()) || (itWarnLow == g_mCalcWarnLow.end()))
            continue;

        double dValue = pCalc->value();
        if (dValue > pCalc->getRunHigh())
        {
            if ((nWarnType == WT_ORER_WARN) || (nWarnType == WT_HIGH_WARN))
            {
                if (!itWarnHigh->second)
                {
                    itWarnHigh->second = true;
                    pCalc->setQuality(QV_HIGH);
                //  cout << "MeasureId:" << nMeasureId << " over run high; value = " << pCalc->value() << endl;
                }
            }
        }
        else if (dValue < pCalc->getRunLow())
        {
            if ((nWarnType == WT_ORER_WARN) || (nWarnType == WT_LOW_WARN))
            {
                if (!itWarnLow->second)
                {
                    itWarnLow->second = true;
                    pCalc->setQuality(QV_LOW);
                //  cout << "MeasureId:" << nMeasureId << " over run high; value = " << pCalc->value() << endl;
                }
            }
        }
        else
        {
            itWarnHigh->second = false;
            itWarnLow->second = false;

        //  pCalc->setQuality(QV_NORMAL);
        }
    }
}

