
#include "gcl_ga.h"
#include <ccxx/ccxx.h>

int main(int argc, const char *argv[])
{
    std::cout << "begin sdk client" << std::endl;

    GclGa::init(argc, argv);

    while(CxApplication::applicationStatus() )
    {
        CxThread::sleep(100);
    }
    return 0;
}

/*


#include <ccxx/cxapplication.h>
#include <ccxx/cxtimer.h>
#include <common/gcl_config.h>

#include <psm/general_access.h>

using namespace std;


int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    cout << "calculate process start." << endl;

    GclConfig::load();

    GeneralAccess::start();

    int iResult = CxApplication::exec();

    return iResult;
}

*/
