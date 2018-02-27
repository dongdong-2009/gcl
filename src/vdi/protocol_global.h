#ifndef PROTOCOL_GLOBAL_H
#define PROTOCOL_GLOBAL_H


#include <ccxx/ccxx.h>


//class ObjectDataLoader
//{
//public:
//    template<class ObjectDataBase>
//    static std::vector<ObjectDataBase*> load(const std::string& sFilePath)
//    {
//        std::vector<ObjectDataBase*> objectList;
//        CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath, true);
//        if (oProfile) {
//            std::vector<std::string> oSections = oProfile->sectionNames();
//            for (size_t i = 0; i < oSections.size(); ++i)
//            {
//                ObjectDataBase* oObject = new ObjectDataBase();
//                oProfile->prepareGet(oSections.at(i));
//                oObject->fromContext( *oProfile );
//                objectList.push_back( oObject );
//            }
//            CxSkverManager::endUse(oProfile, false);
//        }
//        return objectList;
//    }

//    template<class ObjectDataBase>
//    static void save(const std::string& sFilePath, std::vector<ObjectDataBase*>* oObjectList)
//    {
//        if ( ! CxFileSystem::isExist(sFilePath) ) {
//            if ( ! CxFileSystem::createFile(sFilePath) )
//                return;
//        }
//        CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath, false);
//        if (oProfile){
//            for (size_t i = 0; i < oObjectList->size(); ++i) {
//                ObjectDataBase* oObject = oObjectList->at(i);
//                oProfile->prepareSet(CxString::format("%06d",i));
//                oObject->toContext( *oProfile );
//            }
//            CxSkverManager::endUse(oProfile, true);
//        }
//    }

//};

#include "protocol_strings.h"
#include "protocol_config.h"

#endif // PROTOCOL_GLOBAL_H
