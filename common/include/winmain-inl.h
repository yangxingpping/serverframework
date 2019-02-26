#pragma once

#ifdef C_WINDOWS
#include <windows.h>
#endif

namespace 
{
	struct OnApp 
	{
		OnApp() 
		{
#ifdef C_WINDOWS
			// Initialize Winsock 2.2
			WSADATA wsaData;
			int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

			if (err) {
				std::cout << "WSAStartup() failed with error: %d" << err;
			}
#endif
		}
		~OnApp() 
		{
#ifdef C_WINDOWS
			system("pause");
			WSACleanup();
#endif
		}
	} __s_onexit_pause;
}