#include <sstream>
#include <vector>

#include "csmap.h"
#include "regionlist.h"
#include "symbols.h"

#include "fxhelper.h"
#include "FXMenuSeparatorEx.h"

// *********************************************************************************************************
// *** FXRegionItem implementation

class FXAPI FXRegionItem : public FXTreeItem
{
	FXDECLARE(FXRegionItem)
	friend class FXTreeList;
	friend class FXDirList;
protected:
	static FXFont *boldfont;

    FXColor m_textColor = 0;

	FXRegionItem() : FXTreeItem(), m_textColor(0) {}

	virtual void draw(const FXTreeList* list,FXDC& dc,FXint x,FXint y,FXint w,FXint h) const;
	virtual FXint hitItem(const FXTreeList* list,FXint xx,FXint yy) const;
	virtual FXint getWidth(const FXTreeList* list) const;
	virtual FXint getHeight(const FXTreeList* list) const;

public:
	/// Constructor
	FXRegionItem(const FXString& text,FXIcon* oi=nullptr,FXIcon* ci=nullptr,void* ptr=nullptr) : FXTreeItem(text, oi, ci, ptr), m_textColor(0) {}

	bool isBold() const
	{
        const datablock* block = static_cast<const datablock *>(getData());
        if (block) {
            if (block->type() == block_type::TYPE_REGION) {
                if (att_region* stats = static_cast<att_region*>(block->attachment())) {
                    return stats->unconfirmed > 0;
                }
            }
            else if (block->type() == block_type::TYPE_UNIT) {
                return block->valueInt(TYPE_ORDERS_CONFIRMED) == 0;
            }
        }
        return false;
	}

	// ----------------------------------
	static void setBoldFont(FXFont* font)
	{
		boldfont = font;
	}

    FXColor getTextColor() const {
        return m_textColor;
    }
    void setTextColor(FXColor color) {
        m_textColor = color;
    }

};

/*static */ FXFont* FXRegionItem::boldfont;

static const int ICON_SPACING = 4;	// Spacing between parent and child in x direction
static const int TEXT_SPACING = 4;	// Spacing between icon and text
static const int SIDE_SPACING = 4;	// Spacing between side and item

// Object implementation
FXIMPLEMENT(FXRegionItem,FXTreeItem,nullptr,0)

// Draw item
void FXRegionItem::draw(const FXTreeList* list, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) const
{
    FXIcon *icon = (state & OPENED) ? openIcon : closedIcon;
    FXFont *font = list->getFont();
    if (isBold() && boldfont) {
        dc.setFont(font = boldfont);
    }
    FXint th = 0, tw = 0, ih = 0, iw = 0;
    xx += SIDE_SPACING / 2;
    if (icon) {
        iw = icon->getWidth();
        ih = icon->getHeight();
        dc.drawIcon(icon, xx, yy + (hh - ih) / 2);
        xx += ICON_SPACING + iw;
    }
    if (!label.empty()) {
        tw = 4 + font->getTextWidth(label.text(), label.length());
        th = 4 + font->getFontHeight();
        yy += (hh - th) / 2;
        if (isSelected()) {
            dc.setForeground(list->getSelBackColor());
            dc.fillRectangle(xx, yy, tw, th);
        }
        if (hasFocus()) {
            dc.drawFocusRectangle(xx + 1, yy + 1, tw - 2, th - 2);
        }
        if (!isEnabled())
            dc.setForeground(makeShadowColor(list->getBackColor()));
        else if (isSelected())
            dc.setForeground(list->getSelTextColor());
        else if (((FXRegionList *)list)->colorized()) {
            FXColor color = getTextColor();
            if (!color) color = list->getTextColor();
            dc.setForeground(color);
        }
        else {
            dc.setForeground(list->getTextColor());
        }
        dc.drawText(xx + 2, yy + font->getFontAscent() + 2, label);
    }

    if (isBold())
        dc.setFont(list->getFont());
}

// See if item got hit, and where:- 1 is icon, 2 is text
FXint FXRegionItem::hitItem(const FXTreeList* list,FXint xx,FXint yy) const
{
  FXint oiw=0,ciw=0,oih=0,cih=0,tw=0,th=0,iw,ih,ix,iy,tx,ty,h;
  FXFont *font=list->getFont();
  if (isBold() && boldfont)
		font = boldfont;

  if(openIcon){
    oiw=openIcon->getWidth();
    oih=openIcon->getHeight();
    }
  if(closedIcon){
    ciw=closedIcon->getWidth();
    cih=closedIcon->getHeight();
    }
  if(!label.empty()){
    tw=4+font->getTextWidth(label.text(),label.length());
    th=4+font->getFontHeight();
    }
  iw=FXMAX(oiw,ciw);
  ih=FXMAX(oih,cih);
  h=FXMAX(th,ih);
  ix=SIDE_SPACING/2;
  tx=SIDE_SPACING/2;
  if(iw) tx+=iw+ICON_SPACING;
  iy=(h-ih)/2;
  ty=(h-th)/2;

  // In icon?
  if(ix<=xx && iy<=yy && xx<ix+iw && yy<iy+ih) return 1;

  // In text?
  if(tx<=xx && ty<=yy && xx<tx+tw && yy<ty+th) return 2;

  // Outside
  return 0;
  }

// Get item width
FXint FXRegionItem::getWidth(const FXTreeList* list) const
{
  FXint w=0,oiw=0,ciw=0;
  FXFont *font=list->getFont();
  if (isBold() && boldfont)
		font = boldfont;

  if(openIcon) oiw=openIcon->getWidth();
  if(closedIcon) ciw=closedIcon->getWidth();
  w=FXMAX(oiw,ciw);
  if(!label.empty()){
    if(w) w+=ICON_SPACING;
    w+=4+font->getTextWidth(label.text(),label.length());
    }
  return SIDE_SPACING+w;
  }

// Get item height
FXint FXRegionItem::getHeight(const FXTreeList* list) const
{
  FXint th=0,oih=0,cih=0;
  FXFont *font=list->getFont();
  if (isBold() && boldfont)
		font = boldfont;

  if(openIcon) oih=openIcon->getHeight();
  if(closedIcon) cih=closedIcon->getHeight();
  if(!label.empty()) th=4+font->getFontHeight();
  return FXMAX3(th,oih,cih);
  }

// *********************************************************************************************************
// *** FXRegionList implementation

FXDEFMAP(FXRegionList) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_CLICKED,			0,									FXRegionList::onSelected), 

	FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,								FXRegionList::onPopup), 

	FXMAPFUNC(SEL_COMMAND,			FXRegionList::ID_TOGGLE_OWNFACTIONGROUP, FXRegionList::onToggleOwnFactionGroup), 
	FXMAPFUNC(SEL_UPDATE,			FXRegionList::ID_TOGGLE_OWNFACTIONGROUP, FXRegionList::onUpdateOwnFactionGroup),

	FXMAPFUNC(SEL_COMMAND,			FXRegionList::ID_TOGGLE_UNITCOLORS, FXRegionList::onToggleUnitColors),
	FXMAPFUNC(SEL_UPDATE,			FXRegionList::ID_TOGGLE_UNITCOLORS, FXRegionList::onUpdateUnitColors),

	FXMAPFUNC(SEL_COMMAND,			FXRegionList::ID_UPDATE,			FXRegionList::onMapChange), 
	FXMAPFUNC(SEL_QUERY_HELP,		0,									FXRegionList::onQueryHelp), 
}; 

FXIMPLEMENT(FXRegionList,FXTreeList,MessageMap, ARRAYNUMBER(MessageMap))

FXRegionList::FXRegionList(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXTreeList(p, tgt,sel, opts, x, y, w, h),
    active_faction_group(false),
    colorized_units(false),
    mapFile(nullptr)
{
	// modify list style
	setListStyle(getListStyle()|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES);

	// create/load images for terrain types
	for (int i = 0; i <  data::TERRAIN_LAST; i++)
		terrainIcons[i] = new FXGIFIcon(getApp(), data::terrain_icon(i), 0, IMAGE_ALPHAGUESS);

	green = new FXGIFIcon(getApp(), data::green, 0, IMAGE_ALPHAGUESS);
	red = new FXGIFIcon(getApp(), data::red, 0, IMAGE_ALPHAGUESS);
	blue = new FXGIFIcon(getApp(), data::blue, 0, IMAGE_ALPHAGUESS);
	cyan = new FXGIFIcon(getApp(), data::cyan, 0, IMAGE_ALPHAGUESS);
	yellow = new FXGIFIcon(getApp(), data::yellow, 0, IMAGE_ALPHAGUESS);
	orange = new FXGIFIcon(getApp(), data::orange, 0, IMAGE_ALPHAGUESS);
	gray = new FXGIFIcon(getApp(), data::gray, 0, IMAGE_ALPHAGUESS);
	black = new FXGIFIcon(getApp(), data::black, 0, IMAGE_ALPHAGUESS);
}

void FXRegionList::create()
{
	FXTreeList::create();

	FXFontDesc fontdesc;
	getFont()->getFontDesc(fontdesc);
	fontdesc.weight = FXFont::Bold;
	boldfont = new FXFont(getApp(), fontdesc);
	boldfont->create();

	FXRegionItem::setBoldFont(boldfont);

	// create icons for region list
	for (int i = 0; i <  data::TERRAIN_LAST; i++)
		terrainIcons[i]->create();

	green->create();
	red->create();
	blue->create();
	cyan->create();
	yellow->create();
	orange->create();
	gray->create();
	black->create();
}

FXRegionList::~FXRegionList()
{
	delete boldfont;

	for (int i = 0; i <  data::TERRAIN_LAST; i++)
		delete terrainIcons[i];

	delete green;
	delete red;
	delete blue;
	delete cyan;
	delete yellow;
	delete orange;
	delete gray;
	delete black;
}

long FXRegionList::onToggleOwnFactionGroup(FXObject* sender, FXSelector, void* ptr)
{
	active_faction_group = !active_faction_group;
    rebuildTree();
	return 1;
}

long FXRegionList::onUpdateOwnFactionGroup(FXObject* sender, FXSelector, void* ptr)
{
	sender->handle(this, FXSEL(SEL_COMMAND, active_faction_group?ID_CHECK:ID_UNCHECK), nullptr);
    return 1;
}

long FXRegionList::onToggleUnitColors(FXObject *sender, FXSelector sel, void *ptr)
{
    colorized_units = !colorized_units;
    update();
    return onUpdateUnitColors(sender, sel, ptr);
}

long FXRegionList::onUpdateUnitColors(FXObject* sender, FXSelector, void* ptr)
{
    sender->handle(this, FXSEL(SEL_COMMAND, colorized_units ? ID_CHECK : ID_UNCHECK), nullptr);
	return 1;
}

void FXRegionList::setMapFile(datafile *f)
{
    if (mapFile != f) {
        mapFile = f;
    }
}

long FXRegionList::onSelected(FXObject*,FXSelector,void*)
{
    FXTreeItem* current = getCurrentItem();
    if (current) {
        datablock* block = static_cast<datablock*>(current->getData());
        if (block) {
            return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_SETVALUE), block);
        }
    }
    return 0;
}

long FXRegionList::onPopup(FXObject* sender,FXSelector sel, void* ptr)
{
	onRightBtnRelease(sender, sel, ptr);

    // no datafile, no popup
    if (!mapFile)
        return 0;

    FXEvent *event = (FXEvent*)ptr;

	// dont't show popup if mouse has moved
	if (event->last_x != event->click_x || event->last_y != event->click_y)
		return 0;
	
	// create popup
	FXTreeItem *item = getItemAt(event->click_x, event->click_y);
	if (!item)
		return 0;

    return showPopup(item->getText(), static_cast<datablock*>(item->getData()), event->root_x, event->root_y);
}

long FXRegionList::showPopup(const FXString& label, datablock* block, FXint root_x, FXint root_y)
{
    FXMenuPane pane(this);
    FXMenuPane paneCascade(this);
    if (label.length() <= 20) {
        new FXMenuSeparatorEx(&pane, label);
    }
    else {
        FXString str = label.left(17) + "...";
        new FXMenuSeparatorEx(&pane, str);
    }
    new FXMenuCascade(&pane, "&Zwischenablage", NULL, &paneCascade);
    CSMap* csmap = static_cast<CSMap*>(getShell());
    csmap->addClipboardPane(&paneCascade, block);
    pane.create();
    pane.popup(nullptr, root_x, root_y);
    getApp()->runModalWhileShown(&pane);
    return 0;
}

bool FXRegionList::isConfirmed(const datablock::itor& unit) const
{
    return mapFile->isConfirmed(unit);
}

// rekursivly searches item with userdata=data in treeitem list
FXTreeItem* FXRegionList::findTreeItem(FXTreeItem* item, void* udata)
{
	while (item)
	{
		if (item->getData() == udata)
			return item;

		if (item->getFirst())
		{
            FXTreeItem* found = findTreeItem(item->getFirst(), udata);
			if (found)
				return found;
		}

		item = item->getNext();
	}

	return nullptr;
}

void FXRegionList::rebuildTree()
{
    // map change notify rebuilds treelist
    if (mapFile && mapFile->hasActiveFaction()) {
        if (!active_faction_group) {
            if (selection.selected & selection.FACTION) {
                if (selection.faction == mapFile->activefaction()) {
                    selection.selected &= ~selection.FACTION;
                }
            }
        }
        onMapChange(this, 0, &selection);
    }
}

FXColor FXRegionList::getUnitColor(const datablock* unitPtr)
{
    if (unitPtr->hasKey(TYPE_BUILDING)) {
       return FXRGB(0, 127, 0);
    }
    if (unitPtr->hasKey(TYPE_SHIP)) {
        return FXRGB(0, 0, 255);
    }
    return 0;
}

long FXRegionList::onMapChange(FXObject* sender, FXSelector, void* ptr)
{
    datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

	// any data changed, so need to update list?
	if (sender == this || selection.fileChange != pstate->fileChange)
	{
        getApp()->beginWaitCursor();
        selection.fileChange = pstate->fileChange;

		// clear list and build a new one from data in this->files
		clearItems();

        if (mapFile) {
            FXString str, terrainString;

            for (datablock::itor region = mapFile->blocks().begin(); region != mapFile->blocks().end(); ++region)
            {
                const datablock* regionPtr = &*region;
                // handle only regions
                if (regionPtr->type() != block_type::TYPE_REGION)
                    continue;

                terrainString = regionPtr->terrainString();

                FXString name = regionPtr->value(TYPE_NAME);

                if (name.empty())
                    name = terrainString;
                if (name.empty())
                    name = "Unbekannt";

                if (regionPtr->info())
                    str.format("%s (%d,%d,%s)", name.text(), regionPtr->x(), regionPtr->y(), datablock::planeName(regionPtr->info()).text());
                else
                    str.format("%s (%d,%d)", name.text(), regionPtr->x(), regionPtr->y());

                // select terrain image
                FXint terrain = regionPtr->terrain();

                FXTreeItem *regionItem = nullptr;

                FXString regionlabel = str;
                std::map<FXint, FXRegionItem *> factions;

                datablock::itor iend = mapFile->blocks().end();
                for (datablock::itor unit = std::next(region); unit != iend && unit->depth() > regionPtr->depth(); ++unit)
                {
                    const datablock* objectPtr = &*unit;
                    if (unit->type() != block_type::TYPE_UNIT)
                        continue;

                    // get faction id, -1 means unknown faction (or stealth/anonymous)
                    int factionId = mapFile->getFactionIdForUnit(objectPtr);
                    str = mapFile->getFactionName(factionId);
                    FXRegionItem*&entry = factions[factionId];
                    if (!entry)
                    {
                        FXIcon* icon = red;
                        datablock* facPtr = nullptr;
                        if (factionId == (int)datafile::special_faction::ANONYMOUS) {
                            icon = gray;
                        }
                        else if (factionId == (int)datafile::special_faction::TRAITOR) {
                            icon = red;
                        }
                        else {
                            datablock::itor faction;
                            if (mapFile->getFaction(faction, factionId)) {
                                facPtr = &*faction;
                            }
                            if (factionId == mapFile->getFactionId()) {
                                icon = blue;
                            }
                            else if (mapFile->hasActiveFaction()) {
                                datablock::itor block = mapFile->activefaction();
                                for (block++; block != mapFile->blocks().end(); ++block)
                                {
                                    if (block->type() != block_type::TYPE_ALLIANCE &&
                                        block->type() != block_type::TYPE_ITEMS &&
                                        block->type() != block_type::TYPE_OPTIONS &&
                                        block->type() != block_type::TYPE_GROUP)
                                        break;

                                    if (block->type() != block_type::TYPE_ALLIANCE)
                                        continue;

                                    if (block->info() != factionId)
                                        continue;

                                    // change icon to green, if alliance status to faction is set
                                    icon = green;
                                    break;
                                }
                            }
                        }
                        // add region if it has units in it
                        if (!regionItem) {
                            regionItem = appendItem(nullptr, new FXRegionItem(regionlabel, terrainIcons[terrain], terrainIcons[terrain], (void *)regionPtr));
                        }

                        entry = new FXRegionItem(str, icon, icon, facPtr);
                    }
                }

                FXTreeItem* act_faction = nullptr;
                if (!factions.empty()) {
                    FXTreeItem* ins_faction = nullptr;
                    for (auto& val : factions) {
                        FXRegionItem* entry = val.second;
                        FXTreeItem* ins = nullptr;
                        if (entry->getOpenIcon() == blue) {
                            act_faction = prependItem(regionItem, entry);
                        }
                        else if (val.first < 0) {
                            ins = appendItem(regionItem, entry);
                        }
                        else {
                            if (ins_faction == nullptr) {
                                ins = appendItem(regionItem, entry);
                            }
                            else {
                                insertItem(ins_faction, regionItem, entry);
                            }
                        }
                        if (ins && !ins_faction) {
                            ins_faction = ins;
                        }
                    }
                }

                FXTreeItem* ships = nullptr;
                FXTreeItem* buildings = nullptr;
                for (datablock::itor unit = std::next(region); unit != iend && unit->depth() > regionPtr->depth(); ++unit)
                {
                    const datablock* objectPtr = &*unit;
                    if (regionItem || regionPtr->hasKey(TYPE_VISIBILITY)) {
                        FXTreeItem* child = nullptr;
                        if (unit->type() == block_type::TYPE_BUILDING)
                        {
                            if (!buildings) {
                                if (!regionItem) {
                                    regionItem = appendItem(nullptr, new FXRegionItem(regionlabel, terrainIcons[terrain], terrainIcons[terrain], (void*)regionPtr));
                                }
                                buildings = prependItem(regionItem, L"Geb\u00e4ude");
                            }
                            FXString label = objectPtr->getLabel() + ", " + objectPtr->value(TYPE_TYPE) + L" Gr\u00f6\u00dfe " + objectPtr->value(TYPE_SIZE);
                            child = appendItem(buildings, label);
                        }
                        else if (unit->type() == block_type::TYPE_SHIP)
                        {
                            if (!ships) {
                                if (!regionItem) {
                                    regionItem = appendItem(nullptr, new FXRegionItem(regionlabel, terrainIcons[terrain], terrainIcons[terrain], (void*)regionPtr));
                                }
                                ships = prependItem(regionItem, L"Schiffe");
                            }
                            FXString label = objectPtr->getLabel() + ", " + objectPtr->value(TYPE_TYPE) + L" Gr\u00f6\u00dfe " + objectPtr->value(TYPE_SIZE);
                            child = appendItem(ships, label);
                        }
                        if (child) {
                            child->setData((void*)objectPtr);
                        }
                    }
                    // display units until next region
                    if (unit->type() != block_type::TYPE_UNIT)
                        continue;

                    // get treeitem node from faction
                    const datablock* unitPtr = objectPtr;
                    FXint factionId = mapFile->getFactionIdForUnit(unitPtr);

                    FXTreeItem *faction = factions[factionId];

                    FXString label;
                    int number = unitPtr->valueInt(TYPE_NUMBER);

                    label.format("%s: %d", unitPtr->getLabel().text(), number);

                    // with active_faction_group not set, units of own faction are inserted
                    // directly as child of region node.
                    FXRegionItem *item;

                    if (!active_faction_group && faction == act_faction)
                        insertItem(faction, regionItem, item = new FXRegionItem(label, 0, 0, (void*)unitPtr));
                    else
                        appendItem(faction, item = new FXRegionItem(label, 0, 0, (void *)unitPtr));

                    FXColor color = getUnitColor(unitPtr);
                    if (color) {
                        item->setTextColor(color);
                    }
                }

                // with active_faction_group not set, remove node of own faction
                if (!active_faction_group && act_faction)
                    removeItem(act_faction);
            }
        }
        getApp()->endWaitCursor();
    }

	if (sender == this || selection.selChange != pstate->selChange)
	{
        selection = *pstate;
        if (selection.selected & selection.REGION)
        {
            FXTreeItem* top = getFirstItem();
            if (top) {
                FXTreeItem* region = findTreeItem(top, &*selection.region);
                FXTreeItem* item = nullptr;
                if (selection.selected & selection.UNIT) {
                    item = findTreeItem(region ? region : top, &*selection.unit);
                }
                else if (selection.selected & selection.SHIP) {
                    item = findTreeItem(region, &*selection.ship);
                }
                else if (selection.selected & selection.BUILDING) {
                    item = findTreeItem(region, &*selection.building);
                }
                else if (selection.selected & selection.FACTION)
                    item = findTreeItem(region, &*selection.faction);

                if (!item) item = region;
                if (item) {
                    selectItem(item);
                    makeItemVisible(item);
                }
            }
        }
    }

	return 1;
}

long FXRegionList::onQueryHelp(FXObject* /*sender*/, FXSelector, void* /*ptr*/)
{ 
	return 0;
}
