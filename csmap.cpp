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

#include <stdexcept>

FXDEFMAP(CSMap) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_OPEN,		    CSMap::onFileOpen),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_MERGE,		    CSMap::onFileMerge),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE,		    CSMap::onFileSave),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_AS,		    CSMap::onFileSaveAs),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_CLOSE,		    CSMap::onFileClose),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_MAP,	    CSMap::onFileMapExport),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_LOAD_ORDERS,	    CSMap::onFileOpenCommands),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_ORDERS,	    CSMap::onFileSaveCommands),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_ALL,	    CSMap::onFileSaveWithCmds),
	FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_ORDERS,   CSMap::onFileExportCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_RECENT,		    CSMap::onFileRecent),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_QUIT,		    CSMap::onQuit),

	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_MERGE,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_AS,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_CLOSE,		    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_MAP,	    CSMap::updOpenFile),
	FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_ORDERS,   CSMap::updOpenFile),

    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_LOAD_ORDERS,     CSMap::updActiveFaction),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_ORDERS,     CSMap::updActiveFaction),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_ALL,        CSMap::updActiveFaction),

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
};

FXIMPLEMENT(CSMap,FXMainWindow,MessageMap,ARRAYNUMBER(MessageMap))

static CSMap* CSMap_instance = NULL;

// Construct a MainWindow
CSMap::CSMap(FXApp *app) : FXMainWindow(app, CSMAP_APP_TITLE_VERSION, NULL,NULL, DECOR_ALL, 0,0, 800,600)
{
	// set "singleton"
	CSMap_instance = this;

	// create main window icon
	FXIcon* icon = new FXICOIcon(app, data::csmap);
    setMiniIcon(icon);
	setIcon(icon);

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

	// Menübar, Toolbar
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

	for (int i = 0; i < datablock::TERRAIN_LAST; i++)
		icons.terrain[i] = new FXGIFIcon(getApp(), data::terrainSymbols[i], 0, IMAGE_ALPHAGUESS);

	// Buttons
	new FXButton(toolbar,
		display(L"\tDatei öffnen...\tEine neue Datei öffnen."),
		icons.open,
		this,
		ID_FILE_OPEN, BUTTON_TOOLBAR);
	new FXButton(toolbar,
		display(L"\tDatei hinzufügen...\tLädt einen Karten-Report in den aktuellen Report."),
		icons.merge,
		this,
		ID_FILE_MERGE, BUTTON_TOOLBAR);
	new FXButton(toolbar,
		display(L"\tDatei speichern unter...\tDie aktuelle Datei als neue Datei speichern."),
		icons.save,
		this,
		ID_FILE_SAVE_AS, BUTTON_TOOLBAR);
	new FXButton(toolbar,
		display(L"\tDatei schliessen\tDie aktuelle Datei schliessen."),
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

	for (int i = 1; i < datablock::TERRAIN_LASTPUBLIC+1; i++)
	{
		datablock terrainRegion;
		terrainRegion.terrain(i);
		FXival data = FXCSMap::MODUS_SETTERRAIN + i;

		FXMenuCommand *cmd = new FXMenuCommand(terrainPopup, terrainRegion.terrainString() + "\t\tZeichnen-Modus: Setze Regionen", icons.terrain[i], this,ID_MAP_SETMODUS);
		cmd->setUserData((void*)data);
	}

	new FXMenuSeparatorEx(terrainPopup);
	FXMenuCommand *cmd = new FXMenuCommand(terrainPopup, iso2utf("Löschen\t\tZeichnen-Modus: Lösche Regionen"), icons.terrain[0], this,ID_MAP_SETMODUS);
	cmd->setUserData((void*)(FXCSMap::MODUS_SETTERRAIN));

	FXHorizontalFrame *terrainFrame = new FXHorizontalFrame(toolbar, LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0, 0,0);
	terrainSelect = new FXButton(terrainFrame, iso2utf("\tZeichnen-Modus: Erstelle oder Lösche Regionen"), icons.terrain[0], this,ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
	terrainSelect->setUserData((void*)FXCSMap::MODUS_SETTERRAIN);
	new FXMenuButton(terrainFrame, iso2utf("\tZeichnen-Modus: Erstelle oder Lösche Regionen"), NULL, terrainPopup, FRAME_RAISED|BUTTON_TOOLBAR|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|MENUBUTTON_DOWN, 0,0,12,0);

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
		iso2utf("Ö&ffnen...\tCtrl-O\tEine neue Datei öffnen."),
		NULL,
		this,
		ID_FILE_OPEN);
	new FXMenuCommand(
		filemenu,
		iso2utf("&Hinzufügen...\tCtrl-I\tLädt einen Karten-Report in den aktuellen Report."),
		NULL, this, ID_FILE_MERGE);
	new FXMenuCommand(
		filemenu,
		iso2utf("&Speichern\tCtrl-S\tDie Änderungen speichern."),
		NULL, this, ID_FILE_SAVE);
	new FXMenuCommand(
		filemenu,
		iso2utf("Speichern &unter...\tF12\tDie aktuelle Datei als neue Datei speichern."),
		NULL, this, ID_FILE_SAVE_AS);
	new FXMenuCommand(
		filemenu,
		iso2utf("Sch&liessen\t\tDie aktuelle Datei schliessen."),
		NULL, this, ID_FILE_CLOSE);

	new FXMenuSeparatorEx(filemenu);
	new FXMenuCommand(
        filemenu,
        iso2utf("&Befehle laden...\tCtrl-Shift-O\tDie Befehle aus Textdatei laden."),
        NULL, this, ID_FILE_LOAD_ORDERS);
    new FXMenuCommand(
        filemenu,
        iso2utf("Be&fehle speichern...\tF11\tDie Befehle als Textdatei speichern."),
        NULL, this, ID_FILE_SAVE_ORDERS);
    new FXMenuCommand(
        filemenu,
        display(L"Befehle mit &CR speichern...\t\tSpeichert den aktuellen Report zusammen mit den geänderten Befehlen."),
        NULL, this, ID_FILE_SAVE_ALL);

	new FXMenuSeparatorEx(filemenu);
    new FXMenuCommand(
        filemenu,
        L"Befehle exportieren...\t\tDie Befehle versandfertig exportieren.",
        NULL, this, ID_FILE_EXPORT_ORDERS);
	new FXMenuCommand(
		filemenu,
		L"Karte exportieren...\t\tDie Karte als PNG speichern.",
		NULL, this, ID_FILE_EXPORT_MAP);
	new FXMenuSeparatorEx(filemenu);

	recentmenu = new FXMenuPane(this);
	for (int i = 0; i < 6; i++)
		new FXMenuCommand(recentmenu, "", NULL, &recentFiles,FXRecentFiles::ID_FILE_1+i);

	new FXMenuCascade(filemenu, iso2utf("&Zuletzt geöffnet"), NULL, recentmenu, 0);
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
	menu.minimap = new FXMenuCheck(viewmenu,iso2utf("Übersichts&karte\tCtrl-N\tÜbersichtskarte ein- bzw. ausblenden."), this,ID_VIEW_MINIMAP);
	menu.infodlg = new FXMenuCheck(viewmenu,"&Informationen\tCtrl-B\tRegel-Informationen ein- bzw. ausblenden.", this,ID_VIEW_INFODLG);
	new FXMenuSeparatorEx(viewmenu, "Liste");
	menu.ownFactionGroup = new FXMenuCheck(viewmenu,"&Gruppe aktiver Partei\tAlt-G\tDie Einheiten der eigenen Partei stehen in einer Gruppe.");
	new FXMenuSeparatorEx(viewmenu, "Karte");
	menu.streets = new FXMenuCheck(viewmenu,"&Strassen zeigen\tAlt-S\tStrassen auf der Karte anzeigen.");
	menu.visibility = new FXMenuCheck(viewmenu,iso2utf("&Sichtbarkeit zeigen\tAlt-V\tSymbole für Sichtbarkeit der Regionen anzeigen (Leuchtturm und Durchreise)."));
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
	new FXMenuCommand(regionmenu,iso2utf("&Alle markieren\tCtrl-A\tAlle Regionen auswählen."),NULL,this,ID_REGION_SELALL);
	new FXMenuCommand(regionmenu,iso2utf("Alle &Inseln auswählen\t\tAlle Landregionen auswählen (Ozean, Feuerwand und Eisberg zählen nicht als Land)."),NULL,this,ID_REGION_SELALLISLANDS);
	new FXMenuCommand(regionmenu,iso2utf("&Keine markieren\t\tKeine Region auswählen."),NULL,this,ID_REGION_UNSEL);
	new FXMenuCommand(regionmenu,iso2utf("Auswahl &invertieren\t\tAusgewählte Regionen abwählen und umgekehrt."),NULL,this,ID_REGION_INVERTSEL);
		selectionmenu = new FXMenuPane(this);
		new FXMenuCommand(selectionmenu,"Auswahl &erweitern\tCtrl-Shift-F7\tAuswahl mit dem Radius von einer Region erweitern.",NULL,this,ID_REGION_EXTENDSEL);
		new FXMenuCommand(selectionmenu,iso2utf("&Inseln auswählen\tCtrl-Shift-F9\tAuswahl auf komplette Inseln erweitern."),NULL,this,ID_REGION_SELISLANDS);
	new FXMenuCascade(regionmenu, "&Erweitern", NULL, selectionmenu);
	new FXMenuSeparatorEx(regionmenu, "Bearbeiten");
	new FXMenuCommand(regionmenu,iso2utf("Markierte &löschen\t\t"),NULL,this,ID_REGION_REMOVESEL);

	// Faction menu
	factionmenu = new FXMenuPane(this);
	menu.faction = new FXMenuTitle(menubar,"&Partei",NULL,factionmenu);
	menu.faction->disable();

	new FXMenuSeparatorEx(factionmenu, "Parteiinfo");
	menu.name = new FXMenuCommand(factionmenu, "Parteiname\t\tName der Partei");
	menu.type = new FXMenuCommand(factionmenu, iso2utf("Rasse (Silber)\t\tRasse und Typpräfix der Partei, Rekrutierungskosten"));
	menu.magic = new FXMenuCommand(factionmenu, iso2utf("Magiegebiet\t\tgewähltes Magiegebiet"));
	menu.email = new FXMenuCommand(factionmenu, "eMail\t\teMail-Adresse der Partei");

	new FXMenuSeparatorEx(factionmenu, "Statistik");
	menu.number = new FXMenuCommand(factionmenu, "Personen, Einheiten (und Helden)\t\tAnzahl Personen, Einheiten und Helden");
	menu.points = new FXMenuCommand(factionmenu, "Punkte (Durchschnitt)\t\tPunkte der Partei und Durchschnitt gleichaltriger Parteien");
	menu.age = new FXMenuCommand(factionmenu, "Parteialter\t\tAlter der Partei in Runden");

	new FXMenuSeparatorEx(factionmenu, iso2utf("Gegenstände"));
		menu.factionpool = new FXMenuPane(this);
		menu.poolnoitems = new FXMenuCommand(menu.factionpool,iso2utf("Keine Gegenstände\t\tDer Parteipool enthält keine Gegenstände."),NULL);
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
		new FXMenuCommand(zoommenu,iso2utf("Vergrößern\tCtrl-+\tKarte vergrößern."),NULL,this,ID_MAP_ZOOM,0);
		new FXMenuCommand(zoommenu,"Verkleinern\tCtrl--\tKarte verkleinern.",NULL,this,ID_MAP_ZOOM,0);
		new FXMenuSeparatorEx(zoommenu, iso2utf("Größe"));
		new FXMenuRadio(zoommenu,"&1 Pixel\tCtrl-8\t1.6%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&2 Pixel\tCtrl-7\t3.1%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&4 Pixel\tCtrl-6\t6.3%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&8 Pixel\tCtrl-5\t12.5%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&16 Pixel\tCtrl-4\t25%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&32 Pixel\tCtrl-3\t50%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"&64 Pixel\tCtrl-2\t100%",this,ID_MAP_ZOOM,0);
		new FXMenuRadio(zoommenu,"128 Pixel\tCtrl-1\t200%",this,ID_MAP_ZOOM,0);
	new FXMenuCascade(mapmenu,iso2utf("&Größe"), NULL, zoommenu, 0);
	new FXMenuCommand(mapmenu,"&Ursprung setzen\tCtrl-U\tDen Kartenursprung (0/0) auf die markierte Region setzen.",NULL,this,ID_MAP_SETORIGIN,0);
	new FXMenuCommand(mapmenu,iso2utf("&Kartenreport erzeugen\t\tLöscht alle Informationen außer Regionsnamen und -terrain und Inseln."),NULL,this,ID_MAP_MAKEMAP,0);
	new FXMenuSeparatorEx(mapmenu, "Regionsmarker");
	new FXMenuCommand(mapmenu,iso2utf("Markierte &auswählen\tCtrl-Space\tMarkierte Region auswählen."),NULL,this,ID_MAP_SELECTMARKED,0);
	new FXMenuCommand(mapmenu,"Nach Westen\tCtrl-H\tRegionsmarker eins nach Westen setzen.",NULL,this,ID_MAP_MARKERWEST,0);
	new FXMenuCommand(mapmenu,"Nach Osten\tCtrl-L\tRegionsmarker eins nach Osten setzen.",NULL,this,ID_MAP_MARKEREAST,0);
	new FXMenuCommand(mapmenu,iso2utf("Nach Südwesten\tCtrl-J\tRegionsmarker eins nach Südwesten setzen."),NULL,this,ID_MAP_MARKERSOUTHWEST,0);
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
	regions->mapfiles(&files);

	menu.ownFactionGroup->setTarget(regions);
	menu.ownFactionGroup->setSelector(FXRegionList::ID_TOGGLEOWNFACTIONGROUP);

	// Middle splitter
	middle = new FXVerticalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0,0,0,0,0,0,2);
	content->setStretcher(middle);

	FXSplitterEx *mapsplit = new FXSplitterEx(middle, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	// Map window
	map = new FXCSMap(mapsplit, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	map->mapfiles(&files);
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
	messages = new FXMessages(msgBorder, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	messages->mapfiles(&files);

	// Calculator bar
	mathbar = new FXCalculator(middle, this,ID_SELECTION, LAYOUT_FILL_X);
	mathbar->mapfiles(&files);
	mathbar->connectMap(map);
	menu.calc->setTarget(mathbar);
	menu.calc->setSelector(FXCalculator::ID_TOGGLESHOWN);

	// Right splitter
	rightframe = new FXVerticalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,3,0,0);

	FXHorizontalFrame *riFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	riTab = new FXToolBarTab(riFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	riTab->setTipText("Regionsinformationen ein- und ausblenden");
	regioninfos = new FXRegionInfos(riFrame, this,ID_SELECTION, LAYOUT_FILL_X);
	regioninfos->mapfiles(&files);

	menu.regdescription->setTarget(regioninfos);
	menu.regdescription->setSelector(FXRegionInfos::ID_TOGGLEDESCRIPTION);

	FXHorizontalFrame *siFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	siTab = new FXToolBarTab(siFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	siTab->setTipText("Statistik ein- und ausblenden");
	statsinfos = new FXStatsInfos(siFrame, this,ID_SELECTION, LAYOUT_FILL_X);
	statsinfos->mapfiles(&files);

	FXHorizontalFrame *tiFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	tiTab = new FXToolBarTab(tiFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	tiTab->setTipText("Handelsinformationen ein- und ausblenden");
	tradeinfos = new FXTradeInfos(tiFrame, this,ID_SELECTION, LAYOUT_FILL_X);
	tradeinfos->mapfiles(&files);

	commandsplitter = new FXSplitterEx(rightframe, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	tabbook = new FXTabBook(commandsplitter, NULL,0, TABBOOK_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);

	new FXTabItem(tabbook, "Einheiten");
	unitlist = new FXUnitList(tabbook, this,ID_SELECTION, LAYOUT_FILL_X);
	unitlist->mapfiles(&files);
	new FXTabItem(tabbook, "Statistik");
	statistics = new FXStatistics(tabbook, this,ID_SELECTION, LAYOUT_FILL_X);
	statistics->mapfiles(&files);

	// Befehlseditor
	commandframe = new FXVerticalFrame(commandsplitter,LAYOUT_FILL_X|FRAME_LINE, 0,0,0,0, 0,0,0,0);
	commandframe->setBorderColor(getApp()->getShadowColor());
	commands = new FXCommands(commandframe, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	commands->mapfiles(&files);
	commands->connectMap(map);

	// commands editor tools
	FXHorizontalFrame *cmdBottomFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	FXToolBarTab *cmdTab = new FXToolBarTab(cmdBottomFrame, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	cmdTab->setTipText("Befehlstools ein- und ausblenden");

    FXHorizontalFrame *cmdOptFrame = new FXHorizontalFrame(cmdBottomFrame,LAYOUT_FILL_X|FRAME_LINE, 0,0,0,0, 3,3,1,1);
	cmdOptFrame->setBorderColor(getApp()->getShadowColor());
    FXCheckButton * chk = new FXCheckButton(cmdOptFrame,
        iso2utf("bestätigt\tBefehle bestätigen (Ctrl-Y)\tBefehle für diese Einheit bestätigen"),
        commands, FXCommands::ID_UNIT_CONFIRM, CHECKBUTTON_NORMAL);
    getAccelTable()->addAccel(MKUINT(KEY_y, CONTROLMASK), chk, FXSEL(SEL_COMMAND, ID_ACCEL));

    FXButton * btn;
    btn = new FXButton(cmdOptFrame,
        iso2utf("<\tVorherige Einheit (Ctrl-<)\tZur vorhergehenden unbestätigten Einheit"), NULL,
        commands, FXCommands::ID_UNIT_PREV, BUTTON_TOOLBAR);
    getAccelTable()->addAccel(MKUINT(KEY_less, CONTROLMASK), btn, FXSEL(SEL_COMMAND, ID_ACCEL));
    btn = new FXButton(cmdOptFrame,
        iso2utf(">\tNächste Einheit (Ctrl->)\tZur nächsten unbestätigten Einheit"), NULL,
        commands, FXCommands::ID_UNIT_NEXT, BUTTON_TOOLBAR);
    getAccelTable()->addAccel(MKUINT(KEY_greater, CONTROLMASK), btn, FXSEL(SEL_COMMAND, ID_ACCEL));

    new FXButton(cmdOptFrame,
        "+temp\tNeue Temp-Einheit\tTemp-Einheit erstellen", NULL,
        commands, FXCommands::ID_UNIT_ADD, BUTTON_TOOLBAR);

	FXTextField* rowcol = new FXTextField(cmdOptFrame, 5, commands,FXCommands::ID_ROWCOL, TEXTFIELD_READONLY|JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_RIGHT);
	rowcol->disable();
	rowcol->setDefaultCursor(getDefaultCursor());
	rowcol->setTextColor(getApp()->getShadowColor());

	// minimap
	minimap_frame = new FXDialogBox(this, iso2utf("Übersichtskarte"), DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE), 100,100, 640,480, 0,0,0,0);
	minimap_frame->setIcon(icon);
	minimap_frame->getAccelTable()->addAccel(MKUINT(KEY_N,CONTROLMASK), this,FXSEL(SEL_COMMAND,ID_VIEW_MINIMAP));

	FXStatusBar *minimap_bar = new FXStatusBar(minimap_frame,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER);
	minimap_bar->getStatusLine()->setFrameStyle(FRAME_LINE);
	minimap_bar->getStatusLine()->setBorderColor(getApp()->getShadowColor());
	minimap_bar->getStatusLine()->setNormalText("");

	minimap = new FXCSMap(minimap_frame, this,ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y, true /*minimap-mode*/);
	minimap->mapfiles(&files);
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
	searchdlg->mapfiles(&files);
}

/*static*/ CSMap* CSMap::getInstance()
{
	return CSMap_instance;
}

CSMap::~CSMap()
{
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

	for (int i = 0; i < datablock::TERRAIN_LAST; i++)
		delete icons.terrain[i];
}

// Create and initialize
void CSMap::create()
{
	// Create the windows
	FXMainWindow::create();

	// reload window position & size
	FXint xpos = getApp()->reg().readUnsignedEntry("WINDOW", "XPOS", 100);
	FXint ypos = getApp()->reg().readUnsignedEntry("WINDOW", "YPOS", 100);
	FXint width = getApp()->reg().readUnsignedEntry("WINDOW", "WIDTH", 800);
	FXint height = getApp()->reg().readUnsignedEntry("WINDOW", "HEIGHT", 600);

	if (xpos < 0) xpos = 0;
	if (ypos < 21) ypos = 21;
	if (width < 10) width = 10;
	if (height < 10) height = 10;

	position(xpos,ypos, width,height);

	// reload layout information
	FXint regionsWidth = getApp()->reg().readUnsignedEntry("WINDOW", "REGIONS_WIDTH", 200);
	FXint mapWidth = getApp()->reg().readUnsignedEntry("WINDOW", "MAP_WIDTH", 400);
	FXint infoWidth = getApp()->reg().readUnsignedEntry("WINDOW", "INFO_WIDTH", 200);
	FXint msgHeight = getApp()->reg().readUnsignedEntry("WINDOW", "MESSAGES_HEIGHT", 100);
	FXint cmdHeight = getApp()->reg().readUnsignedEntry("WINDOW", "COMMANDS_HEIGHT", 100);

	FXint maximized = getApp()->reg().readUnsignedEntry("WINDOW", "MAXIMIZED", 0);

	leftframe->setWidth(regionsWidth);
	middle->setWidth(mapWidth);
	rightframe->setWidth(infoWidth);

	msgBorder->setHeight(msgHeight);
	commandframe->setHeight(cmdHeight);

	// reload InfoDlg/SearchDlg window size
	searchdlg->loadState(getApp()->reg());
	infodlg->loadState(getApp()->reg());
	statistics->loadState(getApp()->reg());

	// reload menu options
	FXint show_toolbar = getApp()->reg().readUnsignedEntry("SHOW", "TOOLBAR", 1);
	if (!show_toolbar)
		toolbar->handle(this, FXSEL(SEL_COMMAND, FXToolBar::ID_TOGGLESHOWN), NULL);

	FXint show_msg = getApp()->reg().readUnsignedEntry("SHOW", "MESSAGES", 1);
	if (!show_msg)
		handle(this, FXSEL(SEL_COMMAND, ID_VIEW_MESSAGES), NULL);

	FXint show_calc = getApp()->reg().readUnsignedEntry("SHOW", "CALCULATOR", 1);
	if (!show_calc)
		mathbar->handle(this, FXSEL(SEL_COMMAND, FXCalculator::ID_TOGGLESHOWN), NULL);

	FXint show_streets = getApp()->reg().readUnsignedEntry("SHOW", "STREETS", 1);
	if (show_streets)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESTREETS), NULL);

	FXint show_visibility = getApp()->reg().readUnsignedEntry("SHOW", "VISIBILITYSYMBOL", 1);
	if (show_visibility)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLEVISIBILITYSYMBOL), NULL);

	FXint show_shiptravel = getApp()->reg().readUnsignedEntry("SHOW", "SHIPTRAVEL", 1);
	if (show_shiptravel)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESHIPTRAVEL), NULL);

	FXint show_shadowRegions = getApp()->reg().readUnsignedEntry("SHOW", "SHADOWREGIONS", 1);
	if (show_shadowRegions)
		map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESHADOWREGIONS), NULL);

	FXint show_regdescription = getApp()->reg().readUnsignedEntry("SHOW", "REGDESCRIPTION", 1);
	if (show_regdescription)
		regioninfos->handle(this, FXSEL(SEL_COMMAND, FXRegionInfos::ID_TOGGLEDESCRIPTION), NULL);

	FXint show_ownFactionGroup = getApp()->reg().readUnsignedEntry("SHOW", "OWNFACTIONGROUP", 1);
	if (show_ownFactionGroup)
		regions->handle(this, FXSEL(SEL_COMMAND, FXRegionList::ID_TOGGLEOWNFACTIONGROUP), NULL);

	FXint coll_regioninfos = getApp()->reg().readUnsignedEntry("TABS", "REGIONINFOS", 0);
	if (coll_regioninfos)
		riTab->collapse(true);

	FXint coll_statsinfos = getApp()->reg().readUnsignedEntry("TABS", "STATSINFOS", 0);
	if (coll_statsinfos)
		siTab->collapse(true);

	FXint coll_tradeinfos = getApp()->reg().readUnsignedEntry("TABS", "TRADEINFOS", 0);
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

	if (files.empty())
	{
		// save configuration
		if (!isMaximized() && !isMinimized())
		{
			FXint x = getX(), y = getY();
			FXint w = getWidth(), h = getHeight();

			getApp()->reg().writeUnsignedEntry("WINDOW", "XPOS", x);
			getApp()->reg().writeUnsignedEntry("WINDOW", "YPOS", y);
			getApp()->reg().writeUnsignedEntry("WINDOW", "WIDTH", w);
			getApp()->reg().writeUnsignedEntry("WINDOW", "HEIGHT", h);
		}

		getApp()->reg().writeUnsignedEntry("WINDOW", "MAXIMIZED", isMaximized());

		// save splitter size
		FXint regionsWidth = leftframe->getWidth();
		FXint mapWidth = middle->getWidth();
		FXint infoWidth = rightframe->getWidth();

		FXint msgHeight = msgBorder->getHeight();
		FXint cmdHeight = commandframe->getHeight();

		getApp()->reg().writeUnsignedEntry("WINDOW", "REGIONS_WIDTH", regionsWidth);
		getApp()->reg().writeUnsignedEntry("WINDOW", "MAP_WIDTH", mapWidth);
		getApp()->reg().writeUnsignedEntry("WINDOW", "INFO_WIDTH", infoWidth);

		getApp()->reg().writeUnsignedEntry("WINDOW", "MESSAGES_HEIGHT", msgHeight);
		getApp()->reg().writeUnsignedEntry("WINDOW", "COMMANDS_HEIGHT", cmdHeight);

		// save InfoDlg/SearchDlg window size
		searchdlg->saveState(getApp()->reg());
		infodlg->saveState(getApp()->reg());
		statistics->saveState(getApp()->reg());

		// save menu options state
		getApp()->reg().writeUnsignedEntry("SHOW", "TOOLBAR", menu.toolbar->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "MESSAGES", menu.messages->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "CALCULATOR", menu.calc->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "STREETS", menu.streets->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "VISIBILITYSYMBOL", menu.visibility->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "SHIPTRAVEL", menu.shiptravel->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "SHADOWREGIONS", menu.shadowRegions->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "REGDESCRIPTION", menu.regdescription->getCheck());
		getApp()->reg().writeUnsignedEntry("SHOW", "OWNFACTIONGROUP", menu.ownFactionGroup->getCheck());

		// save ToolBarTab on/off state
		getApp()->reg().writeUnsignedEntry("TABS", "REGIONINFOS", riTab->isCollapsed());
		getApp()->reg().writeUnsignedEntry("TABS", "STATSINFOS", siTab->isCollapsed());
		getApp()->reg().writeUnsignedEntry("TABS", "TRADEINFOS", tiTab->isCollapsed());

		// quit application
		return FXMainWindow::close(notify);
	}

	return FALSE;
}

void CSMap::mapChange(bool newfile /*= false*/)
{
	// map changed, let selection-function handle this
	datafile::SelectionState state = selection;
	if (files.empty())
		state.selected = 0,
		state.map = 0;

	state.map |= state.MAPCHANGED;

	if (newfile)
		state.map |= state.NEWFILE;

	if (!(state.selected & (state.REGION|state.UNKNOWN_REGION)))
	{
		state.selected |= state.UNKNOWN_REGION;
		state.sel_x = 0, state.sel_y = 0, state.sel_plane = 0;
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
}

long CSMap::updOpenFile(FXObject *sender, FXSelector, void *)
{
	FXWindow *wnd = (FXWindow *)sender;
	files.empty() ? wnd->disable() : wnd->enable();
	return 1;
}

long CSMap::updActiveFaction(FXObject *sender, FXSelector, void *)
{
    FXWindow *wnd = (FXWindow *)sender;
    haveActiveFaction() ? wnd->disable() : wnd->enable();
    return 1;
}

bool CSMap::haveActiveFaction() const
{
	if (files.empty())
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
	closeFile();

	if (!files.empty())
		return false;

	files.push_back(datafile());
	datafile &file = files.back();

	FXString title = CSMAP_APP_TITLE " - lade " + filename;
	handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &title);

	try
	{
		file.load(filename.text());
	}
	catch(const std::runtime_error& err)
	{
		files.pop_back();
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", err.what());
		return false;
	}

	if (!file.blocks().size())
	{
		files.pop_back();
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht gelesen werden.\nMöglicherweise wird das Format nicht unterstützt.");
		return false;
	}

	if (file.blocks().front().type() != datablock::TYPE_VERSION)
	{
		files.pop_back();
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

	if (!files.size())
		return loadFile(filename);	// normal laden, wenn vorher keine Datei geladen ist.

	// zuerst: Datei normal laden
	files.push_back(datafile());
	datafile &file = files.back();

	FXString title = CSMAP_APP_TITLE " - lade " + filename;
	handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &title);

	try
	{
		file.load(filename.text());
	}
	catch(const std::runtime_error& err)
	{
		files.pop_back();
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", (filename + ": " + iso2utf(err.what())).text());
		return false;
	}

	if (!file.blocks().size())
	{
		files.pop_back();
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s",
		iso2utf("Die Datei konnte nicht gelesen werden.\nMöglicherweise wird das Format nicht unterstützt.").text());
		return false;
	}

	if (file.blocks().front().type() != datablock::TYPE_VERSION)
	{
		files.pop_back();
		recentFiles.removeFile(filename);
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei hat das falsche Format.");
        return false;
	}

	// dann: Datei an den aktuellen CR anfügen (nur Karteninformationen)
	datafile &cr = files.front();

	datablock regionblock;
	regionblock.string("REGION");

	for (datablock::itor block = file.blocks().begin(); block != file.blocks().end(); block++)
	{
		// handle only regions
		if (block->type() == datablock::TYPE_REGION)
		{
			FXint x = block->x();
			FXint y = block->y();
			FXint plane = block->info();

			datablock::itor oldr = cr.region(x, y, plane);
			if (oldr != cr.blocks().end())			// add some info to old cr (island names)
			{
				if (const datakey* islandkey = block->valueKey(datakey::TYPE_ISLAND))
					if (!oldr->valueKey(datakey::TYPE_ISLAND))
					{
						if (!islandkey->isInt())		// add only Vorlage-style islands (easier)
							oldr->data().push_back(*islandkey);
					}
			}
            else //if (oldr == cr.blocks().end())	// add region to old cr
			{
                cr.blocks().push_back(regionblock);
                datablock& newblock = cr.blocks().back();

				newblock.infostr(FXString().format("%d %d %d", x, y, plane));
				newblock.terrain(block->terrain());

				FXString name = block->value(datakey::TYPE_NAME);
				FXString terrain = block->value(datakey::TYPE_TERRAIN);
				FXString island = block->value(datakey::TYPE_ISLAND);
				FXString turn = block->value(datakey::TYPE_TURN);

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

				key.key("Runde");
				if (turn.empty())
					key.value(FXStringVal(cr.turn()));
				else
					key.value(turn);
				newblock.data().push_back(key);
			}
		}
	}

	// dann: 2. Datei freigeben, da jetzt in 1. Datei integriert
	files.pop_back();
	cr.createHashTables();
	mapChange();
	return true;
}

bool CSMap::saveFile(FXString filename, bool merge_commands /*= false*/)
{
	if (filename.empty())
		return false;

	if (!files.size())
		return false;

	datafile &file = files.front();
	FXint res = file.save(filename.text(), false, true);		// nicht überschreiben, mit Befehlen
	if (res == -2)
	{
		FXString text;
		text = "Die Datei " + filename + iso2utf(" existiert bereits.\n\nMöchten Sie sie ersetzen?");

		FXint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
		if (MBOX_CLICKED_YES == answ)
			res = file.save(filename.text(), true, true);	// überschreiben, mit Befehlen
	}
	if (res == -1)
	{
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht geschrieben werden.");
	}

	if (res > 0)
		recentFiles.appendFile(filename);

	mapChange();
	return true;
}

bool CSMap::loadCommands(FXString filename)
{
	if (filename.empty())
		return false;

	if (files.empty())
		return false;

	if (!(selection.map & selection.ACTIVEFACTION))
		return false;

	datafile &file = files.front();

	try
	{
		FXint res = file.loadCmds(filename.text());
		if (res < 0)
		{
			FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht gelesen werden.");
			mapChange(false);
			return false;
		}
	}
	catch(const std::runtime_error& err)
	{
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", iso2utf(err.what()).text());
		mapChange(false);
		return false;
	}

	//recentFiles.appendFile(filename);
	mapChange(true);	// new file flag
	return true;
}

bool CSMap::saveCommands(FXString filename, bool stripped)
{
	if (filename.empty())
		return false;

	if (files.empty())
		return false;

	if (!(selection.map & selection.ACTIVEFACTION))
		return false;

	datafile &file = files.front();
	FXint res = file.saveCmds(filename.text(), stripped, false);	// nicht überschreiben
	if (res == -2)
	{
		FXString text;
		text = "Die Datei " + filename + iso2utf(" existiert bereits.\n\nMöchten Sie sie ersetzen?");

		FXint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
		if (MBOX_CLICKED_YES == answ)
			res = file.saveCmds(filename.text(), stripped, true);	// überschreiben
	}
	if (res == -1)
	{
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht geschrieben werden.");
	}

	//if (res > 0)
	//	recentFiles.appendFile(filename);

	mapChange();
	return true;
}

extern FXbool csmap_savePNG(FXStream& store, FXCSMap& map, FXImage& image, FXProgressDialog& win);

bool CSMap::exportMapFile(FXString filename, FXint scale, bool show_names, bool show_koords, bool show_islands, int color)
{
    if (filename.empty())
		return false;

	if (!files.size())
		return false;

        FXCSMap *map = new FXCSMap(this);
	map->hide();
	map->mapfiles(&files);
	map->create();

	// options
	if (scale == 64)
		map->scaleChange(16/64.0f);	// bugfix: scaleChange(1.0); doesn't work without previous scaleing

	map->scaleChange(scale/64.0f);
	map->setShowNames(show_names);
	map->setShowKoords(show_koords);
	map->setShowIslands(show_islands);

	if (color == 1)	// white background
		map->setBackColor(FXRGB(255, 255, 255));
	else
		map->setBackColor(FXRGB(0, 0, 0));

	FXint width = map->getContentWidth();

	FXImage image(getApp(), NULL, 0, width, 500);
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

        csmap_savePNG(file, *map, image, progress);
	file.close();

	delete map;

    mapChange();
    return true;
}

void CSMap::stripReportToMap()
{
	for (datafile::itor file = files.begin(); file != files.end(); file++)
	{
		for (datablock::itor block = file->blocks().begin(); block != file->blocks().end(); block++)
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
				file->blocks().erase(del);
			}

		}

		file->createHashTables();
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

	if (txt == iso2utf("Vergrößern"))
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

		FXString name = region.value(datakey::TYPE_NAME);
		if (name.empty())
			name = region.terrainString();
	}

	if (name.empty())
		name = "Unbekannt";

	FXString txt;
	txt.format("Koordinaten-Ursprung auf die Region %s (%d,%d) setzen?", name.text(), orig_x,orig_y);

	FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_YES_NO);
	FXuint res = dlg.execute(PLACEMENT_SCREEN);

	if (res != MBOX_CLICKED_YES)
		return 1;

	for (datafile::itor file = files.begin(); file != files.end(); file++)
	{
		for (datablock::itor block = file->blocks().begin(); block != file->blocks().end(); block++)
		{
			// handle only regions
			if (block->type() != datablock::TYPE_REGION)
				continue;

			// handle only the actually selected plain
			if (block->info() != orig_plane)
				continue;

            block->infostr(txt.format("%d %d %d", block->x()-orig_x, block->y()-orig_y, orig_plane));
		}

		file->createHashTables();
	}

	selection.sel_x = 0, selection.sel_y = 0;	// its the origin now

	mapChange();
	return 1;
}

long CSMap::onMakeMap(FXObject*, FXSelector, void*)
{
	FXString txt = iso2utf("Alle Informationen außer reine Karteninformationen\naus aktuellem Report löschen?");

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
	if (!files.size())
		return 0;

	if (map)
		map->handle(sender, FXSEL(SEL_COMMAND, FXCSMap::ID_SETVISIBLEPLANE), ptr);

	if (minimap)
		minimap->handle(sender, FXSEL(SEL_COMMAND, FXCSMap::ID_SETVISIBLEPLANE), ptr);

	return 1;
}

long CSMap::onUpdVisiblePlane(FXObject* sender, FXSelector, void* ptr)
{
	if (!files.size())
		return 0;

	if (map)
		map->handle(sender, FXSEL(SEL_UPDATE, FXCSMap::ID_SETVISIBLEPLANE), ptr);

	return 1;
}

long CSMap::onMapSelectMarked(FXObject*, FXSelector, void*)
{
	if (files.empty())
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
	if (files.empty())
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
	state.region = files.front().region(x, y, plane);

	state.sel_x = x, state.sel_y = y, state.sel_plane = plane;

	if (state.region != files.front().blocks().end())
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
		FXuval data = (FXuval)item->getUserData();

		if (data >= FXCSMap::MODUS_SETTERRAIN && data < FXCSMap::MODUS_SETTERRAIN+datablock::TERRAIN_LAST)
		{
			terrainSelect->setIcon(icons.terrain[data-FXCSMap::MODUS_SETTERRAIN]);
			terrainSelect->setUserData((void*)data);
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
		if (files.size())
		{
			datafile::itor file = files.begin();

			name_of_game = file->blocks().front().value("Spiel");
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

		FXWindow *next, *item = planemenu->getFirst();
		if (item)
			item = item->getNext();
		while (item)
		{
			next = item->getNext();
			delete item;
            item = next;
		}

		if (files.size())
		{
			// get all planes in report
			std::set<int> planeSet;		// what planes are in the report

			for (datafile::itor file = files.begin(); file != files.end(); file++)
			{
				datablock::itor end = file->blocks().end();
				for (datablock::itor block = file->blocks().begin(); block != end; block++)
				{
					// handle only regions
					if (block->type() != datablock::TYPE_REGION)
						continue;

					// insert plane into set
					planeSet.insert(block->info());
				}
			}

			for (std::set<int>::iterator plane = planeSet.begin(); plane != planeSet.end(); plane++)
			{
				FXString label;
				FXuval item = (FXuval)*plane;

				if (item == 0)
					continue;
				switch (item) {
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
					label.format("Ebene %lu", item);
				}
				planes->appendItem(label, NULL, (void*)item);

				FXMenuRadio *radio = new FXMenuRadio(planemenu,label, this,ID_MAP_VISIBLEPLANE,0);
				radio->setUserData((void*)item);
				radio->create();
			}

			planes->setNumVisible(planes->getNumItems());

			// get info about active faction
			datafile::itor firstfile = files.begin();
			if (firstfile->activefaction() != firstfile->end())
			{
				datablock::itor block = firstfile->activefaction();

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

						FXMenuCommand* item = new FXMenuCommand(menu.factionpool,label);
						item->create();

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
		if (files.size() && (state->map & selection.NEWFILE))
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
			datablock::itor end = files.front().blocks().end();	// begin()-- does a wrap-around to end()

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
	FXString title;

    if (files.size())
	{
		// get report file name
		FXString filenames = FXPath::name(files.begin()->filename());

		// append command file name, if any
		if (!files.begin()->cmdfilename().empty())
		{
			filenames += ", ";
			filenames += FXPath::name(files.begin()->cmdfilename());

			// mark modified command file with an asterisk
			if (files.begin()->modifiedCmds())
				filenames += "*";
		}
		else if (files.begin()->modifiedCmds())
			filenames += ", *";

		// put the filenames in the title
		title += " - [" + filenames + "]";

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

		status_turn->setText(FXStringVal(files.front().turn()));
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
		title.append(FXString().format(" - [%lu Regionen markiert]", selection.regionsSelected.size()));
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

		title.append(label);
	}

	if (title.empty())
		title = CSMAP_APP_TITLE_VERSION;
	else
		title = CSMAP_APP_TITLE + title;

	handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &title);

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
		setDNDData(FROM_CLIPBOARD, event->target, iso2utf(clipboard));
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
		clipboard = utf2iso(static_cast<const char*>(ptr));
		acquireClipboard(&stringType, 1);
	}

	return 1;
}

long CSMap::onSearchInfo(FXObject*, FXSelector, void* ptr)
{
	if (ptr)
	{
		infodlg->setSearchText(reinterpret_cast<const char*>(ptr));

		if (!infodlg->shown())
			infodlg->show(PLACEMENT_OWNER);
	}

	return 1;
}

long CSMap::onCalculator(FXObject*, FXSelector, void*)
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

long CSMap::onFileOpen(FXObject*, FXSelector, void*r)
{
	FXFileDialog dlg(this, iso2utf("Öffnen..."));
	dlg.setIcon(icons.open);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(iso2utf("Mögliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nEressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
	if (res)
        loadFile(dlg.getFilename());
	return 1;
}

long CSMap::onFileMerge(FXObject*, FXSelector, void*r)
{
	FXFileDialog dlg(this, iso2utf("Karte hinzufügen..."));
	dlg.setIcon(icons.merge);
	dlg.setSelectMode(SELECTFILE_MULTIPLE);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(iso2utf("Mögliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nEressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
	if (res)
	{
		FXString* filenames = dlg.getFilenames();
		if (filenames)
			for (int i = 0; filenames[i].length(); i++)
				mergeFile(filenames[i]);
	}
	return 1;
}

long CSMap::onFileSave(FXObject*, FXSelector, void*)
{
	FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Nicht implementiert.");
	return 1;
}

long CSMap::onFileSaveAs(FXObject*, FXSelector, void*)
{
	FXFileDialog dlg(this, "Speichern unter...", DLGEX_SAVE);
	dlg.setIcon(icons.save);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(iso2utf("Eressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nMögliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
	if (res)
	{
		FXString filename = dlg.getFilename();
		FXString pattern = dlg.getPattern();

		// Prüft, ob Dateiname bereits Endung enthält.
		FXString ext = filename.rafter('.');
		for (int i = 0; ; i++)
		{
			FXString patt = pattern.section(',',i);
			if (patt.empty())
			{
                // Der Dateiname endet nicht mit ".cr" o.ä., deshalb wird Endung angehangen.
				ext = pattern.section(',',0).after('.');
				if (!ext.empty())
					filename += "." + ext;
				break;
			}

			// Dateiname endet auf ".cr" o.ä.
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
	return 1;
}

void CSMap::closeFile()
{
	// ask if modified commands should be safed
	if (!files.empty() && files.front().modifiedCmds())
	{
		FXuint res = FXMessageBox::question(this, MBOX_SAVE_CANCEL_DONTSAVE, CSMAP_APP_TITLE, "%s",
		iso2utf("Die Änderungen an den Befehlen speichern?").text());

		if (res == MBOX_CLICKED_CANCEL)
			return;					// don't close, cancel clicked
		else if (res == MBOX_CLICKED_SAVE)
		{
            saveCommandsDlg(false);			// save commands

			// cancel close, when save was unsuccessful
			if (!files.empty() && files.front().modifiedCmds())
				return;
		}
	}

	files.clear();
	mapChange();
}

long CSMap::onFileOpenCommands(FXObject *, FXSelector, void *)
{
	FXFileDialog dlg(this, iso2utf("Befehle laden..."));
	dlg.setIcon(icons.open);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(iso2utf("Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nMögliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)"));
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
    saveCommandsDlg(false);
    return 1;
}

long CSMap::onFileExportCommands(FXObject*, FXSelector, void* ptr)
{
    saveCommandsDlg(true);
    return 1;
}

void CSMap::saveCommandsDlg(bool stripped)
{
	if (files.empty())
		return;

	if (files.front().password().empty())
	{
		FXInputDialog dlg(this, "Passwort eingeben", iso2utf("Geben Sie das Passwort für die Partei ein:"), NULL,
			INPUTDIALOG_STRING | INPUTDIALOG_PASSWORD);
		FXint res = dlg.execute(PLACEMENT_SCREEN);
		if (res)
		{
			FXString passwd = dlg.getText();
			if (!passwd.empty())
				passwd = "\"" + passwd + "\"";

			files.front().password(passwd);
		}
	}

	FXFileDialog dlg(this, stripped ? "Versandbefehle speichern unter..." : "Befehle speichern unter...", DLGEX_SAVE);
	dlg.setIcon(icons.save);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(iso2utf("Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nMögliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
	if (res)
	{
		FXString filename = dlg.getFilename();
		FXString pattern = dlg.getPattern();

		// Prüft, ob Dateiname bereits Endung enthält.
		FXString ext = filename.rafter('.');
		for (int i = 0; ; i++)
		{
			FXString patt = pattern.section(',',i);
			if (patt.empty())
			{
                // Der Dateiname endet nicht mit ".cr" o.ä., deshalb wird Endung angehangen.
				ext = pattern.section(',',0).after('.');
				if (!ext.empty())
					filename += "." + ext;
				break;
			}

			// Dateiname endet auf ".cr" o.ä.
            if (ext == patt.after('.'))
				break;
		}

		saveCommands(filename, stripped);
	}
}

long CSMap::onFileSaveWithCmds(FXObject *, FXSelector, void *)
{
	FXFileDialog dlg(this, "Befehle mit CR speichern unter...", DLGEX_SAVE);
	dlg.setIcon(icons.save);
	dlg.setDirectory(dialogDirectory);
	dlg.setPatternList(iso2utf("Eressea Computer Report (*.cr)\nEressea CR, bzip2 gepackt (*.cr.bz2)\nXML Computer Report (*.xml)\nMögliche Computerreporte (*.cr,*.cr.bz2,*.xml)\nAlle Dateien (*)"));
	FXint res = dlg.execute(PLACEMENT_SCREEN);
	dialogDirectory = dlg.getDirectory();
	if (res)
	{
		FXString filename = dlg.getFilename();
		FXString pattern = dlg.getPattern();

		// Prüft, ob Dateiname bereits Endung enthält.
		FXString ext = filename.rafter('.');
		for (int i = 0; ; i++)
		{
			FXString patt = pattern.section(',',i);
			if (patt.empty())
			{
                // Der Dateiname endet nicht mit ".cr" o.ä., deshalb wird Endung angehangen.
				ext = pattern.section(',',0).after('.');
				if (!ext.empty())
					filename += "." + ext;
				break;
			}

			// Dateiname endet auf ".cr" o.ä.
            if (ext == patt.after('.'))
				break;
		}

		saveFile(filename, true);	// save with commands
	}
    return 1;
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

		// Prüft, ob Dateiname bereits Endung enthält.
		FXString ext = filename.rafter('.');
		for (int i = 0; ; i++)
		{
			FXString patt = pattern.section(',',i);
			if (patt.empty())
			{
                // Der Dateiname endet nicht mit ".cr" o.ä., deshalb wird Endung angehangen.
				ext = pattern.section(',',0).after('.');
				if (!ext.empty())
					filename += "." + ext;
				break;
			}

			// Dateiname endet auf ".cr" o.ä.
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

	for (datafile::itor file = files.begin(); file != files.end(); file++)
	{
		datablock::itor end = file->blocks().end();
		for (datablock::itor block = file->blocks().begin(); block != end; block++)
		{
			// handle only regions
			if (block->type() != datablock::TYPE_REGION)
				continue;

			// mark only visible plane
			if (block->info() != visiblePlane)
				continue;

			state.regionsSelected.insert(&*block);
		}
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

	for (datafile::itor file = files.begin(); file != files.end(); file++)
	{
		datablock::itor end = file->blocks().end();

		for (datablock::itor block = file->blocks().begin(); block != end; block++)
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

	datablock::itor notfound = files.begin()->blocks().end();
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
		region = files.begin()->region(x, y+1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = files.begin()->region(x+1, y, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = files.begin()->region(x+1, y-1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = files.begin()->region(x, y-1, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = files.begin()->region(x-1, y, visiblePlane);
		if (region != notfound)
			state.regionsSelected.insert(&*region);

		region = files.begin()->region(x-1, y+1, visiblePlane);
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

	// Inseln auswählen
	datafile::SelectionState state = selection;

	int visiblePlane = map->getVisiblePlane();

	bool changed;
	do
	{
		changed = false;

		datablock::itor notfound = files.begin()->blocks().end();
		for (std::set<datablock*>::iterator itor = state.regionsSelected.begin(); itor != state.regionsSelected.end(); itor++)
		{
			if ((*itor)->info() != visiblePlane)
			{
				// apply other planes without changes
				continue;
			}

			if ((*itor)->terrain() == datablock::TERRAIN_OCEAN || (*itor)->terrain() == datablock::TERRAIN_FIREWALL)
			{
				// ignore non-land regions and firewalls
				continue;
			}

			int x = (*itor)->x();
			int y = (*itor)->y();

			datablock::itor region;

			// each of the six hex directions
			region = files.begin()->region(x, y+1, visiblePlane);
			if (region != notfound && region->terrain() != datablock::TERRAIN_OCEAN
				&& region->terrain() != datablock::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = files.begin()->region(x+1, y, visiblePlane);
			if (region != notfound && region->terrain() != datablock::TERRAIN_OCEAN
				&& region->terrain() != datablock::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = files.begin()->region(x+1, y-1, visiblePlane);
			if (region != notfound && region->terrain() != datablock::TERRAIN_OCEAN
				&& region->terrain() != datablock::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = files.begin()->region(x, y-1, visiblePlane);
			if (region != notfound && region->terrain() != datablock::TERRAIN_OCEAN
				&& region->terrain() != datablock::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = files.begin()->region(x-1, y, visiblePlane);
			if (region != notfound && region->terrain() != datablock::TERRAIN_OCEAN
				&& region->terrain() != datablock::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;

			region = files.begin()->region(x-1, y+1, visiblePlane);
			if (region != notfound && region->terrain() != datablock::TERRAIN_OCEAN
				&& region->terrain() != datablock::TERRAIN_FIREWALL)
				if (state.regionsSelected.find(&*region) == state.regionsSelected.end())
					state.regionsSelected.insert(&*region), changed = true;
		}

	} while (changed);

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionSelAllIslands(FXObject*, FXSelector, void*)
{
	// alle Landregionen markieren (also außer Ozean & Feuerwand & Eisberg)
	datafile::SelectionState state = selection;
	state.selected |= state.MULTIPLE_REGIONS;

	state.regionsSelected.clear();

	int visiblePlane = map->getVisiblePlane();

	for (datafile::itor file = files.begin(); file != files.end(); file++)
	{
		datablock::itor end = file->blocks().end();
		for (datablock::itor block = file->blocks().begin(); block != end; block++)
		{
			// handle only regions
			if (block->type() != datablock::TYPE_REGION)
				continue;

			// mark only visible plane
			if (block->info() != visiblePlane)
				continue;

			// skip ocean, firewall and iceberg
			if (block->terrain() == datablock::TERRAIN_OCEAN || block->terrain() == datablock::TERRAIN_FIREWALL
				|| block->terrain() == datablock::TERRAIN_ICEBERG)
				continue;

			state.regionsSelected.insert(&*block);
		}
	}

	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
	return 1;
}

long CSMap::onRegionRemoveSel(FXObject*, FXSelector, void*)
{
	// markierte Regionen löschen
	if (!(selection.selected & selection.MULTIPLE_REGIONS) || !selection.regionsSelected.size())
	{
		FXString txt = "Keine Region markiert!";

		FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_OK);
		dlg.execute(PLACEMENT_SCREEN);
		return 1;
	}

	FXString txt;
	txt.format(iso2utf("Wirklich die markierten %d Regionen löschen?").text(), selection.regionsSelected.size());

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
		for (datafile::itor file = files.begin(); file != files.end(); file++)
		{
			datablock::itor end = file->blocks().end();
			datablock::itor block = file->region(region->x(), region->y(), region->info());
			if (block == end)
				continue;

			// found the region. delete all blocks until next region.
			datablock::itor srch = block;
			for (srch++; srch != end && srch->depth() > block->depth(); srch++)
			{
				// block is in region
			}

			file->blocks().erase(block, srch);
		}
	}

	for (datafile::itor file = files.begin(); file != files.end(); file++)
		file->createHashTables();

	// Markierung auch löschen
	selection.selected &= ~selection.MULTIPLE_REGIONS;
	selection.regionsSelected.clear();

	datafile::SelectionState state = selection;
	state.selected &= ~state.MULTIPLE_REGIONS;

	state.map |= state.MAPCHANGED;		// Datei wurde geändert
	handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
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
