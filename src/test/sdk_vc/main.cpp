#include <typeinfo>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <bitset>
#include <stack>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <stddef.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>


#   undef UNICODE
#include <windows.h>


#include "gcl_common_sdk.h"


using namespace std;


void gprint(std::string msg)
{
	std::cout << msg << std::endl;
}

std::string ginput()
{
	std::string msg;
	std::cin >> msg;
	return msg;
}


int main(int argc, char*argv[])
{
	gprint("hello begin:");

	gprint("sdk init begin:");
	int r = gci_init(argc, argv);
	std::cout << "sdk init(" << r << ") end.";

    while (1)
	{
		std::cout << "tick : " << clock() << std::endl;

		string s = ginput();

		if (s=="q")
		{
			break;
		}
		else
		{
			r = gci_message_command_send(s.c_str(), NULL, 0, NULL);
			std::cout << "had send message size=" << r;
		}
	}

	gci_cleanup();

	gprint("hello end!");

	return 1;
}

