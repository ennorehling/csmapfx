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
#include <physfs.h>
#include <curl/curl.h>

void showHelpText()
{
	std::ostringstream help;

	help <<
		CSMAP_APP_TITLE_VERSION
		"\n  " CSMAP_APP_COPYRIGHT
		"\n"
		"\n"
		"Syntax: csmapfx [--help] [-hc] <Haupt-CR> <Karten-CRs> [--makemap] [-o <CR>]\n"
		"\n"
		"CsMapFX ist ein Kartenbetrachter f\u00fcr das Spiel Eressea (www.eressea.de).\n"
		"Das Programm \u00f6ffnet Eressea-Computer-Reporte (*.cr) und\n"
		"mit bzip2 gepackte CRs (*.cr.bz2).\n"
		"\n"
		"Optionen:\n"
		"  -h, --help                  Zeigt diesen Text an.\n"
		"  -v, --version               Zeigt Versionsinformationen.\n"
		"  -c                          Startet ein kleines Fenster mit Taschenrechner.\n"
		"Diese Optionen m\u00fcssen hinter einem CR stehen:\n"
		"  --makemap                   Filtert nur die Karteninfos aus den CRs heraus.\n"
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

int main(int argc, char *argv[])
{
    PHYSFS_init(argv[0]);
    PHYSFS_mount(PHYSFS_getBaseDir(), NULL, 0);
    PHYSFS_mount(PHYSFS_getPrefDir("Eressea", "CsMapFX"), NULL, 0);
    initSystems();		// inits COM under windows
    curl_global_init(CURL_GLOBAL_DEFAULT);

	// Make application 
	FXApp CSApp("CSMap", "Eressea"); 

	// Open display
	CSApp.init(argc, argv); 
	CSApp.create();

	CSApp.setTranslator(new GermanTranslation(&CSApp));

	// hide programm file name
	argv++; argc--;

	bool startgui = true;
	bool calculator = false;

	int arg;
	for (arg = 0; arg < argc; arg++)
	{
		if (argv[arg][0] == '-')	// check option
		{
			for (int i = 1; argv[arg][i]; i++)
			{
				char c = argv[arg][i];

				if (c == 'h')			// help
				{
					showHelpText();
					startgui = false;
				}
				else if (c == 'v')		// version
				{
					showVersion();
					startgui = false;
				}
				else if (c == 'c')		// calculator
					calculator = true;
				else if (c == '-')		// --bla
				{
                    if (argv[arg][i+1] == 'h' && argv[arg][i+2] == 'e' && argv[arg][i+3] == 'l' &&
						argv[arg][i+4] == 'p' && argv[arg][i+5] == '\0')
					{
						showHelpText();
						startgui = false;
					}
                    else if (argv[arg][i+1] == 'v' && argv[arg][i+2] == 'e' && argv[arg][i+3] == 'r' &&
						argv[arg][i+4] == 's' && argv[arg][i+5] == 'i' && argv[arg][i+6] == 'o' &&
						argv[arg][i+7] == 'n' && argv[arg][i+8] == '\0')
					{
						showVersion();
						startgui = false;
					}
					else
					{
						showError((std::string)"csmapfx: Ung\u00fcltige Option: --" + (argv[arg]+i+1) + "\nProbier 'csmapfx --help\' f\u00fcr m\u00f6gliche Optionen");
						startgui = false;
					}

					break;
				}
				else
				{
					showError((std::string)"csmapfx: Ung\u00fcltige Option: -" + c + "\nProbier 'csmapfx --help\' f\u00fcr m\u00f6gliche Optionen");
					startgui = false;
				}
			}
		}
		else if (argv[arg][0])		// stop here if a filename was found
		{
			startgui = true;
			break;
		}
	}

	if (!startgui && !calculator)
		return 0;

	// Make window 
	CSMap* csmap = NULL;
	FXMainWindow* shell = NULL;
	
	if (calculator)
	{
		shell = new FXMainWindow(&CSApp, "Taschenrechner", 0,0, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE), 100,100,300);
		FXCalculator* calc = new FXCalculator(shell, NULL,0, LAYOUT_FILL_X);
		calc->recalc();
	}
	else
		shell = csmap = new CSMap(&CSApp); 

	// Create it 
	try
	{
		// create all windows
		for(FXWindow* child = CSApp.getRootWindow()->getFirst(); child; child = child->getNext())
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

	if (shell)
		shell->show(PLACEMENT_DEFAULT);
	
	int numfiles = 0;

	// load command argument files
	for (; arg < argc; arg++)
	{
		if (argv[arg][0] == '-')	// check option
		{
			for (int i = 1; argv[arg][i]; i++)
			{
				char c = argv[arg][i];

				if (c == 'h')			// help
					showHelpText();
				else if (c == 'c')		// calculator
				{
					showError((std::string)"csmapfx: -c kann nicht zusammen mit einem CR benutzt werden.");
				}
				else if (c == 'o')
				{
					if (arg+1 < argc)
					{
						arg++;
						if (csmap)
							csmap->saveFile(FXString(argv[arg]));
						startgui = false;
						break;
					}
					else
						showError((std::string)"csmapfx: -o muss zusammen mit einem Dateinamen benutzt werden.");
				}
				else if (c == '-')		// -- bla
				{
                    if (argv[arg][i+1] == 'h' && argv[arg][i+2] == 'e' && argv[arg][i+3] == 'l' &&
							argv[arg][i+4] == 'p' && argv[arg][i+5] == '\0')
						showHelpText();
					else if (argv[arg][i+1] == 'm' && argv[arg][i+2] == 'a' && argv[arg][i+3] == 'k' &&
							argv[arg][i+4] == 'e' && argv[arg][i+5] == 'm' && argv[arg][i+6] == 'a' &&
							argv[arg][i+7] == 'p' && argv[arg][i+8] == '\0')
					{
						if (csmap)
							csmap->stripReportToMap();
					}
					else
						showError((std::string)"csmapfx: Ung\u00fcltige Option: --" + (argv[arg]+i+1) + "\nProbier 'csmapfx --help\' f\u00fcr m\u00f6gliche Optionen");

					break;
				}
				else
					showError((std::string)"csmapfx: Ung\u00fcltige Option: -" + c + "\nProbier 'csmapfx --help\' f\u00fcr m\u00f6gliche Optionen");
			}
		}
		else if (argv[arg][0])		// stop here if a filename was found
		{
			if (!csmap)
				continue;

			if (!numfiles)			
				csmap->loadFile(FXString(argv[arg]));
			else
				csmap->mergeFile(FXString(argv[arg]));

			numfiles++;
			startgui = true;
		}
	}

	// Programm ohne GUI beenden?
	if (!startgui && !calculator)
		return 0;

	// Run 
	int exitcode = CSApp.run(); 
    curl_global_cleanup();
    PHYSFS_deinit();
    return exitcode;
}
