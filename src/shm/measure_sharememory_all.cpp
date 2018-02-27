#include "measure_sharememory_all.h"

#include <common/gcl_resource.h>


using namespace std;


static fn_void_int_t fn_measureShareMemoryAfterLoadCallBack = 0;
static fn_void_int_t fn_measureShareMemoryBeforeCloseCallBack = 0;

static bool f_bCanWriteMeasureShareMemory = false;


void MeasureShareMemoryAll::open(bool bCanWrite, int iCheckInterval)
{
    f_bCanWriteMeasureShareMemory = bCanWrite;
    timerCheckSharememory(iCheckInterval);
    CxTimerManager::startTimer(MeasureShareMemoryAll::timerCheckSharememory, 200); //程序退出时自动 stopTimer
}

void MeasureShareMemoryAll::timerCheckSharememory(int iInterval)
{
    //加载遥信量的共享内存
    if (MonsbShareMemory::memoryManager()->status() == ci_sharememory_status_invalid)
    {
        std::string sMonsbMapName = CxApplication::findConfig(CS_SectionShareMemory, CS_EntryMonsbMapName, std::string());
        if (sMonsbMapName.size()>0)
        {
            int iMonsbCount = f_bCanWriteMeasureShareMemory ? CxApplication::findConfig(CS_SectionShareMemory, CS_EntryMonsbCount, int(0)) : 0;
            MonsbShareMemory::memoryManager()->load(sMonsbMapName, iMonsbCount);
            if (MonsbShareMemory::memoryManager()->status() == ci_sharememory_status_good)
            {
                if (fn_measureShareMemoryAfterLoadCallBack) fn_measureShareMemoryAfterLoadCallBack(MeasureType_Monsb);
            }
        }
    }
    else if (MonsbShareMemory::memoryManager()->status() == ci_sharememory_status_closing)
    {
        if (fn_measureShareMemoryBeforeCloseCallBack) fn_measureShareMemoryBeforeCloseCallBack(MeasureType_Monsb);
        MonsbShareMemory::memoryManager()->shutdown();
    }

    //加载遥测量的共享内存
    if (YcaddShareMemory::memoryManager()->status() == ci_sharememory_status_invalid)
    {
        std::string sYcaddMapName = CxApplication::findConfig(CS_SectionShareMemory, CS_EntryYcaddMapName, std::string());
        if (sYcaddMapName.size()>0)
        {
            int iYcaddCount = f_bCanWriteMeasureShareMemory ? CxApplication::findConfig(CS_SectionShareMemory, CS_EntryYcaddCount, int(0)) : 0;
            YcaddShareMemory::memoryManager()->load(sYcaddMapName, iYcaddCount);
            if (YcaddShareMemory::memoryManager()->status() == ci_sharememory_status_good)
            {
                if (fn_measureShareMemoryAfterLoadCallBack) fn_measureShareMemoryAfterLoadCallBack(MeasureType_Ycadd);
            }
        }
    }
    else if (YcaddShareMemory::memoryManager()->status() == ci_sharememory_status_closing)
    {
        if (fn_measureShareMemoryBeforeCloseCallBack) fn_measureShareMemoryBeforeCloseCallBack(MeasureType_Ycadd);
        YcaddShareMemory::memoryManager()->shutdown();
    }

    //加载遥文量的共享内存
    if (StrawShareMemory::memoryManager()->status() == ci_sharememory_status_invalid)
    {
        std::string sStrawMapName = CxApplication::findConfig(CS_SectionShareMemory, CS_EntryStrawMapName, std::string());
        if (sStrawMapName.size()>0)
        {
            int iStrawCount = f_bCanWriteMeasureShareMemory ? CxApplication::findConfig(CS_SectionShareMemory, CS_EntryStrawCount, int(0)) : 0;
            StrawShareMemory::memoryManager()->load(sStrawMapName, iStrawCount);
            if (StrawShareMemory::memoryManager()->status() == ci_sharememory_status_good)
            {
                if (fn_measureShareMemoryAfterLoadCallBack) fn_measureShareMemoryAfterLoadCallBack(MeasureType_Straw);
            }
        }
    }
    else if (StrawShareMemory::memoryManager()->status() == ci_sharememory_status_closing)
    {
        if (fn_measureShareMemoryBeforeCloseCallBack) fn_measureShareMemoryBeforeCloseCallBack(MeasureType_Straw);
        StrawShareMemory::memoryManager()->shutdown();
    }
}

void MeasureShareMemoryAll::setBeforeCloseCallback(fn_void_int_t fn)
{
    fn_measureShareMemoryBeforeCloseCallBack = fn;
}

void MeasureShareMemoryAll::setAfterLoadCallback(fn_void_int_t fn)
{
    fn_measureShareMemoryAfterLoadCallBack = fn;
}

void MeasureShareMemoryAll::close()
{
    if (! MonsbShareMemory::memoryManager()->isReadOnly())
    {
        MonsbShareMemory::memoryManager()->setStatus(ci_sharememory_status_closing);
    }
    if (! YcaddShareMemory::memoryManager()->isReadOnly())
    {
        YcaddShareMemory::memoryManager()->setStatus(ci_sharememory_status_closing);
    }
    if (! StrawShareMemory::memoryManager()->isReadOnly())
    {
        StrawShareMemory::memoryManager()->setStatus(ci_sharememory_status_closing);
    }

    MonsbShareMemory::memoryManager()->shutdown();
    YcaddShareMemory::memoryManager()->shutdown();
    StrawShareMemory::memoryManager()->shutdown();
}

void MeasureShareMemoryAll::dealRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    switch (iParamType) {

        //*yx
        //遥信处理 vqt文本方式
    case ci_yx_address_vqt_intText1_t:
    {
        if (iDataLength < sizeof(gcd_address_vqt_intText1_t)*iParamCount)
            return;

        MonsbMemoryManager * oMemoryManager = MonsbShareMemory::memoryManager();
        assert(oMemoryManager != NULL);

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        oMemoryManager->beginSetValue(iSourceId);
        const gcd_address_vqt_intText1_t * oYx = (const gcd_address_vqt_intText1_t *)pData;
        int iValueLength;
        for (int i = 0; i < iParamCount; ++i) {
            iValueLength = strlen(oYx->value);
            if (iValueLength>0 && iValueLength<8)
            {
                oMemoryManager->setValue(oYx->address, CxString::toInt32(oYx->value), oYx->quality, oYx->datetime);
            }
            oYx++;
        }
        oMemoryManager->endSetValue();
    }
        break;

        //遥信处理 vqt整形方式
    case ci_yx_address_int32_vqt_t:
    {
        if (iDataLength < sizeof(gcd_address_int32_vqt_t)*iParamCount)
            return;

        MonsbMemoryManager * oMemoryManager = MonsbShareMemory::memoryManager();
        assert(oMemoryManager != NULL);

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        oMemoryManager->beginSetValue(iSourceId);
        const gcd_address_int32_vqt_t * oYx = (const gcd_address_int32_vqt_t *)pData;
        for (int i = 0; i < iParamCount; ++i) {
            oMemoryManager->setValue(oYx->address, oYx->value, oYx->quality, oYx->datetime);
            oYx++;
        }
        oMemoryManager->endSetValue();
    }
        break;

        //*yc
        //遥测处理 vqt文本方式
    case ci_yc_address_vqt_intText1_t:
    {
        if (iDataLength < sizeof(gcd_address_vqt_realText15_t)*iParamCount)
            return;

        YcaddMemoryManager * oMemoryManager = YcaddShareMemory::memoryManager();
        assert(oMemoryManager != NULL);

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        oMemoryManager->beginSetValue(iSourceId);
        const gcd_address_vqt_realText15_t * oYc = (const gcd_address_vqt_realText15_t *)pData;
        int iValueLength;
        for (int i = 0; i < iParamCount; ++i) {
            iValueLength = strlen(oYc->value);
            if (iValueLength>0 && iValueLength<16)
            {
                oMemoryManager->setValue(oYc->address, CxString::toDouble(oYc->value), oYc->quality, oYc->datetime);
            }
            oYc++;
        }
        oMemoryManager->endSetValue();
    }
        break;

        //遥测处理 vqt浮点方式
    case ci_yc_address_double_vqt_t:
    {
        if (iDataLength < sizeof(gcd_address_double_vqt_t)*iParamCount)
            return;

        YcaddMemoryManager * oMemoryManager = YcaddShareMemory::memoryManager();
        assert(oMemoryManager != NULL);

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        oMemoryManager->beginSetValue(iSourceId);
        const gcd_address_double_vqt_t * oYc = (const gcd_address_double_vqt_t *)pData;
        for (int i = 0; i < iParamCount; ++i) {
            oMemoryManager->setValue(oYc->address, oYc->value, oYc->quality, oYc->datetime);
            oYc++;
        }
        oMemoryManager->endSetValue();
    }
        break;

        //*yw
        //遥文处理 vqt文本方式
    case ci_yw_address_vqt_string128_t:
    {
        if (iDataLength < sizeof(gcd_address_vqt_string128_t)*iParamCount)
            return;

        StrawMemoryManager * oMemoryManager = StrawShareMemory::memoryManager();
        assert(oMemoryManager != NULL);

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        oMemoryManager->beginSetValue(iSourceId);
        const gcd_address_vqt_string128_t * oYw = (const gcd_address_vqt_string128_t *)pData;
//        int iValueLength;
        StrawValue sv;
        for (int i = 0; i < iParamCount; ++i) {
            sv = oYw->value;
            oMemoryManager->setValue(oYw->address, sv, oYw->quality, oYw->datetime);
//            iValueLength = strlen(oYw->value);
//            if (iValueLength>0 && iValueLength<128)
//            {
//                sv = oYw->value;
//                oMemoryManager->setValue(oYw->address, sv, oYw->quality, oYw->datetime);
//            }
            oYw++;
        }
        oMemoryManager->endSetValue();
    }
        break;

    default:
        break;
    }
}
