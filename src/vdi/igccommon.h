
#ifndef IGCCOMMON_H
#define IGCCOMMON_H

#include <ccxx/ccxx.h>

class IGCCommSubject {

protected :
    virtual void notifyInd(uint32 nId,void *wParam, void *lParam)=0;
    friend class GCCommObserver;
};

class GCCommObserver
{
public:
    static const std::vector<IGCCommSubject*>* subjects();

    static bool addObserver(IGCCommSubject* oSubject);

    static void removeObserver(IGCCommSubject* oSubject);

    static void notify(uint32 nId,void *wParam, void *lParam);

    static int getObserverCount();
private:
    static std::vector<IGCCommSubject*>* _subjects;
    static CxMutex   _oMutex;
};

#endif // IGCCOMMON_H
