#ifndef MEASURE_SHAREMEMORY_H
#define MEASURE_SHAREMEMORY_H


#include <ccxx/ccxx.h>
#include <common/measure_strings.h>


/**共享内存只有一个客户端有写的权限，
 * 写的权限方法调用的是 create
 * 读的权限方法调用的是 view
 */

/**共享内存的大小 size
 * 如果想改变 measureCount、changedCount、controlCount : 就在方法(函数） sizeOfMemory 、 create 中改
 *
 */

#define ci_measure_sharememory_version (2)

#define ci_measure_changed_maxid (0x7FFFFFFE)
#define ci_measure_control_maxid (0x7FFFFFFE)

#define ci_measure_control_count (1024)

//共享内存头
struct MeasureShareMemoryHead
{
    size_t size; // 全长 : 32 + 1024*4 + ....
    unsigned int head1; // EB + 90 + EB + 90
    size_t sizeRepeat; //重复下载 size
    unsigned int head2; // A5 + 5A + A5 + 5A
    int version; // 版本
    int type; // 结构体类型
    int count; // 结构体个数
    int index; // 所在父内存中的内存索引
    int status; // 保留
    int res1; // 保留
    int res2; // 保留
    int res3; // 保留

    MeasureShareMemoryHead() :
        size(0), head1(ci_sharememory_head1), sizeRepeat(0), head2(ci_sharememory_head2), version(ci_measure_sharememory_version),
        type(0), count(0), index(0), status(ci_sharememory_status_invalid),
        res1(0), res2(0), res3(0)
    { }
};

//共享内存描述部
//预留 4K


//目前用单倍缓存区来写
//内存的数据格式化查看函数 sizeOfMemory
template<typename T>
class MeasureShareMemory
{
public:

    /**
     * @brief The MeasureData struct
     * 记录量值方面的具体数据
     */
    struct MeasureData
    {
        int measureId;
        T value; // 量值
        int quality; // 值的品质
        msepoch_t refreshTime; // 8
        msepoch_t changedTime; // 8
        int refreshSourceId;
        int changedSourceId; // 值的修改时间
        int refreshReasonId;
        int changedReasonId; // 修改值的原因
        int equalStrategyId;
        int res;
    };

    /**
     * @brief The WroteIndex struct
     * 写量值的配置
     */
    struct WroteIndex
    {
        size_t index; // 写值所在Measure区的索引
        size_t indexBegin; // indexEnd - indexBegin == 缓冲区结构体个数
        size_t indexEnd;
    };

    /**
     * @brief The ChangedData struct
     * 记录量值变化时的数据
     */
    struct ChangedData
    {
        int changedId; // 变化ID : 1,2,3...0x7fffffff
        int measureId; // 量ID
        T value; // 量值
        int quality; // 值的品质
        msepoch_t changedTime; // 值的修改时间
        int changedSourceId; // 修改值的源ID
        int changedReasonId; // 修改值的原因
        T oldValue; // 修改前的值
    };

    /**
     * @brief The ControlData struct
     * 控制的数据 {遥控、遥调、遥设} {yk,yt,ys}
     */
    struct ControlData
    {
        int controlId; // 控制ID : 1,2,3...0x7fffffff
        int measureId; // 量ID
        T value; // 控制值
        msepoch_t controlTime; // 值的控制时间
        int controlSourceId; // 控制的源ID
        int controlReasonId; // 控制的原因
        int controlStrategyId; // 控制的策略
        int controlMethodId; // 控制的方法
        char params[512];
        int transferCount; // 从BUS转发数量
        int res; // 保留
    };

    /**
     * @brief The ConfigData struct
     * 配置的数据 {遥控、遥调、遥设} {yk,yt,ys}
     */
    struct ConfigData
    {
        int measureId;
        int equalStrategyId;
        int res[14];
        T defaultValue; // 默认值
        T value1; // 值1
        T value2; // 值2
    };

    /**
     * @brief The ShareMemory class
     * sample to create ShareMemory : load("gcl_monsbs", 10*1000);
     * sample to view ShareMemory : load("gcl_monsbs");
     */
    class MemoryManagerBase
    {
    private:
        CxShareMemory _shareMemory;

        size_t _measureCount;
        size_t _changedCount;
        size_t _controlCount;
        int _minId;
        int _maxId;

        //* memory memory memory
        MeasureShareMemoryHead * _head;

        char * _res;

        //量值所在的位移值1：用来读量值用
        MeasureShareMemoryHead * _measureIndexRHead;
        size_t * _measureIndexRBegin;
        size_t * _measureIndexREnd;

        //量值所在的位移值2：用来写量值用
        MeasureShareMemoryHead * _measureIndexWHead;
        WroteIndex * _measureIndexWBegin;
        WroteIndex * _measureIndexWEnd;

        //量值所在内存区域1
        MeasureShareMemoryHead * _measuresHead;
        MeasureData * _measuresBegin;
        MeasureData * _measuresEnd;

        MeasureShareMemoryHead * _changedDataHead;
        ChangedData * _changedDataBegin;
        ChangedData * _changedDataEnd;

        MeasureShareMemoryHead * _controlDataHead;
        ControlData * _controlDataBegin;
        ControlData * _controlDataEnd;

        MeasureShareMemoryHead * _configDataHead;
        ConfigData * _configDataBegin;
        ConfigData * _configDataEnd;

        MeasureShareMemoryHead * _end;

        bool _isReadOnly;
        bool _canWrite;

        int _currentSourceId;
        size_t _currentChangedIndex;
        size_t _currentChangedId;

        size_t _currentControlIndex;
        size_t _currentControlId;

    public:
        MemoryManagerBase() {
            doResetAllVariable();
        }

        ~MemoryManagerBase() {
            shutdown();
        }

        void doResetAllVariable () {
            _measureCount = 0;
            _changedCount = 0;
            _controlCount = 0;
            _minId = 0; _maxId = 0;
            _head = NULL;
            _res = NULL;
            _measureIndexRHead = NULL; _measureIndexRBegin = NULL; _measureIndexREnd = NULL;
            _measureIndexWHead = NULL; _measureIndexWBegin = NULL; _measureIndexWEnd = NULL;
            _measuresHead = NULL; _measuresBegin = NULL; _measuresEnd = NULL;
            _changedDataHead = NULL; _changedDataBegin = NULL; _changedDataEnd = NULL;
            _controlDataHead = NULL; _controlDataBegin = NULL; _controlDataEnd = NULL;
            _configDataHead = NULL; _configDataBegin = NULL; _configDataEnd = NULL;
            _end = NULL;
            _isReadOnly = true; _canWrite = false;
            _currentSourceId = 0;
            _currentChangedIndex = 0; _currentChangedId = 1;
            _currentControlIndex = 0; _currentControlId = 1;
        }

        /**
         * @brief load
         * @param sMapName
         * @param iMeasureCount 加载配置量的个数，如果为0：表示查看方式打开已有的共享内存
         */
        inline void load(const std::string & sMapName, int iMeasureCount = 0)
        {
            std::string sShareName = sMapName;
//            std::string sShareName = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(),sMapName);
            if (iMeasureCount>0)
            {
                create(sShareName, iMeasureCount);
            }
            else
            {
                view(sShareName);
            }
        }

        /**
         * @brief create
         * @param sMapName
         * @param iMeasureCount
         */
        inline void create(const std::string & sMapName, int iMeasureCount)
        {
            //已经加载过就不用加载
            GM_VALID_RETURE(_shareMemory.isValid());
            GM_INVALID_RETURE(sMapName.size()>0);
            GM_INVALID_RETURE(iMeasureCount>=100);
            if (_shareMemory.openModify(sMapName.c_str(), sizeOfMemory(iMeasureCount)))
            {
                _measureCount = iMeasureCount;
                _changedCount = iMeasureCount * 2;
                _controlCount = iMeasureCount / 5;
                doAllocatePointers();
                doAllocateValues();
                GM_ASSERT_IFNOT(_head->version==ci_measure_sharememory_version, "Share Memory Version Error, Please Restart All Share Memory's Programer!!!");
                _head->status = ci_sharememory_status_good;
                _isReadOnly = false;
                _canWrite = true;
                _minId = g_fn_getMinMeasureId(getMeasureTypeImpl());
                _maxId = _minId + _measureCount;
//                char * sMessage =
//                    "{"
//                    "\r\n  "MeasureShareMemory": {"
//                    "\r\n    "create": {"
//                    "\r\n      "result": true,"
//                    "\r\n      "_measureCount": %d,"
//                    "\r\n      "_changedCount": %d,"
//                    "\r\n      "_controlCount": %d"
//                    "\r\n    }"
//                    "\r\n  }
//                    "\r\n}
//                ;
                cxPrompt() << CxString::format("{MeasureShareMemory.create:finish,properties:{measureCount:%d}",
                                               _measureCount, _changedCount, _controlCount);
            }
        }

        /**
         * @brief view
         * @param sMapName
         */
        inline void view(const std::string & sMapName)
        {
            //已经加载过就不用加载
            GM_VALID_RETURE(_shareMemory.isValid());
            GM_INVALID_RETURE(sMapName.size()>0);
            if (_shareMemory.openView(sMapName.c_str()))
            {
                doAllocatePointers();
                doResoreOld();
                _isReadOnly = true;
                _canWrite = false;
                _minId = g_fn_getMinMeasureId(getMeasureTypeImpl());
                _maxId = _minId + _measureCount;
            }
        }

        inline void doAllocatePointers()
        {
            GM_INVALID_RETURE(_shareMemory.addr());

            char * pIndex = _shareMemory.addr();

            _head = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            if (_measureCount<=0)
            {
                _measureCount = _head->count;
            }
            _res = pIndex; pIndex += 1024 * 4;
            _measureIndexRHead = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            _measureIndexRBegin = (size_t *)pIndex; pIndex += sizeof(size_t)*_measureCount;
            _measureIndexREnd = (size_t *)pIndex;

            _measureIndexWHead = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            _measureIndexWBegin = (WroteIndex *)pIndex; pIndex += sizeof(WroteIndex)*_measureCount;
            _measureIndexWEnd = (WroteIndex *)pIndex;

            _measuresHead = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            _measuresBegin = (MeasureData *)pIndex; pIndex += sizeof(MeasureData)*_measureCount*2;
            _measuresEnd = (MeasureData *)pIndex;

            _changedDataHead = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            if (_changedCount<=0)
            {
                _changedCount = _changedDataHead->count;
            }
            _changedDataBegin = (ChangedData *)pIndex; pIndex += sizeof(ChangedData)*_changedCount;
            _changedDataEnd = (ChangedData *)pIndex;

            _controlDataHead = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            if (_controlCount<=0)
            {
                _controlCount = _controlDataHead->count;
            }
            _controlDataBegin = (ControlData *)pIndex; pIndex += sizeof(ControlData)*_controlCount;
            _controlDataEnd = (ControlData *)pIndex;

            _configDataHead = (MeasureShareMemoryHead *)pIndex; pIndex += sizeof(MeasureShareMemoryHead);
            _configDataBegin = (ConfigData *)pIndex; pIndex += sizeof(ConfigData)*_measureCount;
            _configDataEnd = (ConfigData *)pIndex;

            _end = (MeasureShareMemoryHead *)pIndex;
        }

        inline void doAllocateValues()
        {
            GM_INVALID_RETURE(_shareMemory.isValid());
            GM_INVALID_RETURE(_head);
            GM_INVALID_RETURE(_measureCount>0);
            GM_INVALID_RETURE(_changedCount>0);
            GM_INVALID_RETURE(_controlCount>0);

            //判断是否已经分配过值
            bool bHasOld = _head->head1 == ci_sharememory_head1 && _head->head2 == ci_sharememory_head2
                    && _end->head1 == ci_sharememory_head1 && _end->head2 == ci_sharememory_head2;

            //已经分配过值
            if (bHasOld)
            {
                doResoreOld();
                return;
            }

            //设置每段的头部信息
            _head->size = _shareMemory.len();
            _head->head1 = ci_sharememory_head1;
            _head->sizeRepeat = _shareMemory.len();
            _head->head2 = ci_sharememory_head2;
            _head->version = ci_measure_sharememory_version;
            _head->type = getMeasureTypeImpl();
            _head->count = _measureCount;
            _head->status = ci_sharememory_status_initing;

            //            _res;

            _measureIndexRHead->size = sizeof(MeasureShareMemoryHead) + sizeof(size_t)*_measureCount;
            _measureIndexRHead->head1 = ci_sharememory_head1;
            _measureIndexRHead->sizeRepeat = _measureIndexRHead->sizeRepeat;
            _measureIndexRHead->head2 = ci_sharememory_head2;
            _measureIndexRHead->count = _measureCount;

            _measureIndexWHead->size = sizeof(MeasureShareMemoryHead) + sizeof(WroteIndex)*_measureCount;
            _measureIndexWHead->head1 = ci_sharememory_head1;
            _measureIndexWHead->sizeRepeat = _measureIndexWHead->sizeRepeat;
            _measureIndexWHead->head2 = ci_sharememory_head2;
            _measureIndexWHead->count = _measureCount;

            _measuresHead->size = sizeof(MeasureShareMemoryHead) + sizeof(MeasureData)*_measureCount*2;
            _measuresHead->head1 = ci_sharememory_head1;
            _measuresHead->sizeRepeat = _measuresHead->size;
            _measuresHead->head2 = ci_sharememory_head2;
            _measuresHead->count = _measureCount*2;

            _changedDataHead->size = sizeof(MeasureShareMemoryHead) + sizeof(ChangedData)*_changedCount;
            _changedDataHead->head1 = ci_sharememory_head1;
            _changedDataHead->sizeRepeat = _changedDataHead->size;
            _changedDataHead->head2 = ci_sharememory_head2;
            _changedDataHead->count = _changedCount;

            _controlDataHead->size = sizeof(MeasureShareMemoryHead) + sizeof(ControlData)*_controlCount;
            _controlDataHead->head1 = ci_sharememory_head1;
            _controlDataHead->sizeRepeat = _controlDataHead->size;
            _controlDataHead->head2 = ci_sharememory_head2;
            _controlDataHead->count = _controlCount;

            _configDataHead->size = sizeof(MeasureShareMemoryHead) + sizeof(ConfigData)*_measureCount;
            _configDataHead->head1 = ci_sharememory_head1;
            _configDataHead->sizeRepeat = _configDataHead->size;
            _configDataHead->head2 = ci_sharememory_head2;
            _configDataHead->count = _measureCount;

            //初始化 控制区
            loadConfigImpl(_configDataBegin, _configDataEnd);
            ConfigData * oConfig = _configDataBegin;

            //初始化（以下可以改成从文件加载）写量值的配置
            //目前只用了双倍缓冲
            WroteIndex *  indexW      = _measureIndexWBegin;
            size_t iIndex = 0;
            while ((iIndex < _measureCount*2) && (indexW < _measureIndexWEnd))
            {
                indexW->indexBegin = iIndex; iIndex += 2;
                indexW->indexEnd = iIndex;
                indexW->index = indexW->indexBegin + 1;
                indexW ++;
            }

            //初始化 读写索引 读写量区 变化区 控制区
            size_t *      indexR      = _measureIndexRBegin;
            MeasureData *  oMeasure   = _measuresBegin;
            indexW      = _measureIndexWBegin;
            int iMinId = g_fn_getMinMeasureId(getMeasureTypeImpl());
            int iMaxId = g_fn_getMaxMeasureId(getMeasureTypeImpl());
            int iMeasureId = iMinId;
            //检查指针对象是否到End，measureId小于最大
            while ((indexR < _measureIndexREnd) && (indexW < _measureIndexWEnd) && (oMeasure < _measuresEnd) && (iMeasureId < iMaxId))
            {
                //初始化读索引
                * indexR = indexW->indexBegin;

                //初始化Measure的数据
                for (size_t i = indexW->indexBegin; i < indexW->indexEnd; ++i)
                {
                    if (oMeasure < _measuresEnd)
                    {
                        oMeasure->measureId           = iMeasureId;
                        oMeasure->value               = getOriginalValueImpl(iMeasureId);
                        oMeasure->quality             = 0;
                        oMeasure->refreshTime         = 0;
                        oMeasure->changedTime         = 0;
                        oMeasure->refreshSourceId     = 0;
                        oMeasure->refreshReasonId     = 0;
                        oMeasure->changedSourceId     = 0;
                        oMeasure->changedReasonId     = 0;
                        oMeasure->equalStrategyId     = oConfig->equalStrategyId;
                        oMeasure->res                 = 0;
                    }
                    oMeasure ++;
                }

                oConfig ++;
                indexR ++;
                indexW ++;
                iMeasureId ++;
            }

            //初始化 changed 的数据，是 measureCount * 2
            ChangedData * oChangedData= _changedDataBegin;
            while (oChangedData < _changedDataEnd)
            {
                oChangedData->changedId       = 0;
                oChangedData->measureId       = 0;
                oChangedData->value           = getOriginalValueImpl();
                oChangedData->quality         = 0;
                oChangedData->changedTime     = 0;
                oChangedData->changedSourceId = 0;
                oChangedData->changedReasonId = 0;
                oChangedData->oldValue        = getOriginalValueImpl();
                oChangedData ++;
            }

            //初始化 控制区
            ControlData * oControlData = _controlDataBegin;
            while (oControlData < _controlDataEnd)
            {
                oControlData->controlId       = 0;
                oControlData->measureId       = 0;
                oControlData->value           = getOriginalValueImpl();
                oControlData->controlTime     = 0;
                oControlData->controlSourceId = 0;
                oControlData->controlReasonId = 0;
                oControlData->controlStrategyId = 0;
                oControlData->controlMethodId = 0;
                memset(oControlData->params, 0, sizeof(oControlData->params));
//                oControlData->params = {0};
                oControlData->transferCount = 0;
                oControlData->res = 0;
                oControlData ++;
            }

            (* _end) = (* _head);
        }

        inline void doResoreOld()
        {
            ChangedData * oCurrentChangedNext = scanCurrentChangedNext();
            if (oCurrentChangedNext)
            {
                if (oCurrentChangedNext == _changedDataBegin)
                {
                    _currentChangedIndex = 0;
                    _currentChangedId = (_changedDataEnd-1)->changedId;
                }
                else
                {
                    _currentChangedIndex = oCurrentChangedNext - _changedDataBegin;
                    _currentChangedId = (oCurrentChangedNext-1)->changedId+1;
                }
            }
            ControlData * oCurrentControlNext = scanCurrentControlNext();
            if (oCurrentControlNext)
            {
                if (oCurrentControlNext == _controlDataBegin)
                {
                    _currentControlIndex = 0;
                    _currentControlId = (_controlDataEnd-1)->controlId;
                }
                else
                {
                    _currentControlIndex = oCurrentControlNext - _controlDataBegin;
                    _currentControlId = (oCurrentControlNext-1)->controlId+1;
                }
            }
        }

        inline void shutdown() {
            if (_head) {
                _shareMemory.close();
                doResetAllVariable();
            }
        }

        inline bool isValid() {
            return _head && _head->status == ci_sharememory_status_good && _measureCount > 0;
        }

        //共享内存状态
        inline int status() {
            if (_head)
                return _head->status;
            else
                return ci_sharememory_status_invalid;
        }

        inline void setStatus(int value) {
            if (_head && ! _isReadOnly)
            {
                _head->status = value;
                _canWrite = (value == ci_sharememory_status_good);
            }
        }

        inline bool canWrite() {
            return _canWrite;
        }

        inline bool isReadOnly() { return _isReadOnly; }

        //*量值区
        //供应给读的Index
        inline size_t * measuresIndexBegin() {
            return _measureIndexRBegin;
        }

        inline size_t * measuresIndexEnd() {
            return _measureIndexREnd;
        }

        inline MeasureData * measuresDataBegin() {
            return _measuresBegin;
        }

        inline MeasureData * measuresDataEnd() {
            return _measuresEnd;
        }

        inline size_t measureCount() {
            return _measureCount;
        }

        inline MeasureData * measureData(int iMeasureId) {
            GM_INVALID_RETURE_(_measuresBegin, NULL);
            GM_INVALID_RETURE_((iMeasureId >= _minId) && (iMeasureId < _maxId), NULL);
            size_t iIndex = * (_measureIndexRBegin + (iMeasureId-_minId));
            return _measuresBegin + iIndex;
        }

        //*量数据改变时保存的区域
        inline size_t changedCount() {
            return _changedCount;
        }

        //changedDataBegin
        inline ChangedData * changedDataBegin() {
            return _changedDataBegin;
        }

        inline ChangedData * changedDataEnd() {
            return _changedDataEnd;
        }

        inline ChangedData * changedDataByChangedId(size_t iChangedId) {
            ChangedData * oChangedData = _changedDataBegin;
            while (oChangedData < _changedDataEnd)
            {
                if (oChangedData->changedId == iChangedId)
                {
                    return oChangedData;
                }
                oChangedData++;
            }
            return NULL;
        }

        //*量的控制区域
        inline size_t controlCount() {
            return _controlCount;
        }

        //controlDataBegin
        inline ControlData * controlDataBegin() {
            return _controlDataBegin;
        }

        inline ControlData * controlDataEnd() {
            return _controlDataEnd;
        }

        inline ControlData * controlDataByControlId(size_t iControlId) {
            ControlData * oControlData = _controlDataBegin;
            while (oControlData < _controlDataEnd)
            {
                if (oControlData->controlId == iControlId)
                {
                    return oControlData;
                }
                oControlData++;
            }
            return NULL;
        }

        //配置数据区
        //ConfigData
        inline ConfigData * configDataBegin() {
            return _configDataBegin;
        }

        inline ConfigData * configDataEnd() {
            return _configDataEnd;
        }

        inline ConfigData * configData(int iMeasureId) {
            GM_INVALID_RETURE_(_configDataBegin, NULL);
            GM_INVALID_RETURE_((iMeasureId >= _minId) && (iMeasureId < _maxId), NULL);
            return _configDataBegin + (iMeasureId-_minId);
        }

        inline void beginSetValue(int sourceId)
        {
            _currentSourceId = sourceId;
        }

        //*设置量的值
        inline void setValue(int iMeasureId, const T & value, int quality, msepoch_t time)
        {
            GM_ASSER(_canWrite);
            GM_INVALID_RETURE((iMeasureId >= _minId) && (iMeasureId < _maxId));

            size_t * rIndex = _measureIndexRBegin + (iMeasureId-_minId);
            MeasureData * oMeasure = (_measuresBegin + (*rIndex));

            bool bEqual;
            if (oMeasure->equalStrategyId>0)
                bEqual = equalValueImpl(oMeasure->value, value, _configDataBegin + (iMeasureId-_minId));
            else
                bEqual = oMeasure->value == value;            

            //复制一份目前的 measure value （在读区）
            MeasureData measureValue = * oMeasure;

            //处理刷新时间
            measureValue.refreshTime = CxTime::currentMsepoch();

            bool bChanged = (! bEqual) || (oMeasure->quality != quality);
            if (bChanged)
            {
                //更新值和品质
                measureValue.value = value;
                measureValue.quality = quality;
                measureValue.changedTime = time;
                measureValue.changedSourceId = _currentSourceId;
            }

            //复制到写区
            WroteIndex * wIndex = _measureIndexWBegin + (iMeasureId-_minId);
            MeasureData * oMeasureData = _measuresBegin + wIndex->index;
            * oMeasureData = measureValue;

            //更新读与写的Index
            size_t wIndexValue = wIndex->index + 1;
            if (wIndexValue >= wIndex->indexEnd)
                wIndexValue = wIndex->indexBegin;
            * rIndex = wIndex->index; //更新读
            wIndex->index = wIndexValue; //更新写

            if (bChanged)
            {
                //获取当前可写的 changedData
                ChangedData * oChangedData = _changedDataBegin + _currentChangedIndex;
                oChangedData->oldValue = measureValue.value;

                //发布修改信息
                oChangedData->measureId = iMeasureId;
                oChangedData->value           = value;
                oChangedData->quality         = quality;
                oChangedData->changedTime     = time;
                oChangedData->changedSourceId = _currentSourceId;
                oChangedData->changedId       = _currentChangedId;

                //更新 _currentChangedId changedSourceId
                _currentChangedId ++;
                _currentChangedIndex ++;
                if (_currentChangedId > ci_measure_changed_maxid)
                    _currentChangedId = 1;
                if (_currentChangedIndex >= _changedCount)
                    _currentChangedIndex = 0;
            }
        }

        inline void endSetValue() {
        }

        //返回 变化的地的下一个
        //找 changedId 前面比后面大的，返回后面的；表示目前变化记录
        //返回NULL：表示没有变化
        //返回 > iPublishedIndex : 表示在 iPublishedIndex 后面，一段
        //返回 < iPublishedIndex : 表示在 iPublishedIndex 前面，分二段
        //返回 = iPublishedIndex : 刚好一轮，分二段
        //返回 begin : 表示 变化已经过了几个轮回
        inline ChangedData * getCurrentChangedEnd(size_t iPublishedIndex = 0) {

            GM_ASSER(iPublishedIndex < _changedCount);

            //没有变化
            if (! _changedDataBegin)
                return NULL;

            //没有变化
            if (_changedDataBegin->changedId == 0)
                return NULL;

            ChangedData * oChangedDataPublished = _changedDataBegin + iPublishedIndex; //已经发布到 oChangedDataPublished
            ChangedData * oChangedDataSearch = oChangedDataPublished + 1; //下一次要发布的
            if (oChangedDataSearch >= _changedDataEnd)
                oChangedDataSearch = _changedDataBegin;
            int iDiff = oChangedDataSearch->changedId - oChangedDataPublished->changedId;

            //没有变化
            if (iDiff <= 0)
            {
                return NULL;
            }
            //正常变化
            else if (iDiff == 1)
            {
                //判断从 iChangedIndex 开始从头判断
                ChangedData * oChangedDataEnd = _changedDataEnd - 1; //必须要 -1
                int iChangedId;
                while (oChangedDataSearch < oChangedDataEnd)
                {
                    iChangedId = oChangedDataSearch->changedId;
                    oChangedDataSearch ++;
                    if (iChangedId > oChangedDataSearch->changedId)
                    {
                        return oChangedDataSearch;
                    }
                }
                //判断 最后一个，但不是End
                iChangedId = oChangedDataSearch->changedId;
                oChangedDataSearch = _changedDataBegin;
                if (iChangedId > oChangedDataSearch->changedId)
                {
                    return _changedDataEnd;
                }
                //判断从 Begin 开始
                oChangedDataEnd = _changedDataBegin + iPublishedIndex;
                while (oChangedDataSearch < oChangedDataEnd)
                {
                    iChangedId = oChangedDataSearch->changedId;
                    oChangedDataSearch ++;
                    if (iChangedId > oChangedDataSearch->changedId)
                    {
                        return oChangedDataSearch;
                    }
                }
                return oChangedDataEnd+1;
            }
            //如果没找到，表示变化已经过了几个轮回
            else
            {
                return _changedDataBegin;
            }
        }

        typedef void (*changedEvent_t)(const ChangedData * const oChangedBegin, const ChangedData * const oChangedEnd);

        /**
         * @brief checkChangeds 策略是从变化的最前空白地开始扫（上一个变化的最后位置的后一个（ XXX_ 就从 _ 开始扫描 ）
         * @param iPublishedIndex ：已经发布到那一个，初始始就到 changedCount()-1
         * @param fnChangedEvent 回调函数：找到改变后回调通知
         * @return 改变后的 iChangedIndex
         */
        int checkChangeds(size_t iPublishedIndex0, changedEvent_t fnChangedEvent)
        {
//            GM_ASSER(iPublishedIndex < _changedCount);
            GM_ASSER_POINTER(fnChangedEvent);
            GM_INVALID_RETURE_(isValid(), 0);

            /*
                int changedId; // 变化ID : 1,2,3...0x7fffffff
                int measureId; // 量ID
                T value; // 量值
                int quality; // 值的品质
                msepoch_t changedTime; // 值的修改时间
                int changedSourceId; // 修改值的源ID
                int changedReasonId; // 修改值的原因
                T oldValue; // 修改前的值
             */
            //量的相关数据


            //返回 变化的地的下一个
            //找 changedId 前面比后面大的，返回后面的；表示目前变化记录
            //返回NULL：表示没有变化
            //返回 > iPublishedIndex : 表示在 iPublishedIndex 后面，一段
            //返回 < iPublishedIndex : 表示在 iPublishedIndex 前面，分二段
            //返回 = iPublishedIndex : 刚好一轮，分二段
            //返回 begin : 表示 变化已经过了几个轮回
            size_t iPublishedIndex = iPublishedIndex0 >= _changedCount ? _changedCount-1 : iPublishedIndex0;
            ChangedData * oChangedEnd = getCurrentChangedEnd(iPublishedIndex);
            if (oChangedEnd)
            {
                ChangedData * oChangedDataPublished = _changedDataBegin + iPublishedIndex;

                ChangedData * oChangedDataPublishBegin;
                ChangedData * oChangedDataPublishEnd;

                if (oChangedEnd > oChangedDataPublished) // 表示在 iPublishedIndex 后面，一段
                {
                    //插入 index -> 找到的
                    oChangedDataPublishBegin = _changedDataBegin + iPublishedIndex + 1;
                    GM_ASSER( oChangedDataPublishBegin < _changedDataEnd ); //否则是内部错误
                    oChangedDataPublishEnd = oChangedEnd;
                    fnChangedEvent(oChangedDataPublishBegin, oChangedDataPublishEnd);
                }
                else if (oChangedEnd != _changedDataBegin) // 表示在 iPublishedIndex 前面，分二段  ||  刚好一轮，分二段
                {
                    oChangedDataPublishBegin = _changedDataBegin + iPublishedIndex + 1;
                    //*分两次发布
                    if (oChangedDataPublishBegin < _changedDataEnd)
                    {
                        //插入 Index -> End
                        oChangedDataPublishEnd = _changedDataEnd;
                        fnChangedEvent(oChangedDataPublishBegin, oChangedDataPublishEnd);
                        //插入 begin -> 找到的
                        oChangedDataPublishBegin = _changedDataBegin;
                        oChangedDataPublishEnd = oChangedEnd;
                        fnChangedEvent(oChangedDataPublishBegin, oChangedDataPublishEnd);
                    }
                    //*分一次发布，只有一个可能，就 iPublishedIndex == changedCount()
                    else
                    {
                        oChangedDataPublishBegin = _changedDataBegin;
                        oChangedDataPublishEnd = oChangedEnd;
                        fnChangedEvent(oChangedDataPublishBegin, oChangedDataPublishEnd);
                    }
                }
                else // if (oChangedEnd == _changedDataBegin) // 表示 变化已经过了几个轮回
                {
                    cxPrompt() << "fn_publishChanged -MeasureType=" << getMeasureTypeImpl() << " oChangedEnd=ChangedBegin : changed tool many";
                    //插入 begin -> end
                    oChangedDataPublishBegin = _changedDataBegin;
                    oChangedDataPublishEnd = _changedDataEnd;
                    fnChangedEvent(oChangedDataPublishBegin, oChangedDataPublishEnd);
                }
                //到尾，从头再来
                if (oChangedDataPublishEnd == _changedDataEnd)
                    return _changedCount-1;
                else
                    return (oChangedDataPublishEnd - _changedDataBegin - 1);
            }
            return iPublishedIndex;
        }

        //*设置量的值
        inline void pushControl(int measureId, // 量ID
                               const T & value, // 控制值
                               msepoch_t controlTime, // 控制时间
                               int controlSourceId, // 控制的源ID
                               int controlReasonId, // 控制的原因
                               int controlStrategyId, // 控制的策略
                               int controlMethodId, // 控制的方法
                               const std::string & params,
                               int transferCount
                               )
        {

            GM_ASSER(_canWrite);
            GM_INVALID_RETURE((measureId >= _minId) && (measureId < _maxId));

            //比较对上一个MeasureId的Control的值是否一样，一样就更新时间而已
//            int controlId; // 控制ID : 1,2,3...0x7fffffff
//            int measureId; // 量ID
//            T value; // 控制值
//            msepoch_t controlTime; // 值的控制时间
//            int controlSourceId; // 控制的源ID
//            int controlReasonId; // 控制的原因
//            int controlStrategyId; // 控制的策略
//            int controlMethodId; // 控制的方法
//            char params[512];
//            int res; // 保留

            ControlData * oControlData = _controlDataBegin + _currentControlIndex;

            oControlData->measureId = measureId;//llb add
            oControlData->controlSourceId = controlSourceId;
            oControlData->controlReasonId = controlReasonId;
            oControlData->controlStrategyId = controlStrategyId;
            oControlData->controlMethodId = controlMethodId;
            if (params.size() > 0 && params.size() < sizeof(oControlData->params))
            {
                memmove(oControlData->params, params.data(), params.size());
                oControlData->params[params.size()] = 0;
            }
            else
            {
                oControlData->params[0] = 0;
            }
            oControlData->value = value;
            oControlData->controlTime = controlTime;
            oControlData->transferCount = transferCount;
            oControlData->controlId = _currentControlId;



            //更新 _currentControlId changedSourceId
            _currentControlId ++;
            _currentControlIndex ++;
            if (_currentControlId > ci_measure_control_maxid)
                _currentControlId = 1;
            if (_currentControlIndex >= _controlCount)
                _currentControlIndex = 0;
        }

        //返回 变化的地的下一个
        //找 controlId 前面比后面大的，返回后面的；表示目前变化记录
        //返回NULL：表示没有变化
        //返回 > iPublishedIndex : 表示在 iPublishedIndex 后面，一段
        //返回 < iPublishedIndex : 表示在 iPublishedIndex 前面，分二段
        //返回 = iPublishedIndex : 刚好一轮，分二段
        //返回 begin : 表示 变化已经过了几个轮回
        inline ControlData * getCurrentControlEnd(size_t iPublishedIndex = 0) {

            GM_ASSER(iPublishedIndex < _controlCount);

            //没有变化
            if (! _controlDataBegin)
                return NULL;

            //没有变化
            if (_controlDataBegin->controlId == 0)
                return NULL;

            ControlData * oControlDataPublished = _controlDataBegin + iPublishedIndex; //已经发布到 oControlDataPublished
            ControlData * oControlDataSearch = oControlDataPublished + 1; //下一次要发布的
            if (oControlDataSearch >= _controlDataEnd)
                oControlDataSearch = _controlDataBegin;
            int iDiff = oControlDataSearch->controlId - oControlDataPublished->controlId;

            //没有变化
            if (iDiff <= 0)
            {
                return NULL;
            }
            //正常变化
            else if (iDiff == 1)
            {
                //判断从 iControlIndex 开始从头判断
                ControlData * oControlDataEnd = _controlDataEnd - 1; //必须要 -1
                int iControlId;
                while (oControlDataSearch < oControlDataEnd)
                {
                    iControlId = oControlDataSearch->controlId;
                    oControlDataSearch ++;
                    if (iControlId > oControlDataSearch->controlId)
                    {
                        return oControlDataSearch;
                    }
                }
                //判断 最后一个，但不是End
                iControlId = oControlDataSearch->controlId;
                oControlDataSearch = _controlDataBegin;
                if (iControlId > oControlDataSearch->controlId)
                {
                    return _controlDataEnd;
                }
                //判断从 Begin 开始
                oControlDataEnd = _controlDataBegin + iPublishedIndex;
                while (oControlDataSearch < oControlDataEnd)
                {
                    iControlId = oControlDataSearch->controlId;
                    oControlDataSearch ++;
                    if (iControlId > oControlDataSearch->controlId)
                    {
                        return oControlDataSearch;
                    }
                }
                return oControlDataEnd+1;
            }
            //如果没找到，表示变化已经过了几个轮回
            else
            {
                return _controlDataBegin;
            }
        }

        typedef void (*controlEvent_t)(const ControlData * const oControlBegin, const ControlData * const oControlEnd);

        /**
         * @brief checkControls 策略是从变化的最前空白地开始扫（上一个变化的最后位置的后一个（ XXX_ 就从 _ 开始扫描 ）
         * @param iPublishedIndex ：已经发布到那一个，初始始就到 controlCount()-1
         * @param fnControlEvent 回调函数：找到改变后回调通知
         * @return 改变后的 iControlIndex
         */
        int checkControls(size_t iPublishedIndex, controlEvent_t fnControlEvent)
        {
            GM_ASSER(iPublishedIndex < _controlCount);
            GM_ASSER_POINTER(fnControlEvent);
            GM_INVALID_RETURE_(isValid(), 0);

            /*
                int controlId; // 变化ID : 1,2,3...0x7fffffff
                int measureId; // 量ID
                T value; // 量值
                int quality; // 值的品质
                msepoch_t controlTime; // 值的修改时间
                int controlSourceId; // 修改值的源ID
                int controlReasonId; // 修改值的原因
                T oldValue; // 修改前的值
             */
            //量的相关数据


            //返回 变化的地的下一个
            //找 controlId 前面比后面大的，返回后面的；表示目前变化记录
            //返回NULL：表示没有变化
            //返回 > iPublishedIndex : 表示在 iPublishedIndex 后面，一段
            //返回 < iPublishedIndex : 表示在 iPublishedIndex 前面，分二段
            //返回 = iPublishedIndex : 刚好一轮，分二段
            //返回 begin : 表示 变化已经过了几个轮回
            ControlData * oControlEnd = getCurrentControlEnd(iPublishedIndex);
            if (oControlEnd)
            {
                ControlData * oControlDataPublished = _controlDataBegin + iPublishedIndex;

                ControlData * oControlDataPublishBegin;
                ControlData * oControlDataPublishEnd;

                if (oControlEnd > oControlDataPublished) // 表示在 iPublishedIndex 后面，一段
                {
                    //插入 index -> 找到的
                    oControlDataPublishBegin = _controlDataBegin + iPublishedIndex + 1;
                    GM_ASSER( oControlDataPublishBegin < _controlDataEnd ); //否则是内部错误
                    oControlDataPublishEnd = oControlEnd;
                    fnControlEvent(oControlDataPublishBegin, oControlDataPublishEnd);
                }
                else if (oControlEnd != _controlDataBegin) // 表示在 iPublishedIndex 前面，分二段  ||  刚好一轮，分二段
                {
                    oControlDataPublishBegin = _controlDataBegin + iPublishedIndex + 1;
                    //*分两次发布
                    if (oControlDataPublishBegin < _controlDataEnd)
                    {
                        //插入 Index -> End
                        oControlDataPublishEnd = _controlDataEnd;
                        fnControlEvent(oControlDataPublishBegin, oControlDataPublishEnd);
                        //插入 begin -> 找到的
                        oControlDataPublishBegin = _controlDataBegin;
                        oControlDataPublishEnd = oControlEnd;
                        fnControlEvent(oControlDataPublishBegin, oControlDataPublishEnd);
                    }
                    //*分一次发布，只有一个可能，就 iPublishedIndex == controlCount()
                    else
                    {
                        oControlDataPublishBegin = _controlDataBegin;
                        oControlDataPublishEnd = oControlEnd;
                        fnControlEvent(oControlDataPublishBegin, oControlDataPublishEnd);
                    }
                }
                else // if (oControlEnd == _controlDataBegin) // 表示 变化已经过了几个轮回
                {
                    cxPrompt() << "fn_publishControl -MeasureType=" << getMeasureTypeImpl() << " oControlEnd=ControlBegin : control tool many";
                    //插入 begin -> end
                    oControlDataPublishBegin = _controlDataBegin;
                    oControlDataPublishEnd = _controlDataEnd;
                    fnControlEvent(oControlDataPublishBegin, oControlDataPublishEnd);
                }
                //到尾，从头再来
                if (oControlDataPublishEnd == _controlDataEnd)
                    return _controlCount-1;
                else
                    return (oControlDataPublishEnd - _controlDataBegin - 1);
            }
            return iPublishedIndex;
        }

        inline int getMeasureType() { return getMeasureTypeImpl(); }

        inline int getMinId() { return _minId > 0 ? _minId : g_fn_getMinMeasureId(getMeasureTypeImpl()); }

        inline int getMaxId() { return _maxId > 0 ? _maxId : _minId + _measureCount; }

        inline int getOriginalValue(int iMeasureId = -1) { return getOriginalValueImpl(iMeasureId); }

        inline size_t analysePublishedChangedIndex() {
            if (_currentChangedIndex == 0)
                return _changedCount-1;
            else
                return _currentChangedIndex-1;
        }

    protected:
        virtual int getMeasureTypeImpl() = 0;
        virtual int loadConfigImpl(ConfigData * oConfigBegin, const ConfigData * oConfigEnd) = 0;
        virtual T getOriginalValueImpl(int iMeasureId = -1) = 0;
        virtual bool equalValueImpl(const T & t1, const T & t2, const ConfigData * oConfigData) { return true; }

    private:
        //扫描当前改变的下一个
        ChangedData * scanCurrentChangedNext() {
            if (! _changedDataBegin)
                return NULL;
            if (_changedDataBegin->changedId == 0)
                return NULL;
            ChangedData * oChangedDataSearch = _changedDataBegin;
            ChangedData * oChangedDataEnd = _changedDataEnd - 1;
            int iChangedId;
            while (oChangedDataSearch < oChangedDataEnd)
            {
                iChangedId = oChangedDataSearch->changedId;
                oChangedDataSearch ++;
                if (iChangedId > oChangedDataSearch->changedId)
                {
                    return oChangedDataSearch;
                }
            }
            return _changedDataBegin;
        }

        ControlData * scanCurrentControlNext() {
            if (! _controlDataBegin)
                return NULL;
            if (_controlDataBegin->controlId == 0)
                return NULL;
            ControlData * oControlDataSearch = _controlDataBegin;
            ControlData * oControlDataEnd = _controlDataEnd - 1;
            int iControlId;
            while (oControlDataSearch < oControlDataEnd)
            {
                iControlId = oControlDataSearch->controlId;
                oControlDataSearch ++;
                if (iControlId > oControlDataSearch->controlId)
                {
                    return oControlDataSearch;
                }
            }
            return _controlDataBegin;
        }

    };

public:
    static size_t sizeOfMemory(int iMeasureCount)
    {
        //总头 + 预留 + 4段
        //1、头 32字节 : ShareMemoryHead
        //2、描述预留 4K
        //3、ShareMemoryHead + MeasureData位移值(int)(用来读) * iMeasureCount : int[iMeasureCount]
        //4、ShareMemoryHead + MeasureData位移值(int)(用来写) * iMeasureCount : int[iMeasureCount]
        //5、ShareMemoryHead + MeasureData值 * iMeasureCount * 2
        //6、ShareMemoryHead + MonsbChangedData值 * iMeasureCount * 2
        return
                sizeof(MeasureShareMemoryHead)
                + 1024*4
                + sizeof(MeasureShareMemoryHead) + sizeof(size_t) * iMeasureCount
                + sizeof(MeasureShareMemoryHead) + sizeof(WroteIndex) * iMeasureCount
                + sizeof(MeasureShareMemoryHead) + sizeof(MeasureData) * iMeasureCount * 2
                + sizeof(MeasureShareMemoryHead) + sizeof(ChangedData) * iMeasureCount * 2
                + sizeof(MeasureShareMemoryHead) + sizeof(ControlData) * iMeasureCount / 5
                + sizeof(MeasureShareMemoryHead) + sizeof(ConfigData) * iMeasureCount
                + sizeof(MeasureShareMemoryHead)
                ;
    }

private:

};

#endif // MEASURE_SHAREMEMORY_H
