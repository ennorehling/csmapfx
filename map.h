#ifndef _CSMAP_MAP
#define _CSMAP_MAP

#include "datafile.h"
#include "terrain.h"
#include "mapcanvas.h"

#include <fx.h>

#include <vector>
#include <list>
#include <map>
#include <memory>
#include <functional>


struct LeftTop
{
	FXshort left, top;
};

struct IslandPos
{
	FXint left, top, right, bottom;
};

class FXCSMap : public FXScrollArea
{
	FXDECLARE(FXCSMap)

public:
	FXCSMap(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=(FRAME_SUNKEN|FRAME_THICK), FXbool minimap=false, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXCSMap();

	LeftTop getMapLeftTop() const;
	std::map<FXString, IslandPos> collectIslandNames() const;
	void paintIslandNames(FXDCWindow& dc, FXPoint const& tl, FXPoint const& br, std::map<FXString, IslandPos> const& islands) const;
	void paintMapLines(FXDCWindow& dc, FXPoint const& tl, FXPoint const& br) const;

	virtual void moveContents(FXint x,FXint y);

	FXbool paintMap(FXDrawable* buffer /*, FXRectangle& rect*/);
	void setMapFile(std::shared_ptr<datafile>& f);
	void connectMap(FXCSMap* map);

	void scrollTo(FXint x, FXint y);

	void registerImages();	// create connection b/w image data and images

	void scaleChange(FXfloat sc);	// scales font and images
	FXfloat getScale();

	void setShowNames(bool show);
	void setShowKoords(bool show);
	void setShowIslands(bool show);
	
	FXival getVisiblePlane() const { return visiblePlane; }

	void calculateContentSize();

	void terraform(FXint x, FXint y, FXint plane, FXint terrain);

	FXint sendRouteCmds(const FXString& str, int which);

	FXint getImageWidth() const { return image_w; }
	FXint getImageHeight() const { return image_h; }

    virtual FXint getContentWidth() override { return getImageWidth(); }
    virtual FXint getContentHeight() override { return getImageHeight(); }

    long onMotion(FXObject*,FXSelector,void*);  
	long onLeftButtonPress(FXObject*,FXSelector,void*);  
	long onLeftButtonRelease(FXObject*,FXSelector,void*);  
	long onRightButtonPress(FXObject*,FXSelector,void*);  
	long onRightButtonRelease(FXObject*,FXSelector,void*);  
	long onWheel(FXObject*,FXSelector,void*);  
	long onPaint(FXObject*,FXSelector,void*);
	long onMapChange(FXObject*,FXSelector,void*);
	long onQueryHelp(FXObject*,FXSelector,void*);
	long onKeyPress(FXObject*,FXSelector,void*);

	long onPopup(FXObject*,FXSelector,void*);
	long onPopupClicked(FXObject*,FXSelector,void*);

	long onToggleStreets(FXObject*,FXSelector,void*);
	long onUpdateStreets(FXObject*,FXSelector,void*);
	long onToggleVisibility(FXObject*,FXSelector,void*);
	long onUpdateVisibility(FXObject*,FXSelector,void*);
	long onToggleShipTravel(FXObject*,FXSelector,void*);
	long onUpdateShipTravel(FXObject*,FXSelector,void*);
	long onToggleShadowRegions(FXObject*,FXSelector,void*);
	long onUpdateShadowRegions(FXObject*,FXSelector,void*);
	long onToggleIslands(FXObject*,FXSelector,void*);
	long onUpdateIslands(FXObject*,FXSelector,void*);

	long onSetModus(FXObject*,FXSelector,void*);
	long onUpdSetModus(FXObject*,FXSelector,void*);

	long onSetVisiblePlane(FXObject*,FXSelector,void*);
	long onUpdVisiblePlane(FXObject*,FXSelector,void*);

public: 
	enum
	{
		ID_MAP = FXScrollArea::ID_LAST, 
		ID_TOGGLESTREETS,
		ID_TOGGLEVISIBILITYSYMBOL,
		ID_TOGGLESHIPTRAVEL,
		ID_TOGGLESHADOWREGIONS,
		ID_TOGGLEISLANDS,

		ID_SETMODUS,

		ID_SETVISIBLEPLANE,

		ID_POPUP_CLICKED,
		ID_LAST 
	};

	enum
	{
		MODUS_NORMAL,			// left-click marks region
		MODUS_SELECT,			// left-click selects/deselects regions
		
		MODUS_SETTERRAIN,		// MODUS_SETTERRAIN+x: left-click converts regions to terrain
		/*MODUS_SETTERRAIN+1,
		MODUS_SETTERRAIN+...,*/
	};

protected:
	//Scout scout;

	FXival visiblePlane;	
	FXint cursor_x, cursor_y;	// coordinates of cursor grap for moving map view
	FXint offset_x, offset_y;	// used to center map on screen
	FXint image_w, image_h;		// width and height of the whole map

	datafile::SelectionState selection;
	FXint sel_x, sel_y, sel_plane;	// selected region koordinates
	FXint popup_x, popup_y;			// which region is popup selected

	FXfloat		scale;			// paint map in this scale
    FXival  	modus;			// mouse button modus
	FXint		mouse_select;	// select_set==1: select regions on mouse-over, select_set==2: unselect them

	FXbool		minimap;		// is this map a minimap?
	FXCSMap		*main_map = nullptr;		// pointer to the normal map
	
	FXCanvas	*map = nullptr;			// the bitmap where the image data will be written to
	
	std::unique_ptr<FXImage> backbuffer, imagebuffer;
    std::unique_ptr<FXFont> font, islandfont;	// font used for region names, " for island names

	FXButton	*button = nullptr;

	FXIcon		*terrain[data::TERRAIN_LAST];
	FXIcon		*terrainShadow[data::TERRAIN_LAST];
	FXIcon		*terrainIcons[data::TERRAIN_LAST];
	FXIcon		*activeRegion, *selectedRegion;
	FXIcon		*troopsunknown, *troopally, *troopenemy;
	FXIcon		*guardown, *guardally, *guardenemy, *guardmixed;
	FXIcon		*ship, *castle, *shiptravel, *lighthouse, *travel;
	FXIcon		*castleown, *castleally, *castleenemy, *castlecoins;
	FXIcon		*monster, *seasnake, *dragon, *battle;
	FXIcon		*wormhole;
	FXIcon		*street[6], *street_undone[6];
	FXIcon		*redarrows[6], *greenarrows[6], *bluearrows[6], *greyarrows[6];

	std::vector< std::vector<FXIcon*> > overlays;		// terrain color overlays
	std::vector< FXColor > overlayColors;

	// records icon <-> image data link
	struct IconRecord {
		typedef std::function<FXColor(FXColor)> transform_t;
		FXIcon**				icon;
		const unsigned char*	data;
		int						width, height;
		bool					scaleable;
		transform_t				transform;
	};

	std::vector< IconRecord > iconRecords;

	// what text is shown
	bool		show_koords, show_names, show_islands;

	// what symbols are shown
	bool		show_streets, show_visibility_symbol, show_ship_travel, show_shadow_regions;

	struct arrow
	{
		int x, y;		// coordinates of arrow origin
		int dir;		// direction of arrow
		
		enum			// color of array
		{
			ARROW_RED,
			ARROW_GREEN,
			ARROW_BLUE,
			ARROW_GREY
		} color;
	};
	std::vector<arrow> arrows;
	std::vector<arrow> routeArrows[2];		// saved
    
    std::shared_ptr<datafile> mapFile;

	// Return default width / height
	virtual FXint getDefaultWidth();
	virtual FXint getDefaultHeight();
	// Recalculate layout
	void layout();
	FXint GetHexFromScreenY(FXint scrx, FXint scry) const;
	FXint GetHexFromScreenX(FXint scrx, FXint scry) const;
	FXint GetScreenFromHexY(FXint x, FXint y) const;
	FXint GetScreenFromHexX(FXint x, FXint y) const;

    void updateMap();

protected:
	FXCSMap() {}
	FXCSMap(const FXCSMap&) {}

};

#endif //_CSMAP_MAP
