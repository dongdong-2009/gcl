#ifndef FILEINFO_SHAREMEMORY_H
#define FILEINFO_SHAREMEMORY_H


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

#define ci_fileinfo_sharememory_version (2)

#define ci_measure_changed_maxid (0x7FFFFFFE)
#define ci_measure_control_maxid (0x7FFFFFFE)

#define ci_measure_control_count (1024)

//共享内存头
struct FileinfoShareMemoryCtrl
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

    FileinfoShareMemoryCtrl() :
        size(0), head1(ci_sharememory_head1), sizeRepeat(0), head2(ci_sharememory_head2), version(ci_fileinfo_sharememory_version),
        type(0), count(0), index(0), status(ci_sharememory_status_invalid),
        res1(0), res2(0), res3(0)
    { }
};

/**
 * @brief The ChangedData struct
 * 记录量值变化时的数据
 */
struct FileInfoChangedData
{
    int changedId; // 变化ID : 1,2,3...0x7fffffff
    int fileType;  //文件类型
    int64 fileSize;
    msepoch_t creationTime;
    msepoch_t lastWriteTime;
    int sourceId;
    int targetId;
    int saveStatus;
    int res[5];
    char fileShell[192];
    char filepath[256];
};


//共享内存描述部
//预留 4K

/**
 * @brief The ShareMemory class
 * sample to create ShareMemory : load("gcl_monsbs", 10*1000);
 * sample to view ShareMemory : load("gcl_monsbs");
 */
class FileInfoShareMemory
{
public:
    static size_t sizeOfMemory(int iCount)
    {
        //总头 + 预留 + 4段
        //1、头 32字节 : ShareMemoryHead
        //2、描述预留 4K
        //6、ShareMemoryHead + MonsbChangedData值 * iChangedCount
        return
                sizeof(FileinfoShareMemoryCtrl)
                + 1024*4
                + sizeof(FileinfoShareMemoryCtrl) + sizeof(FileInfoChangedData) * iCount
                + sizeof(FileinfoShareMemoryCtrl)
                ;
    }


    static FileInfoShareMemory * memoryManager();

    //定时尝试打开共享内存（只读方式），打开后回调 AfterLoadCallback
    static void open(bool bCanWrite = false, int iCheckInterval = 200);

    static void setAfterLoadCallback(fn_void_int_t fn);

    static void setBeforeCloseCallback(fn_void_int_t fn);

    //关闭共享内存（只读方式），打开后回调 AfterLoadCallback
    static void close();

private:
    //定时尝试打开共享内存（只读方式），打开后回调 AfterLoadCallback
    static void timerCheckSharememory(int iInterval);

private:
    CxShareMemory _shareMemory;

    size_t _changedCount;

    //* memory memory memory
    FileinfoShareMemoryCtrl * _head;

    char * _res;

    FileinfoShareMemoryCtrl * _changedDataHead;
    FileInfoChangedData * _changedDataBegin;
    FileInfoChangedData * _changedDataEnd;

    FileinfoShareMemoryCtrl * _end;

    bool _isReadOnly;
    bool _canWrite;

    int _currentSourceId;
    size_t _currentChangedIndex;
    size_t _currentChangedId;

public:
    FileInfoShareMemory() {
        doResetAllVariable();
    }

    ~FileInfoShareMemory() {
        shutdown();
    }

    void doResetAllVariable () {
        _changedCount = 0;
        _head = NULL;
        _res = NULL;
        _changedDataHead = NULL; _changedDataBegin = NULL; _changedDataEnd = NULL;
        _end = NULL;
        _isReadOnly = true; _canWrite = false;
        _currentSourceId = 0;
        _currentChangedIndex = 0; _currentChangedId = 1;
    }

    /**
     * @brief load
     * @param sMapName
     * @param iChangedCount 加载配置量的个数，如果为0：表示查看方式打开已有的共享内存
     */
    inline void load(const std::string & sMapName, int iChangedCount = 0)
    {
        std::string sShareName = sMapName;
//            std::string sShareName = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(),sMapName);
        if (iChangedCount>0)
        {
            create(sShareName, iChangedCount);
        }
        else
        {
            view(sShareName);
        }
    }

    /**
     * @brief create
     * @param sMapName
     * @param iChangedCount
     */
    inline void create(const std::string & sMapName, int iChangedCount)
    {
        //已经加载过就不用加载
        GM_VALID_RETURE(_shareMemory.isValid());
        GM_INVALID_RETURE(sMapName.size()>0);
        GM_INVALID_RETURE(iChangedCount>=10);
        if (_shareMemory.openModify(sMapName.c_str(), sizeOfMemory(iChangedCount)))
        {
            _changedCount = iChangedCount;
            doAllocatePointers();
            doAllocateValues();
            GM_ASSERT_IFNOT(_head->version==ci_fileinfo_sharememory_version, "Share Memory Version Error, Please Restart All Share Memory's Programer!!!");
            _head->status = ci_sharememory_status_good;
            _isReadOnly = false;
            _canWrite = true;
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
        }
    }

    inline void doAllocatePointers()
    {
        GM_INVALID_RETURE(_shareMemory.addr());

        char * pIndex = _shareMemory.addr();

        _head = (FileinfoShareMemoryCtrl *)pIndex; pIndex += sizeof(FileinfoShareMemoryCtrl);
        _res = pIndex; pIndex += 1024 * 4;

        _changedDataHead = (FileinfoShareMemoryCtrl *)pIndex; pIndex += sizeof(FileinfoShareMemoryCtrl);
        if (_changedCount<=0)
        {
            _changedCount = _changedDataHead->count;
        }
        _changedDataBegin = (FileInfoChangedData *)pIndex; pIndex += sizeof(FileInfoChangedData)*_changedCount;
        _changedDataEnd = (FileInfoChangedData *)pIndex;

        _end = (FileinfoShareMemoryCtrl *)pIndex;
    }

    inline void doAllocateValues()
    {
        GM_INVALID_RETURE(_shareMemory.isValid());
        GM_INVALID_RETURE(_head);
        GM_INVALID_RETURE(_changedCount>0);

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
        _head->version = ci_fileinfo_sharememory_version;
        _head->type = 0;
        _head->count = _changedCount;
        _head->status = ci_sharememory_status_initing;

        _changedDataHead->size = sizeof(FileinfoShareMemoryCtrl) + sizeof(FileInfoChangedData)*_changedCount;
        _changedDataHead->head1 = ci_sharememory_head1;
        _changedDataHead->sizeRepeat = _changedDataHead->size;
        _changedDataHead->head2 = ci_sharememory_head2;
        _changedDataHead->count = _changedCount;

        //初始化 changed 的数据
        FileInfoChangedData * oChangedData= _changedDataBegin;
        while (oChangedData < _changedDataEnd)
        {
            oChangedData->changedId       = 0;
            oChangedData->fileType       = 0;
            oChangedData->fileSize       = 0;
            oChangedData->creationTime       = 0;
            oChangedData->lastWriteTime       = 0;
            oChangedData->sourceId = 0;
            oChangedData->targetId = 0;
            oChangedData->saveStatus = 0;
            oChangedData->res[0] = 0;
            oChangedData->filepath[0] = 0;
            oChangedData->filepath[0] = 0;
            oChangedData ++;
        }

        (* _end) = (* _head);
    }

    inline void doResoreOld()
    {
        FileInfoChangedData * oCurrentChangedNext = scanCurrentChangedNext();
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
    }

    inline void shutdown() {
        if (_head) {
            _shareMemory.close();
            doResetAllVariable();
        }
    }

    inline bool isValid() {
        return _head && _head->status == ci_sharememory_status_good && _changedCount > 0;
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

    //*量数据改变时保存的区域
    inline size_t changedCount() {
        return _changedCount;
    }

    //changedDataBegin
    inline FileInfoChangedData * changedDataBegin() {
        return _changedDataBegin;
    }

    inline FileInfoChangedData * changedDataEnd() {
        return _changedDataEnd;
    }

    inline FileInfoChangedData * changedDataByChangedId(size_t iChangedId) {
        FileInfoChangedData * oChangedData = _changedDataBegin;
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

    inline void beginSetValue(int sourceId)
    {
        _currentSourceId = sourceId;
    }

    //*设置量的值
    inline void pushFileInfo(const CxFilePathInfo & fileInfo, const std::string & sFileShell, int iSourceId, int iTargetId, int iSaveStatus)
    {
        GM_ASSER(_canWrite);

        //获取当前可写的 changedData
        FileInfoChangedData * oChangedData = _changedDataBegin + _currentChangedIndex;
        //发布修改信息
        oChangedData->fileType = fileInfo.pathType;
        oChangedData->fileSize = fileInfo.fileSize;
        oChangedData->creationTime = fileInfo.creationTime;
        oChangedData->lastWriteTime = fileInfo.lastWriteTime;
        oChangedData->sourceId = iSourceId;
        oChangedData->targetId = iTargetId;
        oChangedData->saveStatus = iSaveStatus;
        std::string sFilePath = fileInfo.filePath();
        CxStringC::set(oChangedData->filepath, sFilePath.size()+1, sFilePath.data(), sizeof(oChangedData->filepath));
        CxStringC::set(oChangedData->fileShell, sFileShell.size()+1, sFileShell.data(), sizeof(oChangedData->fileShell));
        oChangedData->changedId       = _currentChangedId;

        //更新 _currentChangedId changedSourceId
        _currentChangedId ++;
        _currentChangedIndex ++;
        if (_currentChangedId > ci_measure_changed_maxid)
            _currentChangedId = 1;
        if (_currentChangedIndex >= _changedCount)
            _currentChangedIndex = 0;

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
    inline FileInfoChangedData * getCurrentChangedEnd(size_t iPublishedIndex = 0) {

        GM_ASSER(iPublishedIndex < _changedCount);

        //没有变化
        if (! _changedDataBegin)
            return NULL;

        //没有变化
        if (_changedDataBegin->changedId == 0)
            return NULL;

        FileInfoChangedData * oChangedDataPublished = _changedDataBegin + iPublishedIndex; //已经发布到 oChangedDataPublished
        FileInfoChangedData * oChangedDataSearch = oChangedDataPublished + 1; //下一次要发布的
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
            FileInfoChangedData * oChangedDataEnd = _changedDataEnd - 1; //必须要 -1
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

    typedef void (*changedEvent_t)(const FileInfoChangedData * const oChangedBegin, const FileInfoChangedData * const oChangedEnd);

    /**
     * @brief checkChangeds 策略是从变化的最前空白地开始扫（上一个变化的最后位置的后一个（ XXX_ 就从 _ 开始扫描 ）
     * @param iPublishedIndex ：已经发布到那一个，初始始就到 changedCount()-1
     * @param fnChangedEvent 回调函数：找到改变后回调通知
     * @return 改变后的 iChangedIndex
     */
    int checkChangeds(size_t iPublishedIndex0, changedEvent_t fnChangedEvent)
    {
//        GM_ASSER(iPublishedIndex < _changedCount);
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
        FileInfoChangedData * oChangedEnd = getCurrentChangedEnd(iPublishedIndex);
        if (oChangedEnd)
        {
            FileInfoChangedData * oChangedDataPublished = _changedDataBegin + iPublishedIndex;

            FileInfoChangedData * oChangedDataPublishBegin;
            FileInfoChangedData * oChangedDataPublishEnd;

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
                cxPrompt() << "fn_publishChanged oChangedEnd=ChangedBegin : changed tool many";
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

    inline size_t analysePublishedChangedIndex() {
        if (_currentChangedIndex == 0)
            return _changedCount-1;
        else
            return _currentChangedIndex-1;
    }

private:
    //扫描当前改变的下一个
    FileInfoChangedData * scanCurrentChangedNext() {
        if (! _changedDataBegin)
            return NULL;
        if (_changedDataBegin->changedId == 0)
            return NULL;
        FileInfoChangedData * oChangedDataSearch = _changedDataBegin;
        FileInfoChangedData * oChangedDataEnd = _changedDataEnd - 1;
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

};
//目前用单倍缓存区来写
//内存的数据格式化查看函数 sizeOfMemory

#endif // FILEINFO_SHAREMEMORY_H
