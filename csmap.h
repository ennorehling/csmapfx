#ifndef _CSMAP
#define _CSMAP

#include "terrain.h"
#include "datafile.h"

#include <fx.h>

#include <vector>
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

#ifdef WIN32
    std::vector<FXString> ParseCommandLine();
#else
    std::vector<FXString> ParseCommandLine(int argc, char** argv);
#endif

public:		// this functions are slots for menu commands
    long updOpenFile(FXObject *sender, FXSelector, void *);
	long updActiveFaction(FXObject *sender, FXSelector, void *);

    long onFileOpen(FXObject*, FXSelector, void*);
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
    long onFileCheckCommands(FXObject*, FXSelector, void* ptr);
    long onFileRecent(FXObject*, FXSelector, void* ptr);
    long onFileUploadCommands(FXObject*, FXSelector, void* ptr);

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
	long onRegionSelVisible(FXObject*, FXSelector, void* ptr);
	long onRegionSelAllIslands(FXObject*, FXSelector, void* ptr);
	long onRegionRemoveSel(FXObject*, FXSelector, void* ptr);

	long onHelpAbout(FXObject*, FXSelector, void* ptr);
	long onShowTab(FXObject*, FXSelector, void* ptr);

	long onChangeZoom(FXObject*, FXSelector, void* ptr);
	long onUpdateZoom(FXObject*, FXSelector, void* ptr);

	long onSetOrigin(FXObject*, FXSelector, void* ptr);

    long onModifyCheck(FXObject*, FXSelector, void* ptr);
    long updModifyCheck(FXObject*, FXSelector, void* ptr);

    long onErrorSelected(FXObject*, FXSelector, void* ptr);
    long onResultSelected(FXObject*, FXSelector, void* ptr);

	long onSetVisiblePlane(FXObject*, FXSelector, void* ptr);
	long onUpdVisiblePlane(FXObject*, FXSelector, void* ptr);

	long onMapSelectMarked(FXObject*, FXSelector, void* ptr);
	long onMapMoveMarker(FXObject*, FXSelector, void* ptr);

	long onMapSetModus(FXObject*, FXSelector, void* ptr);
	long onUpdMapSetModus(FXObject*, FXSelector, void* ptr);

	long onMapChange(FXObject*, FXSelector, void* ptr);
	long onQueryMap(FXObject*, FXSelector, void* ptr);
	long onSetSelection(FXObject*, FXSelector, void* ptr);  // ptr = datablock *, update the selection to it.

	long onClipboardRequest(FXObject*, FXSelector, void* ptr);	// somebody wants data from clipboard
	long onClipboardLost(FXObject*, FXSelector, void* ptr);		// we lost ownership of clipboard
	long onSetClipboard(FXObject*, FXSelector, void* ptr);		// a widget want to set clipboard text

	long onSearchInfo(FXObject*, FXSelector, void* ptr);		// a widget wants to open infodlg

    long onWatchFiles(FXObject*, FXSelector, void* ptr);		// timer, watching external files
	
	long onCalculator(FXObject*, FXSelector, void* ptr);

    long onPopupCopyText(FXObject* sender, FXSelector sel, void* ptr);
    long onPopupShowInfo(FXObject* sender, FXSelector sel, void* ptr);
    long onPopupGotoObject(FXObject* sender, FXSelector sel, void* ptr);

    void loadFiles(const std::vector<FXString>& filenames);
    void createPopup(FXMenuPane* menu, FXObject* target, const datablock* block, const FXString& label);

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
        ID_FILE_MODIFY_CHECK,
        ID_FILE_QUIT,
        ID_FILE_UPLOAD_ORDERS,

        // ECheck error list
        ID_ERRROR_SELECTED,
        ID_RESULT_SELECTED,

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
		ID_REGION_SELVISIBLE,
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
        ID_TAB_1,
        ID_TAB_2,
        ID_TAB_3,
        ID_TAB_4,

        // file watch callback
        ID_WATCH_FILES,

        ID_POPUP_GOTO,
        ID_POPUP_SHOW_INFO,
        ID_POPUP_COPY_TEXT,
        
        ID_LAST
	};

private:
    void setClipboard(const char* text);
    void showInfo(const char* text);
    void gotoObject(class datablock* block);

    bool saveReport(const FXString& filename, map_type mode, bool merge_commands = false);
    void mapChange();
    bool haveActiveFaction() const;
    datafile* loadFile(const FXString& filename);
    datafile* mergeFile(const FXString& filename);
    bool closeFile();
    void saveCommandsDlg(bool stripped, bool replace);
    FXString askFileName(const FXString& title, const FXString& patterns);
    FXString askPasswordDlg(const FXString& faction_id);
    bool confirmOverwrite();
    bool loadCommands(const FXString& filename);
    bool updateCommands(const FXString& filename);
    bool saveCommands(const FXString& filename, bool stripped);
    bool checkCommands();
    void updateFileNames();

#ifdef HAVE_PNG
    bool exportMapFile(FXString filename, FXint scale, bool show_text, bool show_koords, bool show_islands, int color);
#endif
#ifdef HAVE_CURL
    long curlUpload();
#endif

    typedef enum class reload_type { RELOAD_NEVER, RELOAD_AUTO, RELOAD_ASK } reload_type;
    reload_type reload_mode = reload_type::RELOAD_NEVER;

    int unlink(const char *pathname);
    FXString askSaveFileName(const FXString& dlgTitle);
    void updateModificationTime();
    void setAutoReload(reload_type mode);
    bool allowReplaceFile(const FXString& filename);

    // Fonts
    class FXFont* fontFixed = nullptr;
    class FXIcon* appIcon = nullptr;

	// Menubar
    class FXMenuBar		*menubar = nullptr;
    class FXMenuPane	*filemenu = nullptr, *recentmenu = nullptr, *viewmenu = nullptr, *regionmenu = nullptr, *factionmenu = nullptr;
    class FXMenuPane	*mapmenu = nullptr, *selectionmenu = nullptr, *planemenu = nullptr, *zoommenu = nullptr, *helpmenu = nullptr;

	// Toolbar
    class FXToolBar		*toolbar = nullptr;
    class FXListBox		*planes = nullptr;
    class FXPopup		*terrainPopup = nullptr;
    class FXButton		*terrainSelect = nullptr;
	
	// Menu: Speichert zuletzt verwendete Dateien
	FXRecentFiles	recentFiles;
	
	// Dialog: Speichert aktuelles Verzeichnis
	FXString		dialogDirectory;

	// Statusbar
    class FXStatusBar	*status = nullptr;
    class FXLabel		*status_lfaction = nullptr, *status_faction = nullptr;
    class FXLabel		*status_lturn = nullptr, *status_turn = nullptr;
    class FXLabel		*status_ldate = nullptr, *status_date = nullptr;
    class FXLabel		*status_lfile = nullptr, *status_file = nullptr;

	// ToolBarTabs
    class FXToolBarTab	*riTab = nullptr, *siTab = nullptr, *tiTab = nullptr;

	// Generelles Layout
    class FXSplitterEx	*content = nullptr;

	// Links
    class FXRegionList* regions = nullptr;
    class FXVerticalFrame *leftframe = nullptr;

	// Mitte
    class FXCSMap* map = nullptr;
    class FXReportInfo* reportInfo = nullptr;
    class FXRegionInfo* regionInfo = nullptr;
    class FXCommands* commands = nullptr;
    class FXCalculator* mathbar = nullptr;
    class FXVerticalFrame	*middle = nullptr;
    class FXVerticalFrame *msgBorder = nullptr;

	// Rechts
    class FXRegionPanel* regionPanel = nullptr;
    class FXStatsPanel* statsPanel = nullptr;
    class FXTradePanel* tradePanel = nullptr;
    class FXVerticalFrame *rightframe = nullptr;
    class FXSplitterEx	*commandsplitter = nullptr;
    class FXTabBook		*tabbook = nullptr;
    class FXTabBook		*outputTabs = nullptr;
    class FXList          *errorList = nullptr;

    class FXVerticalFrame *commandframe = nullptr;

	// Im TabBook
	class FXProperties *unitProperties = nullptr;
	class FXProperties *shipProperties = nullptr;
	class FXProperties *buildingProperties = nullptr;
	class FXStatistics *statistics = nullptr;

	// Floating
    class FXCSMap* minimap = nullptr;
    class FXInfoDlg* infodlg = nullptr;
    class FXSearchDlg* searchdlg = nullptr;
    class FXDialogBox* minimap_frame = nullptr;
    class FXFoldingList* searchResults = nullptr;
    
    time_t last_save_time = 0;

	struct
	{
        class FXMenuCheck *modifycheck = nullptr;	// check if orders are externally modified
        class FXMenuCheck *toolbar = nullptr;		// show toolbar
        class FXMenuCheck *maponly = nullptr;		// show only map
        class FXMenuCheck *minimap = nullptr;		// show minimap
        class FXMenuCheck *infodlg = nullptr;		// show info dialog
        class FXMenuCheck *messages = nullptr;		// show messages window
        class FXMenuCheck *show_left = nullptr;		// show left frame (regions)
        class FXMenuCheck *show_right = nullptr;	// show right frame (properties)
        class FXMenuCheck *calc = nullptr;			// show calculator
        class FXMenuCheck *streets = nullptr;		// show streets on the map
        class FXMenuCheck *visibility = nullptr;	// show symbols for visibility of a region
        class FXMenuCheck *shiptravel = nullptr;	// show a symbol for travelled ships
        class FXMenuCheck *shadowRegions = nullptr;	// paint unseen regions darker
        class FXMenuCheck *colorizeUnits = nullptr;	// colorize units in building/ship
        class FXMenuCheck *islands = nullptr;		// show island names
        class FXMenuCheck *minimap_islands = nullptr;	// show island names on mini map

        class FXMenuCheck *regdescription = nullptr;	// show description of the region

        class FXMenuCheck *ownFactionGroup = nullptr;	// no own faction group in regionlist
        class FXMenuCheck *activeRegionsOnly = nullptr;	// only currently reported regions in regionlist

		// faction menu items
        class FXMenuTitle	*faction = nullptr;

        class FXMenuCommand *name = nullptr, *email = nullptr; // +banner?
        class FXMenuCommand *points = nullptr;
        class FXMenuCommand *age = nullptr;
        class FXMenuCommand *type = nullptr, *number = nullptr;
        class FXMenuCommand *magic = nullptr;

        class FXMenuPane	  *factionpool = nullptr;
        class FXMenuCommand *poolnoitems = nullptr;
	} menu;

	// clipboard data
	FXString clipboard;

    struct MessageInfo {
        FXint unit_id;
        FXint region_x;
        FXint region_y;
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
        class FXIcon *open = nullptr, *merge = nullptr, *save = nullptr, *close = nullptr;
        class FXIcon *info = nullptr;

        class FXIcon *pointer = nullptr, *select = nullptr;
        class FXIcon *terrain[data::TERRAIN_LAST];

	} icons;

    // some data
	class datafile *report = nullptr;
    std::list<MessageInfo *> output;
    datafile::SelectionState selection;			// selected region,faction,unit...

protected: 
	CSMap() {}

    void beginLoading(const FXString& filename);
    FXString gameDate(int turn) const;
    bool isConfirmed(const datablock::itor& unit);
    void showProperties(FXProperties* item, bool visible = true);

};

#endif //_CSMAP
