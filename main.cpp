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

#ifdef WIN32
std::vector<FXString> ParseCommandLine()
{
    LPWSTR* argv;
    int argc;
    std::vector<FXString> args;
    std::vector<FXString> tokens;
    std::vector<FXString> switches;
    std::vector<FXParam> params;

    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    for (int arg = 1; arg != argc; ++arg)
    {
        args.push_back(FXString(argv[arg]));
    }
    LocalFree(argv);

    FXParseCommandLine(args, tokens, switches, params);
    std::vector<FXString> filenames;

    for (const FXString& token : tokens)
    {
        FXString filename = FXPath::simplify(token);
        filenames.push_back(filename);
    }
    return filenames;
}
#else
std::vector<FXString> CSMap::ParseCommandLine(int argc, char** argv)
{
    std::vector<FXString> filenames;
    for (int arg = 0; arg != argc; ++arg)
    {
        if (argv[arg][0] != '-') {
            filenames.push_back(argv[arg]);
        }
    }
    return filenames;
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

#ifdef WIN32
    std::vector<FXString> filenames = ParseCommandLine();
#else
    std::vector<FXString> filenames = ParseCommandLine(argc, argv);
#endif
    if (!filenames.empty()) {
        csmap->loadFiles(filenames);
    }

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
	int exitcode = CSApp.run(); 
#ifdef HAVE_CURL
    curl_global_cleanup();
#endif
#ifdef HAVE_PHYSFS
    PHYSFS_deinit();
#endif
    return exitcode;
}
