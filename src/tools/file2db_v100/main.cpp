
#include <ccxx/ccxx.h>
#include <script/gcl_script.h>
#include <vdi/common_service.h>

#include <ccxx/cxdatabase_sqlite.h>
#include <ccxx/cxdatabase_odbc.h>

using namespace std;


void my_handler (int param)
{
  std::cout << "recv signal : interactive attention : " << param << std::endl;
  std::cout << "application exit!!!" << param << std::endl;
  CxApplication::exit();
}

void exitServer()
{
    CxApplication::exit();
}

int main(int argc, const char*argv[])
{
    fn_void_int_t prev_handler = signal (SIGINT, my_handler);
    if (prev_handler == SIG_ERR)
    {
        std::cout << "error : signal (SIGINT , *)" << std::endl;
    }

    //应用初始化
    CxApplication::init(argc, argv);
    //心跳服务初始化
    CommonService::init();

    CxDatabase * oDb = CxDatabaseManager::getDefaultDb();
    if (! oDb)
    {
        cxPrompt() << "Database is invalid, please config Database!";
        cxPrompt() << "Application will exit after 30 second or Enter Key to exit!";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }

    //脚本执行
    GclLua::start();

    atexit(exitServer);

    int iResult = CxApplication::exec();

    //
    GclLua::stop();

    return iResult;
}

