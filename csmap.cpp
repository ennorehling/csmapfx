#include <climits>
#ifdef WIN32
#include <windows.h>
#include <shlobj_core.h>
#include <shlwapi.h>
#elif defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if !defined(PATH_MAX)
#define PATH_MAX 260
#endif

#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "csmap.h"
#include "commands.h"
#include "datafile.h"
#include "symbols.h"

#include "fxkeys.h"
#include "FXMenuSeparatorEx.h"
#include "FXFileDialogEx.h"
#include <FXRex.h>
#include <FXICOIcon.h>
#include <FXSocket.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <curl/curl.h>

FXDEFMAP(CSMap) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_OPEN,		    CSMap::onFileOpen),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_MERGE,		    CSMap::onFileMerge),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_AS,		    CSMap::onFileSaveAs),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_LOAD_ORDERS,	    CSMap::onFileOpenCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_ORDERS,	    CSMap::onFileSaveCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_CLOSE,		    CSMap::onFileClose),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_CHECK_ORDERS,    CSMap::onFileCheckCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_MAP,	    CSMap::onFileMapExport),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_ORDERS,   CSMap::onFileExportCommands),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_UPLOAD_ORDERS,   CSMap::onFileUploadCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_RECENT,		    CSMap::onFileRecent),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_QUIT,		    CSMap::onQuit),

	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_MERGE,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_AS,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_CLOSE,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_MAP,	    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_ORDERS,   CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_UPLOAD_ORDERS,   CSMap::updOpenFile),

    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_LOAD_ORDERS,     CSMap::updActiveFaction),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_ORDERS,     CSMap::updActiveFaction),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_CHECK_ORDERS,    CSMap::updActiveFaction),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_ERRROR_SELECTED,	    CSMap::onErrorSelected),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_MAPONLY,		    CSMap::onViewMapOnly),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_MAPONLY,		    CSMap::updViewMapOnly),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_MESSAGES,        CSMap::onViewMessages),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_MESSAGES,        CSMap::updViewMessages),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_MINIMAP,         CSMap::onViewMiniMap),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_MINIMAP,         CSMap::updViewMiniMap),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_INFODLG,	        CSMap::onViewInfoDlg),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_INFODLG,         CSMap::updViewInfoDlg),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_SEARCHDLG,       CSMap::onViewSearchDlg),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELALL,        CSMap::onRegionSelAll),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_UNSEL,         CSMap::onRegionUnSel),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_INVERTSEL,	    CSMap::onRegionInvertSel),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_EXTENDSEL,	    CSMap::onRegionExtendSel),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELISLANDS,    CSMap::onRegionSelIslands),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELALLISLANDS, CSMap::onRegionSelAllIslands),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_REMOVESEL,     CSMap::onRegionRemoveSel),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_HELP_ABOUT,		CSMap::onHelpAbout),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_TAB_UNIT,		CSMap::onViewUnitTab),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_TAB_STATS,		CSMap::onViewStatsTab),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_ZOOM,			CSMap::onChangeZoom),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_MAP_ZOOM,			CSMap::onUpdateZoom),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_SETORIGIN,	CSMap::onSetOrigin),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MAKEMAP,		CSMap::onMakeMap),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_VISIBLEPLANE,	CSMap::onSetVisiblePlane),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_MAP_VISIBLEPLANE,	CSMap::onUpdVisiblePlane),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_SELECTMARKED,		CSMap::onMapSelectMarked),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERNORTHWEST,	CSMap::onMapMoveMarker),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERNORTHEAST,	CSMap::onMapMoveMarker),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKEREAST,		CSMap::onMapMoveMarker),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERSOUTHEAST,	CSMap::onMapMoveMarker),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERSOUTHWEST,	CSMap::onMapMoveMarker),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERWEST,		CSMap::onMapMoveMarker),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_SETMODUS,		CSMap::onMapSetModus),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_MAP_SETMODUS,		CSMap::onUpdMapSetModus),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_UPDATE,			CSMap::onMapChange),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_SELECTION,		CSMap::onQueryMap),

	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_CALCULATOR,		CSMap::onCalculator),

	FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0,						CSMap::onClipboardRequest),
	FXMAPFUNC(SEL_CLIPBOARD_LOST, 0,						CSMap::onClipboardLost),
	FXMAPFUNC(SEL_CLIPBOARD_REQUEST, CSMap::ID_SETSTRINGVALUE,	CSMap::onSetClipboard),

	FXMAPFUNC(SEL_QUERY_HELP,	CSMap::ID_SETSTRINGVALUE,	CSMap::onSearchInfo),

    FXMAPFUNC(SEL_TIMEOUT,	CSMap::ID_WATCH_FILES,	CSMap::onWatchFiles),
};

FXIMPLEMENT(CSMap,FXMainWindow,MessageMap,ARRAYNUMBER(MessageMap))

static CSMap* CSMap_instance = NULL;

// Construct a MainWindow
CSMap::CSMap(FXApp *app) : FXMainWindow(app, CSMAP_APP_TITLE_VERSION, NULL, NULL, DECOR_ALL, 0, 0, 800, 600)
{
	// set "singleton"
	CSMap_instance = this;

    last_save_time = 0;
    app->addTimeout(this, CSMap::ID_WATCH_FILES, 1000, NULL);
	// create main window icon
	FXIcon* ico = new FXICOIcon(app, data::csmap);
    setMiniIcon(ico);
	setIcon(ico);

	// activate Alt-F4
	getAccelTable()->addAccel(MKUINT(KEY_F4,ALTMASK),this, FXSEL(SEL_SIGNAL, ID_CLOSE));

	// Statusbar
	status = new FXStatusBar(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER);
	status->getStatusLine()->setFrameStyle(FRAME_LINE);
	status->getStatusLine()->setBorderColor(getApp()->getShadowColor());
	status->getStatusLine()->setNormalText("");

	status_file = new FXLabel(status,"-",0,FRAME_LINE|LAYOUT_RIGHT|LAYOUT_CENTER_Y);
	status_lfile = new FXLabel(status," Datei:",NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
	status_file->setBorderColor(getApp()->getShadowColor());
	status_file->hide(); status_lfile->hide();

	status_turn = new FXLabel(status,"-",0,FRAME_LINE|LAYOUT_RIGHT|LAYOUT_CENTER_Y);
	status_lturn = new FXLabel(status," Runde:",NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
	status_turn->setBorderColor(getApp()->getShadowColor());
	status_turn->hide(); status_lturn->hide();

	status_faction = new FXLabel(status,"-",0,FRAME_LINE|LAYOUT_RIGHT|LAYOUT_CENTER_Y);
	status_lfaction = new FXLabel(status," Partei:",NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
	status_faction->setBorderColor(getApp()->getShadowColor());
	status_faction->hide(); status_lfaction->hide();

	// Men\u00fcbar, Toolbar
	FXDockSite* topdock = new FXDockSite(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
	/*FXDockSite* bottomdock =*/ new FXDockSite(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
	/*FXDockSite* leftdock =*/ new FXDockSite(this,LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y);
	/*FXDockSite* rightdock =*/ new FXDockSite(this,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y);

 	FXToolBarShell* dragshell1 = new FXToolBarShell(this,FRAME_RAISED,0,0,600,600);
	menubar = new FXMenuBar(topdock,dragshell1, LAYOUT_DOCK_NEXT|LAYOUT_SIDE_TOP);
	new FXToolBarGrip(menubar,menubar,FXMenuBar::ID_TOOLBARGRIP,TOOLBARGRIP_DOUBLE);

	// Toolbar
	FXToolBarShell* dragshell2 = new FXToolBarShell(this,FRAME_RAISED,0,0,600,600);
	toolbar = new FXToolBar(topdock,dragshell2, LAYOUT_DOCK_NEXT|LAYOUT_SIDE_TOP);
	new FXToolBarGrip(toolbar,toolbar,FXToolBar::ID_TOOLBARGRIP,TOOLBARGRIP_DOUBLE);

	icons.open = new FXGIFIcon(getApp(), data::tb_open, 0, IMAGE_ALPHAGUESS);
	icons.merge = new FXGIFIcon(getApp(), data::tb_merge, 0, IMAGE_ALPHAGUESS);
	icons.save = new FXGIFIcon(getApp(), data::tb_save, 0, IMAGE_ALPHAGUESS);
	icons.close = new FXGIFIcon(getApp(), data::tb_close, 0, IMAGE_ALPHAGUESS);

	icons.info = new FXGIFIcon(getApp(), data::tb_info, 0, IMAGE_ALPHAGUESS);

	icons.pointer = new FXGIFIcon(getApp(), data::tb_pointer, 0, IMAGE_ALPHAGUESS);
	icons.select = new FXGIFIcon(getApp(), data::tb_select, 0, IMAGE_ALPHAGUESS);

    for (int i = 0; i < data::TERRAIN_LAST; i++) {
        icons.terrain[i] = new FXGIFIcon(getApp(), data::terrain_icon(i), 0, IMAGE_ALPHAGUESS);
    }
	// Buttons
	new FXButton(toolbar,
		FXString(L"\tDatei \u00f6ffnen...\tEine neue Datei \u00f6ffnen."),
		icons.open,
		this,
		ID_FILE_OPEN, BUTTON_TOOLBAR);
	new FXButton(toolbar,
		FXString(L"\tDatei hinzuf\u00fcgen...\tL\u00e4dt einen Karten-Report in den aktuellen Report."),
		icons.merge,
		this,
		ID_FILE_MERGE, BUTTON_TOOLBAR);
	new FXButton(toolbar,
        FXString(L"\tDatei speichern unter...\tDie aktuelle Datei als neue Datei speichern."),
        icons.save,
		this,
		ID_FILE_SAVE_AS, BUTTON_TOOLBAR);
	new FXButton(toolbar,
		FXString(L"\tDatei schliessen\tDie aktuelle Datei schliessen."),
		icons.close,
		this,
		ID_FILE_CLOSE, BUTTON_TOOLBAR);

	FXVerticalSeparator *tb_separator = new FXVerticalSeparator(toolbar, SEPARATOR_LINE|LAYOUT_FILL_Y);
	tb_separator->setBorderColor(getApp()->getShadowColor());

	new FXToggleButton(toolbar, "\tZeige Regel-Informationen.", "\tZeige Regel-Informationen.", icons.info,icons.info, this,ID_VIEW_INFODLG, FRAME_RAISED|TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE);

	tb_separator = new FXVerticalSeparator(toolbar, SEPARATOR_LINE|LAYOUT_FILL_Y);
	tb_separator->setBorderColor(getApp()->getShadowColor());

	planes = new FXListBox(toolbar, this,ID_MAP_VISIBLEPLANE, LISTBOX_NORMAL|LAYOUT_FILL_Y|FRAME_LINE);
	planes->setBorderColor(getApp()->getShadowColor());
	planes->appendItem("Standardebene (0)", NULL, (void*)0);
	planes->setNumVisible(planes->getNumItems());

	tb_separator = new FXVerticalSeparator(toolbar, SEPARATOR_LINE|LAYOUT_FILL_Y);
	tb_separator->setBorderColor(getApp()->getShadowColor());

	FXButton *modus;
	modus = new FXButton(toolbar, "\tNormaler Modus", icons.pointer, this,ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
	modus->setUserData((void*)FXCSMap::MODUS_NORMAL);
	modus = new FXButton(toolbar, "\tAuswahl-Modus: Anklicken selektiert Regionen", icons.select, this,ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
	modus->setUserData((void*)FXCSMap::MODUS_SELECT);

	terrainPopup = new FXPopup(this);

	for (int i = 1; i < data::TERRAIN_LASTPUBLIC+1; i++)
	{
		datablock terrainRegion;
		terrainRegion.terrain(i);
		FXival udata = FXCSMap::MODUS_SETTERRAIN + i;

		FXMenuCommand *cmd = new FXMenuCommand(terrainPopup, terrainRegion.terrainString() + "\t\tZeichnen-Modus: Setze Regionen", icons.terrain[i], this,ID_MAP_SETMODUS);
		cmd->setUserData((void*)udata);
	}

	new FXMenuSeparatorEx(terrainPopup);
	FXMenuCommand *cmd = new FXMenuCommand(terrainPopup, FXString(L"L\u00f6schen\t\tZeichnen-Modus: L\u00f6sche Regionen"), icons.terrain[0], this,ID_MAP_SETMODUS);
	cmd->setUserData((void*)(FXCSMap::MODUS_SETTERRAIN));

	FXHorizontalFrame *terrainFrame = new FXHorizontalFrame(toolbar, LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0, 0,0);
	terrainSelect = new FXButton(terrainFrame, FXString(L"\tZeichnen-Modus: Erstelle oder L\u00f6sche Regionen"), icons.terrain[0], this,ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
	terrainSelect->setUserData((void*)FXCSMap::MODUS_SETTERRAIN);
	new FXMenuButton(terrainFrame, FXString(L"\tZeichnen-Modus: Erstelle oder L\u00f6sche Regionen"), NULL, terrainPopup, FRAME_RAISED|BUTTON_TOOLBAR|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|MENUBUTTON_DOWN, 0,0,12,0);

	// Recent file list
	recentFiles.setTarget(this);
	recentFiles.setSelector(ID_FILE_RECENT);
	recentFiles.setGroupName("MRU");
	recentFiles.setMaxFiles(6);

	// File menu
	filemenu = new FXMenuPane(this);
	new FXMenuTitle(menubar,"&Datei",NULL,filemenu);
	new FXMenuCommand(
		filemenu,
		FXString(L"\u00d6&ffnen...\tCtrl-O\tEinen Report \u00f6ffnen."),
		NULL,
		this,
		ID_FILE_OPEN);
	new FXMenuCommand(
		filemenu,
		FXString(L"Karte h&inzuf\u00fcgen...\tCtrl-I\tL\u00e4dt einen Karten-Report in den aktuellen Report."),
		NULL, this, ID_FILE_MERGE);
    new FXMenuCommand(
        filemenu,
        FXString(L"Karte sp&eichern...\tCtrl-E\tNeuen Karten-Report speichern."),
        NULL, this, ID_FILE_SAVE_AS);
    new FXMenuCommand(
        filemenu,
        FXString(L"Befehle &laden...\tCtrl-Shift-O\tBefehle aus einer Textdatei laden."),
        NULL, this, ID_FILE_LOAD_ORDERS);
    new FXMenuCommand(
		filemenu,
		FXString(L"Befehle &speichern\tCtrl-S\t\u00c4nderungen an Befehlsdatei speichern."),
		NULL, this, ID_FILE_SAVE_ORDERS);
	new FXMenuCommand(
		filemenu,
		FXString(L"Sch&liessen\t\tDie aktuelle Datei schliessen."),
		NULL, this, ID_FILE_CLOSE);

	new FXMenuSeparatorEx(filemenu);
    new FXMenuCommand(
        filemenu,
        FXString(L"Befehle pr\u00fcfen\t\tPr\u00fct die Befehle."),
        NULL, this, ID_FILE_CHECK_ORDERS);

	new FXMenuSeparatorEx(filemenu);
    new FXMenuCommand(
        filemenu,
        L"Befehle exportieren...\t\tDie Befehle versandfertig exportieren.",
        NULL, this, ID_FILE_EXPORT_ORDERS);
    new FXMenuCommand(
        filemenu,
        L"Befehle einsenden...\t\tDie Befehle an den Server versenden.",
        NULL, this, ID_FILE_UPLOAD_ORDERS);
    new FXMenuCommand(
		filemenu,
		L"Karte exportieren...\t\tDie Karte als PNG speichern.",
		NULL, this, ID_FILE_EXPORT_MAP);
	new FXMenuSeparatorEx(filemenu);

	recentmenu = new FXMenuPane(this);
	for (int i = 0; i < 6; i++)
		new FXMenuCommand(recentmenu, "", NULL, &recentFiles,FXRecentFiles::ID_FILE_1+i);

	new FXMenuCascade(filemenu, FXString(L"&Zuletzt ge\u00f6ffnet"), NULL, recentmenu, 0);
	new FXMenuSeparatorEx(filemenu);
	new FXMenuCommand(filemenu,
		L"B&eenden\tCtrl-Q\tDas Programm beenden.", NULL,
		this, ID_FILE_QUIT);

	// View menu
	viewmenu = new FXMenuPane(this);
	new FXMenuTitle(menubar,"&Ansicht",NULL,viewmenu);
	menu.toolbar = new FXMenuCheck(viewmenu,"Tool&bar\tCtrl-T\tToolbar ein- bzw. ausblenden.", toolbar,ID_TOGGLESHOWN);
	menu.maponly = new FXMenuCheck(viewmenu,"&Nur Karte anzeigen\tCtrl-M\tNur die Karte anzeigen, Regionsliste und -infos ausblenden.", this,ID_VIEW_MAPONLY,0);
	menu.messages = new FXMenuCheck(viewmenu,"&Meldungen\tCtrl-V\tRegionsmeldungen ein- bzw. ausblenden.", this,ID_VIEW_MESSAGES);
	menu.calc = new FXMenuCheck(viewmenu,"&Taschenrechner\tCtrl-C\tTaschenrechner-Leiste ein- bzw. ausblenden.");
	menu.minimap = new FXMenuCheck(viewmenu,FXString(L"\u00dcbersichts&karte\tCtrl-N\t\u00dcbersichtskarte ein- bzw. ausblenden."), this,ID_VIEW_MINIMAP);
	menu.infodlg = new FXMenuCheck(viewmenu,"&Informationen\tCtrl-B\tRegel-Informationen ein- bzw. ausblenden.", this,ID_VIEW_INFODLG);
	new FXMenuSeparatorEx(viewmenu, "Liste");
	menu.ownFactionGroup = new FXMenuCheck(viewmenu,"&Gruppe aktiver Partei\tAlt-G\tDie Einheiten der eigenen Partei stehen in einer Gruppe.");
    menu.colorizeUnits = new FXMenuCheck(viewmenu, "Einheiten ko&lorieren\t\tEinheiten in Geb\u00e4uden und Schiffen einf\u00e4rben.");
    new FXMenuSeparatorEx(viewmenu, "Karte");
	menu.streets = new FXMenuCheck(viewmenu,"&Strassen zeigen\tAlt-S\tStrassen auf der Karte anzeigen.");
	menu.visibility = new FXMenuCheck(viewmenu,FXString(L"&Sichtbarkeit zeigen\tAlt-V\tSymbole f\u00fcr Sichtbarkeit der Regionen anzeigen (Leuchtturm und Durchreise)."));
	menu.shiptravel = new FXMenuCheck(viewmenu,"&Durchschiffung\tAlt-T\tEin kleines Schiffsymbol anzeigen, falls Schiffe durch eine Region gereist sind.");
	menu.shadowRegions = new FXMenuCheck(viewmenu,"Regionen ab&dunkeln\tAlt-F\tRegionen abdunkeln, wenn nicht von eigenen Personen gesehen.");
	menu.islands = new FXMenuCheck(viewmenu,"&Inselnamen zeigen\tAlt-I\tInselnamen auf der Karte zeigen.");
	menu.minimap_islands = new FXMenuCheck(viewmenu,"&Inseln auf Minikarte\t\tInselnamen auf der Minikarte zeigen.");

	// Region menu
	regionmenu = new FXMenuPane(this);
	new FXMenuTitle(menubar,"&Region",NULL,regionmenu);
	new FXMenuCommand(regionmenu,"&Suchen...\tCtrl-F\tEine Region, Einheit, Schiff, etc. suchen.",NULL, this,ID_VIEW_SEARCHDLG);
	menu.regdescription = new FXMenuCheck(regionmenu,"&Beschreibung zeigen\t\tRegionsbeschreibung anzeigen.");
	new FXMenuSeparatorEx(regionmenu, "Markieren");
	new FXMenuCommand(regionmenu,FXString(L"&Alle markieren\tCtrl-A\tAlle Regionen ausw\u00e4hlen."),NULL,this,ID_REGION_SELALL);
	new FXMenuCommand(regionmenu,FXString(L"Alle &Inseln ausw\u00e4hlen\t\tAlle Landregionen ausw\u00e4hlen (Ozean, Feuerwand und Eisberg z\u00e4hlen nicht als Land)."),NULL,this,ID_REGION_SELALLISLANDS);
	new FXMenuCommand(regionmenu,FXString(L"&Keine markieren\t\tKeine Region ausw\u00e4hlen."),NULL,this,ID_REGION_UNSEL);
	new FXMenuCommand(regionmenu,FXString(L"Auswahl &invertieren\t\tAusgew\u00e4hlte Regionen abw\u00e4hlen und umgekehrt."),NULL,this,ID_REGION_INVERTSEL);
		selectionmenu = new FXMenuPane(this);
		new FXMenuCommand(selectionmenu,"Auswahl &erweitern\tCtrl-Shift-F7\tAuswahl mit dem Radius von einer Region erweitern.",NULL,this,ID_REGION_EXTENDSEL);
		new FXMenuCommand(selectionmenu,FXString(L"&Inseln ausw\u00e4hlen\tCtrl-Shift-F9\tAuswahl auf komplette Inseln erweitern."),NULL,this,ID_REGION_SELISLANDS);
	new FXMenuCascade(regionmenu, "&Erweitern", NULL, selectionmenu);
	new FXMenuSeparatorEx(regionmenu, "Bearbeiten");
	new FXMenuCommand(regionmenu,FXString(L"Markierte &l\u00f6schen\t\t"),NULL,this,ID_REGION_REMOVESEL);

	// Faction menu
	factionmenu = new FXMenuPane(this);
	menu.faction = new FXMenuTitle(menubar,"&Partei",NULL,factionmenu);
	menu.faction->disable();

	new FXMenuSeparatorEx(factionmenu, "Parteiinfo");
	menu.name = new FXMenuCommand(factionmenu, "Parteiname\t\tName der Partei");
	menu.type = new FXMenuCommand(factionmenu, FXString(L"Rasse (Silber)\t\tRasse und Typpr\u00e4fix der Partei, Rekrutierungskosten"));
	menu.magic = new FXMenuCommand(factionmenu, FXString(L"Magiegebiet\t\tgew\u00e4hltes Magiegebiet"));
	menu.email = new FXMenuCommand(factionmenu, "eMail\t\teMail-Adresse der Partei");

	new FXMenuSeparatorEx(factionmenu, "Statistik");
	menu.number = new FXMenuCommand(factionmenu, "Personen, Einheiten (und Helden)\t\tAnzahl Personen, Einheiten und Helden");
	menu.points = new FXMenuCommand(factionmenu, "Punkte (Durchschnitt)\t\tPunkte der Partei und Durchschnitt gleichaltriger Parteien");
	menu.age = new FXMenuCommand(factionmenu, "Parteialter\t\tAlter der Partei in Runden");

	new FXMenuSeparatorEx(factionmenu, FXString(L"Gegenst\u00e4nde"));
		menu.factionpool = new FXMenuPane(this);
		menu.poolnoitems = new FXMenuCommand(menu.factionpool,FXString(L"Keine Gegenst\u00e4nde\t\tDer Parteipool enth\u00e4lt keine Gegenst\u00e4nde."),NULL);
		menu.poolnoitems->disable();
	new FXMenuCascade(factionmenu, "&Parteipool", NULL, menu.factionpool, 0);

	// Map menu
	mapmenu = new FXMenuPane(this);
	new FXMenuTitle(menubar,"&Karte",NULL,mapmenu);
		planemenu = new FXMenuPane(this);
		FXMenuRadio *radio = new FXMenuRadio(planemenu,"Standardebene (0)", this,ID_MAP_VISIBLEPLANE,0);
		radio->setCheck();
	new FXMenuCascade(mapmenu, "&Ebene", NULL, planemenu, 0);
		zoommenu = new FXMenuPane(this);
		new FXMenuCommand(zoommenu,FXString(L"Vergr\u00f6\u00dfern\tCtrl-+\tKarte vergr\u00f6\u00dfern."),NULL,this,ID_MAP_ZOOM,0);
		new FXMenuCommand(zoommenu,"Verkleinern\tCtrl--\tKarte verkleinern.",NULL,this,ID_MAP_ZOOM,0);
		new FXMenuSeparatorEx(zoommenu, FXString(L"Gr\u00f6\u00dfe"));
		new FXMenuRadio(zoommenu,"&1 Pixel\tCtrl-8\t1.6%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&2 Pixel\tCtrl-7\t3.1%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&4 Pixel\tCtrl-6\t6.3%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&8 Pixel\tCtrl-5\t12.5%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&16 Pixel\tCtrl-4\t25%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&32 Pixel\tCtrl-3\t50%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&64 Pixel\tCtrl-2\t100%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"128 Pixel\tCtrl-1\t200%",this,ID_MAP_ZOOM,0);
	new FXMenuCascade(mapmenu,FXString(L"&Gr\u00f6\u00dfe"), NULL, zoommenu, 0);
	new FXMenuCommand(mapmenu,"&Ursprung setzen\tCtrl-U\tDen Kartenursprung (0/0) auf die markierte Region setzen.",NULL,this,ID_MAP_SETORIGIN,0);
	new FXMenuCommand(mapmenu,FXString(L"&Kartenreport erzeugen\t\tL\u00f6scht alle Informationen au\u00dfer Regionsnamen und -terrain und Inseln."),NULL,this,ID_MAP_MAKEMAP,0);
	new FXMenuSeparatorEx(mapmenu, "Regionsmarker");
	new FXMenuCommand(mapmenu,FXString(L"Markierte &ausw\u00e4hlen\tCtrl-Space\tMarkierte Region ausw\u00e4hlen."),NULL,this,ID_MAP_SELECTMARKED,0);
	new FXMenuCommand(mapmenu,"Nach Westen\tCtrl-H\tRegionsmarker eins nach Westen setzen.",NULL,this,ID_MAP_MARKERWEST,0);
	new FXMenuCommand(mapmenu,"Nach Osten\tCtrl-L\tRegionsmarker eins nach Osten setzen.",NULL,this,ID_MAP_MARKEREAST,0);
	new FXMenuCommand(mapmenu,FXString(L"Nach S\u00fcdwesten\tCtrl-J\tRegionsmarker eins nach S\u00fcdwesten setzen."),NULL,this,ID_MAP_MARKERSOUTHWEST,0);
	new FXMenuCommand(mapmenu,"Nach Nordosten\tCtrl-K\tRegionsmarker eins nach Nordosten setzen.",NULL,this,ID_MAP_MARKERNORTHEAST,0);

	getAccelTable()->addAccel(MKUINT(KEY_KP_5,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_SELECTMARKED));
	getAccelTable()->addAccel(MKUINT(KEY_KP_7,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_MARKERNORTHWEST));
	getAccelTable()->addAccel(MKUINT(KEY_KP_9,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_MARKERNORTHEAST));
	getAccelTable()->addAccel(MKUINT(KEY_KP_6,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_MARKEREAST));
	getAccelTable()->addAccel(MKUINT(KEY_KP_3,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_MARKERSOUTHEAST));
	getAccelTable()->addAccel(MKUINT(KEY_KP_1,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_MARKERSOUTHWEST));
	getAccelTable()->addAccel(MKUINT(KEY_KP_4,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_MAP_MARKERWEST));

	// jump to calculator on ESCAPE
	getAccelTable()->addAccel(MKUINT(KEY_Escape,0), this,FXSEL(SEL_COMMAND,ID_CALCULATOR));

	// Help menu
	helpmenu = new FXMenuPane(this);
	new FXMenuTitle(menubar,"&?",NULL,helpmenu);
	new FXMenuCommand(helpmenu,"Wer mich schuf...\t\tKontaktinformationen",NULL,this,ID_HELP_ABOUT);

	// tooltip
	new FXToolTip(app);

	// *** create workdesk ***
    content = new FXSplitterEx(this, SPLITTER_HORIZONTAL|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	// Left splitter
	leftframe = new FXVerticalFrame(content,LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 3,0,0,0);

	// Region list window
	regions = new FXRegionList(leftframe, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	regions->setMapFile(report);

	menu.ownFactionGroup->setTarget(regions);
	menu.ownFactionGroup->setSelector(FXRegionList::ID_TOGGLEOWNFACTIONGROUP);

	menu.colorizeUnits->setTarget(regions);
	menu.colorizeUnits->setSelector(FXRegionList::ID_TOGGLEUNITCOLORS);

	// Middle splitter
	middle = new FXVerticalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0,0,0,0,0,0,2);
	content->setStretcher(middle);

	FXSplitterEx *mapsplit = new FXSplitterEx(middle, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	// Map window
	map = new FXCSMap(mapsplit, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	map->setMapFile(report);
	mapsplit->setStretcher(map);

	menu.streets->setTarget(map);
	menu.streets->setSelector(FXCSMap::ID_TOGGLESTREETS);

	menu.visibility->setTarget(map);
	menu.visibility->setSelector(FXCSMap::ID_TOGGLEVISIBILITYSYMBOL);

	menu.shiptravel->setTarget(map);
	menu.shiptravel->setSelector(FXCSMap::ID_TOGGLESHIPTRAVEL);

	menu.shadowRegions->setTarget(map);
	menu.shadowRegions->setSelector(FXCSMap::ID_TOGGLESHADOWREGIONS);

	menu.islands->setTarget(map);
	menu.islands->setSelector(FXCSMap::ID_TOGGLEISLANDS);

	// List of regions messages
	msgBorder = new FXVerticalFrame(mapsplit,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_LINE, 0,0,0,0, 0,0,0,0);
	msgBorder->setBorderColor(getApp()->getShadowColor());

    outputTabs = new FXTabBook(msgBorder, NULL, 0, TABBOOK_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    new FXTabItem(outputTabs, "Meldungen");
    messages = new FXMessages(outputTabs, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	messages->setMapFile(report);
    new FXTabItem(outputTabs, "Fehler");
    errorList = new FXList(outputTabs, this, ID_ERRROR_SELECTED, LAYOUT_FILL_X | LAYOUT_FILL_Y);

    // Calculator bar
	mathbar = new FXCalculator(middle, this,ID_SELECTION, LAYOUT_FILL_X);
	mathbar->setMapFile(report);
	mathbar->connectMap(map);
	menu.calc->setTarget(mathbar);
	menu.calc->setSelector(FXCalculator::ID_TOGGLESHOWN);

	// Right splitter
	rightframe = new FXVerticalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,3,0,0);

	FXHorizontalFrame *riFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	riTab = new FXToolBarTab(riFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	riTab->setTipText("Regionsinformationen ein- und ausblenden");
	regioninfos = new FXRegionInfos(riFrame, this,ID_SELECTION, LAYOUT_FILL_X);
	regioninfos->setMapFile(report);

	menu.regdescription->setTarget(regioninfos);
	menu.regdescription->setSelector(FXRegionInfos::ID_TOGGLEDESCRIPTION);

	FXHorizontalFrame *siFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	siTab = new FXToolBarTab(siFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	siTab->setTipText("Statistik ein- und ausblenden");
	statsinfos = new FXStatsInfos(siFrame, this,ID_SELECTION, LAYOUT_FILL_X);
	statsinfos->setMapFile(report);

	FXHorizontalFrame *tiFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	tiTab = new FXToolBarTab(tiFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	tiTab->setTipText("Handelsinformationen ein- und ausblenden");
	tradeinfos = new FXTradeInfos(tiFrame, this,ID_SELECTION, LAYOUT_FILL_X);
	tradeinfos->setMapFile(report);

	commandsplitter = new FXSplitterEx(rightframe, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	tabbook = new FXTabBook(commandsplitter, NULL,0, TABBOOK_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);

    new FXTabItem(tabbook, "Einheiten");
	unitlist = new FXUnitList(tabbook, this,ID_SELECTION, LAYOUT_FILL_X);
	unitlist->setMapFile(report);
    new FXTabItem(tabbook, "Statistik");
	statistics = new FXStatistics(tabbook, this,ID_SELECTION, LAYOUT_FILL_X);
	statistics->setMapFile(report);
    getAccelTable()->addAccel(MKUINT(KEY_1, ALTMASK), this, FXSEL(SEL_COMMAND, ID_TAB_UNIT));
    getAccelTable()->addAccel(MKUINT(KEY_2, ALTMASK), this, FXSEL(SEL_COMMAND, ID_TAB_STATS));

	// Befehlseditor
	commandframe = new FXVerticalFrame(commandsplitter,LAYOUT_FILL_X|FRAME_LINE, 0,0,0,0, 0,0,0,0);
	commandframe->setBorderColor(getApp()->getShadowColor());
	commands = new FXCommands(commandframe, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	commands->setMapFile(report);
	commands->connectMap(map);

	// commands editor tools
	FXHorizontalFrame *cmdBottomFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	FXToolBarTab *cmdTab = new FXToolBarTab(cmdBottomFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	cmdTab->setTipText("Befehlstools ein- und ausblenden");

    FXHorizontalFrame *cmdOptFrame = new FXHorizontalFrame(cmdBottomFrame,LAYOUT_FILL_X|FRAME_LINE, 0,0,0,0, 3,3,1,1);
	cmdOptFrame->setBorderColor(getApp()->getShadowColor());
    new FXCheckButton(cmdOptFrame,
        FXString(L"&best\u00e4tigt\tBefehle best\u00e4tigen\tBefehle f\u00fcr diese Einheit best\u00e4tigen"),
        commands, FXCommands::ID_UNIT_CONFIRM, CHECKBUTTON_NORMAL);
    FXButton * btn;
    btn = new FXButton(cmdOptFrame,
        FXString(L"<\tVorherige Einheit\tZur vorhergehenden unbest\u00e4tigten Einheit"), NULL,
        commands, FXCommands::ID_UNIT_PREV, BUTTON_TOOLBAR);
    btn->addHotKey(FXHotKey(MKUINT(KEY_comma, CONTROLMASK)));
    btn = new FXButton(cmdOptFrame,
        FXString(L">\tN\u00e4chste Einheit\tZur n\u00e4chsten unbest\u00e4tigten Einheit"), NULL,
        commands, FXCommands::ID_UNIT_NEXT, BUTTON_TOOLBAR);
    btn->addHotKey(FXHotKey(MKUINT(KEY_period, CONTROLMASK)));

    new FXButton(cmdOptFrame,
        "+&temp\tNeue Temp-Einheit\tTemp-Einheit erstellen", NULL,
        commands, FXCommands::ID_UNIT_ADD, BUTTON_TOOLBAR);
    btn->addHotKey(FXHotKey(MKUINT(KEY_t, CONTROLMASK)));

	FXTextField* rowcol = new FXTextField(cmdOptFrame, 5, commands,FXCommands::ID_ROWCOL, TEXTFIELD_READONLY|JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_RIGHT);
	rowcol->disable();
	rowcol->setDefaultCursor(getDefaultCursor());
	rowcol->setTextColor(getApp()->getShadowColor());

	// minimap
	minimap_frame = new FXDialogBox(this, FXString(L"\u00dcbersichtskarte"), DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE), 100,100, 640,480, 0,0,0,0);
	minimap_frame->setIcon(icon);
	minimap_frame->getAccelTable()->addAccel(MKUINT(KEY_N,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_VIEW_MINIMAP));

	FXStatusBar *minimap_bar = new FXStatusBar(minimap_frame,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER);
	minimap_bar->getStatusLine()->setFrameStyle(FRAME_LINE);
	minimap_bar->getStatusLine()->setBorderColor(getApp()->getShadowColor());
	minimap_bar->getStatusLine()->setNormalText("");

	minimap = new FXCSMap(minimap_frame, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y, true /*minimap-mode*/);
	minimap->setMapFile(report);
	minimap->connectMap(map);

	menu.minimap_islands->setTarget(minimap);
	menu.minimap_islands->setSelector(FXCSMap::ID_TOGGLEISLANDS);

	// info dialog
	infodlg = new FXInfoDlg(this, "Informationen", icon, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE));
	infodlg->getAccelTable()->addAccel(MKUINT(KEY_B,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_VIEW_INFODLG));
	infodlg->setGame("default");

	// search dialog
	searchdlg = new FXSearchDlg(this, "Suchen...", icon, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE));
	searchdlg->getAccelTable()->addAccel(MKUINT(KEY_F,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_VIEW_SEARCHDLG));
	searchdlg->setMapFile(report);
}

/*static*/ CSMap* CSMap::getInstance()
{
	return CSMap_instance;
}

CSMap::~CSMap()
{
    errorList->clearItems();
    for (auto error : output) delete error;
    output.clear();
    
    // delete menus
	delete filemenu;
		delete recentmenu;
	delete viewmenu;
	delete regionmenu;
		delete selectionmenu;
	delete mapmenu;
		delete planemenu;
		delete zoommenu;
	delete factionmenu;
	delete helpmenu;

	// toolbar
	delete terrainPopup;

	// delete icons
	delete icons.open;
	delete icons.merge;
	delete icons.save;
	delete icons.close;

	delete icons.info;

	delete icons.pointer;
	delete icons.select;

    delete tabbook;
    delete outputTabs;

	for (int i = 0; i < data::TERRAIN_LAST; i++)
		delete icons.terrain[i];
}

// Create and initialize
void CSMap::create()
{
	// Create the windows
	FXMainWindow::create();

	// reload window position & size
    FXRegistry &reg = getApp()->reg();
    const FXchar *echeck = reg.readStringEntry("settings", "echeck_dir", NULL);
    if (echeck) {
        settings.echeck_dir.assign(echeck);
    }
#ifdef WIN32
    else {
        PWSTR wPath = NULL;
        if (S_OK == SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, NULL, &wPath)) {
            FXString path(wPath);
            FXString exefile;
            path.append("\\Eressea\\Echeck");
            exefile = path + "\\echeckw.exe";
            if (PathFileExists(exefile.text())) {
                settings.echeck_dir = path;
            }
        }
        CoTaskMemFree(wPath);
    }
#endif
    const FXchar *passwd = reg.readStringEntry("settings", "password", NULL);
    if (passwd) {
        settings.password.assign(passwd);
        settings.faction_id = reg.readStringEntry("settings", "faction", "");
    }
    FXint x = reg.readUnsignedEntry("WINDOW", "XPOS", 100);
	FXint y = reg.readUnsignedEntry("WINDOW", "YPOS", 100);
	FXint w = reg.readUnsignedEntry("WINDOW", "WIDTH", 800);
	FXint h = reg.readUnsignedEntry("WINDOW", "HEIGHT", 600);

	if (x < 0) x = 0;
	if (y < 21) y = 21;
	if (w < 10) w = 10;
	if (h < 10) h = 10;

	position(x, y, w, h);

	// reload layout information
	FXint regionsWidth = reg.readUnsignedEntry("WINDOW", "REGIONS_WIDTH", 200);
	FXint mapWidth = reg.readUnsignedEntry("WINDOW", "MAP_WIDTH", 400);
	FXint infoWidth = reg.readUnsignedEntry("WINDOW", "INFO_WIDTH", 200);
	FXint msgHeight = reg.readUnsignedEntry("WINDOW", "MESSAGES_HEIGHT", 100);
	FXint cmdHeight = reg.readUnsignedEntry("WINDOW", "COMMANDS_HEIGHT", 100);

	FXint maximized = reg.readUnsignedEntry("WINDOW", "MAXIMIZED", 0);

	leftframe->setWidth(regionsWidth);
	middle->setWidth(mapWidth);
	rightframe->setWidth(infoWidth);

	msgBorder->setHeight(msgHeight);
	commandframe->setHeight(cmdHeight);

	// reload InfoDlg/SearchDlg window size
	searchdlg->loadState(reg);
	infodlg->loadState(reg);
	statistics->loadState(reg);

	// reload menu options
	FXint show_toolbar = reg.readUnsignedEntry("SHOW", "TOOLBAR", 1);
	if (!show_toolbar)
		toolbar->handle(this, FXSEL(SEL_COMMAND, FXToolBar::ID_TOGGLESHOWN), NULL);

	FXint show_msg = reg.readUnsignedEntry("SHOW", "MESSAGES", 1);
	if (!show_msg)
		handle(this, FXSEL(SEL_COMMAND, ID_VIEW_MESSAGES), NULL);

	FXint show_calc = reg.readUnsignedEntry("SHOW", "CALCULATOR", 1);
	if (!show_calc)
		mathbar->handle(this, FXSEL(SEL_COMMAND, FXCalculator::ID_TOGGLESHOWN), NULL);

	FXint show_streets = reg.readUnsignedEntry("SHOW", "STREETS", 1);
	if (show_streets)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESTREETS), NULL);

	FXint show_visibility = reg.readUnsignedEntry("SHOW", "VISIBILITYSYMBOL", 1);
	if (show_visibility)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLEVISIBILITYSYMBOL), NULL);

	FXint show_shiptravel = reg.readUnsignedEntry("SHOW", "SHIPTRAVEL", 1);
	if (show_shiptravel)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESHIPTRAVEL), NULL);

	FXint show_shadowRegions = reg.readUnsignedEntry("SHOW", "SHADOWREGIONS", 1);
	if (show_shadowRegions)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESHADOWREGIONS), NULL);

	FXint show_regdescription = reg.readUnsignedEntry("SHOW", "REGDESCRIPTION", 1);
	if (show_regdescription)
		regioninfos->handle(this, FXSEL(SEL_COMMAND, FXRegionInfos::ID_TOGGLEDESCRIPTION), NULL);

	FXint show_ownFactionGroup = reg.readUnsignedEntry("SHOW", "OWNFACTIONGROUP", 1);
	if (show_ownFactionGroup)
		regions->handle(this, FXSEL(SEL_COMMAND, FXRegionList::ID_TOGGLEOWNFACTIONGROUP), NULL);

    FXint colorize_units = reg.readUnsignedEntry("SHOW", "COLORIZEUNITS", 1);
    if (colorize_units)
        regions->handle(this, FXSEL(SEL_COMMAND, FXRegionList::ID_TOGGLEUNITCOLORS), NULL);

    FXint coll_regioninfos = reg.readUnsignedEntry("TABS", "REGIONINFOS", 0);
	if (coll_regioninfos)
		riTab->collapse(true);

	FXint coll_statsinfos = reg.readUnsignedEntry("TABS", "STATSINFOS", 0);
	if (coll_statsinfos)
		siTab->collapse(true);

	FXint coll_tradeinfos = reg.readUnsignedEntry("TABS", "TRADEINFOS", 0);
	if (coll_tradeinfos)
		tiTab->collapse(true);

	// Make the main window appear
	show(PLACEMENT_DEFAULT);

	if (maximized)
		maximize();
}

/*virtual*/ FXbool CSMap::close(FXbool notify)
{
	// Dateien schliessen, Speicher frei geben
	closeFile();
    mapChange();

    FXRegistry &reg = getApp()->reg();

    if (!settings.echeck_dir.empty()) {
        reg.writeStringEntry("settings", "echeck_dir", settings.echeck_dir.text());
    }
    if (!settings.password.empty()) {
        reg.writeStringEntry("settings", "password", settings.password.text());
        reg.writeStringEntry("settings", "faction", settings.faction_id.text());
    }

	// save configuration
	if (!isMaximized() && !isMinimized())
	{
		FXint x = getX(), y = getY();
		FXint w = getWidth(), h = getHeight();

        reg.writeUnsignedEntry("WINDOW", "XPOS", x);
        reg.writeUnsignedEntry("WINDOW", "YPOS", y);
        reg.writeUnsignedEntry("WINDOW", "WIDTH", w);
        reg.writeUnsignedEntry("WINDOW", "HEIGHT", h);
	}

    reg.writeUnsignedEntry("WINDOW", "MAXIMIZED", isMaximized());

	// save splitter size
	FXint regionsWidth = leftframe->getWidth();
	FXint mapWidth = middle->getWidth();
	FXint infoWidth = rightframe->getWidth();

	FXint msgHeight = msgBorder->getHeight();
	FXint cmdHeight = commandframe->getHeight();

    reg.writeUnsignedEntry("WINDOW", "REGIONS_WIDTH", regionsWidth);
    reg.writeUnsignedEntry("WINDOW", "MAP_WIDTH", mapWidth);
    reg.writeUnsignedEntry("WINDOW", "INFO_WIDTH", infoWidth);

    reg.writeUnsignedEntry("WINDOW", "MESSAGES_HEIGHT", msgHeight);
    reg.writeUnsignedEntry("WINDOW", "COMMANDS_HEIGHT", cmdHeight);

	// save InfoDlg/SearchDlg window size
	searchdlg->saveState(reg);
	infodlg->saveState(reg);
	statistics->saveState(reg);

	// save menu options state
    reg.writeUnsignedEntry("SHOW", "TOOLBAR", menu.toolbar->getCheck());
    reg.writeUnsignedEntry("SHOW", "MESSAGES", menu.messages->getCheck());
    reg.writeUnsignedEntry("SHOW", "CALCULATOR", menu.calc->getCheck());
    reg.writeUnsignedEntry("SHOW", "STREETS", menu.streets->getCheck());
    reg.writeUnsignedEntry("SHOW", "VISIBILITYSYMBOL", menu.visibility->getCheck());
    reg.writeUnsignedEntry("SHOW", "SHIPTRAVEL", menu.shiptravel->getCheck());
    reg.writeUnsignedEntry("SHOW", "SHADOWREGIONS", menu.shadowRegions->getCheck());
    reg.writeUnsignedEntry("SHOW", "COLORIZEUNITS", menu.colorizeUnits->getCheck());
    reg.writeUnsignedEntry("SHOW", "REGDESCRIPTION", menu.regdescription->getCheck());
    reg.writeUnsignedEntry("SHOW", "OWNFACTIONGROUP", menu.ownFactionGroup->getCheck());

	// save ToolBarTab on/off state
    reg.writeUnsignedEntry("TABS", "REGIONINFOS", riTab->isCollapsed());
    reg.writeUnsignedEntry("TABS", "STATSINFOS", siTab->isCollapsed());
    reg.writeUnsignedEntry("TABS", "TRADEINFOS", tiTab->isCollapsed());

	// quit application
	return FXMainWindow::close(notify);
}

void CSMap::mapChange(bool newfile /*= false*/)
{
	// map changed, let selection-function handle this
	datafile::SelectionState state = selection;
    if (!report) {
        state.selected = 0;
        state.map = 0;
    }
	state.map |= state.MAPCHANGED;

	if (newfile)
		state.map |= state.NEWFILE;

	if (!(state.selected & (state.REGION|state.UNKNOWN_REGION)))
	{
		state.selected |= state.UNKNOWN_REGION;
		state.sel_x = 0, state.sel_y = 0, state.sel_plane = 0;
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);

    searchdlg->setMapFile(report);
    minimap->setMapFile(report);
    commands->setMapFile(report);
    unitlist->setMapFile(report);
    tradeinfos->setMapFile(report);
    statsinfos->setMapFile(report);
    regioninfos->setMapFile(report);
    mathbar->setMapFile(report);
    messages->setMapFile(report);
    map->setMapFile(report);
    regions->setMapFile(report);
}

long CSMap::updOpenFile(FXObject *sender, FXSelector, void *)
{
	FXWindow *wnd = (FXWindow *)sender;
    if (!report) {
        wnd->disable();
    }
    else {
        wnd->enable();
    }
	return 1;
}

long CSMap::updActiveFaction(FXObject *sender, FXSelector, void *)
{
    FXWindow *wnd = (FXWindow *)sender;
    haveActiveFaction() ? wnd->enable() : wnd->disable();
    return 1;
}

bool CSMap::haveActiveFaction() const
{
	if (!report)
		return false;

	if (!(selection.map & selection.ACTIVEFACTION))
		return false;

	return true;
}

bool CSMap::loadFile(FXString filename)
{
	if (filename.empty())
		return false;

	// vorherige Dateien schliessen, Speicher frei geben
    if (report) {
        closeFile();
        mapChange();
    }

	report = new datafile();

	FXString app_title = CSMAP_APP_TITLE " - lade " + filename;
	handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &app_title);

	try
	{
		report->load(filename.text());
	}
	catch(const std::runtime_error& err)
	{
        report = nullptr;
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", err.what());
		return false;
	}

	if (report->blocks().empty())
	{
        report = nullptr;
        recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht gelesen werden.\nM\u00f6glicherweise wird das Format nicht unterst\u00fctzt.");
		return false;
	}

	if (report->blocks().front().type() != datablock::TYPE_VERSION)
	{
        report = nullptr;
        recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei hat das falsche Format.");
        return false;
	}

	recentFiles.appendFile(filename);
	mapChange(true);	// new file flag
	return true;
}

bool CSMap::mergeFile(FXString filename)
{
	if (filename.empty())
		return false;

	if (!report) {
		return loadFile(filename);	// normal laden, wenn vorher keine Datei geladen ist.
    }

    // zuerst: Datei normal laden
    datafile new_cr;

	FXString app_title = CSMAP_APP_TITLE " - lade " + filename;
	handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &app_title);

	try
	{
        new_cr.load(filename.text());
	}
	catch(const std::runtime_error& err)
	{
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", (filename + ": " + FXString(err.what())).text());
		return false;
	}

	if (!new_cr.blocks().size())
	{
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s",
		FXString(L"Die Datei konnte nicht gelesen werden.\nM\u00f6glicherweise wird das Format nicht unterst\u00fctzt.").text());
		return false;
	}

	if (new_cr.blocks().front().type() != datablock::TYPE_VERSION)
	{
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei hat das falsche Format.");
        return false;
	}

	// dann: Datei an den aktuellen CR anfuegen (nur Karteninformationen)
	datablock regionblock;
	regionblock.string("REGION");

	for (datablock::itor block = new_cr.blocks().begin(); block != new_cr.blocks().end(); block++)
	{
		// handle only regions
		if (block->type() == datablock::TYPE_REGION)
		{
			FXint x = block->x();
			FXint y = block->y();
			FXint plane = block->info();

			datablock::itor oldr = report->region(x, y, plane);
			if (oldr != report->blocks().end())			// add some info to old cr (island names)
			{
				if (const datakey* islandkey = block->valueKey(datakey::TYPE_ISLAND))
					if (!oldr->valueKey(datakey::TYPE_ISLAND))
					{
						if (!islandkey->isInt())		// add only Vorlage-style islands (easier)
							oldr->data().push_back(*islandkey);
					}
			}
            else //if (oldr == file->blocks().end())	// add region to old cr
			{
                report->blocks().push_back(regionblock);
                datablock& newblock = report->blocks().back();

				newblock.infostr(FXString().format("%d %d %d", x, y, plane));
				newblock.terrain(block->terrain());

				FXString name = block->value(datakey::TYPE_NAME);
				FXString terrain = block->value(datakey::TYPE_TERRAIN);
				FXString island = block->value(datakey::TYPE_ISLAND);
				FXString turn = block->value(datakey::TYPE_TURN);
				FXString id = block->value(datakey::TYPE_ID);

				datakey key;

                if (!name.empty())
				{
					key.key("Name"); key.value(name); newblock.data().push_back(key);
				}
                if (!terrain.empty())
				{
					key.key("Terrain"); key.value(terrain); newblock.data().push_back(key);
				}
                if (!island.empty())
				{
					key.key("Insel"); key.value(island); newblock.data().push_back(key);
				}
                if (!id.empty())
				{
					key.key("id"); key.value(id); newblock.data().push_back(key);
				}

				key.key("Runde");
				if (turn.empty())
					key.value(FXStringVal(report->turn()));
				else
					key.value(turn);
				newblock.data().push_back(key);
			}
		}
	}

    report->createHashTables();
	mapChange();
	return true;
}

bool CSMap::fileExists(const char *filename) {
    struct stat buf;
    return (stat(filename, &buf) == 0);
}

bool CSMap::saveFile(FXString filename, bool merge_commands /*= false*/)
{
	if (filename.empty() || !report)
		return false;

	if (fileExists(filename.text())) {
		FXString text;
		text = filename + FXString(L" existiert bereits.\n\nM\u00f6chten Sie sie ersetzen?");

		FXuint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
        if (MBOX_CLICKED_YES != answ) return false;
	}
    FXint res = report->save(filename.text());	// \u00fcberschreiben, mit Befehlen
    if (res <= 0) {
        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht geschrieben werden.");
        return false;
    }
    recentFiles.appendFile(filename);
    mapChange();
    return true;
}

bool CSMap::loadCommands(const FXString& filename)
{
	if (filename.empty() || !report)
		return false;

	if (!(selection.map & selection.ACTIVEFACTION))
		return false;

	try
	{
		FXint res = report->loadCmds(filename);
		if (res < 0)
		{
			FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht gelesen werden.");
			mapChange(false);
			return false;
		}
	}
	catch(const std::runtime_error& err)
	{
        FXString msg(err.what());
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", msg.text());
		mapChange(false);
		return false;
	}

	mapChange(true);	// new file flag
	return true;
}

bool CSMap::saveCommands(const FXString &filename, bool stripped)
{
	if (filename.empty() || !report)
		return false;

	if (!(selection.map & selection.ACTIVEFACTION))
		return false;

    FXint res = report->saveCmds(filename.text(), settings.password, stripped);	// nicht \u00fcberschreiben
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht geschrieben werden.");
        return false;
	}

    // TODO: why? the map has not changed?
	mapChange();
	return true;
}

extern FXbool csmap_savePNG(FXStream& store, FXCSMap& map, FXImage& image, FXProgressDialog& win);

bool CSMap::exportMapFile(FXString filename, FXint scale, bool show_names, bool show_koords, bool show_islands, int color)
{
    if (filename.empty())
		return false;

	if (!report)
		return false;

    FXCSMap *csmap = new FXCSMap(this);
    csmap->hide();
    csmap->setMapFile(report);
    csmap->create();

	// options
	if (scale == 64)
        csmap->scaleChange(16/64.0f);	// bugfix: scaleChange(1.0); doesn't work without previous scaleing

    csmap->scaleChange(scale/64.0f);
    csmap->setShowNames(show_names);
    csmap->setShowKoords(show_koords);
    csmap->setShowIslands(show_islands);

	if (color == 1)	// white background
        csmap->setBackColor(FXRGB(255, 255, 255));
	else
        csmap->setBackColor(FXRGB(0, 0, 0));

	FXImage image(getApp(), NULL, 0, csmap->getContentWidth(), 500);
	image.create();

	FXFileStream file;
	file.open(filename,FXStreamSave);
	if (file.status() != FXStreamOK)
	{
		return false;
	}

	FXProgressDialog progress(this, "Karte exportieren...", "Erzeuge Abbild der Karte...", PROGRESSDIALOG_NORMAL|PROGRESSDIALOG_CANCEL);
	progress.setIcon(icon);
	progress.create();
	getApp()->refresh();
	progress.show(PLACEMENT_SCREEN);

        csmap_savePNG(file, *csmap, image, progress);
	file.close();

	delete csmap;

    return true;
}

void CSMap::stripReportToMap()
{
	for (datablock::itor block = report->blocks().begin(); block != report->blocks().end(); block++)
	{
		// handle regions
		if (block->type() == datablock::TYPE_REGION)
		{
			block->flags( block->flags() & datablock::FLAG_NONE );		// unset all flags

			// delete keys except ;Name, ;Terrain, ;Insel, ;turn, ;id
			for (datakey::itor key = block->data().begin(); key != block->data().end(); key++)
			{
				if (key->type() == datakey::TYPE_NAME || key->type() == datakey::TYPE_TERRAIN
					|| key->type() == datakey::TYPE_ISLAND || key->type() == datakey::TYPE_TURN
					|| key->type() == datakey::TYPE_ID)
					continue;

				datakey::itor del = key--;
				block->data().erase(del);
			}

		}	// handle VERSION block
		else if (block->type() == datablock::TYPE_VERSION)
		{
			// delete keys except ;Konfiguration, ;Spiel, ;Koordinaten, ;Basis, ;Umlaute
			for (datakey::itor key = block->data().begin(); key != block->data().end(); key++)
			{
				if (key->type() == datakey::TYPE_KONFIGURATION || key->type() == datakey::TYPE_TURN)
					continue;

				if (key->key() == "Spiel" || key->key() == "Koordinaten" || key->key() == "Basis"
					|| key->key() == "Umlaute")
					continue;

				datakey::itor del = key--;
				block->data().erase(del);
			}
		}
		else
		{
			// delete the rest

			datablock::itor del = block--;
            report->blocks().erase(del);
		}

        report->createHashTables();
	}
}

long CSMap::onViewMapOnly(FXObject*, FXSelector, void*)
{
    //sender->handle(this, FXSEL(SEL_COMMAND, (value==scale)?ID_CHECK:ID_UNCHECK), NULL);

	if (leftframe->shown() || rightframe->shown())
	{
		leftframe->hide();
		msgBorder->hide();
		rightframe->hide();
		map->recalc();
	}
	else
	{
		leftframe->show();
		if (messages->shown())
			msgBorder->show();
		rightframe->show();
		map->recalc();
	}

	return 1;
}

long CSMap::updViewMapOnly(FXObject* sender, FXSelector, void*)
{
	bool maponly;

	if (leftframe->shown() || rightframe->shown())
		maponly = false;
	else
		maponly = true;

	sender->handle(this, FXSEL(SEL_COMMAND, maponly?ID_CHECK:ID_UNCHECK), NULL);

	return 1;
}

long CSMap::onViewMessages(FXObject*, FXSelector, void*)
{
	if (messages->shown())
	{
		messages->hide();
		msgBorder->hide();
		msgBorder->recalc();
	}
	else
	{
		messages->show();

		if (leftframe->shown() || rightframe->shown())
		{
			msgBorder->show();
			msgBorder->recalc();
		}
	}

	return 1;
}

long CSMap::updViewMessages(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, messages->shown()?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

long CSMap::onViewMiniMap(FXObject*, FXSelector, void*)
{
	if (minimap_frame->shown())
		minimap_frame->hide();
	else
		minimap_frame->show(PLACEMENT_OWNER);

	return 1;
}

long CSMap::updViewMiniMap(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, minimap_frame->shown()?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

long CSMap::onViewInfoDlg(FXObject*, FXSelector, void*)
{
	if (infodlg->shown())
		infodlg->hide();
	else
		infodlg->show(PLACEMENT_OWNER);

	return 1;
}

long CSMap::updViewInfoDlg(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, infodlg->shown()?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

long CSMap::onViewSearchDlg(FXObject*, FXSelector, void*)
{
	if (searchdlg->shown())
		searchdlg->hide();
	else
		searchdlg->show(PLACEMENT_OWNER);

	return 1;
}

long CSMap::onChangeZoom(FXObject* sender, FXSelector, void*)
{
	FXString txt;
    sender->handle(this, FXSEL(SEL_COMMAND, ID_GETSTRINGVALUE), &txt);

	if (txt.empty())
		return 0;

	FXfloat scale = map->getScale();

	if (txt == FXString(L"Vergr\u00f6\u00dfern"))
	{
		FXfloat jump = 1.2f;

		FXint pixel = FXint(scale*64.0f), altpixel = pixel;

		pixel = FXint(pixel*jump);
		if (pixel == altpixel)
			pixel++;

		scale = pixel/64.0f;
	}
	else if (txt == "Verkleinern")
	{
		FXfloat jump = 1.2f;

		FXint pixel = FXint(scale*64.0f);

		pixel = FXint(pixel/jump);
		scale = pixel/64.0f;
	}
	else
	{
		if (txt[0] == '&')
			scale = 1/64.0f * atoi(txt.after('&').text());
		else
			scale = 1/64.0f * atoi(txt.text());
	}

	map->scaleChange(scale);
	return 1;
}

long CSMap::onUpdateZoom(FXObject* sender, FXSelector, void*)
{
	FXString txt;
    sender->handle(this, FXSEL(SEL_COMMAND, ID_GETSTRINGVALUE), &txt);

	if (txt.empty())
		return 0;

	FXint value;
	if (txt[0] == '&')
		value = atoi(txt.after('&').text());
	else
		value = atoi(txt.text());

	FXint scale = FXint(map->getScale()*64);

	sender->handle(this, FXSEL(SEL_COMMAND, (value==scale)?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

long CSMap::onSetOrigin(FXObject*, FXSelector, void*)
{
	if (!(selection.selected & (selection.REGION|selection.UNKNOWN_REGION)))
	{
		FXMessageBox dlg(this, CSMAP_APP_TITLE, "Es ist keine Region markiert!", 0, MBOX_OK);
		dlg.execute(PLACEMENT_SCREEN);

		return 1;
	}

	FXint orig_x = selection.sel_x;
	FXint orig_y = selection.sel_y;
	FXint orig_plane = selection.sel_plane;

	FXString name;
	if (selection.selected & selection.REGION)
	{
		datablock &region = *selection.region;

		FXString rname = region.value(datakey::TYPE_NAME);
		if (rname.empty())
			rname = region.terrainString();
	}

	if (name.empty())
		name = "Unbekannt";

	FXString txt;
	txt.format("Koordinaten-Ursprung auf die Region %s (%d,%d) setzen?", name.text(), orig_x,orig_y);

	FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_YES_NO);
	FXuint res = dlg.execute(PLACEMENT_SCREEN);

	if (res != MBOX_CLICKED_YES)
		return 1;

	for (datablock::itor block = report->blocks().begin(); block != report->blocks().end(); block++)
	{
		// handle only regions
		if (block->type() != datablock::TYPE_REGION)
			continue;

		// handle only the actually selected plain
		if (block->info() != orig_plane)
			continue;

        block->infostr(txt.format("%d %d %d", block->x()-orig_x, block->y()-orig_y, orig_plane));
	}

    report->createHashTables();

	selection.sel_x = 0, selection.sel_y = 0;	// its the origin now

	mapChange();
	return 1;
}

long CSMap::onMakeMap(FXObject*, FXSelector, void*)
{
	FXString txt = FXString(L"Alle Informationen au\u00dfer reine Karteninformationen\naus aktuellem Report l\u00f6schen?");

	FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_YES_NO);
	FXuint res = dlg.execute(PLACEMENT_SCREEN);

	if (res != MBOX_CLICKED_YES)
		return 1;

    stripReportToMap();

	mapChange();
	return 1;
}

long CSMap::onSetVisiblePlane(FXObject* sender, FXSelector, void* ptr)
{
	if (!report)
		return 0;

	if (map)
		map->handle(sender, FXSEL(SEL_COMMAND, FXCSMap::ID_SETVISIBLEPLANE), ptr);

	if (minimap)
		minimap->handle(sender, FXSEL(SEL_COMMAND, FXCSMap::ID_SETVISIBLEPLANE), ptr);

	return 1;
}

long CSMap::onUpdVisiblePlane(FXObject* sender, FXSelector, void* ptr)
{
	if (!report)
		return 0;

	if (map)
		map->handle(sender, FXSEL(SEL_UPDATE, FXCSMap::ID_SETVISIBLEPLANE), ptr);

	return 1;
}

long CSMap::onErrorSelected(FXObject * sender, FXSelector, void *ptr)
{
    FXList *list = (FXList *)sender;
    FXint item = list->getCurrentItem();
    MessageInfo *info = static_cast<MessageInfo *>(list->getItemData(item));

    if (!report)
        return 0;
    if (info->unit_id) {
        datafile::SelectionState state;
        state.selected = selection.UNIT;
        state.unit = report->unit(info->unit_id);
        if (state.unit != report->end()) {
            handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
        }
    }
    return 1;
}

long CSMap::onMapSelectMarked(FXObject*, FXSelector, void*)
{
	if (!report)
		return 0;

	// if no (existing) region marked, do nothing
	if (!(selection.selected & selection.REGION))
		return 0;

	// select/deselect marked region (toggle selection)
	datafile::SelectionState state;

	state.selected = selection.REGION;
	state.region = selection.region;

	if (selection.selected & selection.MULTIPLE_REGIONS)
		state.regionsSelected = selection.regionsSelected;
	else
		state.regionsSelected.clear();		// should be cleared already

	// already selected? then deselect the region
	std::set<datablock*>::iterator itor = state.regionsSelected.find(&*state.region);
	if (itor == state.regionsSelected.end())
		state.regionsSelected.insert(&*state.region);
	else
		state.regionsSelected.erase(itor);

	if (state.regionsSelected.size())
		state.selected |= state.MULTIPLE_REGIONS;

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onMapMoveMarker(FXObject*, FXSelector sel, void*)
{
	if (!report)
		return 0;

	// move mark cursor
	FXint x = 0, y = 0, plane = 0;

	if (selection.selected & selection.REGION)
	{
		datablock &region = *selection.region;

		x = region.x();
		y = region.y();
		plane = region.info();
	}
	if (selection.selected & selection.UNKNOWN_REGION)
		x = selection.sel_x, y = selection.sel_y, plane = selection.sel_plane;

	if (FXSELID(sel) == ID_MAP_MARKERWEST)
		x--;
	else if (FXSELID(sel) == ID_MAP_MARKEREAST)
		x++;
	else if (FXSELID(sel) == ID_MAP_MARKERNORTHEAST)
		y++;
	else if (FXSELID(sel) == ID_MAP_MARKERSOUTHWEST)
		y--;
	else if (FXSELID(sel) == ID_MAP_MARKERNORTHWEST)
		x--, y++;
	else if (FXSELID(sel) == ID_MAP_MARKERSOUTHEAST)
		x++, y--;

	// set new marked region
	datafile::SelectionState state;

	state.selected = 0;
	state.region = report->region(x, y, plane);

	state.sel_x = x, state.sel_y = y, state.sel_plane = plane;

	if (state.region != report->blocks().end())
		state.selected = state.REGION;
	else
		state.selected = state.UNKNOWN_REGION;

	// dont touch multiregions selected
	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		state.regionsSelected = selection.regionsSelected;
		if (state.regionsSelected.size())
			state.selected |= selection.MULTIPLE_REGIONS;
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onMapSetModus(FXObject* sender, FXSelector, void* ptr)
{
	if (sender && sender->isMemberOf(&FXId::metaClass))
	{
		FXId *item = (FXId*)sender;
		FXuval udata = (FXuval)item->getUserData();

		if (udata >= FXCSMap::MODUS_SETTERRAIN && udata < FXCSMap::MODUS_SETTERRAIN+ data::TERRAIN_LAST)
		{
			terrainSelect->setIcon(icons.terrain[udata-FXCSMap::MODUS_SETTERRAIN]);
			terrainSelect->setUserData((void*)udata);
		}
	}

	map->handle(sender, FXSEL(SEL_COMMAND, FXCSMap::ID_SETMODUS), ptr);
	return 1;
}

long CSMap::onUpdMapSetModus(FXObject* sender, FXSelector, void* ptr)
{
	map->handle(sender, FXSEL(SEL_UPDATE, FXCSMap::ID_SETMODUS), ptr);
	return 1;
}

long CSMap::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *state = (datafile::SelectionState*)ptr;

	// reset state
	selection.selected = 0;

	// file change notification
	if (state->map & selection.MAPCHANGED)
	{
		// notify info dialog of new game type
		FXString name_of_game;
		if (report) {
			name_of_game = report->blocks().front().value("Spiel");
		}
		if (name_of_game.empty())
			name_of_game = "default";

		infodlg->setGame(name_of_game);

		// store flags
		selection.fileChange++;
		selection.map = state->map & ~(selection.ACTIVEFACTION|selection.MAPCHANGED|selection.NEWFILE);

		// delete all planes except default
		planes->clearItems();		// clear planes
		planes->appendItem("Standardebene (0)", NULL, (void*)0);
		planes->setNumVisible(planes->getNumItems());

		FXWindow *nextw, *item = planemenu->getFirst();
		if (item)
			item = item->getNext();
		while (item)
		{
            nextw = item->getNext();
			delete item;
            item = nextw;
		}

		if (report) {
			// get all planes in report
			std::set<int> planeSet;		// what planes are in the report

			datablock::itor end = report->blocks().end();
			for (datablock::itor block = report->blocks().begin(); block != end; block++)
			{
				// handle only regions
				if (block->type() != datablock::TYPE_REGION)
					continue;

				// insert plane into set
				planeSet.insert(block->info());
			}

			for (std::set<int>::iterator plane = planeSet.begin(); plane != planeSet.end(); plane++)
			{
				FXString label;
				FXuval p = (FXuval)*plane;

				if (p == 0)
					continue;
				switch (p) {
				case 1:
					label = "Astralraum";
					break;
				case 1137:
					label = "Arena";
					break;
				case 59034966:
					label = "Eternath";
					break;
				case 2000:
					label = "Weihnachtsinsel";
					break;
				default:
					label.format("Ebene %lu", p);
				}
				planes->appendItem(label, NULL, (void*)p);

				FXMenuRadio *radio = new FXMenuRadio(planemenu,label, this,ID_MAP_VISIBLEPLANE,0);
				radio->setUserData((void*)p);
				radio->create();
			}

			planes->setNumVisible(planes->getNumItems());

			// get info about active faction
			if (report->activefaction() != report->end()) {
				datablock::itor block = report->activefaction();

				// set first faction in file to active faction
				selection.map |= selection.ACTIVEFACTION;
				selection.activefaction = block;

				// write faction statistics into faction menu
				FXString name = block->value(datakey::TYPE_FACTIONNAME);
				FXString id = block->id();

				menu.name->setText(name + " (" + id + ")");

				FXString race_type = block->value(datakey::TYPE_TYPE);
				FXString prefix = block->value("typprefix");
				if (prefix.length() && race_type.length())
				{
					race_type[0] = (char)tolower(race_type[0]);
					race_type = prefix + race_type;
				}
				FXString costs = block->value("Rekrutierungskosten");

				menu.type->setText(race_type + " (Rekruten: " + costs + " Silber)");

				FXString magic = block->value("Magiegebiet");
				if (magic.length())
					magic[0] = (char)toupper(magic[0]);

				menu.magic->setText("Magiegebiet: " + magic);

				FXString email = block->value("email");

				menu.email->setText("eMail: " + email);

				FXString fac_number = block->value("Anzahl Personen");
				FXString fac_heroes = block->value("heroes");
				FXString fac_maxheroes = block->value("max_heroes");

				if (fac_heroes.length() || fac_maxheroes.length())
				{
                    if (!fac_heroes.length()) fac_heroes = "0";
					menu.number->setText(fac_number + " Personen, davon " + fac_heroes + " Helden (max. " + fac_maxheroes + ")");
				}
				else
					menu.number->setText(fac_number + " Personen");

				FXint points = block->valueInt("Punkte");
				FXint average = block->valueInt("Punktedurchschnitt");
				if (points || average)
				{
					float f_points = (float)points;
					float f_average = (float)average;

					FXString percent = FXStringVal(FXint(f_points*100/f_average));

					menu.points->setText("Punkte: " + FXStringVal(points) + " (" + percent + "% von " + FXStringVal(average) + ")");
					menu.points->show();
				}
				else
					menu.points->hide();

				FXint age = block->valueInt("age");
				if (age)
				{
					menu.age->setText("Parteialter: " + FXStringVal(age) + " Runden");
					menu.age->show();
				}
				else
					menu.age->hide();

				// list faction pool
				bool itemsinpool = false;

				while (menu.poolnoitems->getNext())
					delete menu.poolnoitems->getNext();

				++block;
				if (block->type() == datablock::TYPE_ITEMS)
				{
					datakey::list_type itemlist = block->data();

					for (datakey::itor itor = itemlist.begin(); itor != itemlist.end(); itor++)
					{
						FXString label; label.format("%s %s", itor->value().text(), itor->key().text());

						FXMenuCommand* cmd = new FXMenuCommand(menu.factionpool, label);
                        cmd->create();

						itemsinpool = true;
					}
				}

				if (itemsinpool)
					menu.poolnoitems->hide();
				else
					menu.poolnoitems->show();
			}
		}

		// enable/disable FACTION menu
		if (selection.map & selection.ACTIVEFACTION)
		{
			if (!menu.faction->isEnabled())
				menu.faction->enable();
		}
		else
		{
			if (menu.faction->isEnabled())
				menu.faction->disable();
		}

		// automatic map-mode switch on NEWFILE flag
		if (report && (state->map & selection.NEWFILE)) {
			if (selection.map & selection.ACTIVEFACTION)
			{
				// faction report: show regionlist/info
				leftframe->show();
				if (messages->shown())
					msgBorder->show();
				rightframe->show();
				map->recalc();
			}
			else
			{
				// map only report: hide regionlist/info
				leftframe->hide();
				msgBorder->hide();
				rightframe->hide();
				map->recalc();
			}
		}
	}

	// save new selection state
	selection.selChange++;
	selection.selected = state->selected;
	selection.region = state->region;
	selection.faction = state->faction;
	selection.building = state->building;
	selection.ship = state->ship;
	selection.unit = state->unit;

	selection.regionsSelected = state->regionsSelected;

	selection.sel_x = state->sel_x;
	selection.sel_y = state->sel_y;
	selection.sel_plane = state->sel_plane;

	// make sure that a region is always selected (when something in it is selected)
	if (!(selection.selected & selection.REGION))
	{
		if (selection.selected & selection.UNIT)
		{
			// start with selected unit and search containing region
			datablock::itor end = report->blocks().end();	// begin()-- does a wrap-around to end()

			datablock::itor block = selection.unit;
			for (; block != end; block--)
			{
				if (block->type() == datablock::TYPE_REGION)
					break;
			}

			// found region?
			if (block != end)
			{
				selection.region = block;
				selection.selected |= selection.REGION;
			}
		}
	}

	// retrieve x, y, plane of region
	if (selection.selected & selection.REGION)
	{
		selection.selected &= ~selection.UNKNOWN_REGION;

		selection.sel_x = selection.region->x();
		selection.sel_y = selection.region->y();
		selection.sel_plane = selection.region->info();
	}

	// change window title and status bar
	FXString titlestr;

    if (report) {
		// get report file name
		FXString filenames = FXPath::name(report->filename());

		// append command file name, if any
		if (!report->cmdfilename().empty())
		{
			filenames += ", ";
			filenames += FXPath::name(report->cmdfilename());

			// mark modified command file with an asterisk
			if (report->modifiedCmds())
				filenames += "*";
		}
		else if (report->modifiedCmds())
			filenames += ", *";

		// put the filenames in the title
		titlestr += " - [" + filenames + "]";

		// update filename in statusbar
		status_file->setText(filenames);
		status_file->show(), status_lfile->show();
	}
	else
	{
		status_file->hide(), status_lfile->hide();
		status->recalc();
	}

	// update faction name in status bar
	if (selection.map & selection.ACTIVEFACTION)
	{
		// update statusbar
		FXString faction; faction.format("%s (%s)", selection.activefaction->value(datakey::TYPE_FACTIONNAME).text(),
			selection.activefaction->id().text());

		status_faction->setText(faction);
		status_faction->show(), status_lfaction->show();

		status_turn->setText(FXStringVal(report->turn()));
		status_turn->show(), status_lturn->show();
	}
	else
	{
		status_faction->hide(), status_lfaction->hide();
		status_turn->hide(), status_lturn->hide();
		status->recalc();
	}

	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		titlestr.append(FXString().format(" - [%lu Regionen markiert]", selection.regionsSelected.size()));
	}


	if (selection.selected & (selection.REGION|selection.UNKNOWN_REGION))
	{
		FXString label, name, terrain = "Unbekannt";

		if (selection.selected & selection.REGION)
		{
			const datablock& region = *selection.region;

			name = region.value(datakey::TYPE_NAME);
			terrain = region.terrainString();
		}

		if (name.empty())
			label.format(" - %s (%d,%d)", terrain.text(), selection.sel_x,selection.sel_y);
		else
			label.format(" - %s von %s (%d,%d)", terrain.text(), name.text(), selection.sel_x,selection.sel_y);

		titlestr.append(label);
	}

	if (titlestr.empty())
		titlestr = CSMAP_APP_TITLE_VERSION;
	else
		titlestr = CSMAP_APP_TITLE + titlestr;

	handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &titlestr);

	return 1;
}

long CSMap::onQueryMap(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
	return 1;
}

long CSMap::onClipboardRequest(FXObject*, FXSelector, void* ptr)
{
	// somebody wants our clipboard data
	FXEvent *event = (FXEvent*)ptr;

    // Return clipped text as as UTF-8
	if(event->target == utf8Type)
	{
		setDNDData(FROM_CLIPBOARD, event->target, FXString(clipboard));
		return 1;
	}

	if (event->target == stringType || event->target == textType)
	{
/*		FXuchar *data;
		FXuint len = clipboard.length();

#ifdef _WINDOWS
		len++;		// windows needs this to be null-terminated, other OSes don't.
#endif
*/

		// Give the array to the system!
		setDNDData(FROM_CLIPBOARD, event->target, clipboard);
		return 1;
    }

	return 0; //FXWindow::onClipboardRequest(sender, sel, ptr);
}

long CSMap::onClipboardLost(FXObject*, FXSelector, void*)
{
	clipboard.clear();

	return 1;
}

long CSMap::onSetClipboard(FXObject*, FXSelector, void* ptr)
{
	if (hasClipboard())
		releaseClipboard();

if (ptr)
{
    clipboard = utf2iso(static_cast<const char *>(ptr));
    acquireClipboard(&stringType, 1);
}

return 1;
}

long CSMap::onSearchInfo(FXObject *, FXSelector, void *ptr)
{
    if (ptr)
    {
        infodlg->setSearchText(reinterpret_cast<const char *>(ptr));

        if (!infodlg->shown())
            infodlg->show(PLACEMENT_OWNER);
    }

    return 1;
}

long CSMap::onWatchFiles(FXObject *, FXSelector, void *ptr)
{
    if (report) {

        FXString filename = report->cmdfilename();
        if (!filename.empty()) {
            struct stat buf;
            if (stat(filename.text(), &buf) == 0) {
                if (buf.st_mtime > last_save_time) {
                    if (last_save_time != 0) {
                        FXuint res = FXMessageBox::question(this, (FXuint)MBOX_YES_NO, CSMAP_APP_TITLE, "%s was changed on disk. Reload?",
                            filename.text());
                        if (res != MBOX_CLICKED_NO) {
                            loadCommands(filename);
                            last_save_time = buf.st_mtime;
                            getApp()->addTimeout(this, CSMap::ID_WATCH_FILES, 1000, NULL);
                            return 1;
                        }
                    }
                    last_save_time = buf.st_mtime;
                }
            }
        }
    }
    getApp()->addTimeout(this, CSMap::ID_WATCH_FILES, 1000, NULL);
    return 0;
}

long CSMap::onCalculator(FXObject *, FXSelector, void *)
{
    if (!mathbar->shown())
    {
        mathbar->show();
        mathbar->recalc();
    }

    if (!mathbar->hasFocus())
        mathbar->setFocus();

    return 1;
}

long CSMap::onFileOpen(FXObject *, FXSelector, void *r)
{
    FXFileDialog dlg(this, FXString(L"\u00d6ffnen..."));
    dlg.setIcon(icons.open);
    dlg.setDirectory(dialogDirectory);
    dlg.setPatternList(FXString(L"M\u00f6gliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nEressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nAlle Dateien (*)"));
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    dialogDirectory = dlg.getDirectory();
    if (res)
        loadFile(dlg.getFilename());
    return 1;
}

long CSMap::onFileMerge(FXObject *, FXSelector, void *r)
{
    FXFileDialog dlg(this, FXString(L"Karte hinzuf\u00fcgen..."));
    dlg.setIcon(icons.merge);
    dlg.setSelectMode(SELECTFILE_MULTIPLE);
    dlg.setDirectory(dialogDirectory);
    dlg.setPatternList(FXString(L"M\u00f6gliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nEressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nAlle Dateien (*)"));
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    dialogDirectory = dlg.getDirectory();
    if (res)
    {
        FXString *filenames = dlg.getFilenames();
        if (filenames)
            for (int i = 0; filenames[i].length(); i++)
                mergeFile(filenames[i]);
    }
    return 1;
}

char *u_mkstemp(char *buffer) {
#ifdef HAVE_MKSTEMP
    int fd;
    strncpy(buffer, "/tmp/csmapXXXXXX", PATH_MAX);
    if (0 <= (fd = mkstemp(buffer))) {
        close(fd);
        return buffer;
    }
    return NULL;
#else
    return tmpnam(buffer);
#endif
}


long CSMap::onFileCheckCommands(FXObject *, FXSelector, void *)
{
    // save to a temporary file:
    char infile[PATH_MAX];
    char outfile[PATH_MAX];
    FXString cmdline;
#ifdef WIN32
    if (!settings.echeck_dir.empty()) {
        cmdline = "\"" + settings.echeck_dir + "\\echeckw.exe\"";
    }
#else
    if (!settings.echeck_dir.empty()) {
        cmdline = settings.echeck_dir + "/echeck";
    }
    else {
        cmdline = "echeck";
    }
#endif
    if (!cmdline.empty()) {
        if (u_mkstemp(infile) && report->saveCmds(infile, "", true) > 0) {
            if (u_mkstemp(outfile)) {
                // Echeck it:
                cmdline.append(" -w3 -c -Lde -Re2 -O");
                cmdline.append(outfile);
                cmdline.append(" ");
                cmdline.append(infile);
#ifdef WIN32
                STARTUPINFO si;
                PROCESS_INFORMATION pi;

                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));
                if (!CreateProcess(NULL, (LPSTR)cmdline.text(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    throw std::runtime_error("CreateProcess failed");
                }

                // Wait until child process exits.
                WaitForSingleObject(pi.hProcess, INFINITE);

                // Close process and thread handles. 
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
#else
                if (system(cmdline.text()) < 0) {
                    throw std::runtime_error("echeck call failed");
                }
#endif
                errorList->clearItems();
                for (auto error : output) delete error;
                output.clear();
                outputTabs->setCurrent(1);
                
                std::ifstream results;
                results.open(outfile, std::ios::in);
                if (results.is_open()) {
                    std::string str;
                    while (!results.eof()) {
                        std::getline(results, str);
                        if (!str.empty()) {
                            MessageInfo *error = new MessageInfo();
                            if (error) {
                                FXString line, display;
                                line.assign(str.c_str());
                                error->level = FXIntVal(line.section("|", 1));
                                error->unit_id = FXIntVal(line.section("|", 2), 36);
                                output.push_back(error);
                                display = line.section("|", 4);
                                display += ": ";
                                display += line.section("|", 3);
                                errorList->appendItem(display, NULL, error);
                            }
                        }
                    }
                }
                unlink(outfile);
            }
            unlink(infile);
        }
    }
	return 1;
}

long CSMap::onFileSaveAs(FXObject*, FXSelector, void*)
{
	FXFileDialog dlg(this, "Speichern unter...", DLGEX_SAVE);
	dlg.setIcon(icons.save);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(FXString(L"Eressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nM\u00f6gliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
	if (res)
	{
		FXString filename = dlg.getFilename();
		FXString pattern = dlg.getPattern();

		// Pr\u00fcft, ob Dateiname bereits Endung enth\u00e4lt.
		FXString ext = filename.rafter('.');
		for (int i = 0; ; i++)
		{
			FXString patt = pattern.section(',',i);
			if (patt.empty())
			{
                // Der Dateiname endet nicht mit ".cr" o.\u00e4., deshalb wird Endung angehangen.
				ext = pattern.section(',',0).after('.');
				if (!ext.empty())
					filename += "." + ext;
				break;
			}

			// Dateiname endet auf ".cr" o.\u00e4.
            if (ext == patt.after('.'))
				break;
		}

		saveFile(filename);
        return 1;
    }
	return 0;
}

long CSMap::onFileClose(FXObject*, FXSelector, void*)
{
	closeFile();
    mapChange();
    return 1;
}

void CSMap::closeFile()
{
	// ask if modified commands should be safed
	if (report && report->modifiedCmds())
	{
		FXuint res = FXMessageBox::question(this, (FXuint)MBOX_SAVE_CANCEL_DONTSAVE, CSMAP_APP_TITLE, "%s",
            FXString(L"Die \u00c4nderungen an den Befehlen speichern?").text());

		if (res == MBOX_CLICKED_CANCEL)
			return;					// don't close, cancel clicked
		else if (res == MBOX_CLICKED_SAVE)
		{
            saveCommandsDlg(false, true);			// save commands

			// cancel close, when save was unsuccessful
			if (report && report->modifiedCmds())
				return;
		}
	}
    delete report;
    report = nullptr;
}

long CSMap::onFileOpenCommands(FXObject *, FXSelector, void *)
{
	FXFileDialog dlg(this, FXString(L"Befehle laden..."));
	dlg.setIcon(icons.open);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(FXString(L"Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nM\u00f6gliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
    if (res) {
        loadCommands(dlg.getFilename());
        return 1;
    }
    return 0;
}

long CSMap::onFileSaveCommands(FXObject*, FXSelector, void* ptr)
{
    if (report) {
        FXString filename = report->cmdfilename();
        if (filename.empty()) {
            FXString patterns(L"Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nM\u00f6gliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)");
            filename = askFileName("Befehle speichern unter...", patterns);
            if (filename.empty()) return 0;
            if (fileExists(filename.text())) {
                FXString text;
                text = "Die Datei " + filename + FXString(L" existiert bereits.\n\nM\u00f6chten Sie sie ersetzen?");
                FXuint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
                if (MBOX_CLICKED_YES != answ) return 0;
            }
        }
        saveCommands(filename, false);
        return 1;
    }
    return 0;
}

long CSMap::onFileExportCommands(FXObject*, FXSelector, void* ptr)
{
    saveCommandsDlg(true, false);
    return 1;
}

struct memory {
    char *response;
    size_t size;
};

static size_t write_data(void *data, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userp;

    char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL)
        return 0;  /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

long CSMap::onFileUploadCommands(FXObject*, FXSelector, void* ptr)
{
    char infile[PATH_MAX];
    memory response = { 0 };
    if (!report)
        return 0;

    FXString id = report->activefaction()->id();
    FXString passwd = askPasswordDlg(id);
    if (u_mkstemp(infile) && report->saveCmds(infile, passwd, true) > 0) {
        CURL *ch;
        CURLcode success;
        ch = curl_easy_init();
        if (ch) {
            long code = 0;
            curl_mime *form;
            form = curl_mime_init(ch);
            if (form) {
                curl_mimepart *field;
                field = curl_mime_addpart(form);
                curl_mime_name(field, "input");
                curl_mime_filedata(field, infile);
                field = curl_mime_addpart(form);
                curl_mime_name(field, "submit");
                curl_mime_data(field, "submit", CURL_ZERO_TERMINATED);

                curl_easy_setopt(ch, CURLOPT_URL, "https://www.eressea.kn-bremen.de/eressea/orders-php/upload.php");
                curl_easy_setopt(ch, CURLOPT_WRITEDATA, &response);
                curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
                curl_easy_setopt(ch, CURLOPT_USERNAME, id.text());
                curl_easy_setopt(ch, CURLOPT_PASSWORD, passwd.text());
                curl_easy_setopt(ch, CURLOPT_MIMEPOST, form);
                success = curl_easy_perform(ch);
                if (success == CURLE_OK) {
                    curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &code);
                    if (code == 401) {
                        settings.password.clear();
                        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Fehler %ld: Falsches Passwort.", code);
                    }
                    else if (code >= 500) {
                        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Serverfehler %ld: Bitte sp\u00e4ter noch einmal versuchen.", code);
                    }
                    else if (code < 200 || code >= 300) {
                        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Fehler %ld: Befehle nicht akzeptiert.", code);
                    }
                    else {
                        FXString msg(response.response, response.size);
                        FXMessageBox::information(this, MBOX_OK, CSMAP_APP_TITLE, "%s", msg.text());
                    }
                    free(response.response);
                }
                curl_mime_free(form);
            }
            curl_easy_cleanup(ch);
        }
        unlink(infile);
        return 1;
    }
    return 0;
}

FXString CSMap::askPasswordDlg(const FXString &faction_id) {
    FXString passwd = settings.password;
    if (passwd.empty() || faction_id != settings.faction_id)
    {
        FXInputDialog dlg(this, "Passwort eingeben",
            FXString(L"Geben Sie das Passwort f\u00fcr die Partei " + faction_id + " ein:"),
            NULL, INPUTDIALOG_STRING | INPUTDIALOG_PASSWORD);
        FXint res = dlg.execute(PLACEMENT_SCREEN);
        if (res)
        {
            passwd = dlg.getText();
            if (!passwd.empty()) {
                settings.faction_id = faction_id;
                settings.password = passwd;
            }
        }
    }
    return passwd;
}

FXString CSMap::askFileName(const FXString &dlgTitle, const FXString &patterns) {
    FXFileDialog dlg(this, dlgTitle, DLGEX_SAVE);
    dlg.setIcon(icons.save);
    dlg.setDirectory(dialogDirectory);
    dlg.setPatternList(patterns);
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    dialogDirectory = dlg.getDirectory();
    FXString filename = dlg.getFilename();
    if (res) {
        FXString pattern = dlg.getPattern();

        // Pr\u00fcft, ob Dateiname bereits Endung enth\u00e4lt.
        FXString ext = filename.rafter('.');
        for (int i = 0; ; i++)
        {
            FXString patt = pattern.section(',', i);
            if (patt.empty())
            {
                // Der Dateiname endet nicht mit ".cr" o.\u00e4., deshalb wird Endung angehangen.
                ext = pattern.section(',', 0).after('.');
                if (!ext.empty())
                    filename += "." + ext;
                break;
            }

            // Dateiname endet auf ".cr" o.\u00e4.
            if (ext == patt.after('.'))
                break;
        }
    }
    return filename;
}

void CSMap::saveCommandsDlg(bool stripped, bool replace)
{
	if (!report)
		return;

    FXString id = report->activefaction()->id();
    FXString filename = report->cmdfilename();
    FXString passwd = askPasswordDlg(id);

    if (stripped || filename.empty()) {
        filename = askFileName(
            stripped ? "Versandbefehle speichern unter..." : "Befehle speichern unter...",
            FXString(L"Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nM\u00f6gliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)"));
	}
    if (!filename.empty()) {
        if (!replace && fileExists(filename.text())) {
            FXString text;
            text = "Die Datei " + filename + FXString(L" existiert bereits.\n\nM\u00f6chten Sie sie ersetzen?");

            FXuint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
            if (MBOX_CLICKED_YES != answ) return;
        }

        saveCommands(filename, stripped);
    }
}

long CSMap::onFileMapExport(FXObject *, FXSelector, void *)
{
    FXExportDlg exp(this, "Karte exportieren...", icon, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE), 100,100, 400,250);
	FXint res = exp.execute(PLACEMENT_SCREEN);
	if (!res)
		return 0;

    FXint scale = exp.getScale();
	FXint color = exp.getColor();
	bool show_names = exp.getShowNames();
	bool show_koords = exp.getShowKoords();
	bool show_islands = exp.getShowIslands();

	FXFileDialog dlg(this, "Karte exportieren unter...", DLGEX_SAVE);
	dlg.setIcon(icon);
	dlg.setPatternList("PNG Datei (*.png)\nAlle Dateien (*)");
	/*FXint*/ res = dlg.execute(PLACEMENT_SCREEN);
	if (res)
	{
		FXString filename = dlg.getFilename();
		FXString pattern = dlg.getPattern();

		// Pr\u00fcft, ob Dateiname bereits Endung enth\u00e4lt.
		FXString ext = filename.rafter('.');
		for (int i = 0; ; i++)
		{
			FXString patt = pattern.section(',',i);
			if (patt.empty())
			{
                // Der Dateiname endet nicht mit ".cr" o.\u00e4., deshalb wird Endung angehangen.
				ext = pattern.section(',',0).after('.');
				if (!ext.empty())
					filename += "." + ext;
				break;
			}

			// Dateiname endet auf ".cr" o.\u00e4.
            if (ext == patt.after('.'))
				break;
		}

		exportMapFile(filename, scale, show_names, show_koords, show_islands, color);
	}

	return 1;
}

long CSMap::onFileRecent(FXObject*, FXSelector, void* ptr)
{
	const char* filename = (const char*)ptr;

	loadFile(filename);
	return 1;
}

long CSMap::onQuit(FXObject*, FXSelector, void* ptr)
{
	close();
	return 1;
}

long CSMap::onRegionSelAll(FXObject*, FXSelector, void*)
{
	// alle Regionen markieren
	datafile::SelectionState state = selection;
	state.selected |= state.MULTIPLE_REGIONS;

	state.regionsSelected.clear();

	int visiblePlane = map->getVisiblePlane();

	datablock::itor end = report->blocks().end();
	for (datablock::itor block = report->blocks().begin(); block != end; block++)
	{
		// handle only regions
		if (block->type() != datablock::TYPE_REGION)
			continue;

		// mark only visible plane
		if (block->info() != visiblePlane)
			continue;

		state.regionsSelected.insert(&*block);
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionUnSel(FXObject*, FXSelector, void*)
{
	// alle Regionen demarkieren
	datafile::SelectionState state = selection;
	state.selected &= ~state.MULTIPLE_REGIONS;
	state.regionsSelected.clear();
	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionInvertSel(FXObject*, FXSelector, void*)
{
	// Auswahl invertieren
	datafile::SelectionState state = selection;

	if (!(state.selected & state.MULTIPLE_REGIONS))
		state.regionsSelected.clear();

	int visiblePlane = map->getVisiblePlane();

	datablock::itor end = report->blocks().end();

	for (datablock::itor block = report->blocks().begin(); block != end; block++)
	{
		// handle only regions
		if (block->type() != datablock::TYPE_REGION)
			continue;

		// mark only visible plane
		if (block->info() != visiblePlane)
			continue;

		std::set<datablock*>::iterator srch = state.regionsSelected.find(&*block);
		if (srch == state.regionsSelected.end())
			state.regionsSelected.insert(&*block);
		else
			state.regionsSelected.erase(srch);
	}

	if (state.regionsSelected.size())
		state.selected |= state.MULTIPLE_REGIONS;
	else
		state.selected &= ~state.MULTIPLE_REGIONS;

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionExtendSel(FXObject*, FXSelector, void*)
{
	if (!(selection.selected & selection.MULTIPLE_REGIONS))
		return 1;		// nothing to do

	// Auswahl erweitern um eine Region
	datafile::SelectionState state = selection;

	// start without selection
	state.regionsSelected.clear();

	int visiblePlane = map->getVisiblePlane();

	datablock::itor notfound = report->blocks().end();
	for (std::set<datablock*>::iterator itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
	{
		// all previously selected regions are selected here, too
		state.regionsSelected.insert(*itor);

		if ((*itor)->info() != visiblePlane)
		{
			// apply other planes without changes
			continue;
		}

		int x = (*itor)->x();
		int y = (*itor)->y();

		datablock::itor region;

		// each of the six hex directions
		region = report->region(x, y+1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = report->region(x+1, y, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = report->region(x+1, y-1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = report->region(x, y-1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = report->region(x-1, y, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = report->region(x-1, y+1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionSelIslands(FXObject*, FXSelector, void*)
{
	if (!(selection.selected & selection.MULTIPLE_REGIONS))
		return 1;		// nothing to do

	// Inseln ausw\u00e4hlen
	datafile::SelectionState state = selection;

	int visiblePlane = map->getVisiblePlane();

	bool changed;
	do
	{
		changed = false;

		datablock::itor notfound = report->blocks().end();
		for (std::set<datablock*>::iterator itor = state.regionsSelected.begin(); itor != state.regionsSelected.end(); itor++)
		{
			if ((*itor)->info() != visiblePlane)
			{
				// apply other planes without changes
				continue;
			}

			if ((*itor)->terrain() == data::TERRAIN_OCEAN || (*itor)->terrain() == data::TERRAIN_FIREWALL)
			{
				// ignore non-land regions and firewalls
				continue;
			}

			int x = (*itor)->x();
			int y = (*itor)->y();

			datablock::itor region;

			// each of the six hex directions
			region = report->region(x, y+1, visiblePlane);
			if (region != notfound && region->terrain() != data::TERRAIN_OCEAN
				&& region->terrain() != data::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = report->region(x+1, y, visiblePlane);
			if (region != notfound && region->terrain() != data::TERRAIN_OCEAN
				&& region->terrain() != data::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = report->region(x+1, y-1, visiblePlane);
			if (region != notfound && region->terrain() != data::TERRAIN_OCEAN
				&& region->terrain() != data::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = report->region(x, y-1, visiblePlane);
			if (region != notfound && region->terrain() != data::TERRAIN_OCEAN
				&& region->terrain() != data::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = report->region(x-1, y, visiblePlane);
			if (region != notfound && region->terrain() != data::TERRAIN_OCEAN
				&& region->terrain() != data::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = report->region(x-1, y+1, visiblePlane);
			if (region != notfound && region->terrain() != data::TERRAIN_OCEAN
				&& region->terrain() != data::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;
		}

	} while (changed);

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionSelAllIslands(FXObject*, FXSelector, void*)
{
	// alle Landregionen markieren (also au\u00dfer Ozean & Feuerwand & Eisberg)
	datafile::SelectionState state = selection;
	state.selected |= state.MULTIPLE_REGIONS;

	state.regionsSelected.clear();

	int visiblePlane = map->getVisiblePlane();

	datablock::itor end = report->blocks().end();
	for (datablock::itor block = report->blocks().begin(); block != end; block++)
	{
		// handle only regions
		if (block->type() != datablock::TYPE_REGION)
			continue;

		// mark only visible plane
		if (block->info() != visiblePlane)
			continue;

		// skip ocean, firewall and iceberg
		if (block->terrain() == data::TERRAIN_OCEAN || block->terrain() == data::TERRAIN_FIREWALL
			|| block->terrain() == data::TERRAIN_ICEBERG)
			continue;

		state.regionsSelected.insert(&*block);
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionRemoveSel(FXObject*, FXSelector, void*)
{
	// markierte Regionen l\u00f6schen
	if (!(selection.selected & selection.MULTIPLE_REGIONS) || !selection.regionsSelected.size())
	{
		FXString txt = "Keine Region markiert!";

		FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_OK);
		dlg.execute(PLACEMENT_SCREEN);
		return 1;
	}

	FXString txt;
	txt.format(FXString(L"Wirklich die markierten %d Regionen l\u00f6schen?").text(), selection.regionsSelected.size());

	FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_YES_NO);
	FXuint res = dlg.execute(PLACEMENT_SCREEN);
	if (res != MBOX_CLICKED_YES)
		return 1;

	std::set<datablock*>::iterator itor;
	for (itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
	{
		datablock* region = *itor;

		// what could store zero-pointer regions?
		if (!region)
			continue;

		// deselect region if it is deleted here
		if (selection.selected & selection.REGION)
			if (&*selection.region == region)
				selection.selected &= ~selection.REGION;

		// delete this region
		datablock::itor end = report->blocks().end();
		datablock::itor block = report->region(region->x(), region->y(), region->info());
		if (block == end)
			continue;

		// found the region. delete all blocks until next region.
		datablock::itor srch = block;
		for (srch++; srch != end && srch->depth() > block->depth(); srch++)
		{
			// block is in region
		}

        report->blocks().erase(block, srch);
	}

    report->createHashTables();

	// Markierung auch l\u00f6schen
	selection.selected &= ~selection.MULTIPLE_REGIONS;
	selection.regionsSelected.clear();

	datafile::SelectionState state = selection;
	state.selected &= ~state.MULTIPLE_REGIONS;

	state.map |= state.MAPCHANGED;		// Datei wurde ge\u00e4ndert
	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onViewStatsTab(FXObject *, FXSelector, void *)
{
    tabbook->setCurrent(1);
    return 1;
}

long CSMap::onViewUnitTab(FXObject *, FXSelector, void *)
{
    tabbook->setCurrent(0);
    return 1;
}

long CSMap::onHelpAbout(FXObject*, FXSelector, void*)
{
	FXString abouttext = CSMAP_APP_TITLE_VERSION;
	abouttext.append("\n   build: " __DATE__ " " __TIME__);

	abouttext.append("\n\n" CSMAP_APP_COPYRIGHT "\nWeb: " CSMAP_APP_URL);
	abouttext.append("\n\nSpecial thanks to Xolgrim for assembling Eressea rules information.\nThanks to all users for suggesting features and finding bugs.");
	abouttext.append("\n\nThis software uses the FOX Toolkit Library (http://www.fox-toolkit.org).");
	abouttext.append("\nThis software uses the TinyExpr library (https://github.com/codeplea/tinyexpr).");
	abouttext.append("\nThis software is based in part on the work of the Independent JPEG Group.");

	FXMessageBox about(this, "Wer mich schuf...", abouttext, getIcon(), MBOX_OK);

	about.execute(PLACEMENT_SCREEN);

	return 1;
}
