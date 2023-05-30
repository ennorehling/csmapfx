/*
	csmapfx using fox-toolkit

	build fox-toolkit on linux with:
		$ ./configure --with-opengl=no --enable-release --enable-threadsafe --disable-shared
		$ make
		$ make install (as root)

	on windows (vc++ toolkit 2003) for csmapfx and fox-toolkit,
	add these "Command line" parameters to the compiler in "Release" mode:
		/G7 /GL /O2 /Og /Ob2 /Oi /Op /Ot /Oy

	(c) 2004-2006 Thomas J. Gritzan
*/

#define CSMAP_MAIN
#include "fxwin.h"

#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "translator.h"
#include "csmap.h"

#include "calc.h"

#include <fstream>
#include <sstream>
#include <string>
#include <fx.h>
#ifdef HAVE_PHYSFS
#include <physfs.h>
#endif
#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

#ifdef HAVE_DBGHELP
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#endif


void showHelpText()
{
	std::ostringstream help;

	help <<
		CSMAP_APP_TITLE_VERSION
		"\n  " CSMAP_APP_COPYRIGHT
		"\n"
		"\n"
		"Syntax: csmapfx [--help] [-hc] <Haupt-CR> <Karten-CRs> [-o <CR>]\n"
		"\n"
		"CsMapFX ist ein Kartenbetrachter f\u00fcr das Spiel Eressea (www.eressea.de).\n"
		"Das Programm \u00f6ffnet Eressea-Computer-Reporte (*.cr).\n"
		"\n"
		"Optionen:\n"
		"  -h, --help                  Zeigt diesen Text an.\n"
		"  -v, --version               Zeigt Versionsinformationen.\n"
		"  -c                          Startet ein kleines Fenster mit Taschenrechner.\n"
		"Diese Optionen m\u00fcssen hinter einem CR stehen:\n"
		"  -o <Datei>                  Schreibt einen CR aus den aktuellen Daten.\n"
		"\n"
		"  <Haupt-CR> <Karten-CRs...>\n"
		"Die erste Datei in der Liste wird komplett geladen.\n"
		"Alle weiteren Dateien werden als Karten-CRs angesehen und\n"
		"nur die Regionsdaten werden aus dem CR geladen und zum Report hinzugef\u00fcgt.\n"
		"Gibt man keine Parameter an, wird CsMapFX mit einer leeren Karte gestartet."
	;

	showError(help.str());
}

void showVersion()
{
	std::ostringstream help;

	help <<
		CSMAP_APP_TITLE_VERSION
		"\n  " CSMAP_APP_COPYRIGHT
	;

	showError(help.str());
}

static void ParseCommandLine(CSMap *csmap, int argc, char** argv)
{
    std::vector<FXString> args;
#ifdef WIN32
    LPWSTR* cmdArgs;
    int numArgs;

    cmdArgs = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    if (numArgs > 1) {
        for (int arg = 1; arg != numArgs; ++arg)
        {
            args.push_back(FXString(cmdArgs[arg]));
        }
    }
    LocalFree(cmdArgs);
#else
    for (int arg = 0; arg != argc; ++arg)
    {
        args.push_back(argv[arg]);
    }
#endif
    std::vector<FXString> tokens;
    std::vector<FXString> switches;
    std::vector<FXParam> params;
    FXParseCommandLine(args, tokens, switches, params);

    if (!tokens.empty()) {
        std::vector<FXString> errorMessages;
        csmap->loadFiles(tokens, errorMessages);
    }
    for (const FXParam& param : params) {
        if (param.key == "out") {
            csmap->saveFile(param.value);
        }
        else if (param.key == "export") {
            csmap->saveFile(param.value, map_type::MAP_MINIMAL);
        }
    }
    bool quit = false;
    for (const FXString& option : switches) {
        if (option == "quit") {
            quit = true;
        }
    }
    if (quit) {
        exit(0);
    }
}

#ifdef HAVE_DBGHELP
BOOL CALLBACK MyMiniDumpCallback(
    PVOID                            pParam,
    const PMINIDUMP_CALLBACK_INPUT   pInput,
    PMINIDUMP_CALLBACK_OUTPUT        pOutput
)
{
    BOOL bRet = FALSE;


    // Check parameters 

    if (pInput == 0)
        return FALSE;

    if (pOutput == 0)
        return FALSE;


    // Process the callbacks 

    switch (pInput->CallbackType)
    {
    case IncludeModuleCallback:
    {
        // Exclude the module 
        bRet = FALSE;
    }
    break;

    case IncludeThreadCallback:
    {
        // Exclude the module 
        bRet = FALSE;
    }
    break;

    case ModuleCallback:
    {
        _ASSERTE(!_T("It should not be called."));

        bRet = TRUE;
    }
    break;

    case ThreadCallback:
    {
        _ASSERTE(!_T("It should not be called."));

        bRet = TRUE;
    }
    break;

    case ThreadExCallback:
    {
        // Include this information 
        bRet = TRUE;

    }
    break;

    case MemoryCallback:
    {
        // We do not include any information here -> return FALSE 
        bRet = FALSE;
    }
    break;

    case CancelCallback:
        break;
    }


    return bRet;

}
LONG WINAPI CreateMiniDump(PEXCEPTION_POINTERS pep)
{
    // Open the file 

    HANDLE hFile = CreateFile(L"TinyDump.dmp", GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        // Create the minidump 

        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pep;
        mdei.ClientPointers = FALSE;

        MINIDUMP_CALLBACK_INFORMATION mci;

        mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
        mci.CallbackParam = 0;

        MINIDUMP_TYPE mdt = MiniDumpNormal;

        BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
            hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci);

        if (!rv)
            _tprintf(_T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError());
        else
            _tprintf(_T("Minidump created.\n"));

        // Close the file 

        CloseHandle(hFile);

    }
    else
    {
        _tprintf(_T("CreateFile failed. Error: %u \n"), GetLastError());
    }
    return EXCEPTION_EXECUTE_HANDLER;
}


#endif

int main(int argc, char *argv[])
{
#ifdef HAVE_PHYSFS
    PHYSFS_init(argv[0]);
    PHYSFS_mount(PHYSFS_getBaseDir(), NULL, 0);
    PHYSFS_mount(PHYSFS_getPrefDir("Eressea", "CsMapFX"), NULL, 0);
#endif
    initSystems();		// inits COM under windows
#ifdef HAVE_CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
	// Make application 
	FXApp CSApp("CSMap", "Eressea"); 

	// Open display
	CSApp.init(argc, argv); 
	CSApp.create();

	// Make window 
	CSMap* csmap = new CSMap(&CSApp); 

    ParseCommandLine(csmap, argc, argv);

    // Create it 
    try
    {
        // create all windows
        for (FXWindow* child = CSApp.getRootWindow()->getFirst(); child; child = child->getNext())
            if (!child->id())
                child->create();
    }
    catch (const FXException& e)
    {
        FXMessageBox::error(&CSApp, MBOX_OK, "CSMap - Fehler", "CSMap konnte nicht gestartet werden:\n%s", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        FXMessageBox::error(&CSApp, MBOX_OK, "CSMap - Fehler", "CSMap konnte nicht gestartet werden:\n%s", FXString(e.what()).text());
        return -1;
    }
    csmap->mapChange();
    csmap->show(PLACEMENT_DEFAULT);

    // Run 
#ifdef HAVE_DBGHELP
    SetUnhandledExceptionFilter(CreateMiniDump);
#endif
    int exitcode = CSApp.run();

#ifdef HAVE_CURL
    curl_global_cleanup();
#endif
#ifdef HAVE_PHYSFS
    PHYSFS_deinit();
#endif
    return exitcode;
}
