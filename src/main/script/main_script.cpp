
#include <ccxx/cxapplication.h>
#include <vdi/common_service.h>

using namespace std;

int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    CommonService::init();

    int iResult = CxApplication::exec();

    return iResult;
}

