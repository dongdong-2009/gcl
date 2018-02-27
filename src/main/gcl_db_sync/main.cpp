

#include <ccxx/cxapplication.h>
#include <ccxx/cxtimer.h>
#include "db_sync.h"

using namespace std;


int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    CxDatabase * oDb1 = CxDatabaseManager::getDbByIndex(1);
    if (oDb1 && oDb1->openDatabase())
    {
        cxPrompt() << "Error ! Error ! Error ! can not find or open 'ConnectSource1'. Please Enter Any Key To Exit.";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }

    CxDatabase * oDb2 = CxDatabaseManager::getDbByIndex(2);
    if (oDb2 && oDb2->openDatabase())
    {
        cxPrompt() << "Error ! Error ! Error ! can not find or open 'ConnectSource2'. Please Enter Any Key To Exit.";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }

    CxDatabase * oDb3 = CxDatabaseManager::getDbByIndex(3);
    if (oDb3 && oDb3->openDatabase())
    {
        cxPrompt() << "Error ! Error ! Error ! can not find or open 'ConnectSource2'. Please Enter Any Key To Exit.";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }

    int iResult = CxApplication::exec();

    return iResult;
}

