#include "measure_manager_psm.h"

#include <psm/general_access.h>

#include <common/gcl.h>

using namespace std;


static bool f_isLogMeasures = false;


static int f_iMeasureManagerPsmTimes = 0;
static int f_iSendYxInterval = 0;
static int f_iSendYcInterval = 0;
static int f_iSendYwInterval = 0;


static FILE * f_fileLogMonsbs = NULL;
static FILE * f_fileLogYcadds = NULL;
static FILE * f_fileLogStraws = NULL;


MeasureManagerPsm * fn_oMeasureManagerPsm()
{
    static MeasureManagerPsm m;
    //注册 Application 的 Start 与 Stop 回调
    CxApplication::registStartFunction(MeasureManagerPsm::start);
    CxApplication::registStopFunction(MeasureManagerPsm::stop);
    //订阅控制台输入命令
    CxInterinfoIn::addObserver(MeasureManagerPsm::interinfoIn_line);
    CxInterinfoIn::addObserver(MeasureManagerPsm::interinfoIn_cmd);
    //订阅量总表
    MonsbManager::addObserver(&m);
    YcaddManager::addObserver(&m);
    StrawManager::addObserver(&m);
    //订阅Ga
    GeneralAccess::registerRealtimeDataPostDeal(MeasureManagerPsm::dealRrealtimeDataPost);
    return &m;
}

MeasureManagerPsm * f_oMeasureManagerPsm = fn_oMeasureManagerPsm();


int fn_sendAllMonsb()
{
    int iResult = 0;

    vector<gcd_address_int32_vqt_t> yxs;

    const std::vector<MonsbMeasure*> * oMeasures = MonsbManager::measures();
    for (size_t i = 0; i < oMeasures->size(); ++i)
    {
        const MonsbMeasure * oMonsb = oMeasures->at(i);
        if (oMonsb->value() != CxValueType::originalValue<int>())
        {
            gcd_address_int32_vqt_t yx;
            yx.address = oMonsb->measureId();
            yx.value = oMonsb->value();
            yx.quality = oMonsb->quality();
            yx.datetime = oMonsb->lastChangeTime();
            yxs.push_back(yx);
            if (yxs.size()>=ci_psm_packet_default_count(gcd_address_int32_vqt_t))
            {
                iResult += GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (char*)(&yxs.front()), yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), GeneralAccess::defaultAttach());
                yxs.clear();
            }
        }
    }

    if (yxs.size()>0)
    {
        iResult += GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (char*)(&yxs.front()), yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), GeneralAccess::defaultAttach());
    }

//    cxDebug() << CxString::format("send yx by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));

    return iResult;
}

int fn_sendAllYcadd()
{
    int iResult = 0;

    vector<gcd_address_double_vqt_t> ycs;

    const std::vector<YcaddMeasure*> * oMeasures = YcaddManager::measures();
    for (size_t i = 0; i < oMeasures->size(); ++i)
    {
        const YcaddMeasure * oYcadd = oMeasures->at(i);
        if (oYcadd->value() != CxValueType::originalValue<double>())
        {
            gcd_address_double_vqt_t yc;
            yc.address = oYcadd->measureId();
            yc.value = oYcadd->value();
            yc.quality = oYcadd->quality();
            yc.datetime = oYcadd->lastChangeTime();
            ycs.push_back(yc);
            if (ycs.size()>=ci_psm_packet_default_count(gcd_address_double_vqt_t))
            {
                iResult += GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (char*)(&ycs.front()), ycs.size()*sizeof(gcd_address_double_vqt_t), ycs.size(), GeneralAccess::defaultAttach());
                ycs.clear();
            }
        }
    }

    if (ycs.size()>0)
    {
        iResult += GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (char*)(&ycs.front()), ycs.size()*sizeof(gcd_address_double_vqt_t), ycs.size(), GeneralAccess::defaultAttach());
    }

//    cxDebug() << CxString::format("send yc by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, ycs.size(), ycs.size()*sizeof(gcd_address_double_vqt_t));

    return iResult;
}

int fn_sendAllStraw()
{
    int iResult = 0;

    vector<gcd_address_vqt_string128_t> yws;

    const std::vector<StrawMeasure*> * oMeasures = StrawManager::measures();
    for (size_t i = 0; i < oMeasures->size(); ++i)
    {
        const StrawMeasure * oStraw = oMeasures->at(i);
        if (oStraw->value().size()>0)
        {
            gcd_address_vqt_string128_t yw;
            yw.address = oStraw->measureId();
            string sValue = oStraw->value();
            if (sValue.size()<128)
            {
                memcpy(yw.value, sValue.data(), sValue.size());
                yw.value[sValue.size()] = 0;
            }
            yw.quality = oStraw->quality();
            yw.datetime = oStraw->lastChangeTime();
            yws.push_back(yw);
            if (yws.size()>=ci_psm_packet_default_count(gcd_address_vqt_string128_t))
            {
                iResult += GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (char*)(&yws.front()), yws.size()*sizeof(gcd_address_double_vqt_t), yws.size(), GeneralAccess::defaultAttach());
                yws.clear();
            }
        }
    }

    if (yws.size()>0)
    {
        iResult += GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (char*)(&yws.front()), yws.size()*sizeof(gcd_address_double_vqt_t), yws.size(), GeneralAccess::defaultAttach());
    }

//    cxDebug() << CxString::format("send yw by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yws.size(), yws.size()*sizeof(gcd_address_double_vqt_t));

    return iResult;
}




void MeasureManagerPsm::start()
{
    f_iSendYxInterval = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmYxSendInterval, f_iSendYxInterval, true);
    if (f_iSendYxInterval<3000) f_iSendYxInterval = 0; else f_iSendYxInterval = f_iSendYxInterval / 1000;
    f_iSendYcInterval = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmYcSendInterval, f_iSendYcInterval, true);
    if (f_iSendYcInterval<3000) f_iSendYcInterval = 0; else f_iSendYcInterval = f_iSendYcInterval / 1000;
    f_iSendYwInterval = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmYwSendInterval, f_iSendYwInterval, true);
    if (f_iSendYwInterval<3000) f_iSendYwInterval = 0; else f_iSendYwInterval = f_iSendYwInterval / 1000;

    CxTimerManager::startTimer(MeasureManagerPsm::timerTimeout, 1000);
}

void MeasureManagerPsm::stop()
{
}

/**

measure report : 量报告

 */
int MeasureManagerPsm::interinfoIn_line(const std::string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

//#include <vdi/monsb_address_worker.h>
//#include <vdi/ycadd_address_worker.h>
int MeasureManagerPsm::interinfoIn_cmd(const std::string &sCommand, const std::map<std::string, std::string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "measure")
    {
        if (CxContainer::contain(sParams, std::string("report")))
        {

            //            TerminalBase * oTerminal = f_mTerminals.at(2);
            //            MonsbAddressWorkerTable * oTable1 = oTerminal->protocol()->workerTableSon<MonsbAddressWorkerTable>(string("YxTable"));
            //            for (int i = 0; i < MonsbManager::measures()->size(); ++i)
            //            {
            //                MonsbMeasure * oMonsb = MonsbManager::measures()->at(i);
            //                MonsbAddressWorker * oWorker = oTable1->workerByMeasure(oMonsb);
            //                if (oWorker)
            //                    oMonsb->setMeasureName(oWorker->workerName());
            //            }
            //            MonsbManager::saveToFile("c:\monsbs.txt");

            //            YcaddAddressWorkerTable * oTable = oTerminal->protocol()->workerTableSon<YcaddAddressWorkerTable>(string("YcTable"));
            //            for (int i = 0; i < YcaddManager::measures()->size(); ++i)
            //            {
            //                YcaddMeasure * oYcadd = YcaddManager::measures()->at(i);
            //                YcaddAddressWorker * oWorker = oTable->workerByMeasure(oYcadd);
            //                if (oWorker)
            //                    oYcadd->setMeasureName(oWorker->workerName());
            //            }
            //            YcaddManager::saveToFile("c:\ycadds.txt");

            vector<string> sReports = MonsbManager::reportSelf();
            string sFilePath = CxFileSystem::mergeFilePath(GclConfig::reportPath(), CxApplication::applicationTargetName() + "." + CS_FileNameMonsbMeasure);

            if (CxFile::save(sFilePath, sReports))
            {
                cxPrompt() << "report save to file : " << sFilePath;
                if (f_fileLogMonsbs)
                {
                    fclose(f_fileLogMonsbs);
                }
                f_fileLogMonsbs = fopen (sFilePath.data(), "ab+");
            }

            sReports = YcaddManager::reportSelf();
            sFilePath = CxFileSystem::mergeFilePath(GclConfig::reportPath(), CxApplication::applicationTargetName() + "." + CS_FileNameYcaddMeasure);

            if (CxFile::save(sFilePath, sReports))
            {
                cxPrompt() << "report save to file : " << sFilePath;
                if (f_fileLogYcadds)
                {
                    fclose(f_fileLogYcadds);
                }
                f_fileLogYcadds = fopen (sFilePath.data(), "ab+");
            }

            sReports = StrawManager::reportSelf();
            sFilePath = CxFileSystem::mergeFilePath(GclConfig::reportPath(), CxApplication::applicationTargetName() + "." + CS_FileNameStrawMeasure);

            if (CxFile::save(sFilePath, sReports))
            {
                cxPrompt() << "report save to file : " << sFilePath;
                if (f_fileLogStraws)
                {
                    fclose(f_fileLogStraws);
                }
                f_fileLogStraws = fopen (sFilePath.data(), "ab+");
            }

            f_isLogMeasures = CxApplication::findConfig( CS_SectionProgramConfig , "LogMeasureChanged", f_isLogMeasures, true);
        }
        return CxInterinfo::InDeal_Ed;
    }

    return 0;
}

void MeasureManagerPsm::timerTimeout(int iInterval)
{
    ++ f_iMeasureManagerPsmTimes;

    if (GeneralAccess::isConnected())
    {
        if (f_iSendYxInterval > 0 && f_iMeasureManagerPsmTimes % f_iSendYxInterval == 0)
        {
            fn_sendAllMonsb();
        }
        if (f_iSendYcInterval > 0 && f_iMeasureManagerPsmTimes % f_iSendYcInterval == 0)
        {
            fn_sendAllYcadd();
        }
        if (f_iSendYwInterval > 0 && f_iMeasureManagerPsmTimes % f_iSendYwInterval == 0)
        {
            fn_sendAllStraw();
        }
    }
}

int MeasureManagerPsm::dealRrealtimeDataPost(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    switch (iParamType) {

    //*yx
    //遥信处理 vqt文本方式
    case ci_yx_address_vqt_intText1_t:
    {
        if (iDataLength < sizeof(gcd_address_vqt_intText1_t)*iParamCount)
            return TRUE;

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        MonsbMeasure::beginSetValues(0, iSourceId, ci_tag_GeneralAccess);
        const gcd_address_vqt_intText1_t * oYx = (const gcd_address_vqt_intText1_t *)pData;
        int iValueLength;
        MonsbMeasure * oMonsb = NULL;
        for (int i = 0; i < iParamCount; ++i)
        {
            iValueLength = strlen(oYx->value);
            if (iValueLength>0 && iValueLength<8)
            {
                oMonsb = MonsbManager::measure(oYx->address);
                if(oMonsb!=NULL)
                {
                    oMonsb->setValue(CxString::toInt32(oYx->value));
                    oMonsb->setQuality(oYx->quality);
                }
            }
            oYx++;
        }
        MonsbMeasure::endSetValues();
        return TRUE;
    }
        break;

        //遥信处理 vqt整形方式
    case ci_yx_address_int32_vqt_t:
    {
        if (iDataLength < sizeof(gcd_address_int32_vqt_t)*iParamCount)
            return TRUE;

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        MonsbMeasure::beginSetValues(0, iSourceId, ci_tag_GeneralAccess);
        const gcd_address_int32_vqt_t * oYx = (const gcd_address_int32_vqt_t *)pData;
        MonsbMeasure * oMonsb = NULL;
        for (int i = 0; i < iParamCount; ++i)
        {
            oMonsb = MonsbManager::measure(oYx->address);
            if(oMonsb!=NULL)
            {
                oMonsb->setValue(oYx->value);
                oMonsb->setQuality(oYx->quality);
            }
            oYx++;
        }
        MonsbMeasure::endSetValues();
        return TRUE;
    }
        break;

        //*yc
        //遥测处理 vqt文本方式
    case ci_yc_address_vqt_intText1_t:
    {
        if (iDataLength < sizeof(gcd_address_vqt_realText15_t)*iParamCount)
            return TRUE;

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        YcaddMeasure::beginSetValues(0, iSourceId, ci_tag_GeneralAccess);
        const gcd_address_vqt_realText15_t * oYc = (const gcd_address_vqt_realText15_t *)pData;
        int iValueLength;
        YcaddMeasure * oYcadd = NULL;
        for (int i = 0; i < iParamCount; ++i)
        {
            iValueLength = strlen(oYc->value);
            if (iValueLength>0 && iValueLength<16)
            {
                oYcadd = YcaddManager::measure(oYc->address);
                if(oYcadd!=NULL)
                {
                    oYcadd->setValue(CxString::toInt32(oYc->value));
                    oYcadd->setQuality(oYc->quality);
                }
            }
            oYc++;
        }
        YcaddMeasure::endSetValues();
        return TRUE;
    }
        break;

        //遥测处理 vqt浮点方式
    case ci_yc_address_double_vqt_t:
    {
        if (iDataLength < sizeof(gcd_address_double_vqt_t)*iParamCount)
            return TRUE;

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        YcaddMeasure::beginSetValues(0, iSourceId, ci_tag_GeneralAccess);
        const gcd_address_double_vqt_t * oYc = (const gcd_address_double_vqt_t *)pData;
        YcaddMeasure * oYcadd = NULL;
        for (int i = 0; i < iParamCount; ++i)
        {
            oYcadd = YcaddManager::measure(oYc->address);
            if(oYcadd!=NULL)
            {
                oYcadd->setValue(oYc->value);
                oYcadd->setQuality(oYc->quality);
            }
            oYc++;
        }
        YcaddMeasure::endSetValues();
        return TRUE;
    }
        break;

        //*yw
        //遥文处理 vqt文本方式
    case ci_yw_address_vqt_string128_t:
    {
        if (iDataLength < sizeof(gcd_address_vqt_string128_t)*iParamCount)
            return TRUE;

        int iSourceId = oAttach ? oAttach->sourceId : 0;
        StrawMeasure::beginSetValues(0, iSourceId, ci_tag_GeneralAccess);
        const gcd_address_vqt_string128_t * oYw = (const gcd_address_vqt_string128_t *)pData;
//        int iValueLength;
        StrawMeasure * oStraw = NULL;
        for (int i = 0; i < iParamCount; ++i) {
            oStraw = StrawManager::measure(oYw->address);
            if(oStraw!=NULL)
            {
                oStraw->setValue(oYw->value);
                oStraw->setQuality(oYw->quality);
            }
//            iValueLength = strlen(oYw->value);
//            if (iValueLength>0 && iValueLength<128)
//            {
//                oStraw = StrawManager::measure(oYw->address);
//                if(oStraw!=NULL)
//                {
//                    oStraw->setValue(oYw->value);
//                    oStraw->setQuality(oYw->quality);
//                }
//            }
            oYw++;
        }
        StrawMeasure::endSetValues();
        return TRUE;
    }
        break;

    default:
        break;
    }
    return FALSE;
}


void MeasureManagerPsm::measures_valueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, int iMeasureSonType, int iSourceId, int iTag)
{
    if(iTag == ci_tag_ShareMemory)
        return;

#if defined(GM_DEBUG)
    if (f_fileLogMonsbs != NULL && f_isLogMeasures)
    {
        string sDtNow = CxTime::currentMsepochString();
        std::string sText = CxString::format("\n\n%s changed count=%d sourceId=%d:", sDtNow.c_str(), monsbOldValues.size(), iSourceId);
        size_t iWrote = fwrite (const_cast<char *>(sText.data()) , 1, sText.size(), f_fileLogMonsbs);

        for (std::map<const MonsbMeasure *, int>::const_iterator it = monsbOldValues.begin(); it != monsbOldValues.end(); ++it)
        {
            const MonsbMeasure * oMonsb = it->first;
            sText = CxString::format("\nValue=%d OldValue=%d MeasureId=%d MeasureName=%s", oMonsb->value(), it->second, oMonsb->measureId(), oMonsb->measureName().c_str());
            iWrote += fwrite (const_cast<char *>(sText.data()) , 1, sText.size(), f_fileLogMonsbs);
        }

        fflush (f_fileLogMonsbs);
    }
#endif

    //通过 Psm 发出去
    if (GeneralAccess::isConnected())
    {
        vector<gcd_address_int32_vqt_t> yxs;

        for (std::map<const MonsbMeasure *, int>::const_iterator it = monsbOldValues.begin(); it != monsbOldValues.end(); ++it)
        {
            const MonsbMeasure * oMonsb = it->first;
            gcd_address_int32_vqt_t yx;
            yx.address = oMonsb->measureId();
            yx.value = oMonsb->value();
            yx.quality = oMonsb->quality();
            yx.datetime = oMonsb->lastChangeTime();
            yxs.push_back(yx);
        }

        if (yxs.size()>0)
        {
            int iResult = GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (char*)(&yxs.front())/* yxs.data()*/, yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), GeneralAccess::defaultAttach());
//            cxDebug() << CxString::format("send yx by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
        }
    }
}

void MeasureManagerPsm::measures_valueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, int iMeasureSonType, int iSourceId, int iTag)
{
    if(iTag == ci_tag_ShareMemory)
        return;

#if defined(GM_DEBUG)
    if (f_fileLogYcadds != NULL && f_isLogMeasures)
    {
        string sDtNow = CxTime::currentMsepochString();
        std::string sText = CxString::format("\n\n%s changed count=%d sourceId=%d:", sDtNow.c_str(), ycaddOldValues.size(), iSourceId);
        size_t iWrote = fwrite (const_cast<char *>(sText.data()) , 1, sText.size(), f_fileLogYcadds);

        for (std::map<const YcaddMeasure *, double>::const_iterator it = ycaddOldValues.begin(); it != ycaddOldValues.end(); ++it)
        {
            const YcaddMeasure * oYcadd = it->first;
            sText = CxString::format("\nValue=%0.2f OldValue=%f MeasureId=%d MeasureName=%s", oYcadd->value(), it->second, oYcadd->measureId(), oYcadd->measureName().c_str());
            iWrote += fwrite (const_cast<char *>(sText.data()) , 1, sText.size(), f_fileLogYcadds);
        }

        fflush (f_fileLogYcadds);
    }
#endif

    //通过 Psm 发出去
    if (GeneralAccess::isConnected())
    {
        vector<gcd_address_double_vqt_t> ycs;

        for (std::map<const YcaddMeasure *, double>::const_iterator it = ycaddOldValues.begin(); it != ycaddOldValues.end(); ++it)
        {
            const YcaddMeasure * oYcadd = it->first;
            gcd_address_double_vqt_t yc;
            yc.address = oYcadd->measureId();
            yc.value = oYcadd->value();
            yc.quality = oYcadd->quality();
            yc.datetime = oYcadd->lastChangeTime();
            ycs.push_back(yc);
        }

        if (ycs.size()>0)
        {
            int iResult = GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (char*)(&ycs.front())/*ycs.data()*/, ycs.size()*sizeof(gcd_address_double_vqt_t), ycs.size(), GeneralAccess::defaultAttach());
//            cxDebug() << CxString::format("send yc by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, ycs.size(), ycs.size()*sizeof(gcd_address_double_vqt_t));
        }
    }
}

void MeasureManagerPsm::measures_valueChanged(const std::map<const StrawMeasure *, string> &strawOldValues, int iMeasureSonType, int iSourceId, int iTag)
{
    if(iTag == ci_tag_ShareMemory)
        return;

#if defined(GM_DEBUG)
    if (f_fileLogStraws != NULL && f_isLogMeasures)
    {
        string sDtNow = CxTime::currentMsepochString();
        std::string sText = CxString::format("\n\n%s changed count=%d sourceId=%d:", sDtNow.c_str(), strawOldValues.size(), iSourceId);
        size_t iWrote = fwrite (const_cast<char *>(sText.data()) , 1, sText.size(), f_fileLogStraws);

        for (std::map<const StrawMeasure *, string>::const_iterator it = strawOldValues.begin(); it != strawOldValues.end(); ++it)
        {
            const StrawMeasure * oStraw = it->first;
            sText = CxString::format("\nValue=%s OldValue=%s MeasureId=%d MeasureName=%s", oStraw->value().c_str(), it->second.c_str(), oStraw->measureId(), oStraw->measureName().c_str());
            iWrote += fwrite (const_cast<char *>(sText.data()) , 1, sText.size(), f_fileLogStraws);
        }

        fflush (f_fileLogStraws);
    }
#endif

    //通过 Psm 发出去
    if (GeneralAccess::isConnected())
    {
        vector<gcd_address_vqt_string128_t> yws;

        for (std::map<const StrawMeasure *, string>::const_iterator it = strawOldValues.begin(); it != strawOldValues.end(); ++it)
        {
            const StrawMeasure * oStraw = it->first;
            gcd_address_vqt_string128_t yw;
            yw.address = oStraw->measureId();
            string sValue = oStraw->value();
            if (sValue.size()<128)
            {
                memcpy(yw.value, sValue.data(), sValue.size());
                yw.value[sValue.size()] = 0;
            }
            yw.quality = oStraw->quality();
            yw.datetime = oStraw->lastChangeTime();
            yws.push_back(yw);
        }

        if (yws.size()>0)
        {
            int iResult = GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (char*)(&yws.front())/*yws.data()*/, yws.size()*sizeof(gcd_address_vqt_string128_t), yws.size(), GeneralAccess::defaultAttach());
//            cxDebug() << CxString::format("send yw by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yws.size(), yws.size()*sizeof(gcd_address_vqt_string128_t));
        }
    }
}

