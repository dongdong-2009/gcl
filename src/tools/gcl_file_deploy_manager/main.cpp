
#include <ccxx/ccxx.h>
#include <ccxx/qtcommon.h>
#include <ccxx/cxapplication_qt.h>

#include <QApplication>

#include "fdc.h"
#include "fdv.h"
#include "mainwindow.h"

using namespace std;

#undef main
int main(int argc, const char *argv[])
{
    QApplication app(argc, (char **)argv);

    CxApplicationQt::init(argc, argv);

    string sConfigFile = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");
//    CxFileSystem::createDirMultiLevel(sConfigFile);
    sConfigFile = CxFileSystem::mergeFilePath(sConfigFile, CxFileSystem::extractFileName(CxApplication::configFilePath()));

    string sProductPath;
    string sProjectPath;
    CxSkverBase * oProfile = CxSkverManager::beginUse( sConfigFile );
    if (oProfile)
    {
        sProductPath = oProfile->getValue(CS_SectionProgramConfig, string("ProductPath"), std::string());
        sProjectPath = oProfile->getValue(CS_SectionProgramConfig, string("ProjectPath"), std::string());
        CxSkverManager::endUse(oProfile);
    }

    if (sProductPath.empty())
    {
        QString sqProductPath = QtCommonDialog::DialogDir(QString(""), QString::fromUtf8("请选择ICS产品的目录"));
        if (sqProductPath.size()>0)
        {
            sProductPath = QtCommonString::gbkToStdString(sqProductPath);
        }
    }
    if (sProductPath.size()>0)
    {
        vector<CxFilePathInfo> pathInfos;
        CxFileSystem::scanDir(sProductPath, pathInfos);
        const std::vector<string> & sDirs = FdcManager::getProductLevel1Paths();
        for (size_t j = 0; j < sDirs.size(); ++j)
        {
            string sDir = sDirs.at(j);
            bool bFind = false;
            for (size_t i = 0; i < pathInfos.size(); ++i)
            {
                const CxFilePathInfo & filePathInfo = pathInfos.at(i);
                if (filePathInfo.pathType == CxFileSystem::PathTypeDir && filePathInfo.fileName == sDir)
                {
                    bFind = true;
                    break;
                }
            }
            if (! bFind)
            {
                QtCommonDialog::ShowPrompt(QString::fromUtf8("产品目录不是有效的ICS产品目录，系统将退出"));
                return 0;
            }
        }
        FdcManager::setProductRootPath(sProductPath);
    }
    else
    {
        QtCommonDialog::ShowPrompt(QString::fromUtf8("产品目录为空，系统将退出"));
        return 0;
    }

    if (sProjectPath.empty())
    {
        QString sqProjectPath = QtCommonDialog::DialogDir(QString(""), QString::fromUtf8("请选择项目的目录"));
        if (sqProjectPath.size()>0)
        {
            sProjectPath = QtCommonString::gbkToStdString(sqProjectPath);
        }
    }
    if (sProjectPath.size()>0)
    {
        bool bOk;
        CxFileSystem::relativeFilePath(sProjectPath, sProductPath, &bOk);
        if (! bOk) CxFileSystem::relativeFilePath(sProductPath, sProjectPath, &bOk);
        if (bOk)
        {
            QtCommonDialog::ShowPrompt(QString::fromUtf8("项目的目录与产品的目录不能有包含关系，系统将退出"));
            return 0;
        }

        PraManager::setProjectRootPath(sProjectPath);
    }
    else
    {
        QtCommonDialog::ShowPrompt(QString::fromUtf8("项目的目录无效，系统将退出"));
        return 0;
    }

    oProfile = CxSkverManager::beginUse( sConfigFile );
    if (oProfile)
    {
        oProfile->setValue(CS_SectionProgramConfig, string("ProductPath"), sProductPath);
        oProfile->setValue(CS_SectionProgramConfig, string("ProjectPath"), sProjectPath);
        CxSkverManager::endUse(oProfile, true);
    }

    FdvManager::load();
    FdcManager::load();
    PraManager::load();

    MainWindow window;
    window.show();

    int iResult = CxApplicationQt::exec();

    FdvManager::save();
    FdcManager::save();
    PraManager::save();

    return iResult;
}
