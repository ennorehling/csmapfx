// used for windows compilation only

#ifndef FXWIN_H
#define FXWIN_H

// only windows
#ifdef WIN32

#define NTDDI_VERSION NTDDI_WIN7  // Specifies that the minimum required platform is Windows 7.
#define STRICT_TYPED_ITEMIDS      // Utilize strictly typed IDLists
/*
#define WINVER 0x0500
#define _WIN32_WINNT WINVER
#define _WIN32_DCOM
*/
#define WINVER _WIN32_WINNT_WIN7
#define _WIN32_WINNT WINVER

#define NOMINMAX

#include <windows.h>
#include <combaseapi.h>
#include <commdlg.h>
#include <shlobj_core.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincrypt.h>
#include <wininet.h>

extern LPCWSTR g_wszAppID;

#ifdef WITH_MAP_EXPORT
#include <gdiplus.h>
#include <gdiplusimagecodec.h>
#include <gdiplusimaging.h>
#endif
#endif // WIN32

#ifdef CSMAP_MAIN

#ifdef WIN32
// use main() as starting point
#pragma comment(linker, "/entry:mainCRTStartup")
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
