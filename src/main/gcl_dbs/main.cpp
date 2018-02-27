#include <ccxx/cxapplication.h>
#include <ccxx/cxtimer.h>
#include <common/gcl_config.h>

#include <psm/general_access.h>
#include <vdi/common_service.h>

using namespace std;

int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    CommonService::init();

    cout << "calculate process start." << endl;

    GeneralAccess::start();

    int iResult = CxApplication::exec();

    return iResult;
}

