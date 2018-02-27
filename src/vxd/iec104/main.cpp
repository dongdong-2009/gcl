
#include "iec104.h"
#include <signal.h>
#include <iostream>

void my_handler (int param)
{
  std::cout << "recv signal : interactive attention : " << param << std::endl;
  std::cout << "application exit!!!" << param << std::endl;
  CxApplication::exit();
}

int main(int argc, char *argv[])
{
    std::cout << "begin sdk server" << std::endl;
//    void (*prev_handler)(int);

    fn_void_int_t prev_handler = signal (SIGINT, my_handler);
    if (prev_handler == SIG_ERR)
    {
        std::cout << "error : signal (SIGINT , *)" << std::endl;
    }

    return IEC_104::start(argc, argv);
}

