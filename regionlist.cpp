#include <algorithm>
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

    FXColor m_textColor = 0;

	FXRegionItem() : FXTreeItem(), m_textColor(0) {}

	virtual void draw(const FXTreeList* list,FXDC& dc,FXint x,FXint y,FXint w,FXint h) const;
	virtual FXint hitItem(const FXTreeList* list,FXint xx,FXint yy) const;
	virtual FXint getWidth(const FXTreeList* list) const;
	virtual FXint getHeight(const FXTreeList* list) const;

public:
	/// Constructor
	FXRegionItem(const FXString& text,FXIcon* oi=nullptr,FXIcon* ci=nullptr,void* ptr=nullptr) : FXTreeItem(text, oi, ci, ptr), m_textColor(0) {}

	// ----------------------------------
    FXColor getTextColor() const {
        return m_textColor;
    }
    void setTextColor(FXColor color) {
        m_textColor = color;
    }

};

static const int ICON_SPACING = 4;	// Spacing between parent and child in x direction
static const int TEXT_SPACING = 4;	// Spacing between icon and text
static const int SIDE_SPACING = 4;	// Spacing between side and item

// Object implementation
FXIMPLEMENT(FXRegionItem,FXTreeItem,nullptr,0)

// Draw item
void FXRegionItem::draw(const FXTreeList* l, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) const
{
    const FXRegionList* list = static_cast<const FXRegionList*>(l);
    FXFont* boldfont = list->getBoldFont();
    const datablock* block = static_cast<const datablock*>(getData());
    bool is_bold = boldfont && block && !list->isConfirmed(*block);
    FXIcon *icon = (state & OPENED) ? openIcon : closedIcon;
    FXFont *font = list->getFont();

    if (is_bold) {
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

    if (is_bold)
        dc.setFont(list->getFont());
}

// See if item got hit, and where:- 1 is icon, 2 is text
FXint FXRegionItem::hitItem(const FXTreeList* l,FXint xx,FXint yy) const
{
    const FXRegionList* list = static_cast<const FXRegionList*>(l);
    FXFont* boldfont = list->getBoldFont();
    FXint oiw = 0, ciw = 0, oih = 0, cih = 0, tw = 0, th = 0, iw, ih, ix, iy, tx, ty, h;
    FXFont* font = list->getFont();
    const datablock* block = static_cast<const datablock*>(getData());
    bool is_bold = boldfont && block && !list->isConfirmed(*block);
    if (is_bold) font = boldfont;

    if (openIcon) {
        oiw = openIcon->getWidth();
        oih = openIcon->getHeight();
    }
    if (closedIcon) {
        ciw = closedIcon->getWidth();
        cih = closedIcon->getHeight();
    }
    if (!label.empty()) {
        tw = 4 + font->getTextWidth(label.text(), label.length());
        th = 4 + font->getFontHeight();
    }
    iw = FXMAX(oiw, ciw);
    ih = FXMAX(oih, cih);
    h = FXMAX(th, ih);
    ix = SIDE_SPACING / 2;
    tx = SIDE_SPACING / 2;
    if (iw) tx += iw + ICON_SPACING;
    iy = (h - ih) / 2;
    ty = (h - th) / 2;

    // In icon?
    if (ix <= xx && iy <= yy && xx < ix + iw && yy < iy + ih) return 1;

    // In text?
    if (tx <= xx && ty <= yy && xx < tx + tw && yy < ty + th) return 2;

    // Outside
    return 0;
}

// Get item width
FXint FXRegionItem::getWidth(const FXTreeList* l) const
{
    const FXRegionList* list = static_cast<const FXRegionList*>(l);
    FXFont* boldfont = list->getBoldFont();
    const datablock* block = static_cast<const datablock*>(getData());
    bool is_bold = boldfont && block && !list->isConfirmed(*block);
    FXint w = 0, oiw = 0, ciw = 0;
    FXFont *font=list->getFont();
    if (is_bold)
        font = boldfont;

    if (openIcon) oiw = openIcon->getWidth();
    if (closedIcon) ciw = closedIcon->getWidth();
    w = FXMAX(oiw, ciw);
    if (!label.empty()) {
        if (w) w += ICON_SPACING;
        w += 4 + font->getTextWidth(label.text(), label.length());
    }
    return SIDE_SPACING + w;
}

// Get item height
FXint FXRegionItem::getHeight(const FXTreeList* l) const
{
    const FXRegionList* list = static_cast<const FXRegionList*>(l);
    FXFont* boldfont = list->getBoldFont();
    const datablock* block = static_cast<const datablock*>(getData());
    bool is_bold = boldfont && block && !list->isConfirmed(*block);
    FXFont* font = list->getFont();
    FXint th = 0, oih = 0, cih = 0;
    if (is_bold)
        font = boldfont;

    if (openIcon) oih = openIcon->getHeight();
    if (closedIcon) cih = closedIcon->getHeight();
    if (!label.empty()) th = 4 + font->getFontHeight();
    return FXMAX3(th, oih, cih);
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

void FXRegionList::setMapFile(std::shared_ptr<datafile>& f)
{
    if (mapFile != f) {
        mapFile = f;

        rebuildTree();
    }
}

long FXRegionList::onSelected(FXObject*,FXSelector,void*)
{
    FXTreeItem* current = getCurrentItem();
    if (current) {
        datablock* block = static_cast<datablock*>(current->getData());
        if (block) {
            if (block->type() == block_type::TYPE_FACTION) {
                // select current region instead;
                FXTreeItem* parent = current->getParent();
                selection.selected = 0;
                if (mapFile->getFaction(selection.faction, block->info())) {
                    selection.selected |= selection.FACTION;
                }
                if (parent) {
                    block = static_cast<datablock*>(parent->getData());
                    if (block && mapFile->getRegion(selection.region, *block)) {
                        selection.selected |= selection.REGION;
                    }
                }
                return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
            }
            else {
                return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_SETVALUE), block);
            }
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
	FXTreeItem *item = getCursorItem();
	if (!item)
		return 0;

    datablock* block = static_cast<datablock*>(item->getData());
    if (!block) 
        return 0;

    return showPopup(item->getText(), block, event->root_x, event->root_y);
}

long FXRegionList::showPopup(const FXString& label, datablock* block, FXint root_x, FXint root_y)
{
    FXMenuPane pane(this);
    FXMenuPane paneCascade(this);
    CSMap* csmap = static_cast<CSMap*>(getShell());
    csmap->addClipboardPane(&pane, block);
    pane.create();
    pane.popup(nullptr, root_x, root_y);
    getApp()->runModalWhileShown(&pane);
    return 0;
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

static bool CompareFactionItems(const FXRegionItem* a, const FXRegionItem* b)
{
    const datablock* blockA = static_cast<const datablock *>(a->getData());
    const datablock* blockB = static_cast<const datablock *>(b->getData());
    return !blockB || (blockA && (blockA->getName() < blockB->getName()));
}

struct FactionKeyValue {
    int id;
    FXRegionItem* item;
};

void FXRegionList::rebuildTree()
{
    // clear list and build a new one from data in this->files
    clearItems();

    if (mapFile) {
        FXRegionItem* regionItem = nullptr;
        FXRegionItem* firstFactionItem = nullptr;
        FXTreeItem* ships = nullptr;
        FXTreeItem* buildings = nullptr;
        const datablock* regionPtr = nullptr;
        std::vector<FactionKeyValue> factions;

        for (datablock::itor block = mapFile->blocks().begin(); block != mapFile->blocks().end(); ++block)
        {
            const datablock* objectPtr = &*block;
            block_type type = objectPtr->type();
            if (type == block_type::TYPE_REGION) {
                const att_region* att = static_cast<const att_region*>(objectPtr->attachment());
                factions.clear();
                regionItem = nullptr;
                firstFactionItem = nullptr;
                ships = nullptr;
                buildings = nullptr;
                regionPtr = (att && !att->people.empty()) ? objectPtr : nullptr;
            }
            if (regionPtr) {
                FXRegionItem* child = nullptr;
                if (type == block_type::TYPE_UNIT) {
                    // get faction id, -1 means unknown faction (or stealth/anonymous)
                    int factionId = mapFile->getFactionIdForUnit(objectPtr);
                    FXRegionItem* factionItem = nullptr;
                    auto it = std::find_if(factions.begin(), factions.end(), [factionId](const FactionKeyValue& item) {
                        return factionId == item.id;
                     });
                    if (it != factions.end()) {
                        factionItem = (*it).item;
                    }
                    if (!regionItem) {
                        regionItem = appendRegion(regionPtr);
                    }
                    if (!factionItem) {
                        FXIcon* bullet = red;
                        datablock* facPtr = nullptr;
                        if (factionId == (int)datafile::special_faction::ANONYMOUS) {
                            bullet = gray;
                        }
                        else if (factionId == (int)datafile::special_faction::TRAITOR) {
                            bullet = red;
                        }
                        else {
                            datablock::itor faction;
                            if (mapFile->getFaction(faction, factionId)) {
                                facPtr = &*faction;
                            }
                            if (factionId == mapFile->getFactionId()) {
                                bullet = blue;
                            }
                            else if (mapFile->hasActiveFaction()) {
                                for (datablock::itor iter = std::next(mapFile->activefaction()); iter != mapFile->blocks().end(); ++iter)
                                {
                                    const datablock& blockRef = *iter;
                                    block_type type = blockRef.type();
                                    if (type != block_type::TYPE_ALLIANCE &&
                                        type != block_type::TYPE_ITEMS &&
                                        type != block_type::TYPE_OPTIONS &&
                                        type != block_type::TYPE_GROUP)
                                        break;

                                    if (type != block_type::TYPE_ALLIANCE)
                                        continue;

                                    if (blockRef.info() != factionId)
                                        continue;

                                    // change icon to green, if alliance status to faction is set
                                    bullet = green;
                                    break;
                                }
                            }
                        }
                        if (active_faction_group || bullet != blue) {
                            factionItem = new FXRegionItem(mapFile->getFactionName(factionId), bullet, bullet, facPtr);
                            factions.push_back({ factionId, factionItem });
                            if (bullet == blue) {
                                // active faction should always be the first in the list
                                insertItem(firstFactionItem, regionItem, factionItem);
                                firstFactionItem = factionItem;
                            }
                            else {
                                appendItem(regionItem, factionItem);
                            }
                            if (!firstFactionItem) {
                                firstFactionItem = factionItem;
                            }
                        }
                    }

                    const datablock* unitPtr = objectPtr;
                    int number = unitPtr->valueInt(TYPE_NUMBER);
                    FXString label;
                    label.format("%s: %d", unitPtr->getLabel().text(), number);

                    // with active_faction_group not set, units of own faction are inserted
                    // directly as child of region node.
                    FXRegionItem* unitItem = new FXRegionItem(label, 0, 0, const_cast<datablock*>(unitPtr));

                    if (factionItem) {
                        appendItem(factionItem, unitItem);
                    }
                    else {
                        if (!regionItem) {
                            regionItem = appendRegion(regionPtr);
                        }
                        if (active_faction_group) {
                            appendItem(factionItem, unitItem);
                        }
                        else {
                            insertItem(firstFactionItem, regionItem, unitItem);
                        }
                    }

                    FXColor color = getItemColor(*unitPtr);
                    if (color) {
                        unitItem->setTextColor(color);
                    }
                }
                else if (type == block_type::TYPE_SHIP) {
                    if (!regionItem) {
                        regionItem = appendRegion(regionPtr);
                    }
                    if (!ships) {
                        ships = prependItem(regionItem, L"Schiffe");
                    }
                    FXString label = objectPtr->getLabel() + ", " + objectPtr->value(TYPE_TYPE) + L" Gr\u00f6\u00dfe " + objectPtr->value(TYPE_SIZE);
                    appendItem(ships, child = new FXRegionItem(label));
                }
                else if (type == block_type::TYPE_BUILDING) {
                    if (!regionItem) {
                        regionItem = appendRegion(regionPtr);
                    }
                    if (!buildings) {
                        buildings = prependItem(regionItem, L"Geb\u00e4ude");
                    }
                    FXString label = objectPtr->getLabel() + ", " + objectPtr->value(TYPE_TYPE) + L" Gr\u00f6\u00dfe " + objectPtr->value(TYPE_SIZE);
                    appendItem(buildings, child = new FXRegionItem(label));
                }
                if (child) {
                    FXColor color = getItemColor(*block);
                    if (color) {
                        child->setTextColor(color);
                    }
                    child->setData(const_cast<datablock *>(objectPtr));
                }
            }
        }
    }
}

FXRegionItem* FXRegionList::appendRegion(const datablock* regionPtr)
{
    FXIcon* terrain = terrainIcons[regionPtr->terrain()];

    FXRegionItem * newItem = new FXRegionItem(regionPtr->getLabel(), terrain, terrain, const_cast<datablock *>(regionPtr));
    appendItem(nullptr, newItem);
    return newItem;
}

FXColor FXRegionList::getItemColor(const datablock& block) const
{
    if (block.type() == block_type::TYPE_UNIT) {
        if (block.hasKey(TYPE_BUILDING)) {
            return FXRGB(0, 127, 0);
        }
        if (block.hasKey(TYPE_SHIP)) {
            return FXRGB(0, 0, 255);
        }
    }
    else if (block.type() == block_type::TYPE_SHIP || block.type() == block_type::TYPE_BUILDING) {
        int factionId = block.valueInt(key_type::TYPE_FACTION);
        if (factionId != mapFile->getActiveFactionId()) {
            return FXRGB(127, 127, 127);
        }
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

        // map change notify rebuilds treelist
        rebuildTree();

        getApp()->endWaitCursor();
    }

       
    if (sender == this || selection.selChange != pstate->selChange)
	{
        selection = *pstate;
        if (mapFile && mapFile->hasActiveFaction())
        {
            if (!active_faction_group)
            {
                if (selection.selected & selection.FACTION)
                {
                    if (selection.faction == mapFile->activefaction())
                    {
                        selection.selected &= ~selection.FACTION;
                    }
                }
            }
        }
        if (selection.selected & selection.REGION)
        {
            FXTreeItem* top = getFirstItem();
            if (top) {
                FXTreeItem* region = findTreeItem(top, &*selection.region);
                if (region) {
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
                else {
                    killSelection();
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

bool FXRegionList::isConfirmed(const datablock& block) const
{
    return mapFile->isConfirmed(block);
}
