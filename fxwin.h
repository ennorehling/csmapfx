// used for windows compilation only

#ifndef FXWIN_H
#define FXWIN_H

// only windows
#ifdef WIN32

#define WINVER 0x0500
#define _WIN32_WINNT WINVER
#define _WIN32_DCOM

#include <windows.h>

#ifdef NDEBUG
#pragma comment(lib,"fox-1.6.lib")
#else
#pragma comment(lib,"foxd-1.6.lib")
#endif

#endif // WIN32

#ifdef CSMAP_MAIN

#ifdef WIN32

// use main() as starting point
#pragma comment(linker, "/entry:mainCRTStartup")

// libcmt conflicts with another lib
#pragma comment(linker, "/nodefaultlib:libcmt")

// use these libs, too:
#pragma comment(lib,"comctl32.lib")	// ...
#pragma comment(lib,"Ws2_32.lib")	// for gethostname (open/save dialogs)

#endif // WIN32

#ifdef WIN32
void deinitSystems()
{
	CoUninitialize();
}
#endif

void initSystems()
{
#ifdef WIN32
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	atexit(deinitSystems);
#endif
}

#endif // CSMAP_MAIN
#endif // FXWIN_H
