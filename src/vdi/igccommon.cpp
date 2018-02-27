#include "igccommon.h"


std::vector<IGCCommSubject*>* GCCommObserver::_subjects = new std::vector<IGCCommSubject*>();
CxMutex  GCCommObserver::_oMutex;

const std::vector<IGCCommSubject *> *GCCommObserver::subjects()
{    
    return _subjects;
}

bool GCCommObserver::addObserver(IGCCommSubject *oSubject)
{
    CxMutexScope m(_oMutex);
    if (!CxContainer::contain(_subjects,oSubject))
    {
        _subjects->push_back(oSubject);
    }
    return true;
}

void GCCommObserver::removeObserver(IGCCommSubject *oSubject)
{
    CxMutexScope m(_oMutex);
    CxContainer::remove(*_subjects,oSubject);
}

void GCCommObserver::notify(uint32 nId,void *wParam, void *lParam)
{
    CxMutexScope m(_oMutex);
    for (int i = 0; i < _subjects->size(); ++i)
    {
        IGCCommSubject* oSubject = _subjects->at(i);
        oSubject->notifyInd(nId,wParam,lParam);
    }
}

int GCCommObserver::getObserverCount()
{
    CxMutexScope m(_oMutex);
    return _subjects->size();
}



