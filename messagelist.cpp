#include "main.h"
#include "fxhelper.h"
#include "messagelist.h"

#include <set>

FXMessageList::FXMessageList(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXTreeList(p, tgt,sel, opts|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES, x,y,w,h)
{
	// init variables
	mapFile = nullptr;

	// set styles...
	setNumVisible(7);

	setSelTextColor(getTextColor());
	setSelBackColor(getBackColor());

}

void FXMessageList::create()
{
	FXTreeList::create();
}

void FXMessageList::setMapFile(datafile *f)
{
    if (f != mapFile) {
        datablock::itor block, end;
        mapFile = f;
    }
}

void FXMessageList::addMessage(FXTreeItem* group, datablock * block)
{
    if (block->type() == block_type::TYPE_BATTLE) {
        datablock::itor region;
        if (mapFile->getRegion(region, block->x(), block->y(), block->info())) {
            FXString name = region->value(key_type::TYPE_NAME);
            FXString label;
            if (name.empty()) name.assign("Unbekannt");
            FXTreeItem* item = appendItem(group, label.format("In %s (%d, %d) findet ein Kampf statt.", name.text(), block->x(), block->y()));
            item->setData((void*)&*region);
        }
    }
    else if (block->type() == block_type::TYPE_MESSAGE) {
        FXTreeItem* item = appendItem(group, block->value("rendered"));
        FXival uid = block->getReference(block_type::TYPE_UNIT);

        datablock::itor select;
        if (uid > 0 && mapFile->getUnit(select, uid)) {
            item->setData((void*)&*select);
        }
        else {
            FXString loc = block->value("region");
            if (loc.empty()) {
                item->setData(nullptr);
            }
            else {
                int x, y, plane;
                x = FXIntVal(loc.section(' ', 0));
                y = FXIntVal(loc.section(' ', 1));
                plane = FXIntVal(loc.section(' ', 2));
                if (mapFile->getRegion(select, x, y, plane)) {
                    item->setData((void*)&*select);
                }
            }
        }
    }
}

long FXMessageList::onMapChange(FXObject*target, FXSelector sel, void*ptr)
{
    datafile::SelectionState* pstate = (datafile::SelectionState*)ptr;

    // any data changed, so need to update list?
    if (selection.fileChange != pstate->fileChange)
    {
        selection.fileChange = pstate->fileChange;
    }
    selection = *pstate;
    return 1;
}

long FXMessageList::onDoubleClick(FXObject* sender, FXSelector sel, void* ptr)
{
    if (this != sender) {
        return 0;
    }
	if (!mapFile)
		return 0;

	FXTreeItem* item = (FXTreeItem*)ptr;
	if (!item)
		return FXTreeList::onDoubleClicked(this, sel, ptr);

	// select MESSAGE.unit on double-click
    datablock* select = (datablock*)item->getData();
    if (select != nullptr)
	{
        datafile::SelectionState sel_state = selection;
        sel_state.selected = 0;

        if (select->type() == block_type::TYPE_REGION) {
            if (mapFile->getRegion(sel_state.region, select->x(), select->y(), select->info())) {
                sel_state.selected = selection.selected & selection.REGION;
            }
        }
        else {
            datablock::itor block;
            datablock::itor region, end = mapFile->blocks().end();
            mapFile->findSelection(select, block, region);
            if (region != end) {
                sel_state.region = region;
                sel_state.selected = selection.selected & selection.REGION;
            }
            if (select->type() == block_type::TYPE_UNIT) {
                sel_state.unit = block;
                sel_state.selected |= sel_state.UNIT;
            }
        }

		getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &sel_state);
		return 1;
	}

	return FXTreeList::onDoubleClicked(this, sel, ptr);
}

void FXMessageList::clearSiblings(FXTreeItem* parent_item)
{
    if (parent_item) {
        FXTreeItem* item = parent_item->getFirst();
        while (item)
        {
            FXTreeItem* next_item = item->getNext();
            removeItem(item);
            item = next_item;
        }
    }
}
