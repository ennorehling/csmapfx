#ifndef _CSMAP
#define _CSMAP

#include "regionlist.h"
#include "regioninfos.h"
#include "statsinfos.h"
#include "tradeinfos.h"
#include "unitlist.h"
#include "statistics.h"
#include "commands.h"
#include "messages.h"
#include "calc.h"
#include "map.h"
#include "terrain.h"

#include "FXSplitterEx.h"

#include <fx.h>

#include <ctime>

class FXInfoDlg;
class FXSearchDlg;

class CSMap : public FXMainWindow
{ 
  // Macro for class hierarchy declarations 
	FXDECLARE(CSMap)

public: 
	CSMap(FXApp* app); 
	~CSMap();
	virtual void create();
	virtual FXbool close(FXbool notify=FALSE);

    static CSMap* getInstance();

    bool saveReport(const FXString& filename, map_type mode, bool merge_commands = false);
    void mapChange(bool newfile = false);
	bool haveActiveFaction() const;
	bool loadFile(const FXString& filename);
    void mergeBlock(datablock::itor& block, const datablock::itor& begin, const datablock::itor& end, block_type parent_type);
	bool mergeFile(const FXString& filename);
    bool closeFile();
    void saveCommandsDlg(bool stripped, bool replace);
    FXString askFileName(const FXString &title, const FXString &patterns);
    FXString askPasswordDlg(const FXString &faction_id);
	bool loadCommands(const FXString& filename);
    bool updateCommands(const FXString &filename);
    bool saveCommands(const FXString &filename, bool stripped);
	bool exportMapFile(FXString filename, FXint scale, bool show_text, bool show_koords, bool show_islands, int color);
    void updateFileNames();

public:		// this functions are slots for menu commands
	long onFileOpen(FXObject*, FXSelector, void*);
	long updOpenFile(FXObject *sender, FXSelector, void *);
	long updActiveFaction(FXObject *sender, FXSelector, void *);

	long onFileMerge(FXObject*, FXSelector, void*);
	long onFileSaveMap(FXObject*, FXSelector, void*);
	long onFileSaveAll(FXObject*, FXSelector, void*);
    long onFileExportMap(FXObject*, FXSelector, void*);
	long onFileExportImage(FXObject*, FXSelector, void*);
    long onFileClose(FXObject*, FXSelector, void*);
    long onFilePreferences(FXObject*, FXSelector, void*);

	long onFileOpenCommands(FXObject*, FXSelector, void* ptr);
	long onFileSaveCommands(FXObject*, FXSelector, void* ptr);
    long onFileExportCommands(FXObject*, FXSelector, void* ptr);
    long onFileUploadCommands(FXObject*, FXSelector, void* ptr);
    long onFileCheckCommands(FXObject*, FXSelector, void* ptr);
    long onFileRecent(FXObject*, FXSelector, void* ptr);

    long onQuit(FXObject*, FXSelector, void* ptr);

	long onViewMapOnly(FXObject*, FXSelector, void* ptr);
	long updViewMapOnly(FXObject*, FXSelector, void* ptr);

	long onViewMessages(FXObject*, FXSelector, void* ptr);
	long updViewMessages(FXObject*, FXSelector, void* ptr);

	long onViewLeftFrame(FXObject*, FXSelector, void* ptr);
	long updViewLeftFrame(FXObject*, FXSelector, void* ptr);

	long onViewRightFrame(FXObject*, FXSelector, void* ptr);
	long updViewRightFrame(FXObject*, FXSelector, void* ptr);

	long onViewMiniMap(FXObject*, FXSelector, void* ptr);
	long updViewMiniMap(FXObject*, FXSelector, void* ptr);

	long onViewInfoDlg(FXObject*, FXSelector, void* ptr);
	long updViewInfoDlg(FXObject*, FXSelector, void* ptr);

	long onViewSearchDlg(FXObject*, FXSelector, void* ptr);

	long onRegionSelAll(FXObject*, FXSelector, void* ptr);
	long onRegionUnSel(FXObject*, FXSelector, void* ptr);
	long onRegionInvertSel(FXObject*, FXSelector, void* ptr);
	long onRegionExtendSel(FXObject*, FXSelector, void* ptr);
	long onRegionSelIslands(FXObject*, FXSelector, void* ptr);
	long onRegionSelAllIslands(FXObject*, FXSelector, void* ptr);
	long onRegionRemoveSel(FXObject*, FXSelector, void* ptr);

	long onHelpAbout(FXObject*, FXSelector, void* ptr);
	long onViewUnitTab(FXObject*, FXSelector, void* ptr);
	long onViewStatsTab(FXObject*, FXSelector, void* ptr);

	long onChangeZoom(FXObject*, FXSelector, void* ptr);
	long onUpdateZoom(FXObject*, FXSelector, void* ptr);

	long onSetOrigin(FXObject*, FXSelector, void* ptr);

    long onErrorSelected(FXObject*, FXSelector, void* ptr);
	
	long onSetVisiblePlane(FXObject*, FXSelector, void* ptr);
	long onUpdVisiblePlane(FXObject*, FXSelector, void* ptr);

	long onMapSelectMarked(FXObject*, FXSelector, void* ptr);
	long onMapMoveMarker(FXObject*, FXSelector, void* ptr);

	long onMapSetModus(FXObject*, FXSelector, void* ptr);
	long onUpdMapSetModus(FXObject*, FXSelector, void* ptr);

	long onMapChange(FXObject*, FXSelector, void* ptr);
	long onQueryMap(FXObject*, FXSelector, void* ptr);

	long onClipboardRequest(FXObject*, FXSelector, void* ptr);	// somebody wants data from clipboard
	long onClipboardLost(FXObject*, FXSelector, void* ptr);		// we lost ownership of clipboard
	long onSetClipboard(FXObject*, FXSelector, void* ptr);		// a widget want to set clipboard text

	long onSearchInfo(FXObject*, FXSelector, void* ptr);		// a widget wants to open infodlg

    long onWatchFiles(FXObject*, FXSelector, void* ptr);		// timer, watching external files
	
	long onCalculator(FXObject*, FXSelector, void* ptr);

public:
	enum
	{
		ID_REGIONS = FXMainWindow::ID_LAST,
		ID_SELECTION,
		ID_CALCULATOR,

		// File menu
		ID_FILE_PREFERENCES,
		ID_FILE_RECENT,
		ID_FILE_OPEN,
		ID_FILE_MERGE,
		ID_FILE_SAVE_ALL,
		ID_FILE_SAVE_MAP,
        ID_FILE_EXPORT_MAP,
        ID_FILE_EXPORT_IMAGE,
        ID_FILE_CLOSE,
        ID_FILE_LOAD_ORDERS,
        ID_FILE_SAVE_ORDERS,
        ID_FILE_CHECK_ORDERS,
        ID_FILE_EXPORT_ORDERS,
        ID_FILE_UPLOAD_ORDERS,
        ID_FILE_QUIT,

        // ECheck error list
        ID_ERRROR_SELECTED,

		// View menu
		ID_VIEW_MAPONLY,
		ID_VIEW_MESSAGES,
        ID_VIEW_REGIONLIST,
        ID_VIEW_PROPERTIES,
		ID_VIEW_MINIMAP,
		ID_VIEW_INFODLG,
		ID_VIEW_SEARCHDLG,

		// Region menu
		ID_REGION_SELALL,
		ID_REGION_UNSEL,
		ID_REGION_INVERTSEL,
		ID_REGION_EXTENDSEL,
		ID_REGION_SELISLANDS,
		ID_REGION_SELALLISLANDS,
		ID_REGION_REMOVESEL,

		// Map menu
		ID_MAP_ZOOM,
		ID_MAP_SETORIGIN,
		ID_MAP_VISIBLEPLANE,

		ID_MAP_SETMODUS,

		ID_MAP_SELECTMARKED,
		ID_MAP_MARKERNORTHWEST,
		ID_MAP_MARKERNORTHEAST,
		ID_MAP_MARKEREAST,
		ID_MAP_MARKERSOUTHEAST,
		ID_MAP_MARKERSOUTHWEST,
		ID_MAP_MARKERWEST,

		// Help menu
		ID_HELP_ABOUT,

        // TabBook accelarators
        ID_TAB_UNIT,
        ID_TAB_STATS,

        // file watch callback
        ID_WATCH_FILES,

		ID_LAST
	};

private: 
    int unlink(const char *pathname);
    FXString askSaveFileName(const FXString& dlgTitle);

	// Menubar
	FXMenuBar		*menubar;
	FXMenuPane		*filemenu, *recentmenu, *viewmenu, *regionmenu, *factionmenu;
	FXMenuPane		*mapmenu, *selectionmenu, *planemenu, *zoommenu, *helpmenu;

	// Toolbar
	FXToolBar		*toolbar;
	FXListBox		*planes;
	FXPopup			*terrainPopup;
	FXButton		*terrainSelect;
	
	// Menu: Speichert zuletzt verwendete Dateien
	FXRecentFiles	recentFiles;
	
	// Dialog: Speichert aktuelles Verzeichnis
	FXString		dialogDirectory;

	// Statusbar
	FXStatusBar		*status;
	FXLabel			*status_lfaction, *status_faction;
	FXLabel			*status_lturn, *status_turn;
	FXLabel			*status_lfile, *status_file;

	// ToolBarTabs
	FXToolBarTab	*riTab, *siTab, *tiTab;

	// Generelles Layout
	FXSplitterEx	*content;

	// Links
	FXVerticalFrame *leftframe;
	FXRegionList	*regions;

	// Mitte
	FXVerticalFrame	*middle;
	FXCSMap			*map;
	FXVerticalFrame *msgBorder;
	FXMessages		*messages;
	FXCalculator	*mathbar;

	// Rechts
	FXVerticalFrame *rightframe;
	FXRegionInfos	*regioninfos;
	FXStatsInfos	*statsinfos;
	FXTradeInfos	*tradeinfos;
	FXSplitterEx	*commandsplitter;
	FXTabBook		*tabbook;
	FXTabBook		*outputTabs;
	FXCommands		*commands;
    FXList          *errorList;

	FXVerticalFrame *commandframe;

	// Im TabBook
	FXUnitList		*unitlist;
	FXStatistics	*statistics;

	// Floating
	FXDialogBox		*minimap_frame;
	FXCSMap			*minimap;
	class FXInfoDlg	*infodlg;
	class FXSearchDlg *searchdlg;
    time_t last_save_time;
    enum class reload_type { RELOAD_NEVER, RELOAD_AUTO, RELOAD_ASK } reload_mode;

	struct
	{
		FXMenuCheck	*toolbar;		// show toolbar
		FXMenuCheck	*maponly;		// show only map
		FXMenuCheck	*minimap;		// show minimap
		FXMenuCheck	*infodlg;		// show info dialog
		FXMenuCheck	*messages;		// show messages window
		FXMenuCheck	*show_left;		// show left frame (regions)
		FXMenuCheck	*show_right;	// show right frame (properties)
		FXMenuCheck	*calc;			// show calculator
		FXMenuCheck	*streets;		// show streets on the map
		FXMenuCheck *visibility;	// show symbols for visibility of a region
		FXMenuCheck *shiptravel;	// show a symbol for travelled ships
		FXMenuCheck *shadowRegions;	// paint unseen regions darker
		FXMenuCheck *colorizeUnits;	// colorize units in building/ship
		FXMenuCheck *islands;		// show island names
		FXMenuCheck *minimap_islands;	// show island names on mini map

		FXMenuCheck *regdescription;	// show description of the region

		FXMenuCheck *ownFactionGroup;	// no own faction group in regionlist

		// faction menu items
		FXMenuTitle	*faction;

		FXMenuCommand *name, *email; // +banner?
		FXMenuCommand *points;
		FXMenuCommand *age;
		FXMenuCommand *type, *number;
		FXMenuCommand *magic;

		FXMenuPane	  *factionpool;
		FXMenuCommand *poolnoitems;
	} menu;

	// clipboard data
	FXString clipboard;

    struct MessageInfo {
        FXint unit_id;
        FXint level;
    };

    struct {
        // password for active faction
        FXString faction_id;
        FXString password;
        FXString echeck_dir;
    } settings;

	// icons for toolbar
	struct
	{
		FXIcon *open, *merge, *save, *close;
		FXIcon *info;

		FXIcon *pointer, *select;
		FXIcon *terrain[data::TERRAIN_LAST];

	} icons;

    // some data
	datafile *report;
    std::list<MessageInfo *> output;
    datafile::SelectionState selection;			// selected region,faction,unit...

protected: 
	CSMap() {}
};

#endif //_CSMAP
