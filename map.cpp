#include "version.h"
#include "fxhelper.h"
#include "map.h"
#include "symbols.h"

#include <fxkeys.h>
#include <functional>
#include "FXMenuSeparatorEx.h"
#include "csmap.h"
#include "datablock.h"

#include <algorithm>
#include <sstream>
#include <cmath>
#include <climits>

using namespace std::placeholders;

// *********************************************************************************************************
// *** FXCSMap implementation

FXDEFMAP(FXCSMap) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_MOTION,				FXCSMap::ID_MAP,				FXCSMap::onMotion),
	FXMAPFUNC(SEL_LEFTBUTTONPRESS,		FXCSMap::ID_MAP,				FXCSMap::onLeftButtonPress),
	FXMAPFUNC(SEL_LEFTBUTTONRELEASE,	FXCSMap::ID_MAP,				FXCSMap::onLeftButtonRelease),
	FXMAPFUNC(SEL_RIGHTBUTTONPRESS,		FXCSMap::ID_MAP,				FXCSMap::onRightButtonPress),
	FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	FXCSMap::ID_MAP,				FXCSMap::onRightButtonRelease),
	FXMAPFUNC(SEL_MOUSEWHEEL,			FXCSMap::ID_MAP,				FXCSMap::onWheel),
	FXMAPFUNC(SEL_PAINT,				FXCSMap::ID_MAP,				FXCSMap::onPaint),

	FXMAPFUNC(SEL_KEYPRESS,				FXCSMap::ID_MAP,				FXCSMap::onKeyPress),

    FXMAPFUNC(SEL_CONFIGURE,			FXCSMap::ID_MAP,				FXCSMap::onConfigure),

	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_POPUP_CLICKED,		FXCSMap::onPopupClicked),

	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_UPDATE,				FXCSMap::onMapChange),
	FXMAPFUNC(SEL_QUERY_HELP,			0,								FXCSMap::onQueryHelp),

	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_TOGGLESTREETS,		FXCSMap::onToggleStreets),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_TOGGLESTREETS,		FXCSMap::onUpdateStreets),
	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_TOGGLEVISIBILITYSYMBOL,	FXCSMap::onToggleVisibility),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_TOGGLEVISIBILITYSYMBOL,	FXCSMap::onUpdateVisibility),
	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_TOGGLESHIPTRAVEL,	FXCSMap::onToggleShipTravel),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_TOGGLESHIPTRAVEL,	FXCSMap::onUpdateShipTravel),
	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_TOGGLESHADOWREGIONS,	FXCSMap::onToggleShadowRegions),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_TOGGLESHADOWREGIONS,	FXCSMap::onUpdateShadowRegions),
	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_TOGGLEISLANDS,		FXCSMap::onToggleIslands),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_TOGGLEISLANDS,		FXCSMap::onUpdateIslands),

	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_SETMODUS,			FXCSMap::onSetModus),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_SETMODUS,			FXCSMap::onUpdSetModus),

	FXMAPFUNC(SEL_COMMAND,				FXCSMap::ID_SETVISIBLEPLANE,	FXCSMap::onSetVisiblePlane),
	FXMAPFUNC(SEL_UPDATE,				FXCSMap::ID_SETVISIBLEPLANE,	FXCSMap::onUpdVisiblePlane),
};

FXIMPLEMENT(FXCSMap,FXScrollArea,MessageMap, ARRAYNUMBER(MessageMap))

// helper function
void scaleTransformIcon(FXIcon *icon, const unsigned char data[], FXint w, FXint h, FXfloat scale, std::function<FXColor(FXColor)> transform);

FXCSMap::FXCSMap(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXbool minimap, FXint x,FXint y,FXint w,FXint h)
	: FXScrollArea(p, opts, x, y, w, h), minimap(minimap)
{
	// set target and selector
	setTarget(tgt);
	setSelector(sel);

	// createing resources
	font = std::unique_ptr<FXFont>(new FXFont(getApp(), "helvetica", 8));
	islandfont = std::unique_ptr<FXFont>(new FXFont(getApp(), "helvetica", 5*8, FXFont::Bold));

	// create client area
	map = new FXCSMapCanvas(this, this,FXCSMap::ID_MAP, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
    map->disable();
	map->setBackColor(FXRGB(0,0,0));

	verticalScrollBar()->setLine(10);
	horizontalScrollBar()->setLine(10);

	backbuffer = std::unique_ptr<FXImage>(new FXImage(getApp(), nullptr, IMAGE_SHMI|IMAGE_SHMP));
	if (minimap)
		imagebuffer = std::unique_ptr<FXImage>(new FXImage(getApp(), nullptr, IMAGE_SHMI|IMAGE_SHMP));

	// button between the scrollbars, toggling shadowed map mode
	if (!minimap)
	{
		delete corner;
		corner = (FXScrollCorner*)new FXToggleButton(this, "\t\tRegionen abdunkeln, wenn nicht von eigenen Personen gesehen.", "", 0,0, this,ID_TOGGLESHADOWREGIONS, TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
	}

	// and allocate all images
	registerImages();

	for (std::vector<IconRecord>::iterator it = iconRecords.begin(); it != iconRecords.end(); ++it)
		(*it->icon) = new FXGIFIcon(getApp(), 0, 0, IMAGE_ALPHAGUESS);

    // create fonts and image buffers
    font->create();
    islandfont->create();
    backbuffer->create();
    if (imagebuffer)
        imagebuffer->create();

    // create all registered images
    for (std::vector<IconRecord>::iterator it = iconRecords.begin(); it != iconRecords.end(); ++it)
    {
        IconRecord &icon = *it;

        (*icon.icon)->create();	// create image

        // load image data and resize
        if (icon.scaleable)
            scaleTransformIcon(*icon.icon, icon.data, icon.width, icon.height, scale, icon.transform);
        else
            scaleTransformIcon(*icon.icon, icon.data, icon.width, icon.height, 1.0, icon.transform);
    }
}

FXCSMap::~FXCSMap()
{
	// delete all registered images
	for (std::vector<IconRecord>::iterator it = iconRecords.begin(); it != iconRecords.end(); ++it)
		delete (*it->icon);
}

std::vector<FXCSMap::arrow> FXCSMap::makeArrows(int which) const
{
    std::vector<arrow> arrows;
    FXString line = routeString[which];
    line.upper();

    // NW, NO, O, SO, SW, W
    int x_offset[6] = { -1,  0, +1, +1,  0, -1 };
    int y_offset[6] = { +1, +1,  0, -1, -1,  0 };

    FXCSMap::arrow arrow;
    int x = selection.sel_x;
    int y = selection.sel_y;
    int paused = 0;

    while (!(line = line.after(' ').trimBegin()).empty())
    {
        FXString cmd = line.before(' ');
        int dir = -1;

        if (cmd == "NW")
            dir = 0;
        else if (cmd == "NO")
            dir = 1;
        else if (cmd == "O")
            dir = 2;
        else if (cmd == "SO")
            dir = 3;
        else if (cmd == "SW")
            dir = 4;
        else if (cmd == "W")
            dir = 5;
        /*else if (!strncmp(cmd.text(), "NORDWESTEN", cmd.length()))
            dir = 0;
        else if (!strncmp(cmd.text(), "NORDOSTEN", cmd.length()))
            dir = 1;*/
        else if (!strncmp(cmd.text(), "OSTEN", cmd.length()))
            dir = 2;
        /*else if (!strncmp(cmd.text(), "S\u00dcDOSTEN", cmd.length()) || !strncmp(cmd.text(), "SUEDOSTEN", cmd.length()))
            dir = 3;
        else if (!strncmp(cmd.text(), "S\u00dcDWESTEN", cmd.length()) || !strncmp(cmd.text(), "SUEDWESTEN", cmd.length()))
            dir = 4;*/
        else if (!strncmp(cmd.text(), "WESTEN", cmd.length()))
            dir = 5;
        else if (!strncmp(cmd.text(), "PAUSE", cmd.length()))
        {
            paused++;
            continue;
        }
        else if (cmd == "NACH" || cmd == "ROUTE")
        {
            x = selection.sel_x;
            y = selection.sel_y;
            paused = 0;
            continue;
        }
        else
            break;

        arrow.x = x;
        arrow.y = y;
        arrow.dir = dir;
        arrow.color = paused ? arrow::ARROW_GREY : (which ? arrow::ARROW_RED : arrow::ARROW_BLUE);
        arrows.push_back(arrow);

        x += x_offset[dir];
        y += y_offset[dir];
    }
    return arrows;
}

// Get minimum window width/height for layout
FXint FXCSMap::getDefaultWidth()
{
	return FXScrollArea::getDefaultWidth();
}

FXint FXCSMap::getDefaultHeight()
{
	return FXScrollArea::getDefaultHeight();
}

// Determine minimum content width / height of scroll area
void FXCSMap::calculateContentSize()
{
	if (!mapFile)
	{
		image_w = image_h = 0;
		return;
	}

    FXRectangle contentSize = mapFile->getContentSize(visiblePlane);

	FXint min_x = contentSize.x;
	FXint max_x = contentSize.x + contentSize.w;

	FXint min_y = contentSize.y;
	FXint max_y = contentSize.y + contentSize.h;

	offset_x = -FXint(scale * 32 * min_x);
	offset_y = -FXint(scale * 48 * min_y);

	image_w = 32 * (max_x - min_x);
	image_h = 48 * (max_y - min_y) + 16;
    if (image_w > 0 && image_h > 0) {
        map->enable();
    }
    else {
        map->disable();
    }
}

void FXCSMap::layout()
{
	if (!(flags&FLAG_PRESSED)			// don't relayout when we are painting
		|| modus < MODUS_SETTERRAIN || modus >= MODUS_SETTERRAIN+data::TERRAIN_LAST)
	{
		calculateContentSize();
		map->position(0, 0, getWidth(), getHeight());
		map->update();
		backbuffer->resize(getWidth(), getHeight());
	}

	flags&=~FLAG_DIRTY;

	FXScrollArea::layout();
}

long FXCSMap::onToggleStreets(FXObject* /* sender */, FXSelector, void*)
{
	show_streets = !show_streets;

	map->update();
	return 1;
}

long FXCSMap::onUpdateStreets(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, show_streets?ID_CHECK:ID_UNCHECK), nullptr);
	return 1;
}

long FXCSMap::onToggleVisibility(FXObject* /* sender */, FXSelector, void*)
{
	show_visibility_symbol = !show_visibility_symbol;

	map->update();
	return 1;
}

long FXCSMap::onUpdateVisibility(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, show_visibility_symbol?ID_CHECK:ID_UNCHECK), nullptr);
	return 1;
}

long FXCSMap::onToggleShipTravel(FXObject* /* sender */, FXSelector, void*)
{
	show_ship_travel = !show_ship_travel;

	map->update();
	return 1;
}

long FXCSMap::onUpdateShipTravel(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, show_ship_travel?ID_CHECK:ID_UNCHECK), nullptr);
	return 1;
}

long FXCSMap::onToggleShadowRegions(FXObject* /* sender */, FXSelector, void*)
{
	show_shadow_regions = !show_shadow_regions;

	map->update();
	return 1;
}

long FXCSMap::onUpdateShadowRegions(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, show_shadow_regions?ID_CHECK:ID_UNCHECK), nullptr);
	return 1;
}

long FXCSMap::onToggleIslands(FXObject* /* sender */, FXSelector, void*)
{
	show_islands = !show_islands;

    if (imagebuffer) {
        FXDCWindow dc(imagebuffer.get());
        paintMap(dc);
    }

	map->update();
	return 1;
}

long FXCSMap::onUpdateIslands(FXObject* sender, FXSelector, void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, show_islands?ID_CHECK:ID_UNCHECK), nullptr);
	return 1;
}

long FXCSMap::onSetModus(FXObject* sender, FXSelector, void*)
{
	if (!mapFile || !sender || !sender->isMemberOf(&FXId::metaClass))
		return 0;

	FXId *item = (FXId*)sender;
	FXival udata = (FXival)item->getUserData();

	modus = udata;		// set new modus
	return 1;
}

long FXCSMap::onUpdSetModus(FXObject* sender, FXSelector, void*)
{
	if (!sender || !sender->isMemberOf(&FXId::metaClass))
		return 0;

	FXId *item = (FXId*)sender;
	FXival udata = (FXival)item->getUserData();

	// check button, if actual modus is button's modus
	sender->handle(this, FXSEL(SEL_COMMAND, (udata==modus)?ID_CHECK:ID_UNCHECK), nullptr);
	return 1;
}

long FXCSMap::onSetVisiblePlane(FXObject* sender, FXSelector, void* ptr)
{
    FXival plane = visiblePlane;

	if (sender && sender->isMemberOf(&FXListBox::metaClass))
	{
		FXListBox *box = (FXListBox*)sender;
        	plane = (FXival)box->getItemData((long)ptr);
	}
	else if (sender && sender->isMemberOf(&FXId::metaClass))
	{
		FXId *item = (FXId*)sender;
		plane = (FXival)item->getUserData();
	}

	if (plane < 0 || visiblePlane == plane)
		return 1;

	visiblePlane = plane;

	if (minimap)
	{
        FXint w = getWidth();
        FXint h = getHeight();
		scale = 1.0f;
		calculateContentSize();

		while (scale >= 2/64.0f && (getImageWidth() > w || getImageHeight() > h))
		{
			scale /= 2;
			image_w /= 2;
			image_h /= 2;
		}

        // resize (& layout() & calculateContentSize())
        image_w = image_h = 0;
		scaleChange(scale);

		// paint map into buffer
		imagebuffer->resize(getImageWidth(), getImageHeight());
        FXDCWindow dc(imagebuffer.get());
		paintMap(dc);	// minimap paints map only when changed data
	}
    else {
        layout();
    }


	scrollTo(sel_x, sel_y);

	map->update();
	return 1;
}

long FXCSMap::onUpdVisiblePlane(FXObject* sender, FXSelector, void*)
{
	if (!sender)
		return 0;

	if (sender->isMemberOf(&FXListBox::metaClass))
	{
		FXListBox *box = (FXListBox*)sender;
		FXint item = box->findItemByData((void*)visiblePlane);
		if (item < 0)
			item = 0;
        box->setCurrentItem(item);
	}
	else if (sender->isMemberOf(&FXId::metaClass))
	{
		FXId *item = (FXId*)sender;
		FXival plane = (FXival)item->getUserData();
		sender->handle(this, FXSEL(SEL_COMMAND, (visiblePlane == plane)?ID_CHECK:ID_UNCHECK), nullptr);
	}

	return 1;
}

FXint FXCSMap::GetHexFromScreenY(FXint scrx, FXint scry) const
{
	// Berechne y-Koordinate
	float fy = (scry) / 48.0f/scale;
	FXint y = (FXint)floorf(fy);

    // Pruefe, ob Koordinate an schraeger Kante eines Hexfeldes
    float fx = (scrx) / 64.0f/scale + y/2.0f + 0.5f;

	if (fy - y < 0.3f - 0.6f * fabs(fx - floorf(fx) - 0.5f))
		y--;

	return -y;
}

FXint FXCSMap::GetHexFromScreenX(FXint scrx, FXint scry) const
{
	return (FXint)floorf(scrx / 64.0f/scale - GetHexFromScreenY(scrx, scry)/2.0f);
}

FXint FXCSMap::GetScreenFromHexY(FXint /*x*/, FXint y) const
{
	return (FXint)(-y*48);

}

FXint FXCSMap::GetScreenFromHexX(FXint x, FXint y) const
{
	return (FXint)(x*64+ y*32);
}

FXPoint FXCSMap::GetScreenOffset() const
{
    FXint scr_offset_x = offset_x;
    FXint scr_offset_y = offset_y;

    if (!minimap)
    {
        scr_offset_x += pos_x;
        scr_offset_y += pos_y;

        if (getWidth() > getImageWidth())	scr_offset_x += (getWidth() - getImageWidth()) / 2;
        if (getHeight() > getImageHeight())	scr_offset_y += (getHeight() - getImageHeight()) / 2;
    }
    return FXPoint(scr_offset_x, scr_offset_y);
}

// Move content
void FXCSMap::moveContents(FXint x,FXint y)
{
	//FXint dx=x-pos_x;
	//FXint dy=y-pos_y;
	//scroll(0,0,viewport_w,viewport_h,dx,dy);

	// set new position and update map
	pos_x=x;
	pos_y=y;
    repaint = true;
	map->update();
}

void FXCSMap::setMapFile(std::shared_ptr<datafile>& f)
{
    mapFile = f;
    if (!f) {
        sel_x = sel_y = offset_x = offset_y = 0;
    }
    layout();
}

void FXCSMap::connectMap(FXCSMap* a_map)
{
	main_map = a_map;
}

void FXCSMap::scrollTo(FXint x, FXint y)
{
	// calculate 'screen' position of region coordinates
	FXint scr_x = getWidth() / 2 - (FXint(scale * (32 + GetScreenFromHexX(x, y))) + offset_x);
	FXint scr_y = getHeight() / 2 - (FXint(scale * (32 + GetScreenFromHexY(x, y))) + offset_y);

	if (getWidth() > getImageWidth())	scr_x -= (getWidth() - getImageWidth())/2;
	if (getHeight() > getImageHeight())	scr_y -= (getHeight() - getImageHeight())/2;

	// is region currently visible (in view pane)?
	FXint diff_x = scr_x - pos_x; if (diff_x < 0) diff_x = -diff_x;
	FXint diff_y = scr_y - pos_y; if (diff_y < 0) diff_y = -diff_y;

	// if not, scroll to this region (centered)
	if (diff_x > getWidth()/2 || diff_y > getHeight()/2)
		setPosition(scr_x, scr_y);
}

// functions that scale and transform icons
// ----------------------------------------

struct colorTransform {
	typedef FXColor result_type;
	FXColor operator()(FXColor col) const {
		return col;
	}
};

// overlays pixels with an overlay color
struct overlayPixelColor : colorTransform {

	overlayPixelColor(double opaque, FXColor overlay) {
		opaque_ = std::max(0.0, std::min(1.0, opaque));

		red_    = (FXuchar)((1 - opaque_) * FXREDVAL(overlay));
		green_  = (FXuchar)((1 - opaque_) * FXGREENVAL(overlay));
		blue_   = (FXuchar)((1 - opaque_) * FXBLUEVAL(overlay));
	}

	FXColor operator()(FXColor col) const {
		return FXRGB(
			FXREDVAL(col)*opaque_ + red_,
			FXGREENVAL(col)*opaque_ + green_,
			FXBLUEVAL(col)*opaque_ + blue_);
	}

private:
	double opaque_;
	FXuchar red_, green_, blue_;
};

// removes hue from color, painting it in shades of grey
struct greyPixelColor : colorTransform {
	FXColor operator()(FXColor col) const {
		FXuchar light = (FXREDVAL(col) + FXGREENVAL(col) + FXBLUEVAL(col)) / 3;
		return FXRGB( light, light, light );
	}
};

// rotating color parts, RGB -> BRG (applied once) or GBR (applied twice)
struct rotateHuePixelColor : colorTransform {
	FXColor operator()(FXColor col) const {
		return FXRGB( FXBLUEVAL(col), FXREDVAL(col), FXGREENVAL(col) );
	}
};

// transforms all non-transparent pixels of an icon
void transformIcon(FXIcon& icon, std::function<FXColor(FXColor)> transform)
{
	FXColor transparent = icon.getTransparentColor();

	int width = icon.getWidth();
	int height = icon.getHeight();

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			FXColor col = icon.getPixel(x,y);
			if (col != transparent)
				icon.setPixel(x,y, transform(col));
		}
}

void scaleTransformIcon(FXIcon* icon, const unsigned char data[], FXint w, FXint h, FXfloat scale, std::function<FXColor(FXColor)> transform)
{
	FXMemoryStream ms;		// memory stream for loading bitmap data
    ms.open(FXStreamLoad,(FXuchar*)data);
	icon->resize(w,h);
	icon->loadPixels(ms);
	transformIcon(*icon, transform);
	icon->scale((FXint)floorf(w*scale),(FXint)floorf(h*scale));
	icon->render();
    ms.close();
}

void FXCSMap::registerImages()
{
	// overlay colors
	FXColor colors[] =
	{
		FXRGB(255,0,0),		// red
		FXRGB(0,255,0),		// green
		FXRGB(0,0,255),		// blue
		FXRGB(255,255,0),	// yellow
		FXRGB(255,0,255),	// magenta
		FXRGB(0,255,255),	// cyan
	};

	overlayColors.assign( begin(colors), end(colors) );

	// default transformations
	bool non_scaleable = false, scaleable = true;
	IconRecord::transform_t no_transform = colorTransform();
	IconRecord::transform_t shadow_transform = overlayPixelColor(0.5, FXRGB(0,0,0));
	IconRecord::transform_t grey_transform = greyPixelColor();
	IconRecord::transform_t green_transform = rotateHuePixelColor();
	IconRecord::transform_t blue_transform = std::bind(rotateHuePixelColor(), std::bind(rotateHuePixelColor(), _1));

	// selection marks image data
	IconRecord selectionMarks[] = {
		&activeRegion, data::active, 64, 64, scaleable, no_transform,
		&selectedRegion, data::selected, 64, 64, scaleable, no_transform,
	};

	// special region symbols; not needed in minimap mode
	IconRecord regionSymbols[] = {
		// information symbols
		&troopsunknown, data::troopsunknown, 32, 32, scaleable, no_transform,
		&troopally, data::troopally, 32, 32, scaleable, no_transform,
		&troopenemy, data::troopenemy, 32, 32, scaleable, no_transform,
		&ship, data::ship, 32, 32, scaleable, no_transform,
		&castle, data::castle, 32, 32, scaleable, no_transform,
		&shiptravel, data::shiptravel, 32, 32, scaleable, no_transform,
		&lighthouse, data::lighthouse, 32, 32, scaleable, no_transform,
		&travel, data::travel, 32, 32, scaleable, no_transform,
		&monster, data::monster, 32, 32, scaleable, no_transform,
		&battle, data::battle, 32, 32, scaleable, no_transform,
		&seasnake, data::seasnake, 32, 32, scaleable, no_transform,
		&dragon, data::dragon, 32, 32, scaleable, no_transform,
		&wormhole, data::wormhole, 32, 32, scaleable, no_transform,

		&guardown, data::eye, 32, 32, scaleable, blue_transform,
		&guardally, data::eye, 32, 32, scaleable, green_transform,
		&guardenemy, data::eye, 32, 32, scaleable, no_transform,
		&guardmixed, data::eye_mixed, 32, 32, scaleable, no_transform,

		&castleown, data::castle_owned, 32, 32, scaleable, blue_transform,
		&castleally, data::castle_owned, 32, 32, scaleable, green_transform,
		&castleenemy, data::castle_owned, 32, 32, scaleable, no_transform,
		&castlecoins, data::coins, 32, 32, scaleable, no_transform,

		// street symbols
		&street[0], data::street_nw, 64, 64, scaleable, no_transform,
		&street[1], data::street_no, 64, 64, scaleable, no_transform,
		&street[2], data::street_o, 64, 64, scaleable, no_transform,
		&street[3], data::street_so, 64, 64, scaleable, no_transform,
		&street[4], data::street_sw, 64, 64, scaleable, no_transform,
		&street[5], data::street_w, 64, 64, scaleable, no_transform,

		&street_undone[0], data::street_undone_nw, 64, 64, scaleable, no_transform,
		&street_undone[1], data::street_undone_no, 64, 64, scaleable, no_transform,
		&street_undone[2], data::street_undone_o, 64, 64, scaleable, no_transform,
		&street_undone[3], data::street_undone_so, 64, 64, scaleable, no_transform,
		&street_undone[4], data::street_undone_sw, 64, 64, scaleable, no_transform,
		&street_undone[5], data::street_undone_w, 64, 64, scaleable, no_transform,

		// traveller arrows
		&redarrows[0], data::redarrow_nw, 35, 35, scaleable, no_transform,
		&redarrows[1], data::redarrow_no, 35, 35, scaleable, no_transform,
		&redarrows[2], data::redarrow_o, 35, 35, scaleable, no_transform,
		&redarrows[3], data::redarrow_so, 35, 35, scaleable, no_transform,
		&redarrows[4], data::redarrow_sw, 35, 35, scaleable, no_transform,
		&redarrows[5], data::redarrow_w, 35, 35, scaleable, no_transform,

		&greyarrows[0], data::redarrow_nw, 35, 35, scaleable, grey_transform,
		&greyarrows[1], data::redarrow_no, 35, 35, scaleable, grey_transform,
		&greyarrows[2], data::redarrow_o, 35, 35, scaleable, grey_transform,
		&greyarrows[3], data::redarrow_so, 35, 35, scaleable, grey_transform,
		&greyarrows[4], data::redarrow_sw, 35, 35, scaleable, grey_transform,
		&greyarrows[5], data::redarrow_w, 35, 35, scaleable, grey_transform,

		&greenarrows[0], data::redarrow_nw, 35, 35, scaleable, green_transform,
		&greenarrows[1], data::redarrow_no, 35, 35, scaleable, green_transform,
		&greenarrows[2], data::redarrow_o, 35, 35, scaleable, green_transform,
		&greenarrows[3], data::redarrow_so, 35, 35, scaleable, green_transform,
		&greenarrows[4], data::redarrow_sw, 35, 35, scaleable, green_transform,
		&greenarrows[5], data::redarrow_w, 35, 35, scaleable, green_transform,

		&bluearrows[0], data::redarrow_nw, 35, 35, scaleable, blue_transform,
		&bluearrows[1], data::redarrow_no, 35, 35, scaleable, blue_transform,
		&bluearrows[2], data::redarrow_o, 35, 35, scaleable, blue_transform,
		&bluearrows[3], data::redarrow_so, 35, 35, scaleable, blue_transform,
		&bluearrows[4], data::redarrow_sw, 35, 35, scaleable, blue_transform,
		&bluearrows[5], data::redarrow_w, 35, 35, scaleable, blue_transform,
	};

	// insert static images
	iconRecords.clear();
	iconRecords.insert( iconRecords.end(), begin(selectionMarks), end(selectionMarks) );
	if (!minimap)
		iconRecords.insert( iconRecords.end(), begin(regionSymbols), end(regionSymbols) );

    // terrains with overlay
    overlays.resize(overlayColors.size(), std::vector<FXIcon*>(data::TERRAIN_LAST));

    // terrain images
	for (int i = 0; i < data::TERRAIN_LAST; i++)
	{
        const unsigned char *gif_data = data::terrain_data(i);
        const unsigned char* gif_icon = data::terrain_icon(i);
		IconRecord image = { &terrain[i], gif_data, 64, 64, scaleable, no_transform };
		IconRecord shadow_image = { &terrainShadow[i], gif_data, 64, 64, scaleable, shadow_transform };
		IconRecord small_image = { &terrainIcons[i], gif_icon, 16, 16, non_scaleable, no_transform };

		iconRecords.push_back(image);
		iconRecords.push_back(shadow_image);

        if (!minimap) {
            iconRecords.push_back(small_image);		// terrain symbols for popup menu
        }

        for (size_t ov = 0; ov < overlays.size(); ov++)
	    {
		    IconRecord::transform_t colorOverlay = overlayPixelColor(0, overlayColors[ov]);
            IconRecord icon = { &overlays[ov][i], gif_data, 64, 64, scaleable, colorOverlay };
			iconRecords.push_back(icon);
		}
	}
}

#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : (((hi) < (lo)) ? (hi) : (v)))

void FXCSMap::scaleChange(FXfloat new_scale)
{
	// scale must be in range [1/64, 2]
	new_scale = CLAMP(new_scale, 1/64.0f, 2.0f);

	if (image_w != 0 && scale == new_scale)	// don't resize stuff when not necessary.
		return;
	scale = new_scale;

	if (!minimap)
	{
		// change font size
		FXFontDesc fontdesc;
		font->destroy();
		font->getFontDesc(fontdesc);
		fontdesc.size = (FXushort)(80*scale);
		if (fontdesc.size < 8)
			fontdesc.size = 8;	// linux seems to have problems with fontsize smaller 8
		font->setFontDesc(fontdesc);
		font->create();
	}

	// minimap has island font
	{
		// change islandfont size
		FXFontDesc fontdesc;
		islandfont->destroy();
		islandfont->getFontDesc(fontdesc);
		fontdesc.size = (FXushort)(5*80*std::sqrt(scale));
		if (fontdesc.size < 8)
			fontdesc.size = 8;	// linux seems to have problems with fontsize smaller 8
		islandfont->setFontDesc(fontdesc);
		islandfont->create();
	}

	// rescale all scaleable icons
	for (std::vector<IconRecord>::iterator it = iconRecords.begin(); it != iconRecords.end(); ++it)
	{
		IconRecord& icon = *it;
		if (icon.scaleable)		// skip non-scaleable icons
			scaleTransformIcon(*icon.icon, icon.data, icon.width, icon.height, scale, icon.transform);
	}

	// relayout and repaint
	layout();
	scrollTo(sel_x, sel_y);
}

FXfloat FXCSMap::getScale()
{
	return scale;
}

void FXCSMap::setShowNames(bool show)
{
    show_names = show;
}

void FXCSMap::setShowKoords(bool show)
{
	show_koords = show;
}

void FXCSMap::setShowIslands(bool show)
{
	show_islands = show;
}

// Moving
long FXCSMap::onMotion(FXObject*,FXSelector,void* ptr)
{
	FXEvent *event = (FXEvent*)ptr;

	if (flags&FLAG_SCROLLING)
	{
        setPosition(event->last_x - cursor_x, event->last_y - cursor_y);
        repaint = true;
//        map->update();
		return 1;
	}

	if (flags&FLAG_PRESSED)
	{
		cursor_x = event->last_x - pos_x - offset_x;
		cursor_y = event->last_y - pos_y - offset_y;

		// calculate map offset
		if (getWidth() > getImageWidth())	cursor_x -= (getWidth() - getImageWidth())/2;
		if (getHeight() > getImageHeight())	cursor_y -= (getHeight() - getImageHeight())/2;

		// minimap: set position to mouse-click coordinates
		if (main_map)
		{
			// calculate position in main map
			float fscale = main_map->getScale() / getScale();

			cursor_x = -int(cursor_x * fscale) - main_map->offset_x + main_map->getWidth()/2;
			cursor_y = -int(cursor_y * fscale) - main_map->offset_y + main_map->getHeight()/2;

			main_map->setPosition(cursor_x, cursor_y);
            main_map->repaint = true;
            map->update();
			return 1;
		}

		// normal map: select or transform clicked region
		FXint x = GetHexFromScreenX(cursor_x, cursor_y);
		FXint y = GetHexFromScreenY(cursor_x, cursor_y);

		if (modus >= MODUS_SETTERRAIN && modus < MODUS_SETTERRAIN+data::TERRAIN_LAST)
		{
			// in SETTERRAIN modus, every click transforms region
            terraform(x, y, visiblePlane, modus-MODUS_SETTERRAIN);
			return 1;
		}

		// set mark cursor
		if (mapFile)
		{
            if (mapFile->getRegion(selection.region, x, y, visiblePlane)) {
                selection.selected = selection.REGION;
                selection.selected &= ~selection.UNKNOWN_REGION;

                if ((event->state & CONTROLMASK) || modus == MODUS_SELECT)
				{
					// already selected? then deselect the region
					std::set<datablock*>::iterator itor = selection.regionsSelected.find(&*selection.region);
					if (itor == selection.regionsSelected.end())
					{
						if (mouse_select == 0 || mouse_select == 1)		// select on mouse-over
						{
                            selection.regionsSelected.insert(&*selection.region);
							mouse_select = 1;
						}
					}
					else
					{
						if (mouse_select == 0 || mouse_select == 2)		// unselect on mouse-over
						{
                            selection.regionsSelected.erase(itor);
							mouse_select = 2;
						}
					}
				}

            }
            else {
				// mark unknown region (no region-block in report)
                selection.sel_x = x, selection.sel_y = y, selection.sel_plane = visiblePlane;
                selection.selected = selection.UNKNOWN_REGION;
                selection.selected &= ~selection.REGION;
			}
			getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
		}
		return 1;
	}

	return 1;	// also return 1 to force update of statusbar
}

long FXCSMap::onLeftButtonPress(FXObject* sender,FXSelector sel,void* ptr)
{
	if (map->isEnabled())
	{
		map->grab();					// grab mouse input.
		flags |= FLAG_PRESSED;

		mouse_select = 0;				// yet undefined if mouse-over selects or unselects regions

		onMotion(sender, sel, ptr);		// forward to onMotion, where minimap and normal map is handled.
		return 1;
	}

	return 0;
}

long FXCSMap::onLeftButtonRelease(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/)
{
	if (map->isEnabled())
	{
		map->ungrab();
		flags &= ~FLAG_PRESSED;

		// when in painting mode, relayout should not be done when button is pressed, so do it on buttonrelease
		if (modus >= MODUS_SETTERRAIN && modus < MODUS_SETTERRAIN+ data::TERRAIN_LAST)
		{
            if (mapFile) {
                selection.selected &= ~(selection.REGION | selection.UNKNOWN_REGION);
                ++selection.fileChange;
                getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
            }
		}
		return 1;
	}
	return 0;
}

long FXCSMap::onRightButtonPress(FXObject* /*sender*/, FXSelector /*sel*/, void* ptr)
{
	if (map->isEnabled())
	{
        map->grab();

		FXEvent *event = (FXEvent*)ptr;

		cursor_x = event->click_x - pos_x;
		cursor_y = event->click_y - pos_y;

		flags |= FLAG_SCROLLING;
		return 1;
	}
	return 0;
}

long FXCSMap::onRightButtonRelease(FXObject* sender,FXSelector sel,void* ptr)
{
	if (map->isEnabled())
	{
        map->ungrab();
		flags &= ~FLAG_SCROLLING;

		FXEvent *event = (FXEvent*)ptr;

		// dont't show popup if mouse has moved
		if (event->last_x == event->click_x && event->last_y == event->click_y)
			return onPopup(sender, sel, ptr);

		return 1;
	}
	return 0;
}

long FXCSMap::onWheel(FXObject* /*sender*/, FXSelector /*sel*/, void* ptr)
{
	FXEvent *ev = (FXEvent*)ptr;

	if (map->isEnabled())
	{
		// zoom
		if(ev->state&CONTROLMASK)
		{
			FXfloat jump = 1.2f;
			if(ev->state&SHIFTMASK)
				jump = 2.0f;

			FXint pixel = FXint(scale*64.0f), altpixel = pixel;

			if (ev->code > 0)
			{
				pixel = FXint(pixel*jump);
				if (pixel == altpixel)
					pixel++;
			}
			else
				pixel = FXint(pixel/jump);

			if (!minimap)
				scaleChange(pixel/64.0f);		// can't zoom in minimap
			return 1;
		}

		FXint x, y;
		getPosition(x, y);

		FXint offset = ((ev->state&ALTMASK) ? map->getWidth() : map->getHeight()) / 5;
		if(ev->state&SHIFTMASK)
			offset = 5*offset;
		if (ev->code < 0)
			offset = -offset;

		if(ev->state&ALTMASK)
				x += offset;
		else
				y += offset;

		setPosition(x, y);
		map->update();

        // scroll?
		return 1;
	}

	return 0;
}

enum
{
	POPUP_RENAME,
	POPUP_TERRAFORM,
	POPUP_SETISLAND,

	POPUP_LAST
};

long FXCSMap::onPopup(FXObject* /*sender*/, FXSelector /*sel*/, void* ptr)
{
	FXEvent *event = (FXEvent*)ptr;

	cursor_x = event->last_x - pos_x;
	cursor_y = event->last_y - pos_y;

	if (getWidth() > getImageWidth())	cursor_x -= (getWidth() - getImageWidth())/2;
	if (getHeight() > getImageHeight())	cursor_y -= (getHeight() - getImageHeight())/2;

	popup_x = GetHexFromScreenX(cursor_x-offset_x, cursor_y-offset_y);
	popup_y = GetHexFromScreenY(cursor_x-offset_x, cursor_y-offset_y);

	if (minimap)
		return 1;

	if (mapFile)
	{
        FXMenuPane pane(this);
		if (!selection.regionsSelected.empty())
		{
			// Popup-Menu fuer mehrere Regionen
			FXString label;
			label.format("%zu Regionen", selection.regionsSelected.size());

			new FXMenuSeparatorEx(&pane, label);
			FXMenuCommand *cmd;

			cmd = new FXMenuCommand(&pane, "&Insel benennen...", nullptr, this, ID_POPUP_CLICKED);
			cmd->setUserData((void*)POPUP_SETISLAND);

			FXMenuPane* terraform = new FXMenuPane(this);

			datablock terrainRegion;
			for (FXuval i = 1; i <  data::TERRAIN_LASTPUBLIC+1; i++)
			{
				terrainRegion.terrain(i);
				cmd = new FXMenuCommand(terraform, terrainRegion.terrainString(), terrainIcons[i], this,ID_POPUP_CLICKED);
				cmd->setUserData((void*)(FXuval)MKUINT(POPUP_TERRAFORM, i));
			}

			//new FXMenuSeparatorEx(terraform);
			//cmd = new FXMenuCommand(terraform, FXString("L\u00f6schen"), terrainIcons[0], this,ID_POPUP_CLICKED);
			//cmd->setUserData((void*)MKUINT(POPUP_TERRAFORM, 0));

			new FXMenuCascade(&pane, "&Terraformen", terrainIcons[0], terraform);

			// show popup
			pane.create();
            pane.popup(nullptr, event->root_x,event->root_y);

			getApp()->runModalWhileShown(&pane);

			return 1;
		}

        datablock::itor region;
        if (mapFile->getRegion(region, popup_x, popup_y, visiblePlane)) {
            // create popup
			FXString label, name, terrainString = region->terrainString();

			name = region->value(TYPE_NAME);
			if (name.empty())
				label.format("%s (%d,%d)", terrainString.text(), popup_x,popup_y);
			else
				label.format("%s (%d,%d)", name.text(), popup_x,popup_y);

			FXMenuCommand *cmd;

			// clipboard submenu

            CSMap* csmap = static_cast<CSMap*>(getShell());
            csmap->addClipboardPane(&pane, &*region);

            new FXMenuSeparatorEx(&pane, label);

            // normal menu
			cmd = new FXMenuCommand(&pane, "&Insel benennen...", nullptr, this,ID_POPUP_CLICKED);
			cmd->setUserData((void*)POPUP_SETISLAND);

			cmd = new FXMenuCommand(&pane, "Um&benennen...", nullptr, this,ID_POPUP_CLICKED);
			cmd->setUserData((void*)POPUP_RENAME);

			FXMenuPane terraformPane(this);

			datablock terrainRegion;
			for (FXuval i = 1; i <  data::TERRAIN_LASTPUBLIC+1; i++)
			{
				terrainRegion.terrain(i);
				cmd = new FXMenuCommand(&terraformPane, terrainRegion.terrainString(), terrainIcons[i], this,ID_POPUP_CLICKED);
				cmd->setUserData((void*)(FXuval)MKUINT(POPUP_TERRAFORM, i));
			}

			new FXMenuSeparatorEx(&terraformPane);
			cmd = new FXMenuCommand(&terraformPane, FXString(L"L\u00f6schen"), terrainIcons[0], this,ID_POPUP_CLICKED);
			cmd->setUserData((void*)(FXuval)MKUINT(POPUP_TERRAFORM, 0));

			new FXMenuCascade(&pane, "&Terraformen", terrainIcons[region->terrain()], &terraformPane);

			// show popup
            pane.create();
            pane.popup(nullptr, event->root_x,event->root_y);

			getApp()->runModalWhileShown(&pane);

			return 1;
		}
	}

	// no region clicked. create popup for new region
	// create popup
	FXMenuPane *menu = new FXMenuPane(this);

	FXString label;
	label.format("Unbekannt (%d,%d)", popup_x, popup_y);

	new FXMenuSeparatorEx(menu, label);

	FXMenuPane* terraform = new FXMenuPane(this);
	FXMenuCommand *cmd;

	datablock terrainRegion;
	for (FXuval i = 1; i <  data::TERRAIN_LASTPUBLIC+1; i++)
	{
		terrainRegion.terrain(i);
		cmd = new FXMenuCommand(terraform, terrainRegion.terrainString(), terrainIcons[i], this,ID_POPUP_CLICKED);
		cmd->setUserData((void*)(FXuval)MKUINT(POPUP_TERRAFORM, i));
	}

	new FXMenuCascade(menu, "&Neue Region", terrainIcons[0], terraform);

	// show popup
	menu->create();
	menu->popup(nullptr, event->root_x,event->root_y);

	getApp()->runModalWhileShown(menu);
	delete menu;

	return 1;
}

long FXCSMap::onPopupClicked(FXObject* sender, FXSelector /*sel*/, void* /*ptr*/)
{
	if (sender && sender->isMemberOf(&FXMenuCommand::metaClass))
	{
		FXMenuCommand *item = (FXMenuCommand*)sender;

		// extract popup cmd
        FXival cmd = 0xffff & (FXival)item->getUserData();
		FXival param = 0xffff & (FXival)item->getUserData() >> 16;

		if (cmd == POPUP_TERRAFORM)
		{
			terraform(popup_x, popup_y, visiblePlane, param);
			return 1;
		}

		if (!selection.regionsSelected.empty())
		{
			if (cmd == POPUP_SETISLAND)
			{
				FXInputDialog input(this, "Insel benennen", "Wie soll die Insel heissen?");
				input.setText("");

				if (input.execute(PLACEMENT_SCREEN))
				{
					datablock* region = *selection.regionsSelected.begin();

					// change island name
                    if (input.getText().empty()) {
                        region->removeKey(TYPE_ISLAND);
                    }
                    else {
                        region->setKey(TYPE_ISLAND, input.getText());
                    }

                    // delete Insel key from other regions
					std::set<datablock*>::iterator itor = selection.regionsSelected.begin();
					for (itor++; itor != selection.regionsSelected.end(); itor++)
					{
						region = *itor;
                        region->removeKey(TYPE_ISLAND);
					}

                    // islands changed, rebuild:
                    if (mapFile) {
                        mapFile->createIslands();

                        ++selection.fileChange;
                        getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
                    }
				}
				return 1;
			}

			return 1;
		}

		if (mapFile)
		{
            datablock::itor region;
            if (mapFile->getRegion(region, popup_x, popup_y, visiblePlane)) {
                if (cmd == POPUP_SETISLAND) {
					FXInputDialog input(this, "Insel benennen", "Wie soll die Insel heissen?");
					input.setText(region->value(TYPE_ISLAND));

					if (input.execute(PLACEMENT_SCREEN))
					{
                        const FXString& name = input.getText();
						// change island name
                        if (name.empty()) {
                            region->removeKey(TYPE_ISLAND);
                        }
                        else {
                            region->setKey(TYPE_ISLAND, name);
                        }

						// islands changed, rebuild:
                        mapFile->createIslands();

                        ++selection.fileChange;
                        getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
					}
					return 1;
				}
				else if (cmd == POPUP_RENAME)
				{
					FXInputDialog input(this, "Region umbenennen", "Wie soll die Region heissen?");
					input.setText(region->value(TYPE_NAME));

					if (input.execute(PLACEMENT_SCREEN))
					{
						// change name
                        const FXString& name = input.getText();
                        if (name.empty())
                            region->removeKey(TYPE_NAME);
                        else
                            region->setKey(TYPE_NAME, name);
                        
                        // map changed
                        ++selection.fileChange;
						getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
					}
					return 1;
				}
			}
		}
	}

	return 0;
}

long FXCSMap::onConfigure(FXObject *, FXSelector, void *)
{
    // resize event
    repaint = true;
    return 0;
}

void FXCSMap::terraform(FXint x, FXint y, FXint plane, FXint new_terrain)
{
	if (!mapFile) {
		return;
	}

	if (!selection.regionsSelected.empty())
	{
		// del or terraform regions in list
		for (datablock* region : selection.regionsSelected)
		{
			// terraform: change terrain of region
			region->terrain(new_terrain);

			// delete ;terrain tags (they are for special terrain names)
            region->removeKey(TYPE_TERRAIN);
		}
	}

	// don't use hash table only since it might be out-of-date
    datablock::itor end = mapFile->blocks().end();
	datablock::itor iregion = end;

    // TODO: I don't understand what new_terrain is for (enno).
    if (new_terrain) {			// speeds things up but only works when we don't delete
        mapFile->getRegion(iregion, x, y, plane);
    }

	if (iregion == end)
	{
		for (iregion = mapFile->blocks().begin(); iregion != end; iregion++)
		{
			if (iregion->type() != block_type::TYPE_REGION)
				continue;

			if (iregion->x() == x && iregion->y() == y && iregion->info() == plane)
				break;		// found!
		}
	}

	if (iregion != end)
	{
		// found another region. change or delete
		if (new_terrain)
		{
			// terraform: change terrain of region
            iregion->terrain(new_terrain);

			// delete ;terrain tags (they are for special terrain names)
            iregion->removeKey(TYPE_TERRAIN);
		}
		else
		{
			// delete region
            if (mapFile->deleteRegion(*iregion)) {

                if (selection.selected & selection.REGION)
				{
					// deleted region should not be selected
					if (selection.region == iregion)
						selection.selected &= ~selection.REGION;
				}
				if (!selection.regionsSelected.empty())
				{
					// markierte Regionenen aus der Markierung nehmen
					std::set<datablock*>::iterator itor;
					for (itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
						if (*itor == &*selection.region)
						{
							selection.regionsSelected.erase(itor);
							break;
						}
				}
			}
		}
	}
	else
	{
		if (!new_terrain)	// don't add new region if it should be deleted.
			return;

        // append new region of given terrain
		datablock region;
		region.string("REGION");
		region.infostr(FXStringFormat("%d %d %d", x, y, plane));
		region.terrain(new_terrain);
		mapFile->addRegion(region);
	}

	// map changed
	if (!(flags&FLAG_PRESSED) || modus < MODUS_SETTERRAIN || modus >= MODUS_SETTERRAIN+ data::TERRAIN_LAST)
	{
        if (mapFile) {
            selection.selected &= ~(selection.REGION | selection.UNKNOWN_REGION);
            ++selection.fileChange;
            getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
        }
	}

	map->update();
}

FXbool FXCSMap::paintMap(FXDCWindow &dc)
{
    FXint w = dc.getClipWidth();
    FXint h = dc.getClipHeight();
    dc.setFont(font.get());
	dc.setForeground(map->getBackColor());
	dc.fillRectangle(0, 0, w, h);

    // init vars and set font
	FXString label;
	FXRectangle clip(0, 0, FXshort(64*scale), FXshort(64*scale));

    FXPoint scr_offset = GetScreenOffset();

	IslandInfo islands;

    if (mapFile) {
        FXint regionSize = FXint(64 * scale);
        // iterate through all regions
        for (datablock &block : mapFile->blocks())
        {
            // handle only regions
            // handle only the actually visible plane
            if (block.type() != block_type::TYPE_REGION ||
                block.info() != visiblePlane)
                continue;

            // map coordinates to screen
            FXint scr_x = FXint(scale * GetScreenFromHexX(block.x(), block.y())) + scr_offset.x;
            FXint scr_y = FXint(scale * GetScreenFromHexY(block.x(), block.y())) + scr_offset.y;

            // clip regions outside of view
            if (scr_x < -regionSize || scr_y < -regionSize || scr_x > w || scr_y > h)
                continue;

            // draw terrain image
            FXIcon **icons = terrain;

            if (show_shadow_regions && !(block.flags() & datablock::FLAG_REGION_SEEN))
                icons = terrainShadow;

            dc.drawIcon(icons[block.terrain()], scr_x, scr_y);

            // island names
            if (show_islands)
            {
                updateIslands(islands, block, scr_x, scr_y, regionSize);
            }

            // don't draw special icons on minimap
            if (minimap)
                continue;

            // draw diagramm
            if (block.flags() & datablock::FLAG_TROOPS && block.attachment())
            {
                if (att_region *stats = static_cast<att_region *>(block.attachment()))
                {
                    static FXColor colors[] = { FXRGB(64,64,255), FXRGB(64,255,64), FXRGB(255,64,64), FXRGB(255,255,64), FXRGB(64,255,255), FXRGB(255,255,255), FXRGB(0,0,0) };

                    int rl = scr_x + int(regionSize / 1.5), rt = scr_y + int(regionSize / 1.8);
                    int rw = int(regionSize / 4.2), rh = int(regionSize / 4.2);
                    size_t size = stats->people.size();
                    if (size == 0) {
                        stats->people.push_back(0);
                        ++size;
                    }
                    else if (size > sizeof(colors) / sizeof(colors[0])) {
                        stats->people.resize(sizeof(colors) / sizeof(colors[0]));
                    }
                    int rw_part = rw / size;

                    for (size_t i = 0; i < size; i++)
                    {
                        float val = stats->people[i];
                        if (val < 0) val = 0;
                        if (val > 1) val = 1;
                        dc.setForeground(colors[i]);
                        dc.fillRectangle(rl + i * rw_part, rt + rh - int(val * rh), rw_part, int(val * rh));
                    }

                    dc.setForeground(map->getBackColor());

                    for (size_t i = 0; i < size; i++)
                    {
                        float val = stats->people[i];
                        if (val < 0) val = 0;
                        if (val > 1) val = 1;
                        dc.drawRectangle(rl + i * rw_part, rt + rh - int(val * rh), rw_part, int(val * rh));
                    }
                }
            }

            // draw region icons (ship, castle...)
            if (block.flags() & datablock::FLAG_REGION_OWN)
                dc.drawIcon(castleown, scr_x + FXint(regionSize / 1.75), scr_y + FXint(regionSize / 18));
            else if (block.flags() & datablock::FLAG_REGION_ALLY)
                dc.drawIcon(castleally, scr_x + FXint(regionSize / 1.75), scr_y + FXint(regionSize / 18));
            else if (block.flags() & datablock::FLAG_REGION_ENEMY)
                dc.drawIcon(castleenemy, scr_x + FXint(regionSize / 1.75), scr_y + FXint(regionSize / 18));
            else if (block.flags() & datablock::FLAG_CASTLE)
                dc.drawIcon(castle, scr_x + FXint(regionSize / 1.75), scr_y + FXint(regionSize / 18));

            if (block.flags() & datablock::FLAG_REGION_TAXES)
                dc.drawIcon(castlecoins, scr_x + FXint(regionSize / 1.75), scr_y + FXint(regionSize / 18));

            // draw guarded icons
            if ((block.flags() & (datablock::FLAG_GUARD_OWN | datablock::FLAG_GUARD_ALLY))
                && (block.flags() & datablock::FLAG_GUARD_ENEMY))
                dc.drawIcon(guardmixed, scr_x - FXint(regionSize / 12), scr_y + FXint(regionSize / 18));
            else if (block.flags() & datablock::FLAG_GUARD_OWN)
                dc.drawIcon(guardown, scr_x - FXint(regionSize / 12), scr_y + FXint(regionSize / 18));
            else if (block.flags() & datablock::FLAG_GUARD_ALLY)
                dc.drawIcon(guardally, scr_x - FXint(regionSize / 12), scr_y + FXint(regionSize / 18));
            else if (block.flags() & datablock::FLAG_GUARD_ENEMY)
                dc.drawIcon(guardenemy, scr_x - FXint(regionSize / 12), scr_y + FXint(regionSize / 18));

            if (block.flags() & datablock::FLAG_SHIP)
                dc.drawIcon(ship, scr_x - FXint(regionSize / 12), scr_y + FXint(regionSize / 2.5));
            else if ((block.flags() & datablock::FLAG_SHIPTRAVEL) && show_ship_travel)
                dc.drawIcon(shiptravel, scr_x - FXint(regionSize / 12), scr_y + FXint(regionSize / 2.5));

            // draw region "seen by" icons
            if (show_visibility_symbol)
            {
                if (block.flags() & datablock::FLAG_LIGHTHOUSE)
                    dc.drawIcon(lighthouse, scr_x + FXint(regionSize / 4), scr_y + FXint(regionSize / 18));
                if (block.flags() & datablock::FLAG_TRAVEL)
                    dc.drawIcon(travel, scr_x + FXint(regionSize / 4), scr_y + FXint(regionSize / 18));
            }

            // show symbol for dragon, seasnake or any other monster by priority
            if (block.flags() & datablock::FLAG_DRAGON)
                dc.drawIcon(dragon, scr_x + FXint(regionSize / 4), scr_y + FXint(regionSize / 2));
            else if (block.flags() & datablock::FLAG_SEASNAKE)
                dc.drawIcon(seasnake, scr_x + FXint(regionSize / 4), scr_y + FXint(regionSize / 2));
            else if (block.flags() & datablock::FLAG_MONSTER) {
                dc.drawIcon(monster, scr_x + FXint(regionSize / 4), scr_y + FXint(regionSize / 2));
            }
            if (mapFile->hasBattle(block.x(), block.y(), block.info())) {
                dc.drawIcon(battle, scr_x + FXint(regionSize / 4), scr_y + FXint(regionSize / 18));
            }

            // show symbol for wormhole
            if (block.flags() & datablock::FLAG_WORMHOLE)
                dc.drawIcon(wormhole, scr_x + FXint(regionSize / 4), scr_y);

            // draw street symbols
            if (show_streets)
                for (int i = 0; i < 6; i++)
                {
                    if (block.flags() & (datablock::FLAG_STREET << i))
                        dc.drawIcon(street[i], scr_x, scr_y);
                    else if (block.flags() & (datablock::FLAG_STREET_UNDONE << i))
                        dc.drawIcon(street_undone[i], scr_x, scr_y);
                }

            // draw selection mark if region is in multiple selection
            if (selection.regionsSelected.find(&block) != selection.regionsSelected.end())
                dc.drawIcon(selectedRegion, scr_x, scr_y);

            // skip label
            if (regionSize < 30) {
                continue;
            }

            if (show_names)
                label = block.value(TYPE_NAME);
            else
                label.clear();

            if (show_koords)
            {
                if (!label.empty())
                    label += "\r";
                label += FXStringVal(block.x()) + ":" + FXStringVal(block.y());
            }

            // clip label to region
            if (!label.empty())
            {
                FXint label_x, label_y;
                clip.x = (FXshort)scr_x;
                clip.y = (FXshort)scr_y;
                dc.setClipRectangle(clip);

                // draw region label
                if (label.find('\r') != -1)
                {
                    FXString name = label.before('\r'); label = label.after('\r');

                    label_x = scr_x + regionSize / 2 - font->getTextWidth(name.text(), name.length()) / 2;
                    label_y = scr_y + regionSize / 2 - font->getTextHeight(name.text(), name.length()) / 6;
                    dc.drawText(label_x, label_y, name.text(), name.length());

                    label_x = scr_x + regionSize / 2 - font->getTextWidth(label.text(), label.length()) / 2;
                    label_y = scr_y + regionSize / 2 + font->getTextHeight(label.text(), label.length()) * 5 / 6;
                    dc.drawText(label_x, label_y, label.text(), label.length());
                }
                else
                {
                    label_x = scr_x + regionSize / 2 - font->getTextWidth(label.text(), label.length()) / 2;
                    label_y = scr_y + regionSize / 2 + font->getTextHeight(label.text(), label.length()) / 3;
                    dc.drawText(label_x, label_y, label.text(), label.length());
                }

                // end drawing this region
                dc.clearClipRectangle();
            }
        }
    }

    FXPoint tl(0, 0);
    FXPoint br((FXshort)w, (FXshort)h);
    paintIslandNames(dc, tl, br, islands);

	if (minimap)
	{
		return true;			// minimap only draws regions and island names
	}

    // draw selection mark
	if ((selection.selected & (selection.REGION|selection.UNKNOWN_REGION)) && (sel_plane == visiblePlane))
	{
		FXint scr_x = scr_offset.x + FXint(scale * GetScreenFromHexX(sel_x, sel_y));
		FXint scr_y = scr_offset.y + FXint(scale * GetScreenFromHexY(sel_x, sel_y));
		dc.drawIcon(activeRegion, scr_x, scr_y);
	}

	return true;
}

void FXCSMap::paintArrows(FXDCWindow &dc)
{
    FXPoint scr_offset = GetScreenOffset();
    // NW, NO, O, SO, SW, W
    int arrow_offset_x[6] = { 6, 36, 46, 22, -7, -18 };
    int arrow_offset_y[6] = { -13, -5, 22, 40, 34, 8 };

    for (int i = 0; i != 2; ++i) {
        const std::vector<arrow> arrows = makeArrows(i);
        for (std::vector<FXCSMap::arrow>::const_iterator arrow = arrows.begin(); arrow != arrows.end(); arrow++)
        {
            FXint scr_x = scr_offset.x + FXint(scale * (arrow_offset_x[arrow->dir] + GetScreenFromHexX(arrow->x, arrow->y)));
            FXint scr_y = scr_offset.y + FXint(scale * (arrow_offset_y[arrow->dir] + GetScreenFromHexY(arrow->x, arrow->y)));

            FXIcon **iconSet = nullptr;
            if (arrow->color == arrow::ARROW_RED)
                iconSet = redarrows;
            else if (arrow->color == arrow::ARROW_GREEN)
                iconSet = greenarrows;
            else if (arrow->color == arrow::ARROW_BLUE)
                iconSet = bluearrows;
            else /*if (arrow->color == arrow::ARROW_GREY)*/ // catch-all
                iconSet = greyarrows;

            dc.drawIcon(iconSet[arrow->dir], scr_x, scr_y);
        }
    }
}

void FXCSMap::drawSlice(FXImage &image, const FXRectangle &rect, const IslandInfo *islands) const
{
    FXPoint br(FXshort(rect.x + rect.w), FXshort(rect.y + rect.h));
    FXDCWindow dc(&image);
    dc.setForeground(getBackColor());
    dc.fillRectangle(0, 0, image.getWidth(), image.getHeight());
    paintMapLines(dc, rect.tl(), rect.br());
    if (islands && !islands->empty()) {
        paintIslandNames(dc, rect.tl(), rect.br(), *islands);
    }
    image.restore();
}

void FXCSMap::paintMapLines(FXDCWindow& dc, FXPoint const& tl, FXPoint const& br) const
{
	dc.setFont(font.get());
	dc.setForeground(map->getBackColor());

	// init vars and set font
	FXRectangle clip(0, 0, FXshort(64*scale), FXshort(64*scale));
	FXint w = br.x - tl.x, h = br.y - tl.y;

	FXint regionSize = FXint(64*scale);

	// iterate throu all datafiles and all regions
	for (datablock& block : mapFile->blocks())
	{
		// handle only regions
		// handle only the actually visible plain
		if (block.type() != block_type::TYPE_REGION ||
			block.info() != visiblePlane)
			continue;

		// map coordinates to screen
		FXint scr_x = FXint(scale * GetScreenFromHexX(block.x(), block.y())) - tl.x;
		FXint scr_y = FXint(scale * GetScreenFromHexY(block.x(), block.y())) - tl.y;

		// clip regions outside of view
		if (scr_x < -regionSize || scr_y < -regionSize || scr_x > w || scr_y > h)
			continue;

		// draw terrain image
		dc.drawIcon(terrain[block.terrain()], scr_x, scr_y);

		// skip label
		if (regionSize < 30)
			continue;

        FXString label;
        if (show_names)
			label = block.value(TYPE_NAME);
		else
			label.clear();

		if (show_koords)
		{
			if (!label.empty())
				label += "\r";
			label += FXStringVal(block.x()) + ":" + FXStringVal(block.y());
		}

		// clip label to region
		if (!label.empty())
		{
            FXint label_x, label_y;
            clip.x = (FXshort)scr_x;
			clip.y = (FXshort)scr_y;
			dc.setClipRectangle(clip);

			// draw region label
			if (label.find('\r') != -1)
			{
				FXString name = label.before('\r'); label = label.after('\r');

				label_x = scr_x+regionSize/2 - font->getTextWidth(name.text(), name.length())/2;
				label_y = scr_y+regionSize/2 - font->getTextHeight(name.text(), name.length())/6;
				dc.drawText(label_x, label_y, name.text(), name.length());

				label_x = scr_x+regionSize/2 - font->getTextWidth(label.text(), label.length())/2;
				label_y = scr_y+regionSize/2 + font->getTextHeight(label.text(), label.length())*5/6;
				dc.drawText(label_x, label_y, label.text(), label.length());
			}
			else
			{
				label_x = scr_x+regionSize/2 - font->getTextWidth(label.text(), label.length())/2;
				label_y = scr_y+regionSize/2 + font->getTextHeight(label.text(), label.length())/3;
				dc.drawText(label_x, label_y, label.text(), label.length());
			}

			// end drawing this region
			dc.clearClipRectangle();
		}
	}
}

FXPoint FXCSMap::getMapLeftTop() const
{
	// no datafile loaded?
	if (!mapFile) return FXPoint{0, 0};

	// initialize to 'impossible' values
	FXshort min_x = SHRT_MAX, min_y = SHRT_MAX;

	// iterate throu all datafiles and all regions
	for (datablock& block : mapFile->blocks())
	{
		// handle only regions
		// handle only the actually visible plain
		if (block.type() != block_type::TYPE_REGION ||
			block.info() != visiblePlane)
			continue;

        FXshort scr_x = FXshort(scale * GetScreenFromHexX(block.x(), block.y()));
        FXshort scr_y = FXshort(scale * GetScreenFromHexY(block.x(), block.y()));

		if (scr_x < min_x) min_x = scr_x;
		if (scr_y < min_y) min_y = scr_y;
	}

	return FXPoint{min_x, min_y};
}

void FXCSMap::collectIslandNames(IslandInfo& islands) const
{
	// no datafile loaded?
    if (!mapFile) {
        return;
    }
    if (!show_islands) {
        islands.clear();
        return;
    }

	FXint regionSize = FXint(64*scale);

	// iterate throu all datafiles and all regions
	for (datablock& block : mapFile->blocks())
	{
		// handle only regions
		// handle only the actually visible plain
		if (block.type() != block_type::TYPE_REGION ||
			block.info() != visiblePlane)
			continue;

        FXint scr_x = FXint(scale * GetScreenFromHexX(block.x(), block.y()));
        FXint scr_y = FXint(scale * GetScreenFromHexY(block.x(), block.y()));

        updateIslands(islands, block, scr_x, scr_y, regionSize);
	}
}

void FXCSMap::updateIslands(IslandInfo &islands, datablock &block, FXint scr_x, FXint scr_y, FXint regionSize) const
{
    //  calculate island rect
    if (att_region *stats = static_cast<att_region *>(block.attachment()))
    {
        if (!stats->island.empty())
        {
            IslandInfo::iterator itor = islands.find(stats->island);
            if (itor == islands.end())
            {
                IslandPos is{};
                is.left = scr_x; is.top = scr_y;
                is.right = scr_x + regionSize; is.bottom = scr_y + regionSize;
                islands[stats->island] = is;
            }
            else
            {
                IslandPos &is = itor->second;
                is.left = std::min(is.left, scr_x); is.top = std::min(is.top, scr_y);
                is.right = std::max(is.right, scr_x + regionSize); is.bottom = std::max(is.bottom, scr_y + regionSize);
            }
        }
    }
}

void FXCSMap::paintIslandNames(FXDCWindow& dc, FXPoint const& tl, FXPoint const& br, IslandInfo const& islands) const
{
	if (!show_islands) return;

	// create DC and setup font
	dc.setFont(islandfont.get());
	dc.setForeground(map->getBackColor());

	for (IslandInfo::value_type const& entry : islands)
	{
		const FXString& label = entry.first;
		const IslandPos& is = entry.second;

		//  skip islands outside of view
		if (is.right < tl.x || is.left > br.x ||
			is.bottom < tl.y || is.top > br.y) {
			continue;
		}

		FXint textWidth = islandfont->getTextWidth(label.text(),  label.length());
		FXint textHeight = islandfont->getTextHeight(label.text(), label.length());

		FXint label_x = is.left + (is.right - is.left) / 2 - textWidth / 2 - tl.x;
		FXint label_y = is.top  + (is.bottom - is.top) / 2 + textHeight / 3 - tl.y;

		dc.drawText(label_x, label_y, label.text(), label.length());
	}
}

// Paint the map
long FXCSMap::onPaint(FXObject*, FXSelector, void* ptr)
{
	FXEvent *ev = (FXEvent*)ptr;

    FXint center_x = pos_x;
    FXint center_y = pos_y;
    if (minimap)
	{
		// on minimap, the complete map is painted at imagebuffer
		// so copy imagebuffer to backbuffer

		FXDCWindow dc(backbuffer.get());
		dc.setForeground(map->getBackColor());
		dc.fillRectangle(0, 0, backbuffer->getWidth(), backbuffer->getHeight());

		if (getWidth() > getImageWidth())	center_x += (getWidth() - getImageWidth())/2;
		if (getHeight() > getImageHeight())	center_y += (getHeight() - getImageHeight())/2;

		dc.drawImage(imagebuffer.get(), center_x,center_y);

		if (main_map)
		{
			// paint rect of view of connected main map
			float map_scale = getScale() / main_map->getScale();

			FXint left = center_x + offset_x - int((main_map->offset_x + main_map->pos_x)*map_scale);
			FXint top = center_y + offset_y - int((main_map->offset_y + main_map->pos_y)*map_scale);

			if (main_map->getWidth() > main_map->getImageWidth())	left -= int((main_map->getWidth() - main_map->getImageWidth())*0.5*map_scale);
			if (main_map->getHeight() > main_map->getImageHeight())	top -= int((main_map->getHeight() - main_map->getImageHeight())*0.5*map_scale);

			FXint right = left + int(main_map->getWidth()*map_scale);
			FXint bottom = top + int(main_map->getHeight()*map_scale);

			// check if rect is visible
			if (left < center_x) left = center_x;
			if (top < center_y) top = center_y;

			if (right > left + imagebuffer->getWidth())
				right = left + imagebuffer->getWidth();
			if (bottom > top + imagebuffer->getHeight())
				bottom = top + imagebuffer->getHeight();

			FXPoint points[5];
			points[0].x = (FXshort)left, points[0].y = (FXshort)top;
			points[1].x = (FXshort)right, points[1].y = (FXshort)top;
			points[2].x = (FXshort)right, points[2].y = (FXshort)bottom;
			points[3].x = (FXshort)left, points[3].y = (FXshort)bottom;
			points[4].x = (FXshort)left, points[4].y = (FXshort)top;

			dc.setForeground(FXRGB(255,255,255));
			dc.drawLines(points, 5);
		}
	}
    else if (repaint) {
        FXDCWindow dc(backbuffer.get());
        paintMap(dc);		// paint the map onto the backbuffer
        repaint = false;
    }
	// copy backbuffer to screen
	FXDCWindow screendc(map, ev);
	screendc.drawImage(backbuffer.get(), 0, 0);
    // draw traveller arrows
    paintArrows(screendc);

	return 1;
}

long FXCSMap::onMapChange(FXObject*sender, FXSelector sel, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

    bool datachanged = false;

    if (selection.fileChange != pstate->fileChange)
    {
        selection = *pstate;
        datachanged = true;
    }
    if (selection.selChange != pstate->selChange)
	{
        selection = *pstate;
        repaint = true;
		selection.regionsSelected = pstate->regionsSelected;

		// does state->region contain valid informaion?
        if (selection.selected & (selection.REGION | selection.UNKNOWN_REGION))
        {
            if (selection.selected & selection.REGION)
                sel_x = selection.region->x(), sel_y = selection.region->y(), sel_plane = selection.region->info();
            else
                sel_x = selection.sel_x, sel_y = selection.sel_y, sel_plane = selection.sel_plane;

            if (visiblePlane != sel_plane)
            {
                visiblePlane = sel_plane;
            }

            scrollTo(sel_x, sel_y);
        }
	}

    // things changed
    if (datachanged) {
        if (minimap) {
            updateMap();
        }
        else {
            repaint = true;
        }
    }

    if (minimap && shown())
    {
        // this is important, redraws the minimap
        map->update();
    }
    return 1;
}

void FXCSMap::updateMap()
{
    if (minimap) {
        scale = 1.0f;
        calculateContentSize();

        while (scale >= 2 / 64.0f && (getImageWidth() > getWidth() || getImageHeight() > getHeight()))
        {
            scale /= 2;
        }
        image_h = image_w = 0;
        scaleChange(scale);
        calculateContentSize();

        // paint map into buffer
        imagebuffer->resize(getImageWidth(), getImageHeight());
        FXDCWindow dc(imagebuffer.get());
        paintMap(dc);	// minimap paints map only when changed data
    }
}


void FXCSMap::clearRoute(int which)
{
    FXASSERT(which >= 0 && which < 2);
    routeString[which].clear();
    map->update();
}

FXint FXCSMap::sendRouteCmds(const FXString &str, int which)
{
    FXASSERT(which >= 0 && which < 2);
    if (str != routeString[which]) {
        routeString[which] = str;
        map->update();
    }
    return makeArrows(which).size();
}

long FXCSMap::onKeyPress(FXObject*, FXSelector, void* ptr)
{
	FXEvent *event = (FXEvent*)ptr;

	FXint offx = 0, offy = 0;
	bool move_marker = false;

	// Cursor keys pressed?
	switch(event->code)
	{
	// move map by cursor keys
	case FX::KEY_KP_Up:
	case FX::KEY_Up:
		offy = 1;
		break;

	case FX::KEY_KP_Down:
	case FX::KEY_Down:
		offy = -1;
		break;

	case FX::KEY_KP_Left:
	case FX::KEY_Left:
		offx = 1;
		break;

	case FX::KEY_KP_Right:
	case FX::KEY_Right:
		offx = -1;
		break;

	// select marked region by numpad-4
	case FX::KEY_KP_5:
		move_marker = true;
		break;

	// move region marker by hjkl or numpad
	case FX::KEY_KP_7:
		move_marker = true, offx = -1, offy = +1;
		break;
	case FX::KEY_KP_9:
	case 'k':
		move_marker = true, offy = +1;
		break;
	case FX::KEY_KP_6:
	case 'l':
		move_marker = true, offx = +1;
		break;
	case FX::KEY_KP_3:
		move_marker = true, offx = +1, offy = -1;
		break;
	case FX::KEY_KP_1:
	case 'j':
		move_marker = true, offy = -1;
		break;
	case FX::KEY_KP_4:
	case 'h':
		move_marker = true, offx = -1;
		break;
	}

	if (move_marker && (offx || offy))		// move marker
	{
		FXint x = selection.sel_x + offx;
		FXint y = selection.sel_y + offy;
		FXint plane = selection.sel_plane;

		// set new marked region
        selection.selected |= ~(selection.REGION | selection.UNKNOWN_REGION);
        selection.sel_x = x, selection.sel_y = y, selection.sel_plane = plane;
        selection.selected = mapFile->getRegion(selection.region, x, y, plane) ? selection.REGION : selection.UNKNOWN_REGION;

		getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
		return 1;
	}
	else if (move_marker)
	{
		// if no (existing) region marked, do nothing
		if (!(selection.selected & selection.REGION))
			return 0;

        selection.selected = selection.REGION;

		// already selected? then deselect the region
		std::set<datablock*>::iterator itor = selection.regionsSelected.find(&*selection.region);
		if (itor == selection.regionsSelected.end())
            selection.regionsSelected.insert(&*selection.region);
		else
            selection.regionsSelected.erase(itor);

		getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
		return 1;
	}
	else if (offx || offy)	// move map
	{
		FXint x, y;
		getPosition(x, y);

		if (offx)
			x += offx * map->getWidth() / 5;

		if (offy)
			y += offy * map->getHeight() / 5;

		setPosition(x, y);
		map->update();
		return 1;
	}

	return 0;
}

long FXCSMap::onQueryHelp(FXObject* sender, FXSelector, void*)
{
	FXint scrx, scry;
	FXuint buttons;

    if (!mapFile) {
        return 0;
    }

	getCursorPosition(scrx, scry, buttons);

	scrx -= offset_x + pos_x;
	scry -= offset_y + pos_y;

	if (getWidth() > getImageWidth())	scrx -= (getWidth() - getImageWidth())/2;
	if (getHeight() > getImageHeight())	scry -= (getHeight() - getImageHeight())/2;

	FXint x = GetHexFromScreenX(scrx, scry);
	FXint y = GetHexFromScreenY(scrx, scry);

	// now get the region
    FXString help;
    datablock::itor block;
    if (mapFile->getRegion(block, x, y, visiblePlane)) {
		FXString name, terrainString = (*block).terrainString();

		// Terrain, Name, Koordinaten
		name = (*block).value(TYPE_NAME);

		if (name.empty())
			help.format("%s (%d,%d)", terrainString.text(), x,y);
		else
			help.format("%s von %s (%d,%d)", terrainString.text(), name.text(), x,y);

		// Inselname
		if (att_region* stats = static_cast<att_region*>((*block).attachment()))
			if (!stats->island.empty())
				help.append(" auf ").append(stats->island);
	}
    else {
        help.format("Unbekannt (%d,%d)", x, y);
    }
	sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &help);

	return 1;
}
