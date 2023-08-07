#define _CRT_SECURE_NO_WARNINGS

#ifdef WIN32
#include <windows.h>
#include <shlobj_core.h>
#endif
#ifdef HAVE_PHYSFS
#include <physfs.h>
#endif
#ifdef HAVE_PNG
#include <png.h>
#endif
#ifdef HAVE_CURL
#include <curl/curl.h>
#endif
#include <sys/stat.h>

#include "platform.h"

#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "csmap.h"
#include "reportinfo.h"
#include "regioninfo.h"
#include "commands.h"
#include "datafile.h"
#include "symbols.h"
#include "regionlist.h"
#include "regionpanel.h"
#include "statspanel.h"
#include "tradepanel.h"
#include "unitlist.h"
#include "ship_props.h"
#include "building_props.h"
#include "statistics.h"
#include "commands.h"
#include "calc.h"
#include "map.h"

#include "exportdlg.h"
#include "infodlg.h"
#include "searchdlg.h"
#include "prefsdlg.h"

#include <png.h>
#include "fxkeys.h"
#include "FXMenuSeparatorEx.h"
#include "FXSplitterEx.h"
#include "FXFileDialogEx.h"
#include <FXPath.h>
#include <FXFont.h>
#include <FXRex.h>
#include <FXICOIcon.h>
#include <FXSocket.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

FXDEFMAP(CSMap) MessageMap[]=
{
    //________Message_Type_____________________ID_______________Message_Handler_______
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST,    SEL_NONE,                   CSMap::onClipboardRequest),
    FXMAPFUNC(SEL_CLIPBOARD_LOST,       SEL_NONE,                   CSMap::onClipboardLost),
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST,    FXWindow::ID_SETSTRINGVALUE,   CSMap::onSetClipboard),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_OPEN,                CSMap::onFileOpen),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_MERGE,               CSMap::onFileMerge),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_LOAD_ORDERS,         CSMap::onFileOpenCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_ORDERS,         CSMap::onFileSaveCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_CLOSE,               CSMap::onFileClose),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_CHECK_ORDERS,        CSMap::onFileCheckCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_ALL,            CSMap::onFileSaveAll),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_SAVE_MAP,            CSMap::onFileSaveMap),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_ORDERS,       CSMap::onFileExportCommands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_RECENT,              CSMap::onFileRecent),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_PREFERENCES,         CSMap::onFilePreferences),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_QUIT,                CSMap::onQuit),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_MAP,          CSMap::onFileExportMap),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_MAP,          CSMap::updOpenFile),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_EXPORT_IMAGE,        CSMap::onFileExportImage),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_IMAGE,        CSMap::updOpenFile),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_UPLOAD_ORDERS,       CSMap::onFileUploadCommands),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_UPLOAD_ORDERS,       CSMap::updOpenFile),

    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_MERGE,               CSMap::updOpenFile),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_ALL,            CSMap::updOpenFile),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_MAP,            CSMap::updOpenFile),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_CLOSE,               CSMap::updOpenFile),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_EXPORT_ORDERS,       CSMap::updOpenFile),

    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_LOAD_ORDERS,         CSMap::updActiveFaction),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_SAVE_ORDERS,         CSMap::updActiveFaction),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_CHECK_ORDERS,        CSMap::updActiveFaction),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_ERRROR_SELECTED,          CSMap::onErrorSelected),
    FXMAPFUNC(SEL_COMMAND,	CSMap::ID_RESULT_SELECTED,		    CSMap::onResultSelected),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_FILE_MODIFY_CHECK,        CSMap::onModifyCheck),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_FILE_MODIFY_CHECK,        CSMap::updModifyCheck),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_MAPONLY,             CSMap::onViewMapOnly),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_MAPONLY,             CSMap::updViewMapOnly),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_MESSAGES,            CSMap::onViewMessages),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_MESSAGES,            CSMap::updViewMessages),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_PROPERTIES,          CSMap::onViewRightFrame),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_PROPERTIES,          CSMap::updViewRightFrame),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_REGIONLIST,          CSMap::onViewLeftFrame),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_REGIONLIST,          CSMap::updViewLeftFrame),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_MINIMAP,             CSMap::onViewMiniMap),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_MINIMAP,             CSMap::updViewMiniMap),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_INFODLG,             CSMap::onViewInfoDlg),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_VIEW_INFODLG,             CSMap::updViewInfoDlg),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_VIEW_SEARCHDLG,           CSMap::onViewSearchDlg),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_UNIT_NEXT,		        CSMap::onNextUnit),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_UNIT_PREV,		        CSMap::onPrevUnit),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELALL,            CSMap::onRegionSelAll),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_UNSEL,             CSMap::onRegionUnSel),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_INVERTSEL,         CSMap::onRegionInvertSel),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_EXTENDSEL,         CSMap::onRegionExtendSel),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELISLANDS,        CSMap::onRegionSelIslands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELALLISLANDS,     CSMap::onRegionSelAllIslands),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_SELVISIBLE,        CSMap::onRegionSelVisible),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_REGION_REMOVESEL,         CSMap::onRegionRemoveSel),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_HELP_ABOUT,               CSMap::onHelpAbout),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_TAB_1,                    CSMap::onShowTab),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_TAB_2,                    CSMap::onShowTab),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_ZOOM,                 CSMap::onChangeZoom),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_MAP_ZOOM,                 CSMap::onUpdateZoom),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_SETORIGIN,            CSMap::onSetOrigin),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_VISIBLEPLANE,         CSMap::onSetVisiblePlane),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_MAP_VISIBLEPLANE,         CSMap::onUpdVisiblePlane),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_SELECTMARKED,         CSMap::onMapSelectMarked),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERNORTHWEST,      CSMap::onMapMoveMarker),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERNORTHEAST,      CSMap::onMapMoveMarker),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKEREAST,           CSMap::onMapMoveMarker),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERSOUTHEAST,      CSMap::onMapMoveMarker),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERSOUTHWEST,      CSMap::onMapMoveMarker),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_MARKERWEST,           CSMap::onMapMoveMarker),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_MAP_SETMODUS,             CSMap::onMapSetModus),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_MAP_SETMODUS,             CSMap::onUpdMapSetModus),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_SETVALUE,                 CSMap::onSetSelection),
    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_UPDATE,                   CSMap::onMapChange),
    FXMAPFUNC(SEL_UPDATE,   CSMap::ID_SELECTION,                CSMap::onQueryMap),

    FXMAPFUNC(SEL_COMMAND,  CSMap::ID_CALCULATOR,               CSMap::onCalculator),

    FXMAPFUNC(SEL_QUERY_HELP, CSMap::ID_SETSTRINGVALUE,         CSMap::onSearchInfo),


    FXMAPFUNC(SEL_COMMAND, CSMap::ID_POPUP_SHOW_INFO,           CSMap::onPopupShowInfo),
    FXMAPFUNC(SEL_COMMAND, CSMap::ID_POPUP_COPY_TEXT,           CSMap::onPopupCopyText),
    FXMAPFUNC(SEL_COMMAND, CSMap::ID_POPUP_COPY_NAME,           CSMap::onPopupCopySpecial),
    FXMAPFUNC(SEL_COMMAND, CSMap::ID_POPUP_COPY_ID,             CSMap::onPopupCopySpecial),
    FXMAPFUNC(SEL_COMMAND, CSMap::ID_POPUP_GOTO,                CSMap::onPopupGotoObject),
    FXMAPFUNC(SEL_TIMEOUT, CSMap::ID_WATCH_FILES,               CSMap::onWatchFiles),

    FXMAPFUNC(SEL_DOUBLECLICKED, 0,    CSMap::onMessageSelected),
};

FXIMPLEMENT(CSMap,FXMainWindow,MessageMap,ARRAYNUMBER(MessageMap))

static CSMap* CSMap_instance = nullptr;

// Construct a MainWindow
CSMap::CSMap(FXApp *app) :
    FXMainWindow(app, CSMAP_APP_TITLE_VERSION, nullptr, nullptr, DECOR_ALL, 0, 0, 800, 600),
    reload_mode(CSMap::reload_type::RELOAD_NEVER),
    last_save_time(0),
    report(nullptr)
{
    fontFixed = new FXFont(app, "courier", 10);

    // set "singleton"
    CSMap_instance = this;

    // create main window icon
    appIcon = new FXICOIcon(app, data::csmap);
    setMiniIcon(appIcon);
    setIcon(appIcon);

    // activate Alt-F4
    getAccelTable()->addAccel(MKUINT(KEY_F4,ALTMASK),this, FXSEL(SEL_SIGNAL, ID_CLOSE));

    // Statusbar
    status = new FXStatusBar(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER);
    status->getStatusLine()->setFrameStyle(FRAME_LINE);
    status->getStatusLine()->setBorderColor(getApp()->getShadowColor());
    status->getStatusLine()->setNormalText("");

    status_file = new FXLabel(status, "-", 0,FRAME_LINE|LAYOUT_RIGHT|LAYOUT_CENTER_Y);
    status_lfile = new FXLabel(status, " Datei:", nullptr,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
    status_file->setBorderColor(getApp()->getShadowColor());
    status_file->hide(); status_lfile->hide();

    status_turn = new FXLabel(status, "-", 0,FRAME_LINE|LAYOUT_RIGHT|LAYOUT_CENTER_Y);
    status_lturn = new FXLabel(status, " Runde:", nullptr,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
    status_turn->setBorderColor(getApp()->getShadowColor());
    status_turn->hide(); status_lturn->hide();

    status_faction = new FXLabel(status, "-", 0,FRAME_LINE|LAYOUT_RIGHT|LAYOUT_CENTER_Y);
    status_lfaction = new FXLabel(status, " Partei:", nullptr,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
    status_faction->setBorderColor(getApp()->getShadowColor());
    status_faction->hide(); status_lfaction->hide();

    status_date = new FXLabel(status, "-", 0, FRAME_LINE | LAYOUT_RIGHT | LAYOUT_CENTER_Y);
    status_ldate = new FXLabel(status, " Datum:", nullptr, LAYOUT_RIGHT | LAYOUT_CENTER_Y);
    status_date->setBorderColor(getApp()->getShadowColor());
    status_date->hide(); status_ldate->hide();

    // Menubar, Toolbar
    FXDockSite* topdock = new FXDockSite(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
    /*FXDockSite* bottomdock =*/ new FXDockSite(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
    /*FXDockSite* leftdock =*/ new FXDockSite(this,LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y);
    /*FXDockSite* rightdock =*/ new FXDockSite(this,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y);

     FXToolBarShell* dragshell1 = new FXToolBarShell(this,FRAME_RAISED, 0, 0,600,600);
    menubar = new FXMenuBar(topdock,dragshell1, LAYOUT_DOCK_NEXT|LAYOUT_SIDE_TOP);
    new FXToolBarGrip(menubar,menubar,FXMenuBar::ID_TOOLBARGRIP,TOOLBARGRIP_DOUBLE);

    // Toolbar
    FXToolBarShell* dragshell2 = new FXToolBarShell(this,FRAME_RAISED, 0, 0,600,600);
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
        FXString(L"\tBefehle speichern\tBefehlsdatei speichern."),
        icons.save,
        this,
        ID_FILE_SAVE_ORDERS, BUTTON_TOOLBAR);
    new FXButton(toolbar,
        FXString(L"\tDatei schliessen\tDie aktuelle Datei schliessen."),
        icons.close,
        this,
        ID_FILE_CLOSE, BUTTON_TOOLBAR);

    FXVerticalSeparator *tb_separator = new FXVerticalSeparator(toolbar, SEPARATOR_LINE|LAYOUT_FILL_Y);
    tb_separator->setBorderColor(getApp()->getShadowColor());

    new FXToggleButton(toolbar, "\tZeige Regel-Informationen.", "\tZeige Regel-Informationen.", icons.info,icons.info, this, ID_VIEW_INFODLG, FRAME_RAISED|TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE);

    tb_separator = new FXVerticalSeparator(toolbar, SEPARATOR_LINE|LAYOUT_FILL_Y);
    tb_separator->setBorderColor(getApp()->getShadowColor());

    planes = new FXListBox(toolbar, this, ID_MAP_VISIBLEPLANE, LISTBOX_NORMAL|LAYOUT_FILL_Y|FRAME_LINE);
    planes->setBorderColor(getApp()->getShadowColor());
    planes->appendItem("Standardebene (0)", nullptr, (void*)0);
    planes->setNumVisible(planes->getNumItems());

    tb_separator = new FXVerticalSeparator(toolbar, SEPARATOR_LINE|LAYOUT_FILL_Y);
    tb_separator->setBorderColor(getApp()->getShadowColor());

    FXButton *modus;
    modus = new FXButton(toolbar, "\tNormaler Modus", icons.pointer, this, ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
    modus->setUserData((void*)FXCSMap::MODUS_NORMAL);
    modus = new FXButton(toolbar, "\tAuswahl-Modus: Anklicken selektiert Regionen", icons.select, this, ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
    modus->setUserData((void*)FXCSMap::MODUS_SELECT);

    terrainPopup = new FXPopup(this);

    for (int i = 1; i < data::TERRAIN_LASTPUBLIC+1; i++)
    {
        datablock terrainRegion;
        terrainRegion.terrain(i);
        FXival udata = FXCSMap::MODUS_SETTERRAIN + i;

        FXMenuCommand *cmd = new FXMenuCommand(terrainPopup, terrainRegion.terrainString() + "\t\tZeichnen-Modus: Setze Regionen", icons.terrain[i], this, ID_MAP_SETMODUS);
        cmd->setUserData((void*)udata);
    }

    new FXMenuSeparatorEx(terrainPopup);
    FXMenuCommand *cmd = new FXMenuCommand(terrainPopup, FXString(L"L\u00f6schen\t\tZeichnen-Modus: L\u00f6sche Regionen"), icons.terrain[0], this, ID_MAP_SETMODUS);
    cmd->setUserData((void*)(FXCSMap::MODUS_SETTERRAIN));

    FXHorizontalFrame *terrainFrame = new FXHorizontalFrame(toolbar, LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    terrainSelect = new FXButton(terrainFrame, FXString(L"\tZeichnen-Modus: Erstelle oder L\u00f6sche Regionen"), icons.terrain[0], this, ID_MAP_SETMODUS, FRAME_RAISED|BUTTON_TOOLBAR);
    terrainSelect->setUserData((void*)FXCSMap::MODUS_SETTERRAIN);
    new FXMenuButton(terrainFrame, FXString(L"\tZeichnen-Modus: Erstelle oder L\u00f6sche Regionen"), nullptr, terrainPopup, FRAME_RAISED|BUTTON_TOOLBAR|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|MENUBUTTON_DOWN, 0, 0,12, 0);

    // Recent file list
    recentFiles.setTarget(this);
    recentFiles.setSelector(ID_FILE_RECENT);
    recentFiles.setGroupName("MRU");
    recentFiles.setMaxFiles(6);

    // File menu
    filemenu = new FXMenuPane(this);
    new FXMenuTitle(menubar, "&Datei", nullptr,filemenu);
    new FXMenuCommand(
        filemenu,
        L"\u00d6&ffnen...\tCtrl-O\tEinen Report \u00f6ffnen.",
        icons.open,
        this,
        ID_FILE_OPEN);
    new FXMenuCommand(
        filemenu,
        L"Speichern...\tCtrl-Shift-S\tAktuellen Zustand speichern.",
        icons.open,
        this,
        ID_FILE_SAVE_ALL);
    new FXMenuCommand(
        filemenu,
        L"Befehle &laden...\tCtrl-Shift-O\tBefehle aus einer Textdatei laden.",
        icons.open, this, ID_FILE_LOAD_ORDERS);
    new FXMenuCommand(
        filemenu,
        L"Befehle &speichern\tCtrl-S\tBefehlsdatei speichern.",
        icons.save, this, ID_FILE_SAVE_ORDERS);

    // Map menu
    mapmenu = new FXMenuPane(this);
    new FXMenuCascade(filemenu, "&Karte", nullptr, mapmenu, 0);
    new FXMenuCommand(
        mapmenu,
        L"H&inzuf\u00fcgen...\tCtrl-I\tL\u00e4dt einen Karten-Report in den aktuellen Report.",
        icons.merge, this, ID_FILE_MERGE);
    new FXMenuCommand(
        mapmenu,
        L"Sp&eichern...\t\tDetailierten Karten-Report speichern.",
        icons.save, this, ID_FILE_SAVE_MAP);
    new FXMenuCommand(
        mapmenu,
        L"E&xportieren...\t\tKarte ohne Details speichern.",
        icons.save, this, ID_FILE_EXPORT_MAP);
#ifdef WITH_PNG_EXPORT
    new FXMenuCommand(
        mapmenu,
        L"Als &PNG exportieren...\t\tDie Karte als PNG speichern.",
        nullptr, this, ID_FILE_EXPORT_IMAGE);
#endif

    new FXMenuCommand(
        filemenu,
        L"Sch&liessen\t\tDie aktuelle Datei schliessen.",
        icons.close, this, ID_FILE_CLOSE);

    new FXMenuSeparatorEx(filemenu);
    new FXMenuCommand(
        filemenu,
        L"Befehle pr\u00fcfen\t\tPr\u00fct die Befehle.",
        nullptr, this, ID_FILE_CHECK_ORDERS);
    new FXMenuCommand(
        filemenu,
        L"Befehle einsenden...\t\tDie Befehle an den Server versenden.",
        nullptr, this, ID_FILE_UPLOAD_ORDERS);
    new FXMenuCommand(
        filemenu,
        L"Befehle exportieren...\t\tDie Befehle versandfertig exportieren.",
        nullptr, this, ID_FILE_EXPORT_ORDERS);
    menu.modifycheck = new FXMenuCheck(filemenu, 
        L"\u00c4nderungserkennung\t\tAutomatische \u00c4nderungserkennung.", this, ID_FILE_MODIFY_CHECK);

    FXString noText;
    new FXMenuSeparatorEx(filemenu);
    recentmenu = new FXMenuPane(this);
    for (int i = 0; i != 6; ++i)
        new FXMenuCommand(recentmenu, noText, nullptr, &recentFiles, FXRecentFiles::ID_FILE_1 + i);

    new FXMenuCascade(filemenu, L"&Zuletzt ge\u00f6ffnet", nullptr, recentmenu, 0);

    new FXMenuSeparatorEx(filemenu);
    new FXMenuCommand(filemenu,
        L"B&eenden\tCtrl-Q\tDas Programm beenden.", nullptr,
        this, ID_FILE_QUIT);

    // View menu
    viewmenu = new FXMenuPane(this);
    new FXMenuTitle(menubar, "&Ansicht", nullptr,viewmenu);
    menu.toolbar = new FXMenuCheck(viewmenu, "Tool&bar\t\tToolbar ein- bzw. ausblenden.", toolbar, ID_TOGGLESHOWN);
    menu.maponly = new FXMenuCheck(viewmenu, "Nur &Karte anzeigen\tCtrl-Shift-M\tNur die Karte anzeigen, Regionsliste und -infos ausblenden.", this, ID_VIEW_MAPONLY, 0);
    menu.messages = new FXMenuCheck(viewmenu, "&Meldungen\tCtrl-Shift-V\tRegionsmeldungen ein- bzw. ausblenden.", this, ID_VIEW_MESSAGES);
    menu.show_left = new FXMenuCheck(viewmenu, "&Regionsliste\tCtrl-Shift-R\tRegionsliste ein- bzw. ausblenden.", this, ID_VIEW_REGIONLIST);
    menu.show_right = new FXMenuCheck(viewmenu, "&Eigenschaften\tCtrl-Shift-E\tEinheiten- und Regionsdetails ein- bzw. ausblenden.", this, ID_VIEW_PROPERTIES);
    menu.calc = new FXMenuCheck(viewmenu, "&Taschenrechner\tCtrl-Shift-C\tTaschenrechner-Leiste ein- bzw. ausblenden.");
    menu.minimap = new FXMenuCheck(viewmenu, FXString(L"\u00dcber&sichtskarte\tCtrl-Shift-M\t\u00dcbersichtskarte ein- bzw. ausblenden."), this, ID_VIEW_MINIMAP);
    menu.infodlg = new FXMenuCheck(viewmenu, "&Informationen\tCtrl-Shift-I\tRegel-Informationen ein- bzw. ausblenden.", this, ID_VIEW_INFODLG);
    new FXMenuSeparatorEx(viewmenu, "Liste");
    menu.ownFactionGroup = new FXMenuCheck(viewmenu, "&Gruppe aktiver Partei\tCtrl-Shift-G\tDie Einheiten der eigenen Partei stehen in einer Gruppe.");
    menu.colorizeUnits = new FXMenuCheck(viewmenu, "Einheiten ko&lorieren\t\tEinheiten in Geb\u00e4uden und Schiffen einf\u00e4rben.");
    new FXMenuSeparatorEx(viewmenu, "Karte");
    menu.streets = new FXMenuCheck(viewmenu, "S&trassen zeigen\tCtrl-F1\tStrassen auf der Karte anzeigen.");
    menu.visibility = new FXMenuCheck(viewmenu, FXString(L"Si&chtbarkeit zeigen\tCtrl-F2\tSymbole f\u00fcr Sichtbarkeit der Regionen anzeigen (Leuchtturm und Durchreise)."));
    menu.shiptravel = new FXMenuCheck(viewmenu, "&Durchschiffung\tCtrl-F3\tEin kleines Schiffsymbol anzeigen, falls Schiffe durch eine Region gereist sind.");
    menu.shadowRegions = new FXMenuCheck(viewmenu, "Regionen ab&dunkeln\tCtrl-F4\tRegionen abdunkeln, wenn nicht von eigenen Personen gesehen.");
    menu.islands = new FXMenuCheck(viewmenu, "Insel&namen zeigen\tCtrl-F5\tInselnamen auf der Karte zeigen.");
    menu.minimap_islands = new FXMenuCheck(viewmenu, "In&seln auf Minikarte\t\tInselnamen auf der Minikarte zeigen.");
    planemenu = new FXMenuPane(this);
    planemenu->disable();
    FXMenuRadio* radio = new FXMenuRadio(planemenu, "Standardebene (0)", this, ID_MAP_VISIBLEPLANE, 0);
    radio->setCheck();
    new FXMenuCascade(viewmenu, "&Ebene", nullptr, planemenu, 0);
    zoommenu = new FXMenuPane(this);
    new FXMenuCommand(zoommenu, FXString(L"Vergr\u00f6\u00dfern\tCtrl-+\tKarte vergr\u00f6\u00dfern."), nullptr, this, ID_MAP_ZOOM, 0);
    new FXMenuCommand(zoommenu, "Verkleinern\tCtrl--\tKarte verkleinern.", nullptr, this, ID_MAP_ZOOM, 0);
    new FXMenuSeparatorEx(zoommenu, FXString(L"Gr\u00f6\u00dfe"));
    new FXMenuRadio(zoommenu, "&1 Pixel\tCtrl-8\t1.6%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "&2 Pixel\tCtrl-7\t3.1%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "&4 Pixel\tCtrl-6\t6.3%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "&8 Pixel\tCtrl-5\t12.5%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "&16 Pixel\tCtrl-4\t25%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "&32 Pixel\tCtrl-3\t50%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "&64 Pixel\tCtrl-2\t100%", this, ID_MAP_ZOOM, 0);
    new FXMenuRadio(zoommenu, "128 Pixel\tCtrl-1\t200%", this, ID_MAP_ZOOM, 0);
    new FXMenuCascade(viewmenu, FXString(L"&Gr\u00f6\u00dfe"), nullptr, zoommenu, 0);

    // Region menu
    regionmenu = new FXMenuPane(this);
    new FXMenuTitle(menubar, "B&earbeiten", nullptr,regionmenu);
    new FXMenuCommand(regionmenu, "&Suchen...\tCtrl-F\tEine Region, Einheit, Schiff, etc. suchen.", nullptr, this, ID_VIEW_SEARCHDLG);
    new FXMenuCommand(regionmenu, "&Ursprung setzen\t\tDen Kartenursprung (0/0) auf die markierte Region setzen.", nullptr, this, ID_MAP_SETORIGIN, 0);
    new FXMenuSeparatorEx(regionmenu, "Regionen");
    new FXMenuCommand(regionmenu,FXString(L"&Alle markieren\tCtrl-Shift-A\tAlle Regionen ausw\u00e4hlen."), nullptr,this, ID_REGION_SELALL);
    new FXMenuCommand(regionmenu,FXString(L"Alle &Inseln ausw\u00e4hlen\t\tAlle Landregionen ausw\u00e4hlen (Ozean, Feuerwand und Eisberg z\u00e4hlen nicht als Land)."), nullptr, this, ID_REGION_SELALLISLANDS);
    new FXMenuCommand(regionmenu, FXString(L"&Sichtbare markieren\t\tSichtbare Regionen ausw\u00e4hlen."), nullptr, this, ID_REGION_SELVISIBLE);
    new FXMenuCommand(regionmenu,FXString(L"&Keine markieren\tEscape\tKeine Region ausw\u00e4hlen."), nullptr, this, ID_REGION_UNSEL);
    new FXMenuCommand(regionmenu,FXString(L"Auswahl &invertieren\tCtrl-Shift-N\tAusgew\u00e4hlte Regionen abw\u00e4hlen und umgekehrt."), nullptr, this, ID_REGION_INVERTSEL);

    selectionmenu = new FXMenuPane(this);
    new FXMenuCascade(regionmenu, "&Erweitern", nullptr, selectionmenu);
    new FXMenuCommand(selectionmenu, "Auswahl &erweitern\tCtrl-F7\tAuswahl mit dem Radius von einer Region erweitern.", nullptr, this, ID_REGION_EXTENDSEL);
    new FXMenuCommand(selectionmenu,FXString(L"&Inseln ausw\u00e4hlen\tCtrl-F9\tAuswahl auf komplette Inseln erweitern."), nullptr, this, ID_REGION_SELISLANDS);

    new FXMenuCommand(regionmenu, L"Markierte &ausw\u00e4hlen\tCtrl-Space\tMarkierte Region ausw\u00e4hlen.", nullptr, this, ID_MAP_SELECTMARKED, 0);
    new FXMenuCommand(regionmenu, FXString(L"Markierte &l\u00f6schen\t\t"), nullptr, this, ID_REGION_REMOVESEL);

    // Faction menu
    factionmenu = new FXMenuPane(this);
    menu.faction = new FXMenuTitle(menubar, "&Partei", nullptr,factionmenu);
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
        menu.poolnoitems = new FXMenuCommand(menu.factionpool,FXString(L"Keine Gegenst\u00e4nde\t\tDer Parteipool enth\u00e4lt keine Gegenst\u00e4nde."), nullptr);
        menu.poolnoitems->disable();
    new FXMenuCascade(factionmenu, "&Parteipool", nullptr, menu.factionpool, 0);

    getAccelTable()->addAccel(MKUINT(KEY_KP_5,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_SELECTMARKED));
    getAccelTable()->addAccel(MKUINT(KEY_KP_7,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERNORTHWEST));
    getAccelTable()->addAccel(MKUINT(KEY_KP_9,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERNORTHEAST));
    getAccelTable()->addAccel(MKUINT(KEY_KP_6,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKEREAST));
    getAccelTable()->addAccel(MKUINT(KEY_KP_3,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERSOUTHEAST));
    getAccelTable()->addAccel(MKUINT(KEY_KP_1,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERSOUTHWEST));
    getAccelTable()->addAccel(MKUINT(KEY_KP_4,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERWEST));

    getAccelTable()->addAccel(MKUINT(KEY_H,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERWEST));
    getAccelTable()->addAccel(MKUINT(KEY_L,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKEREAST));
    getAccelTable()->addAccel(MKUINT(KEY_J,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERSOUTHWEST));
    getAccelTable()->addAccel(MKUINT(KEY_K,CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_MAP_MARKERNORTHEAST));

    // jump to calculator on ESCAPE
    // getAccelTable()->addAccel(MKUINT(KEY_R, CONTROLMASK), this,FXSEL(SEL_COMMAND, ID_CALCULATOR));

    // Help menu
    helpmenu = new FXMenuPane(this);
    new FXMenuTitle(menubar, "&?", nullptr,helpmenu);
    new FXMenuCommand(helpmenu, "Wer mich schuf...\t\tKontaktinformationen", nullptr,this, ID_HELP_ABOUT);

    // tooltip
    new FXToolTip(app);

    // *** create workdesk ***
    content = new FXSplitterEx(this, SPLITTER_HORIZONTAL|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);

    // Left splitter
    leftframe = new FXVerticalFrame(content,LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 3, 0, 0, 0);

    // Region list window
    regions = new FXRegionList(leftframe, this, ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y| TREELIST_BROWSESELECT);

    menu.ownFactionGroup->setTarget(regions);
    menu.ownFactionGroup->setSelector(FXRegionList::ID_TOGGLE_OWNFACTIONGROUP);

    menu.colorizeUnits->setTarget(regions);
    menu.colorizeUnits->setSelector(FXRegionList::ID_TOGGLE_UNITCOLORS);

    // Middle splitter
    middle = new FXVerticalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0,2);
    content->setStretcher(middle);

    FXSplitterEx *mapsplit = new FXSplitterEx(middle, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

    // Map window
    map = new FXCSMap(mapsplit, this, ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
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
    msgBorder = new FXVerticalFrame(mapsplit,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_LINE, 0, 0, 0, 0, 0, 0, 0, 0);
    msgBorder->setBorderColor(getApp()->getShadowColor());

    outputTabs = new FXTabBook(msgBorder, nullptr, 0, TABBOOK_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    new FXTabItem(outputTabs, "Rep&ort");
    reportInfo = new FXReportInfo(outputTabs, this, ID_SELECTION, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXTabItem(outputTabs, "Re&gion");
    regionInfo = new FXRegionInfo(outputTabs, this, ID_SELECTION, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXTabItem(outputTabs, "&Fehler");
    errorList = new FXList(outputTabs, this, ID_ERRROR_SELECTED, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXTabItem(outputTabs, "&Suchergebnisse");
    searchResults = new FXSearchResults(outputTabs, this, ID_RESULT_SELECTED, FOLDINGLIST_BROWSESELECT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    searchResults->getHeader()->setHeaderStyle(HEADER_RESIZE | HEADER_TRACKING);
    searchResults->appendHeader("Region");
    searchResults->appendHeader(FXString(L"Einheit/Geb\u00e4ude/Schiff"));
    searchResults->appendHeader("Partei");

    // Calculator bar
    mathbar = new FXCalculator(middle, this, ID_SELECTION, LAYOUT_FILL_X);
    mathbar->connectMap(map);
    menu.calc->setTarget(mathbar);
    menu.calc->setSelector(FXCalculator::ID_TOGGLESHOWN);

    // Right splitter
    rightframe = new FXVerticalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0,3, 0, 0);

    FXHorizontalFrame *riFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    riTab = new FXToolBarTab(riFrame, nullptr, 0, TOOLBARTAB_HORIZONTAL, 0, 0, 0, 0);
    riTab->setTipText("Regionsinformationen ein- und ausblenden");
    regionPanel = new FXRegionPanel(riFrame, this, ID_SELECTION, LAYOUT_FILL_X);

    FXHorizontalFrame *siFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    siTab = new FXToolBarTab(siFrame, nullptr, 0, TOOLBARTAB_HORIZONTAL, 0, 0, 0, 0);
    siTab->setTipText("Statistik ein- und ausblenden");
    statsPanel = new FXStatsPanel(siFrame, this, ID_SELECTION, LAYOUT_FILL_X);

    FXHorizontalFrame *tiFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    tiTab = new FXToolBarTab(tiFrame, nullptr, 0, TOOLBARTAB_HORIZONTAL, 0, 0, 0, 0);
    tiTab->setTipText("Handelsinformationen ein- und ausblenden");
    tradePanel = new FXTradePanel(tiFrame, this, ID_SELECTION, LAYOUT_FILL_X);

    commandsplitter = new FXSplitterEx(rightframe, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

    tabbook = new FXTabBook(commandsplitter, nullptr, 0, TABBOOK_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);

    new FXTabItem(tabbook, "Statistik");
    statistics = new FXStatistics(tabbook, this, ID_SELECTION, LAYOUT_FILL_X);
    new FXTabItem(tabbook, "Details");
    FXVerticalFrame *frame = new FXVerticalFrame(tabbook, LAYOUT_FILL_X);
    frame->setBorderColor(getApp()->getShadowColor());
    frame->setFrameStyle(FRAME_LINE);

    FXHorizontalFrame* hFrame = new FXHorizontalFrame(frame, LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    FXToolBarTab * dTab = new FXToolBarTab(hFrame, nullptr, 0, TOOLBARTAB_HORIZONTAL, 0, 0, 0, 0);
    dTab->setTipText("Handelsinformationen ein- und ausblenden");
    descriptionText = new FXText(hFrame, this, ID_SELECTION, LAYOUT_FILL_X|TEXT_READONLY|TEXT_WORDWRAP|HSCROLLER_NEVER);
    descriptionText->disable();

    unitProperties = new FXUnitList(frame, this, ID_SELECTION, 
        LAYOUT_FILL_X | LAYOUT_FILL_Y);
    unitProperties->hide();
    shipProperties = new FXShipProperties(frame, this, ID_SELECTION, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    shipProperties->hide();
    buildingProperties = new FXBuildingProperties(frame, this, ID_SELECTION, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    buildingProperties->hide();

    getAccelTable()->addAccel(MKUINT(KEY_1, ALTMASK), this, FXSEL(SEL_COMMAND, ID_TAB_1));
    getAccelTable()->addAccel(MKUINT(KEY_2, ALTMASK), this, FXSEL(SEL_COMMAND, ID_TAB_2));

    // Befehlseditor
    commandframe = new FXVerticalFrame(commandsplitter,LAYOUT_FILL_X|FRAME_LINE, 0, 0, 0, 0, 0, 0, 0, 0);
    commandframe->setBorderColor(getApp()->getShadowColor());
    commands = new FXCommands(commandframe, this, ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    commands->connectMap(map);
    if (fontFixed) {
        commands->setFont(fontFixed);
    }

    // commands editor tools
    FXHorizontalFrame *cmdBottomFrame = new FXHorizontalFrame(rightframe,LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    FXToolBarTab *cmdTab = new FXToolBarTab(cmdBottomFrame, nullptr, 0, TOOLBARTAB_HORIZONTAL, 0, 0, 0, 0);
    cmdTab->setTipText("Befehlstools ein- und ausblenden");

    FXHorizontalFrame *cmdOptFrame = new FXHorizontalFrame(cmdBottomFrame,LAYOUT_FILL_X|FRAME_LINE, 0, 0, 0, 0, 3,3,1,1);
    cmdOptFrame->setBorderColor(getApp()->getShadowColor());
    FXCheckButton *chk = new FXCheckButton(cmdOptFrame,
        FXString(L"&best\u00e4tigt\tBefehle best\u00e4tigen\tBefehle f\u00fcr diese Einheit best\u00e4tigen"),
        commands, FXCommands::ID_UNIT_CONFIRM, CHECKBUTTON_NORMAL);
    chk->addHotKey(FXHotKey(MKUINT(KEY_b, CONTROLMASK)));
    FXButton* btn;
    btn = new FXButton(cmdOptFrame,
        FXString(L"<\tVorherige Einheit\tZur vorhergehenden unbest\u00e4tigten Einheit"), nullptr,
        this, ID_UNIT_PREV, BUTTON_TOOLBAR);
    btn->addHotKey(FXHotKey(MKUINT(KEY_comma, CONTROLMASK)));
    btn->addHotKey(FXHotKey(MKUINT(KEY_p, CONTROLMASK)));

    btn = new FXButton(cmdOptFrame,
        FXString(L">\tN\u00e4chste Einheit\tZur n\u00e4chsten unbest\u00e4tigten Einheit"), nullptr,
        this, ID_UNIT_NEXT, BUTTON_TOOLBAR);
    btn->addHotKey(FXHotKey(MKUINT(KEY_period, CONTROLMASK)));
    btn->addHotKey(FXHotKey(MKUINT(KEY_n, CONTROLMASK)));

    new FXButton(cmdOptFrame,
        "+&temp\tNeue Temp-Einheit\tTemp-Einheit erstellen", nullptr,
        commands, FXCommands::ID_UNIT_ADD, BUTTON_TOOLBAR);

    FXTextField* rowcol = new FXTextField(cmdOptFrame, 5, commands,FXCommands::ID_ROWCOL, TEXTFIELD_READONLY|JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_RIGHT);
    rowcol->disable();
    rowcol->setDefaultCursor(getDefaultCursor());
    rowcol->setTextColor(getApp()->getShadowColor());

    // minimap
    minimap_frame = new FXDialogBox(this, FXString(L"\u00dcbersichtskarte"), DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE), 100,100, 640,480, 0, 0, 0, 0);
    minimap_frame->setIcon(icon);
    minimap_frame->getAccelTable()->addAccel(MKUINT(KEY_M, CONTROLMASK | SHIFTMASK), this, FXSEL(SEL_COMMAND, ID_VIEW_MINIMAP));

    FXStatusBar *minimap_bar = new FXStatusBar(minimap_frame,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER);
    minimap_bar->getStatusLine()->setFrameStyle(FRAME_LINE);
    minimap_bar->getStatusLine()->setBorderColor(getApp()->getShadowColor());
    minimap_bar->getStatusLine()->setNormalText("");

    minimap = new FXCSMap(minimap_frame, this, ID_SELECTION, LAYOUT_FILL_X|LAYOUT_FILL_Y, true /*minimap-mode*/);
    minimap->connectMap(map);

    menu.minimap_islands->setTarget(minimap);
    menu.minimap_islands->setSelector(FXCSMap::ID_TOGGLEISLANDS);

    // info dialog
    infodlg = new FXInfoDlg(this, "Informationen", icon, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE));
    infodlg->getAccelTable()->addAccel(MKUINT(KEY_I, CONTROLMASK|SHIFTMASK), this,FXSEL(SEL_COMMAND, ID_VIEW_INFODLG));

    // search dialog
    searchdlg = new FXSearchDlg(this, this, ID_SELECTION, searchResults, "Suchen...", icon, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE));
    searchdlg->getAccelTable()->addAccel(MKUINT(KEY_F, CONTROLMASK), this, FXSEL(SEL_COMMAND, ID_VIEW_SEARCHDLG));
}

CSMap::~CSMap()
{
    errorList->clearItems();
    searchResults->clearItems();
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

    delete fontFixed;
    delete appIcon;

    for (int i = 0; i < data::TERRAIN_LAST; i++)
        delete icons.terrain[i];
}

#ifdef WIN32
static FXString echeck_locate(const GUID folderId) {
    PWSTR wPath = nullptr;
    if (S_OK == SHGetKnownFolderPath(folderId, 0, nullptr, &wPath)) {
        FXString path(wPath);
        FXString exefile;
        path.append("\\Eressea\\Echeck");
        exefile = path + "\\echeckw.exe";
        if (FXStat::exists(exefile)) {
            return path;
        }
    }
    CoTaskMemFree(wPath);
    return "";
}
#endif
// Create and initialize
void CSMap::create()
{
    // Create the windows
    FXMainWindow::create();

    // reload window position & size
    FXRegistry &reg = getApp()->reg();
    const FXchar *echeck = reg.readStringEntry("settings", "echeck_dir", nullptr);
    if (echeck) {
        settings.echeck_dir.assign(echeck);
    }
#ifdef WIN32
    else {
        FXString path;
        path = echeck_locate(FOLDERID_ProgramFiles);
        if (path.empty()) {
            path = echeck_locate(FOLDERID_ProgramFilesX64);
        }
        if (path.empty()) {
            path = echeck_locate(FOLDERID_ProgramFilesX86);
        }
        settings.echeck_dir = path;
    }
#endif
    const FXchar *passwd = reg.readStringEntry("settings", "password", nullptr);
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
    if (!reg.readUnsignedEntry("SHOW", "TOOLBAR", 1))
        toolbar->handle(this, FXSEL(SEL_COMMAND, FXToolBar::ID_TOGGLESHOWN), nullptr);

    if (!reg.readUnsignedEntry("SHOW", "MESSAGES", 1))
        handle(this, FXSEL(SEL_COMMAND, ID_VIEW_MESSAGES), nullptr);

    if (!reg.readUnsignedEntry("SHOW", "PROPERTIES", 1))
        handle(this, FXSEL(SEL_COMMAND, ID_VIEW_PROPERTIES), nullptr);

    if (!reg.readUnsignedEntry("SHOW", "REGIONLIST", 1))
        handle(this, FXSEL(SEL_COMMAND, ID_VIEW_REGIONLIST), nullptr);

    if (!reg.readUnsignedEntry("SHOW", "CALCULATOR", 1))
        mathbar->handle(this, FXSEL(SEL_COMMAND, FXCalculator::ID_TOGGLESHOWN), nullptr);

    if (reg.readUnsignedEntry("SHOW", "STREETS", 1))
        map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESTREETS), nullptr);

    if (reg.readUnsignedEntry("SHOW", "VISIBILITYSYMBOL", 1))
        map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLEVISIBILITYSYMBOL), nullptr);

    if (reg.readUnsignedEntry("SHOW", "SHIPTRAVEL", 1))
        map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESHIPTRAVEL), nullptr);

    if (reg.readUnsignedEntry("SHOW", "SHADOWREGIONS", 1))
        map->handle(this, FXSEL(SEL_COMMAND, FXCSMap::ID_TOGGLESHADOWREGIONS), nullptr);

    if (reg.readUnsignedEntry("SHOW", "OWNFACTIONGROUP", 1))
        regions->handle(this, FXSEL(SEL_COMMAND, FXRegionList::ID_TOGGLE_OWNFACTIONGROUP), nullptr);

    if (reg.readUnsignedEntry("SHOW", "COLORIZEUNITS", 1))
        regions->handle(this, FXSEL(SEL_COMMAND, FXRegionList::ID_TOGGLE_UNITCOLORS), nullptr);

    reload_type check = (reload_type)reg.readUnsignedEntry("SHOW", "RELOAD", (int)reload_type::RELOAD_ASK);
    setAutoReload(check);

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

    // resize table headers
    int wid = (outputTabs->getWidth() - outputTabs->getPadLeft() - outputTabs->getPadRight() - 20) / searchResults->getNumHeaders();
    for (int i = 0; i < searchResults->getNumHeaders(); i++)
        searchResults->setHeaderSize(i, wid);
}

FXbool CSMap::close(FXbool notify)
{
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
    reg.writeUnsignedEntry("SHOW", "PROPERTIES", menu.show_right->getCheck());
    reg.writeUnsignedEntry("SHOW", "REGIONLIST", menu.show_left->getCheck());
    reg.writeUnsignedEntry("SHOW", "CALCULATOR", menu.calc->getCheck());
    reg.writeUnsignedEntry("SHOW", "STREETS", menu.streets->getCheck());
    reg.writeUnsignedEntry("SHOW", "VISIBILITYSYMBOL", menu.visibility->getCheck());
    reg.writeUnsignedEntry("SHOW", "SHIPTRAVEL", menu.shiptravel->getCheck());
    reg.writeUnsignedEntry("SHOW", "SHADOWREGIONS", menu.shadowRegions->getCheck());
    reg.writeUnsignedEntry("SHOW", "COLORIZEUNITS", menu.colorizeUnits->getCheck());
    reg.writeUnsignedEntry("SHOW", "OWNFACTIONGROUP", menu.ownFactionGroup->getCheck());
    reg.writeUnsignedEntry("SHOW", "RELOAD", (FXuint)reload_mode);

    // save ToolBarTab on/off state
    reg.writeUnsignedEntry("TABS", "REGIONINFOS", riTab->isCollapsed());
    reg.writeUnsignedEntry("TABS", "STATSINFOS", siTab->isCollapsed());
    reg.writeUnsignedEntry("TABS", "TRADEINFOS", tiTab->isCollapsed());

    if (FXMainWindow::close(notify)) {
        exit(0);
    }
    return false;
}

void CSMap::mapChange()
{
    // delete all planes except default
    resetPlanes();

    searchResults->clearItems();
    errorList->clearItems();

    // map changed, let selection-function handle this
    if (report == nullptr) {
        selection.selected = 0;
    }
    else {
        // notify info dialog of new game type
        FXString name_of_game;
        name_of_game = report->blocks().front().value("Spiel");
        if (name_of_game.empty())
            name_of_game = "default";

        infodlg->setGame(name_of_game);

        // get info about active faction
        if (report->getFactionId() != 0) {
            datablock::itor block = report->activefaction();

            // enable/disable FACTION menu
            if (haveActiveFaction())
            {
                if (!menu.faction->isEnabled())
                    menu.faction->enable();
            }
            else
            {
                if (menu.faction->isEnabled())
                    menu.faction->disable();
            }
            // write faction statistics into faction menu
            FXString name = block->value(TYPE_FACTIONNAME);
            FXString id = block->id();

            menu.name->setText(name + " (" + id + ")");

            FXString race_type = block->value(TYPE_TYPE);
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

                FXString percent = FXStringVal(FXint(f_points * 100 / f_average));

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
            if (block->type() == block_type::TYPE_ITEMS)
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

            if (selection.selected) {
                // iterators may be pointing to blocks the previous report, point to the current report instead.
                if (selection.selected & selection.REGION) {
                    selection.sel_x = selection.region->x();
                    selection.sel_y = selection.region->y();
                    selection.sel_plane = selection.region->info();
                    // update iterator to the new report:
                    if (!report->getRegion(selection.region, selection.sel_x, selection.sel_y, selection.sel_plane)) {
                        selection.selected |= selection.UNKNOWN_REGION;
                        selection.selected &= ~selection.REGION;
                    }
                }
                if (selection.selected & selection.UNIT) {
                    if (!report->getUnit(selection.unit, selection.unit->info())) {
                        selection.selected &= ~selection.UNIT;
                    }
                }
                if (selection.selected & selection.SHIP) {
                    if (!report->getShip(selection.ship, selection.ship->info())) {
                        selection.selected &= ~selection.SHIP;
                    }
                }
                if (selection.selected & selection.BUILDING) {
                    if (!report->getShip(selection.building, selection.building->info())) {
                        selection.selected &= ~selection.BUILDING;
                    }
                }
            }
            selection.regionsSelected.clear();
        }

        // get all planes in report
        std::set<int> planeSet;        // what planes are in the report
        datablock::itor end = report->blocks().end();
        for (datablock::itor block = report->blocks().begin(); block != end; block++)
        {
            // handle only regions
            if (block->type() != block_type::TYPE_REGION)
                continue;

            // insert plane into set
            int p = block->info();
            if (p != 0 && planeSet.insert(p).second) {
                addPlane(p);
            }
        }
        planes->setNumVisible(planes->getNumItems());

        if (!(selection.selected & (selection.REGION | selection.UNKNOWN_REGION)))
        {
            selection.selected |= selection.UNKNOWN_REGION;
            selection.sel_x = 0, selection.sel_y = 0, selection.sel_plane = 0;
        }
    }
    ++selection.fileChange;

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);

    searchdlg->setMapFile(report);
    minimap->setMapFile(report);
    commands->setMapFile(report);
    unitProperties->setMapFile(report);
    shipProperties->setMapFile(report);
    buildingProperties->setMapFile(report);
    tradePanel->setMapFile(report);
    statistics->setMapFile(report);
    statsPanel->setMapFile(report);
    regionPanel->setMapFile(report);
    mathbar->setMapFile(report);
    reportInfo->setMapFile(report);
    regionInfo->setMapFile(report);
    map->setMapFile(report);
    regions->setMapFile(report);
    if (report) {
        checkCommands();
    }
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

    return report->getFactionId() != 0;
}

void CSMap::loadFile(const FXString& filename)
{
    FXString errorMessage;
    
    report.reset(loadFile(filename, errorMessage));
    if (!errorMessage.empty()) {
        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", errorMessage.text());
    }
    if (report) {
        report->createHashTables();
        report->parseMessages();
    }
}

datafile* CSMap::loadFile(const FXString& filename, FXString& errorMessage)
{
    datafile* cr = new datafile();

    try
    {
        cr->filename(filename);
        if (!cr->load(filename, errorMessage)) {
            delete cr;
            cr = nullptr;
        }
        else {
            cr->createHashTables();
        }
    }
    catch(const std::runtime_error& err)
    {
        recentFiles.removeFile(filename);
        errorMessage.assign(err.what());
        delete cr;
        cr = nullptr;
    }
    return cr;
}

bool CSMap::mergeFile(const FXString& filename, FXString & errorMessage)
{
    // zuerst: Datei normal laden
    std::shared_ptr<datafile> new_cr = std::make_shared<datafile>();
    beginLoading(filename);

    try
    {
        if (!new_cr->load(filename, errorMessage)) {
            new_cr.reset();
        }
    }
    catch(const std::runtime_error& err)
    {
        errorMessage = FXString(filename) + ": " + FXString(err.what());
        new_cr.reset();
    }
    if (!new_cr) {
        return false;
    }

    int turn = report->turn();
    int new_turn = new_cr->turn();

    int x_offset = 0, y_offset = 0;
    if (new_turn < turn) {
        new_cr->removeTemporary();
    }
    else if (turn < new_turn) {
        report->removeTemporary();
    }

    if (new_turn < turn || (new_turn == turn && report->hasUnits())) {
        datafile* input = new_cr.get();
        report->findOffset(input, &x_offset, &y_offset);
        report->merge(input, x_offset, y_offset);
        // adjust selection by offset
        if (x_offset || y_offset) {
            if (selection.selected & selection.UNKNOWN_REGION) {
                if (selection.sel_plane == 0) {
                    selection.sel_x += x_offset;
                    selection.sel_y += y_offset;
                    ++selection.selChange;
                }
            }
        }
    }
    else {
        datafile* input = report.get();
        new_cr->createHashTables();
        new_cr->findOffset(input, &x_offset, &y_offset);
        new_cr->merge(input, x_offset, y_offset);
        selection.transfer(input, new_cr.get(), x_offset, y_offset);
        new_cr->filename(report->filename());
        new_cr->cmdfilename(report->cmdfilename());
        report.swap(new_cr);
    }
    return true;
}

bool CSMap::loadCommands(const FXString& filename)
{
    if (filename.empty() || !report)
        return false;

    if (!haveActiveFaction())
        return false;

    try
    {
        FXint res = report->loadCmds(filename);
        if (res < 0)
        {
            FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht gelesen werden.");
            mapChange();
            return false;
        }
        FXString password = report->getPassword();
        if (!password.empty()) {
            settings.password = password;
        }
        int factionId = report->getFactionId();
        if (factionId > 0) {
            settings.faction_id = FXStringValEx(factionId, 36);
        }
        updateModificationTime();
    }
    catch(const std::runtime_error& err)
    {
        FXString msg(err.what());
        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", msg.text());
        mapChange();
        return false;
    }

    reload_mode = CSMap::reload_type::RELOAD_ASK;
    mapChange();
    updateFileNames();
    return true;
}

bool CSMap::saveCommands(const FXString &filename, bool stripped)
{
    if (filename.empty() || !report)
        return false;

    if (!haveActiveFaction())
        return false;

    FXint res = report->saveCmds(filename.text(), settings.password, stripped);    // nicht \u00fcberschreiben
    if (res < 0) {
        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht geschrieben werden.");
        return false;
    }
    last_save_time = 0;
    report->modifiedCmds(false);
    updateFileNames();
    return true;
}

static char* u_mkstemp(char* buffer) {
#ifdef HAVE_MKSTEMP
    int fd;
    strncpy(buffer, "/tmp/csmapXXXXXX", PATH_MAX);
    if (0 <= (fd = mkstemp(buffer))) {
#ifdef HAVE_UNISTD_H
        close(fd);
#endif
        return buffer;
    }
    return nullptr;
#else
    return tmpnam(buffer);
#endif
}

bool CSMap::checkCommands()
{
    // save to a temporary file:
    char infile[PATH_MAX];
    char outfile[PATH_MAX];
    FXString cmdline("echeck");
#ifdef WIN32
    cmdline += "w.exe";
    if (!settings.echeck_dir.empty()) {
        cmdline = "\"" + settings.echeck_dir + "\\" + cmdline + "\"";
    }
#else
    if (!settings.echeck_dir.empty()) {
        cmdline = settings.echeck_dir + "/echeck";
    }
#endif
    errorList->clearItems();
    searchResults->clearItems();
    if (cmdline.empty()) {
        errorList->appendItem("Could not find the echeck executable.");
    }
    else {
        if (!u_mkstemp(infile) || report->saveCmds(infile, "", true) != 0) {
            errorList->appendItem("Could not save commands for analysis.");
        }
        else {
            if (!u_mkstemp(outfile)) {
                errorList->appendItem("Could not create output file for analysis.");
            }
            else {
                // Echeck it:
                cmdline.append(" -w3 -c1 -Lde -Re2 -O");
                cmdline.append(outfile);
                cmdline.append(" ");
                cmdline.append(infile);
#ifdef WIN32
                STARTUPINFOA si;
                PROCESS_INFORMATION pi;

                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));
                // hack: set ECheck locale to German, since we don't support English CsMap yet:
                if (!CreateProcessA(nullptr, (LPSTR)cmdline.text(), nullptr, nullptr, FALSE, 0, (LPVOID)"LC_MESSAGES=de\0",
                    settings.echeck_dir.text(), &si, &pi)) {
                    errorList->appendItem("CreateProcess failed: " + cmdline);
                }

                // Wait until child process exits.
                WaitForSingleObject(pi.hProcess, INFINITE);

                // Close process and thread handles. 
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
#else
                if (system(cmdline.text()) < 0) {
                    errorList->appendItem("echeck system call failed: " + cmdline);
                    return false;
                }
#endif
                for (auto error : output) delete error;
                output.clear();

                std::ifstream results;
                results.open(outfile, std::ios::in);
                if (results.is_open()) {
                    std::string line;
                    while (std::getline(results, line)) {
                        if (!line.empty()) {
                            MessageInfo* error = new MessageInfo();
                            if (error) {
                                FXString str, display;
                                FXString tok;
                                str.assign(line.c_str());
                                error->level = FXIntVal(str.section("|", 1));
                                tok = str.section("|", 2);
                                if (tok == "U") {
                                    error->unit_id = FXIntVal(str.section("|", 3), 36);
                                }
                                else if (tok == "R") {
                                    tok = str.section("|", 3);
                                    error->region_x = FXIntVal(tok.section(", ", 0));
                                    error->region_y = FXIntVal(tok.section(", ", 1));
                                }
                                output.push_back(error);
                                display = str.section("|", 5);
                                if (!display.empty()) {
                                    display += ": ";
                                }
                                display += str.section("|", 4);
                                errorList->appendItem(display, nullptr, error);
                            }
                        }
                    }
                }
                remove(outfile);
            }
            remove(infile);
        }
    }
    return true;
}

#ifdef WITH_PNG_EXPORT
bool CSMap::exportMapFile(const FXString &filename, FXint scale, FXColor bgColor, FXint options)
{
    FXProgressDialog progress(this, "Karte exportieren...", "Erzeuge Abbild der Karte...", PROGRESSDIALOG_NORMAL | PROGRESSDIALOG_CANCEL);
    progress.setIcon(icon);
    progress.create();
    getApp()->refresh(); // TODO: why?
    progress.show(PLACEMENT_SCREEN);
    return savePNG(filename, scale, bgColor, options, &progress);
}

bool CSMap::savePNG(const FXString &filename, FXint scale, FXColor color, FXint options, FXProgressDialog *progress)
{
    if (filename.empty())
        return false;

    if (!report)
        return false;

    FXCSMap csmap(this);
    csmap.hide();
    csmap.setMapFile(report);
    // csmap.create();

    // options
    if (scale == 64)
        csmap.scaleChange(16/64.0f);    // bugfix: scaleChange(1.0); doesn't work without previous scaleing

    csmap.scaleChange(scale/64.0f);
    csmap.setShowNames(options & exportNames);
    csmap.setShowKoords(options & exportCoordinates);
    csmap.setShowIslands(options & exportIslands);
    csmap.setBackColor(color);

    return SavePNG(filename, csmap, csmap.getApp(), progress);
}
#endif

long CSMap::onViewMapOnly(FXObject*, FXSelector, void*)
{
    //sender->handle(this, FXSEL(SEL_COMMAND, (value==scale)?ID_CHECK:ID_UNCHECK), nullptr);

    if (leftframe->shown() || rightframe->shown())
    {
        leftframe->hide();
        msgBorder->hide();
        rightframe->hide();
        map->recalc();
    }
    else
    {
        if (outputTabs->shown()) {
            msgBorder->show();
        }
        leftframe->show();
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

    sender->handle(this, FXSEL(SEL_COMMAND, maponly?ID_CHECK:ID_UNCHECK), nullptr);

    return 1;
}

long CSMap::onViewMessages(FXObject*, FXSelector, void*)
{
    if (outputTabs->shown())
    {
        outputTabs->hide();
        msgBorder->hide();
        msgBorder->recalc();
    }
    else
    {
        outputTabs->show();

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
    sender->handle(this, FXSEL(SEL_COMMAND, outputTabs->shown()?ID_CHECK:ID_UNCHECK), nullptr);
    return 1;
}

long CSMap::onViewRightFrame(FXObject*, FXSelector, void*)
{
    if (rightframe->shown())
    {
        rightframe->hide();
    }
    else
    {
        rightframe->show();
    }
    map->recalc();

    return 1;
}

long CSMap::updViewRightFrame(FXObject* sender, FXSelector, void*)
{
    sender->handle(this, FXSEL(SEL_COMMAND, rightframe->shown()?ID_CHECK:ID_UNCHECK), nullptr);
    return 1;
}

long CSMap::onViewLeftFrame(FXObject*, FXSelector, void*)
{
    if (leftframe->shown())
    {
        leftframe->hide();
    }
    else
    {
        leftframe->show();
    }
    map->recalc();

    return 1;
}

long CSMap::updViewLeftFrame(FXObject* sender, FXSelector, void*)
{
    sender->handle(this, FXSEL(SEL_COMMAND, leftframe->shown()?ID_CHECK:ID_UNCHECK), nullptr);
    return 1;
}

long CSMap::onViewMiniMap(FXObject*, FXSelector, void*)
{
    if (minimap_frame->shown()) {
        minimap_frame->hide();
    }
    else {
        minimap_frame->show(PLACEMENT_OWNER);
    }

    return 1;
}

long CSMap::updViewMiniMap(FXObject* sender, FXSelector, void*)
{
    sender->handle(this, FXSEL(SEL_COMMAND, minimap_frame->shown()?ID_CHECK:ID_UNCHECK), nullptr);
    return 1;
}

long CSMap::onViewInfoDlg(FXObject*, FXSelector, void*)
{
    if (infodlg->shown()) {
        infodlg->hide();
    }
    else {
        infodlg->show(PLACEMENT_OWNER);
    }
    return 1;
}

long CSMap::updViewInfoDlg(FXObject* sender, FXSelector, void*)
{
    sender->handle(this, FXSEL(SEL_COMMAND, infodlg->shown()?ID_CHECK:ID_UNCHECK), nullptr);
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

    sender->handle(this, FXSEL(SEL_COMMAND, (value==scale)?ID_CHECK:ID_UNCHECK), nullptr);
    return 1;
}

long CSMap::onModifyCheck(FXObject* sender, FXSelector sel, void*)
{
    FXMenuCheck* check = static_cast<FXMenuCheck*>(sender);
    if (check->getCheck()) {
        setAutoReload(CSMap::reload_type::RELOAD_ASK);
    }
    else {
        setAutoReload(CSMap::reload_type::RELOAD_NEVER);
    }
    return 1;
}

long CSMap::updModifyCheck(FXObject* sender, FXSelector, void*)
{
    bool checked;

    if (reload_mode == CSMap::reload_type::RELOAD_NEVER)
        checked = false;
    else
        checked = true;

    sender->handle(this, FXSEL(SEL_COMMAND, checked ? ID_CHECK : ID_UNCHECK), nullptr);

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
        name = selection.region->getName();
    }
    else {
        name = "Unbekannt";
    }

    FXString txt;
    txt.format("Koordinaten-Ursprung auf die Region %s (%d,%d) setzen?", name.text(), orig_x, orig_y);

    FXMessageBox dlg(this, CSMAP_APP_TITLE, txt, 0, MBOX_YES_NO);
    FXuint res = dlg.execute(PLACEMENT_SCREEN);

    if (res != MBOX_CLICKED_YES)
        return 1;

    getApp()->beginWaitCursor();
    for (datablock::itor block = report->blocks().begin(); block != report->blocks().end(); block++)
    {
        // handle only regions
        if (block->type() != block_type::TYPE_REGION)
            continue;

        // handle only the actually selected plane
        if (block->info() != orig_plane)
            continue;

        block->infostr(txt.format("%d %d %d", block->x()-orig_x, block->y()-orig_y, orig_plane));
    }

    selection.sel_x = 0, selection.sel_y = 0;    // its the origin now

    mapChange();
    getApp()->endWaitCursor();
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
    if (report) {
        FXList *list = (FXList *)sender;
        FXint item = list->getCurrentItem();
        MessageInfo *info = static_cast<MessageInfo *>(list->getItemData(item));

        if (info) {
            if (info->unit_id) {
                if (report->getUnit(selection.unit, info->unit_id)) {
                    selection.selected = selection.UNIT;
                    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
                    return 1;
                }
            }
            else {
                if (report->getRegion(selection.region, info->region_x, info->region_y, 0)) {
                    selection.selected = selection.REGION;
                    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
                    return 1;
                }
            }
        }
    }
    return 0;
}

long CSMap::onMapSelectMarked(FXObject*, FXSelector, void*)
{
    if (!report)
        return 0;

    // if no (existing) region marked, do nothing
    if (!(selection.selected & selection.REGION))
        return 0;

    // already selected? then deselect the region
    datablock* regionPtr = &*selection.region;
    std::set<datablock*>::iterator itor = selection.regionsSelected.find(regionPtr);
    if (itor == selection.regionsSelected.end())
        selection.regionsSelected.insert(regionPtr);
    else
        selection.regionsSelected.erase(itor);

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
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
    selection.sel_x = x, selection.sel_y = y, selection.sel_plane = plane;
    if (report->getRegion(selection.region, x, y, plane)) {
        selection.selected = selection.REGION;
    }
    else {
        selection.selected = selection.UNKNOWN_REGION;
    }

    // dont touch multiregions selected
    if (!selection.regionsSelected.empty())
    {
        selection.regionsSelected = selection.regionsSelected;
    }

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
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

void CSMap::updateFileNames() {
    // change window title and status bar
    FXString titlestr = CSMAP_APP_TITLE_VERSION;

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
        titlestr = CSMAP_APP_TITLE;
        titlestr += " - [" + filenames + "]";
        // update filename in statusbar
        status_file->setText(filenames);
        status_file->show(), status_lfile->show();
    }

    handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &titlestr);
}

static FXString descText(const datablock & block)
{
    const FXString& descr = block.value(TYPE_DESCRIPTION);
    const FXString& notes = block.value(TYPE_NOTES);
    if (notes.empty()) {
        return descr;
    } else if (descr.empty()) {
        return FXString("Privat: ") + notes;
    }
    else {
        return descr + "\nPrivat: " + notes;
    }
}

long CSMap::onMapChange(FXObject*, FXSelector, void* ptr)
{
    datafile::SelectionState* pstate = (datafile::SelectionState*)ptr;

    if (pstate->fileChange != selection.fileChange) {
        searchResults->setActiveFactionId(report ? report->getActiveFactionId() : 0);
    }
    if (!report) return 0;
    getApp()->beginWaitCursor();

    updateFileNames();

    // make sure that a region is always selected (when something in it is selected)
    // start with selected item and search containing region
    datablock::itor block = report->blocks().begin();
    if (pstate->selected & selection.UNIT)
    {
        block = pstate->unit;
        descriptionText->setText(descText(*block));
    }
    else if (pstate->selected & selection.SHIP)
    {
        block = pstate->ship;
        descriptionText->setText(descText(*block));
    }
    else if (pstate->selected & selection.BUILDING)
    {
        block = pstate->building;
        descriptionText->setText(descText(*block));
    }
    else {
        descriptionText->setText("");
    }
    showProperties(unitProperties, pstate->selected& selection.UNIT);
    showProperties(shipProperties, pstate->selected& selection.SHIP);
    showProperties(buildingProperties, pstate->selected& selection.BUILDING);

    selection = *pstate;
    ++selection.selChange;
    while (block != report->blocks().begin()) {
        --block;
        if (block->type() == block_type::TYPE_REGION)
        {
            selection.region = block;
            selection.selected |= selection.REGION;
            break;
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
    else if (selection.selected & selection.UNKNOWN_REGION)
    {
        if (report->getRegion(selection.region, selection.sel_x, selection.sel_y, selection.sel_plane))
        {
            selection.selected &= ~selection.UNKNOWN_REGION;
            selection.selected |= selection.REGION;
        }
    }

    int factionId = 0;
    if (report) {
        int turn = report->turn();
        status_turn->setText(FXStringVal(turn));
        status_turn->show();
        status_lturn->show();
        FXString date = gameDate(turn);
        status_date->setText(date);
        status_date->show();
        status_ldate->show();
        factionId = report->getFactionId();
    }
    else {
        status_turn->hide();
        status_lturn->hide();
        status_file->hide();
        status_lfile->hide();
        status_date->hide();
        status_ldate->hide();
        status->recalc();
    }

    // update faction name in status bar
    if (factionId > 0)
    {
        // update statusbar
        FXString faction;
        faction.format("%s (%s)", report->activefaction()->value(TYPE_FACTIONNAME).text(),
            FXStringValEx(factionId, 36).text());

        status_faction->setText(faction);
        status_faction->show();
        status_lfaction->show();
    }
    else
    {
        status_faction->hide();
        status_lfaction->hide();
        status->recalc();
    }
    // only need to update the regionlist when confirmation stauts was changed.
    if (selection.selected & selection.CONFIRMATION) {
        regions->update();
        selection.selected &= ~selection.CONFIRMATION;
    }
    getApp()->endWaitCursor();
    return 1;
}

long CSMap::onQueryMap(FXObject* sender, FXSelector sel, void*)
{
    sender->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onSetSelection(FXObject* sender, FXSelector sel, void* ptr)
{
    datablock* block = static_cast<datablock*>(ptr);
    ++selection.selChange;
    selection.selected = 0;
    if (block->type() == block_type::TYPE_REGION) {
        if (report->getRegion(selection.region, *block)) {
            selection.selected = selection.REGION;
        }
    }
    else if (block->type() == block_type::TYPE_FACTION) {
        if (report->getFaction(selection.faction, block->info())) {
            selection.selected = selection.FACTION;
        }
    }
    else {
        if (block->type() == block_type::TYPE_UNIT) {
            if (report->getUnit(selection.unit, block->info())) {
                selection.selected = selection.UNIT;
                if (report->getParent(selection.region, selection.unit)) {
                    selection.selected |= selection.REGION;
                }
            }
        }
        else if (block->type() == block_type::TYPE_SHIP) {
            if (report->getShip(selection.ship, block->info())) {
                selection.selected = selection.SHIP;
                if (report->getParent(selection.region, selection.ship)) {
                    selection.selected |= selection.REGION;
                }
            }
        }
        else if (block->type() == block_type::TYPE_BUILDING) {
            if (report->getBuilding(selection.building, block->info())) {
                selection.selected = selection.BUILDING;
                if (report->getParent(selection.region, selection.building)) {
                    selection.selected |= selection.REGION;
                }
            }
        }
    }
    return handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
}

long CSMap::onClipboardRequest(FXObject*, FXSelector, void* ptr)
{
    // somebody wants our clipboard data
    FXEvent *event = (FXEvent*)ptr;

    // Return clipped text as as UTF-8
    if(event->target == utf8Type)
    {
        setDNDData(FROM_CLIPBOARD, utf8Type, FXString(clipboard));
        return 1;
    }

    if (event->target == stringType || event->target == textType)
    {
        // Give the array to the system!
        FXString text = utf2iso(clipboard);
        setDNDData(FROM_CLIPBOARD, event->target, text);
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
    clipboard.assign(static_cast<const char *>(ptr));
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

bool CSMap::updateCommands(const FXString &filename) {
    bool modified = report->modifiedCmds();
    if (reload_mode == CSMap::reload_type::RELOAD_ASK) {
        FXString ask = modified ?
            L"Eine andere Anwendung hat die Datei %s ge\u00e4ndert.\nNeu laden und die in CSMap gemachten \u00c4nderungen verlieren?" :
            L"Eine andere Anwendung hat die Datei %s ge\u00e4ndert.\nNeu laden?";
        FXuint res = FXMessageBox::question(this,
            (FXuint)MBOX_YES_NO, CSMAP_APP_TITLE,
            ask.text(),
            filename.text());
        if (res != MBOX_CLICKED_NO) {
            return true;
        }
        // setAutoReload(CSMap::reload_type::RELOAD_NEVER);
    }
    else if (reload_mode == CSMap::reload_type::RELOAD_AUTO) {
        return true;
    }
    return false;
}

long CSMap::onWatchFiles(FXObject *, FXSelector, void *ptr)
{
    if (report && reload_mode != CSMap::reload_type::RELOAD_NEVER) {
        if (hasFocus()) {
            FXString filename = report->cmdfilename();
            if (!filename.empty()) {
                struct stat buf;
                if (stat(filename.text(), &buf) == 0) {
                    if (buf.st_mtime > last_save_time) {
                        if (last_save_time != 0) {
                            if (updateCommands(filename)) {
                                last_save_time = buf.st_mtime;
                                if (stat(filename.text(), &buf) == 0) {
                                    last_save_time = buf.st_mtime;
                                }
                                loadCommands(filename);
                                getApp()->addTimeout(this, CSMap::ID_WATCH_FILES, 1000, nullptr);
                                return 1;
                            }
                        }
                        last_save_time = buf.st_mtime;
                    }
                }
            }
        }
    }
    if (reload_mode != CSMap::reload_type::RELOAD_NEVER) {
        getApp()->addTimeout(this, CSMap::ID_WATCH_FILES, 1000, nullptr);
    }
    return 0;
}

long CSMap::onMessageSelected(FXObject* sender, FXSelector sel, void* ptr)
{
    return 0;
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

long CSMap::onPopupCopyText(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* menuitem = static_cast<FXMenuCommand*>(sender);
    const char* text = static_cast<const char*>(menuitem->getUserData());
    setClipboard(text);
    return 1;
}

long CSMap::onPopupCopySpecial(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* menuitem = static_cast<FXMenuCommand*>(sender);
    const datablock* block = static_cast<const datablock*>(menuitem->getUserData());
    if (block) {
        if (FXSELID(sel) == ID_POPUP_COPY_NAME) {
            setClipboard(block->getName().text());
        }
        else if (FXSELID(sel) == ID_POPUP_COPY_ID) {
            if (block->type() == block_type::TYPE_REGION) {
                FXString coor = datafile::regionCoordinates(*block);
                setClipboard(coor.text());
            }
            else {
                setClipboard(block->id().text());
            }
        }
    }
    return 1;
}

long CSMap::onPopupShowInfo(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* menuitem = static_cast<FXMenuCommand*>(sender);
    const char* text = static_cast<const char*>(menuitem->getUserData());
    showInfo(text);
    setClipboard(text);
    return 0;
}

long CSMap::onPopupGotoObject(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* command = static_cast<FXMenuCommand*>(sender);
    datablock* block = static_cast<datablock*>(command->getUserData());
    if (block) {
        gotoObject(block);
    }
    return 0;
}

long CSMap::onNextUnit(FXObject*, FXSelector, void*)
{
    if (!report)
        return 0;

    datablock::citor end = report->blocks().end();
    datablock::itor start;
    if (selection.selected & selection.UNIT)
        start = selection.unit;
    else if (selection.selected & selection.REGION)
        start = selection.region;
    else
        return 0;

    bool wrap = true;
    for (datablock::itor unit = std::next(start); wrap || unit != end; ++unit)
    {
        if (unit == start) break;
        if (unit == end) {
            if (!wrap) break;
            wrap = false;
            unit = report->blocks().begin();
        }
        if (unit->type() != block_type::TYPE_UNIT)
            continue;

        // search command block
        if (!report->isConfirmed(*unit))
        {
            selection.selected = selection.UNIT;
            selection.unit = unit;
            handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);

            return 1;
        }
    }

    // no next unit found: beep
    getApp()->beep();
    return 0;
}

long CSMap::onPrevUnit(FXObject*, FXSelector, void*)
{
    if (!report)
        return 0;

    datablock::itor begin;
    if (selection.selected & selection.UNIT)
        begin = selection.unit;
    else if (selection.selected & selection.REGION)
        begin = selection.region;
    else
        return 0;
    bool wrap = true;
    datablock::itor unit = begin;
    for (;;)
    {
        if (unit == report->blocks().begin()) {
            unit = report->blocks().end();
        }
        --unit;
        if (unit == begin) {
            if (!wrap) break;
            wrap = false;
            unit = std::prev(report->blocks().end());
        }
        if (unit->type() != block_type::TYPE_UNIT)
            continue;

        if (!report->isConfirmed(*unit))
        {
            // send selectionchange, select this unit

            selection.selected = selection.UNIT;
            selection.unit = unit;
            handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
            return 1;
        }
    }

    // no next unit found: beep
    getApp()->beep();
    return 1;
}

long CSMap::onResultSelected(FXObject*, FXSelector, void* ptr)
{
    if (!report)
        return 0;

    FXFoldingItem* item = searchResults->getCurrentItem();
    if (!item)
        return 0;

    datablock* select = (datablock*)item->getData();

    datablock::itor region, block, end = report->blocks().end();
    report->findSelection(select, block, region);

    // propagate selection
    int selected = 0;
    if (region != end) {
        selection.region = region;
        selected |= selection.REGION;
    }
    if (block != end) {
        if (block->type() == block_type::TYPE_UNIT) {
            selection.unit = block;
            selected |= selection.UNIT;
        }
        else if (block->type() == block_type::TYPE_BUILDING) {
            selection.building = block;
            selected |= selection.BUILDING;
        }
        else if (block->type() == block_type::TYPE_SHIP) {
            selection.ship = block;
            selected |= selection.SHIP;
        }
    }
    selection.selected = selected;
    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

void CSMap::addRecentFile(const FXString& filename)
{
    FXString path = FXPath::convert(FXPath::simplify(filename));
    recentFiles.appendFile(path);
}

void CSMap::loadFiles(const std::vector<FXString> &filenames, std::vector<FXString> & errorMessages)
{
    if (!filenames.empty()) {

        for (FXString const& filename : filenames) {
            FXString errorMessage;
            if (!report) {
                report.reset(loadFile(filename, errorMessage));    // normal laden, wenn vorher keine Datei geladen ist.
                if (!errorMessage.empty()) {
                    errorMessages.push_back(errorMessage);
                }
                else {
                    addRecentFile(filename);
                }
            }
            else {
                mergeFile(filename, errorMessage); // updates report in case of success
                if (!errorMessage.empty()) {
                    errorMessages.push_back(errorMessage);
                }
            }
        }
        ++selection.fileChange;
    }
    if (report) {
        report->parseMessages();
    }
}

int CSMap::saveFile(const FXString& filename, map_type mode)
{
    return report->save(filename.text(), mode, &selection.regionsSelected);
}

struct clip_t {
    FXint sel_id;
    FXString label;
    void* data;
};

void CSMap::addClipboardPane(FXMenuPane *pane, datablock * block)
{
    datablock::itor match;
    datablock* parent = nullptr;
    std::vector<clip_t> clips;
    block_type type = block->type();
    new FXMenuSeparatorEx(pane, "Zwischenablage");
    
    FXString str = block->getName();
    if (!str.empty()) {
        clips.push_back(clip_t{ ID_POPUP_COPY_NAME, str + "\t\tName", block });
    }

    if (block->type() == block_type::TYPE_REGION)
    {
        FXString coor = datafile::regionCoordinates(*block);
        clips.push_back(clip_t{ ID_POPUP_COPY_ID, coor + "\t\tKoordinaten", const_cast<datablock*>(block) });

        const FXString& terrain = block->terrainString();
        clips.push_back(clip_t{ ID_POPUP_COPY_TEXT, terrain + "\t\tTerrain", const_cast<char*>(terrain.text()) });
    }
    else
    {
        str = block->id();
        clips.push_back(clip_t{ ID_POPUP_COPY_ID, str + "\t\tNummer", const_cast<datablock*>(block) });

        if (block->type() == block_type::TYPE_FACTION)
        {
            const FXString& email = block->value(TYPE_EMAIL);
            if (!email.empty())
            {
                clips.push_back(clip_t{ ID_POPUP_COPY_TEXT, email + "\t\tEmail", const_cast<char*>(email.text()) });
            }
        }
    }
    if (!clips.empty()) {
        for (const clip_t& clip : clips)
        {
            FXMenuCommand* menuitem = new FXMenuCommand(pane, clip.label, NULL, this, clip.sel_id);
            menuitem->setUserData(clip.data);
        }
    }
    if (parent) {
        addClipboardPane(pane, parent);
    }
}

void CSMap::addPlane(int planeId)
{
    FXString label = datablock::planeName(planeId);
    void* data = reinterpret_cast<void*>((ptrdiff_t)planeId);
    planes->appendItem(label, nullptr, data);
    FXMenuRadio* radio = new FXMenuRadio(planemenu, label, this, ID_MAP_VISIBLEPLANE, 0);
    radio->setUserData(data);
    radio->create();
}

void CSMap::resetPlanes()
{
    planes->clearItems();
    FXWindow* item = planemenu->getFirst();
    while (item)
    {
        FXWindow* nextw = item->getNext();
        delete item;
        item = nextw;
    }

    addPlane(0);
}

int CSMap::getActiveFactionId() const
{
    if (report) {
        return report->getActiveFactionId();
    }
    return 0;
}

long CSMap::onFileOpen(FXObject*, FXSelector, void* r)
{
    FXFileDialog dlg(this, FXString(L"\u00d6ffnen..."));
    dlg.setIcon(icons.open);
    dlg.setDirectory(dialogDirectory);
    dlg.setPatternList(FXString(L"Eressea Computer Report (*.cr)\nAlle Dateien (*)"));
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    dialogDirectory = dlg.getDirectory();
    if (res) {
        getApp()->beginWaitCursor();
        // vorherige Dateien schliessen, Speicher frei geben
        if (closeFile()) {
            FXString filename = dlg.getFilename();
            beginLoading(filename);
            loadFile(filename);
            if (report) {
                addRecentFile(filename);
                checkCommands();
            }
            mapChange();
            updateFileNames();
        }
        getApp()->endWaitCursor();
    }
    return 1;
}

long CSMap::onFileMerge(FXObject *, FXSelector, void *r)
{
    FXFileDialog dlg(this, FXString(L"Karte hinzuf\u00fcgen..."));
    dlg.setIcon(icons.merge);
    dlg.setSelectMode(SELECTFILE_MULTIPLE);
    dlg.setDirectory(dialogDirectory);
    dlg.setPatternList(FXString(L"Eressea Computer Report (*.cr)\nAlle Dateien (*)"));
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    dialogDirectory = dlg.getDirectory();
    if (res)
    {
        FXString* choices = dlg.getFilenames();
        std::vector<FXString> filenames;
        for (int i = 0; !choices[i].empty(); ++i)
        {
            filenames.push_back(choices[i]);
        }
        getApp()->beginWaitCursor();
        std::vector<FXString> errorMessages;
        loadFiles(filenames, errorMessages);
        if (!errorMessages.empty()) {
            for (const FXString& errorMessage : errorMessages) {
                FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "%s", errorMessage.text());
            }
        }
        mapChange();
        getApp()->endWaitCursor();
    }
    return 1;
}

long CSMap::onFileCheckCommands(FXObject *, FXSelector, void *)
{
    if (checkCommands()) {
        /* select the errors tab */
        outputTabs->setCurrent(outputTabs->indexOfChild(errorList) / 2);
        return 1;
    }
    return 0;
}

void CSMap::setClipboard(const char* text)
{
    handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), const_cast<char*>(text));
}

void CSMap::showInfo(const char* text)
{
    handle(this, FXSEL(SEL_QUERY_HELP, ID_SETSTRINGVALUE), const_cast<char*>(text));
}

void CSMap::gotoObject(datablock* block)
{
    handle(this, FXSEL(SEL_COMMAND, ID_SETVALUE), block);
}

bool CSMap::saveReport(const FXString& filename, map_type mode, bool merge_commands /*= false*/)
{
    if (filename.empty() || !report)
        return false;

    FXint res = saveFile(filename, mode);
    if (res <= 0) {
        FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Die Datei konnte nicht geschrieben werden.");
        return false;
    }
    return true;
}

FXString CSMap::askSaveFileName(const FXString &dlgTitle)
{
    return askFileName(dlgTitle, "Eressea Computer Report (*.cr)\nAlle Dateien (*)");
}

void CSMap::updateModificationTime()
{
    if (report) {
        FXString filename = report->cmdfilename();
        struct stat buf;
        if (stat(filename.text(), &buf) == 0) {
            last_save_time = buf.st_mtime;
        }
    }
}

void CSMap::setAutoReload(CSMap::reload_type mode)
{
    menu.modifycheck->setCheck(mode != reload_type::RELOAD_NEVER);
    if (reload_mode != mode) {
        if (mode != reload_type::RELOAD_NEVER) {
            updateModificationTime();
        }
        if (reload_mode == reload_type::RELOAD_NEVER) {
            getApp()->addTimeout(this, CSMap::ID_WATCH_FILES, 1000, nullptr);
        }
        reload_mode = mode;
    }
}

bool CSMap::allowReplaceFile(const FXString& filename)
{
    if (FXStat::exists(filename)) {
        FXString text;
        text = filename + FXString(L" existiert bereits.\n\nM\u00f6chten Sie sie ersetzen?");

        FXuint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
        if (MBOX_CLICKED_YES != answ) {
            return false;
        }
    }
    return true;
}

long CSMap::onFileSaveMap(FXObject*, FXSelector, void*)
{
    FXString filename = askSaveFileName("Speichern unter...");
    if (!filename.empty() && allowReplaceFile(filename)) {
        getApp()->beginWaitCursor();
        if (saveReport(filename, map_type::MAP_NORMAL)) {
            addRecentFile(filename);
        }
        getApp()->endWaitCursor();
        return 1;
    }
    return 0;
}

long CSMap::onFileSaveAll(FXObject*, FXSelector, void*)
{
    FXString filename = askSaveFileName("Speichern unter...");
    if (!filename.empty() && allowReplaceFile(filename)) {
        getApp()->beginWaitCursor();
        if (saveReport(filename, map_type::MAP_FULL)) {
            addRecentFile(filename);
            report->filename(filename);
            updateFileNames();
        }
        getApp()->endWaitCursor();
        return 1;
    }
    return 0;
}

long CSMap::onFileExportMap(FXObject*, FXSelector, void*)
{
    FXString filename = askSaveFileName("Speichern unter...");
    if (!filename.empty() && allowReplaceFile(filename)) {
        getApp()->beginWaitCursor();
        saveReport(filename, map_type::MAP_MINIMAL);
        getApp()->endWaitCursor();
        return 1;
    }
    return 0;
}

long CSMap::onFileClose(FXObject*, FXSelector, void*)
{
    if (closeFile()) {
        selection.clear();
        mapChange();
        updateFileNames();
        return 1;
    }
    return 0;
}

bool CSMap::closeFile()
{
    // ask if modified commands should be safed
    if (report && report->modifiedCmds())
    {
        FXuint res = FXMessageBox::question(this, (FXuint)MBOX_SAVE_CANCEL_DONTSAVE, CSMAP_APP_TITLE, "%s",
            FXString(L"Die \u00c4nderungen an den Befehlen speichern?").text());

        if (res == MBOX_CLICKED_CANCEL)
            return false;                    // don't close, cancel clicked
        else if (res == MBOX_CLICKED_SAVE)
        {
            saveCommandsDlg(false, true);            // save commands

            // cancel close, when save was unsuccessful
            if (report && report->modifiedCmds())
                return false;
        }
    }
    selection.selected = 0;
    selection.regionsSelected.clear();
    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    report = nullptr;
    return true;
}

bool CSMap::confirmOverwrite()
{
    if (report && report->modifiedCmds())
    {
        FXString ask =
            L"Seit dem letzten Speichern gemachte \u00c4nderungen verlieren?";
        FXuint res = FXMessageBox::question(this,
            (FXuint)MBOX_YES_NO, CSMAP_APP_TITLE,
            "%s", ask.text());
        return (res != MBOX_CLICKED_NO);
    }
    return true;
}

long CSMap::onFileOpenCommands(FXObject *, FXSelector, void *)
{
    if (!confirmOverwrite()) {
        return 0;
    }
    FXFileDialog dlg(this, FXString(L"Befehle laden..."));
    dlg.setIcon(icons.open);
    dlg.setDirectory(dialogDirectory);
    dlg.setPatternList(FXString(L"Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nM\u00f6gliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)"));
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    dialogDirectory = dlg.getDirectory();
    if (res) {
        getApp()->beginWaitCursor();
        FXString filename = dlg.getFilename();
        if (loadCommands(filename)) {
            addRecentFile(filename);
            updateFileNames();
            checkCommands();
        }
        getApp()->endWaitCursor();
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
            if (FXStat::exists(filename)) {
                FXString text;
                text = "Die Datei " + filename + FXString(L" existiert bereits.\n\nM\u00f6chten Sie sie ersetzen?");
                FXuint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
                if (MBOX_CLICKED_YES != answ) return 0;
            }
        }
        getApp()->beginWaitCursor();
        saveCommands(filename, false);
        report->cmdfilename(filename);
        getApp()->endWaitCursor();
        return 1;
    }
    return 0;
}

long CSMap::onFileExportCommands(FXObject*, FXSelector, void* ptr)
{
    saveCommandsDlg(true, false);
    return 1;
}

long CSMap::onFileUploadCommands(FXObject*, FXSelector, void* ptr)
{
    char infile[PATH_MAX];
    if (!report)
        return 0;

#if !defined(HAVE_CURL) && !defined(WIN32)
    return FXMessageBox::error(this, MBOX_OK, "Not Implemented",
        "Sorry, this version of CsMap was compiled without CURL support");
#endif
    int factionId = report->getFactionId();
    if (factionId == 0)
        return 0;
    FXString id = FXStringValEx(factionId, 36);
    FXString passwd = askPasswordDlg(id);
    if (u_mkstemp(infile)) {
        FXString filename(infile);
        if (report->saveCmds(filename, passwd, true) == 0) {
            long code;
            FXString body;
            code = UploadFile(filename, id, passwd, body);
            remove(infile);
            if (code == 401) {
                settings.password.clear();
                FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Fehler %ld: Falsches Passwort.", code);
            }
            else if (code >= 500) {
                FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Serverfehler %ld: Bitte sp\u00e4ter noch einmal versuchen.", code);
            }
            else if (code < 200 || code >= 300) {
                FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "Fehler %ld: %s (Befehle nicht akzeptiert).", code, body.text());
            }
            else {
                FXMessageBox::information(this, MBOX_OK, CSMAP_APP_TITLE, "Die Befehle wurden empfangen.");
            }
        }
    }
    return 0;
}

FXString CSMap::askPasswordDlg(const FXString &faction_id) {
    FXString passwd = settings.password;
    if (passwd.empty() || faction_id != settings.faction_id)
    {
        FXInputDialog dlg(this, "Passwort eingeben",
            FXString(L"Geben Sie das Passwort f\u00fcr die Partei " + faction_id + " ein:"),
            nullptr, INPUTDIALOG_STRING | INPUTDIALOG_PASSWORD);
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

    int factionId = report->getFactionId();
    if (factionId == 0)
        return;
    FXString id = FXStringValEx(factionId, 36);
    FXString filename = report->cmdfilename();
    FXString passwd = askPasswordDlg(id);

    if (stripped || filename.empty()) {
        filename = askFileName(
            stripped ? "Versandbefehle speichern unter..." : "Befehle speichern unter...",
            FXString(L"Textdatei (*.txt)\nZug-Datei (*.zug)\nBefehlsdatei (*.bef)\nM\u00f6gliche Befehlsdateien (*.txt,*.bef,*.zug)\nAlle Dateien (*)"));
    }
    if (!filename.empty()) {
        if (!replace && FXStat::exists(filename)) {
            FXString text;
            text = "Die Datei " + filename + FXString(L" existiert bereits.\n\nM\u00f6chten Sie sie ersetzen?");

            FXuint answ = FXMessageBox::question(this, MBOX_YES_NO, "Datei ersetzen?", "%s", text.text());
            if (MBOX_CLICKED_YES != answ) return;
        }

        saveCommands(filename, stripped);
    }
}

long CSMap::onFilePreferences(FXObject*, FXSelector, void*)
{
    FXPrefsDlg dlg(this, "Einstellungen", icon, DECOR_ALL & ~(DECOR_MENU | DECOR_MAXIMIZE), 100, 100, 250, 250);
    FXint res = dlg.execute(PLACEMENT_SCREEN);
    if (!res) {
        return 0;
    }
    return 1;
}

long CSMap::onFileExportImage(FXObject *, FXSelector, void *)
{
#ifdef WITH_PNG_EXPORT
    FXRegistry &reg = getApp()->reg();
    FXExportDlg exp(this, "Karte exportieren...", icon, DECOR_ALL&~(DECOR_MENU|DECOR_MAXIMIZE), 100, 100, 400, 250);
    exp.loadState(reg);
    FXint res = exp.execute(PLACEMENT_SCREEN);
    if (!res)
        return 0;

    exp.saveState(reg);
    FXint scale = exp.getScale();
    FXint color = exp.getColor();
    FXint options = 0;
    if (exp.getShowNames()) options |= exportNames;
    if (exp.getShowKoords()) options |= exportCoordinates;
    if (exp.getShowIslands()) options |= exportIslands;

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
                ext = pattern.section(',', 0).after('.');
                if (!ext.empty())
                    filename += "." + ext;
                break;
            }

            // Dateiname endet auf ".cr" o.\u00e4.
            if (ext == patt.after('.'))
                break;
        }

        getApp()->beginWaitCursor();
        FXColor rgb = FXRGB(0, 0, 0);
        if (color == 1)    // white background
            rgb = FXRGB(255, 255, 255);
        exportMapFile(filename, scale, rgb, options);
        getApp()->endWaitCursor();
    }
    return 0;
#else
    return FXMessageBox::error(this, MBOX_OK, "Not Implemented",
        "Sorry, this version of CsMap was compiled without PNG support");
#endif
}

long CSMap::onFileRecent(FXObject*, FXSelector, void* ptr)
{
    if (!confirmOverwrite()) {
        return 0;
    }
    FXString filename((const char *)ptr);
    FXint dotPos = filename.rfind('.');
    bool loadReport = true;
    if (dotPos > 0) {
        if ("cr" != filename.mid(dotPos + 1, 2)) {
            loadReport = false;
        }
    }
    recentFiles.removeFile(filename);
    getApp()->beginWaitCursor();
    beginLoading(filename);
    if (loadReport) {
        if (closeFile()) {
            FXString errorMessage;
            loadFile(filename);
            mapChange();
            updateFileNames();
            if (report) {
                addRecentFile(filename);
                checkCommands();
            }
        }
    }
    else {
        if (loadCommands(filename)) {
            addRecentFile(filename);
            checkCommands();
        }
        updateFileNames();
    }
    getApp()->endWaitCursor();
    return 1;
}

long CSMap::onQuit(FXObject*, FXSelector, void* ptr)
{
    close();
    return 1;
}

long CSMap::onRegionSelAll(FXObject*, FXSelector, void*)
{
    if (!report) return 1;
    // alle Regionen markieren

    selection.regionsSelected.clear();

    int visiblePlane = map->getVisiblePlane();

    datablock::itor end = report->blocks().end();
    for (datablock::itor block = report->blocks().begin(); block != end; block++)
    {
        datablock* regionPtr = &*block;
        // handle only regions
        if (regionPtr->type() != block_type::TYPE_REGION)
            continue;

        // mark only visible plane
        if (regionPtr->info() != visiblePlane)
            continue;

        selection.regionsSelected.insert(regionPtr);
    }

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onRegionUnSel(FXObject*, FXSelector, void*)
{
    if (!report) return 1;

    // alle Regionen demarkieren
    selection.regionsSelected.clear();
    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onRegionInvertSel(FXObject*, FXSelector, void*)
{
    if (!report) return 1;
    // Auswahl invertieren
    std::set<datablock*> regionsSelected;

    int visiblePlane = map->getVisiblePlane();

    datablock::itor end = report->blocks().end();

    for (datablock::itor block = report->blocks().begin(); block != end; block++)
    {
        datablock* regionPtr = &*block;
        // handle only regions
        if (regionPtr->type() != block_type::TYPE_REGION)
            continue;

        // mark only visible plane
        if (regionPtr->info() != visiblePlane)
            continue;

        std::set<datablock*>::iterator srch = selection.regionsSelected.find(regionPtr);
        if (srch == selection.regionsSelected.end())
            regionsSelected.insert(regionPtr);
    }

    selection.regionsSelected = regionsSelected;
    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

// each of the six hex directions
static const int hex_offset[6][2] = {
    {0, 1},
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, 0},
    {-1, 1},
};


long CSMap::onRegionExtendSel(FXObject*, FXSelector, void*)
{
    if (!report) return 1;
    if (selection.regionsSelected.empty())
    {
        if (!(selection.selected & selection.REGION)) {
            // nothing to do
            return 1;
        }
        selection.regionsSelected.insert(&*selection.region);
    }

    // Auswahl erweitern um eine Region
    std::set<datablock*> regionsSelected;

    int visiblePlane = map->getVisiblePlane();

    for (std::set<datablock*>::iterator itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
    {
        datablock* blockPtr = *itor;
        // all previously selected regions are selected here, too
        regionsSelected.insert(blockPtr);

        if (blockPtr->info() != visiblePlane)
        {
            // apply other planes without changes
            continue;
        }

        int x = blockPtr->x();
        int y = blockPtr->y();

        for (int d = 0; d != 6; ++d) {
            datablock::itor region;
            if (report->getRegion(region, x + hex_offset[d][0], y + hex_offset[d][1], visiblePlane)) {
                datablock* regionPtr = &*region;
                regionsSelected.insert(regionPtr);
            }
        }
    }
    selection.regionsSelected = regionsSelected;

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onRegionSelIslands(FXObject*, FXSelector, void*)
{
    if (!report) return 1;
    if (selection.regionsSelected.empty())
    {
        if (!(selection.selected & selection.REGION) || (selection.region->terrain() == data::TERRAIN_OCEAN)) {
            // nothing to do
            return 1;
        }
        selection.regionsSelected.insert(&*selection.region);
    }

    // select islands
    int visiblePlane = map->getVisiblePlane();
    bool changed;
    do
    {
        changed = false;

        datablock::itor notfound = report->blocks().end();
        for (std::set<datablock*>::iterator itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); ++itor)
        {
            datablock* blockPtr = *itor;
            int terrain = blockPtr->terrain();
            if (blockPtr->info() != visiblePlane)
            {
                // apply other planes without changes
                continue;
            }

            if (terrain == data::TERRAIN_OCEAN)
            {
                // always ignore ocean
                continue;
            }

            int x = blockPtr->x();
            int y = blockPtr->y();

            // each of the six hex directions
            for (int d = 0; d != 6; ++d) {
                datablock::itor region;
                if (report->getRegion(region, x + hex_offset[d][0], y + hex_offset[d][1], visiblePlane)) {
                    datablock* regionPtr = &*region;
                    if (regionPtr->terrain() != data::TERRAIN_OCEAN) {
                        if ((terrain == data::TERRAIN_FIREWALL) == (regionPtr->terrain() == data::TERRAIN_FIREWALL)) {
                            if (selection.regionsSelected.find(regionPtr) == selection.regionsSelected.end()) {
                                selection.regionsSelected.insert(regionPtr);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changed);

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onRegionSelVisible(FXObject*, FXSelector, void* ptr)
{
    if (!report) return 1;
    // alle Regionen markieren
    selection.regionsSelected.clear();

    int visiblePlane = map->getVisiblePlane();

    for (datablock &block : report->blocks())
    {
        // handle only regions
        if (block.type() != block_type::TYPE_REGION)
            continue;
        // mark only visible plane
        if (block.info() != visiblePlane)
            continue;

        if (!block.hasKey(TYPE_VISIBILITY)) {
            const att_region* att = static_cast<const att_region*>(block.attachment());
            if (!att || att->people.empty()) {
                continue;
            }
        }
        selection.regionsSelected.insert(&block);
    }

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onRegionSelAllIslands(FXObject*, FXSelector, void*)
{
    if (!report) return 1;
    // alle Landregionen markieren (also au\u00dfer Ozean & Feuerwand & Eisberg)
    selection.regionsSelected.clear();

    int visiblePlane = map->getVisiblePlane();

    datablock::itor end = report->blocks().end();
    for (datablock::itor block = report->blocks().begin(); block != end; block++)
    {
        // handle only regions
        if (block->type() != block_type::TYPE_REGION)
            continue;

        // mark only visible plane
        if (block->info() != visiblePlane)
            continue;

        // skip ocean, firewall and iceberg
        if (block->terrain() == data::TERRAIN_OCEAN || block->terrain() == data::TERRAIN_FIREWALL
            || block->terrain() == data::TERRAIN_ICEBERG)
            continue;

        selection.regionsSelected.insert(&*block);
    }

    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    return 1;
}

long CSMap::onRegionRemoveSel(FXObject*, FXSelector, void*)
{
    if (!report) return 1;
    // markierte Regionen l\u00f6schen
    if (selection.regionsSelected.empty())
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

    getApp()->beginWaitCursor();

    // deselect region if it is deleted here
    if (selection.selected & selection.REGION) {
        datablock* regionPtr = &*selection.region;
        if (selection.regionsSelected.find(regionPtr) != selection.regionsSelected.end()) {
            selection.selected &= ~selection.REGION;
            selection.selected |= selection.UNKNOWN_REGION;
            selection.sel_x = regionPtr->x();
            selection.sel_y = regionPtr->y();
            selection.sel_plane = regionPtr->info();
        }
    }

    report->deleteRegions(selection.regionsSelected);
    // Markierung auch loeschen
    selection.regionsSelected.clear();
    getApp()->endWaitCursor();
    ++selection.fileChange;
    handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
    getApp()->endWaitCursor();
    return 1;
}

long CSMap::onShowTab(FXObject *, FXSelector sel, void *)
{
    tabbook->setCurrent(FXSELID(sel) - ID_TAB_1);
    return 1;
}

long CSMap::onHelpAbout(FXObject*, FXSelector, void*)
{
    FXString abouttext = CSMAP_APP_TITLE_VERSION;
    FXString version;

    abouttext.append("\n\n" CSMAP_APP_COPYRIGHT "\nWeb: " CSMAP_APP_URL);
    abouttext.append("\n\nSpecial thanks to Xolgrim for assembling Eressea rules information.\nThanks to all users for suggesting features and finding bugs.");
    abouttext.append("\n\nThis software uses the following libraries:\n");
    abouttext.append(version.format("FOX Toolkit %d.%d.%d  (http://www.fox-toolkit.org)\n", (int)fxversion[0], (int)fxversion[1], (int)fxversion[2]));
    abouttext.append("cparse (https://github.com/cparse/cparse)\n");
#ifdef HAVE_PHYSFS
    PHYSFS_Version linked;
    PHYSFS_getLinkedVersion(&linked);
    abouttext.append(version.format("PhysFS %d.%d.%d\n",
        linked.major, linked.minor, linked.patch));
#endif
#ifdef HAVE_CURL
    abouttext.append(version.format("%s\n", curl_version()));
#endif
#ifdef HAVE_PNG
    abouttext.append(png_get_copyright(NULL));
#endif
    FXMessageBox about(this, "Wer mich schuf...", abouttext, getIcon(), MBOX_OK);

    about.execute(PLACEMENT_SCREEN);

    return 1;
}

FXString CSMap::gameDate(int turn) const
{
    struct season {
        int index;
        int offset;
    } seasons[] = { {1, 3}, {2, 0}, {2, 3}, {3, 0}, {3, 3}, {3, 6}, {0, 0}, {0, 3}, {1, 0} };
    const wchar_t* season_names[] = { L"Fr\u00fchling", L"Sommer", L"Herbst", L"Winter", L"Winter" };
    int since_epoch = (turn < 184) ? turn : (turn - 184);
    int year = 1 + since_epoch / 27;
    int week = since_epoch % 27;
    int month = week / 3;
    int season = seasons[month].index;
    int week_of_month = 1 + seasons[month].offset + week % 3;
    return FXStringVal(week_of_month) + ". Woche im " + season_names[season] + " des Jahres " + FXStringVal(year);
}

bool CSMap::isConfirmed(const datablock::itor& unit)
{
    if (unit->type() == block_type::TYPE_UNIT) {
        return report->isConfirmed(*selection.unit);
    }
    return true;
}

void CSMap::showProperties(FXProperties* item, bool visible) {
    if (visible) {
        item->show();
    }
    else {
        item->hide();
    }
}

void CSMap::beginLoading(const FXString& filename)
{
    FXString app_title = "Lade " + filename + "...";
#if WIN32
    app_title.substitute('/', '\\', true);
#endif
    status->getStatusLine()->setText(app_title);
}

