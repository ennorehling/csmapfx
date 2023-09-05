#include "fxhelper.h"
#include "messagelist.h"

#include <fxkeys.h>
#include <set>

FXDEFMAP(FXMessageList) FXMessageListMap[] =
{
    //________Message_Type_____________________ID_______________Message_Handler_______
    FXMAPFUNC(SEL_DOUBLECLICKED,	    0,							    FXMessageList::onDoubleClicked),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,  						    FXMessageList::onRightBtnRelease),
    FXMAPFUNC(SEL_KEYPRESS,         	0,  						    FXMessageList::onKeyPress),
    FXMAPFUNC(SEL_COMMAND,			    FXMessageList::ID_UPDATE,	    FXMessageList::onMapChange),
    FXMAPFUNC(SEL_COMMAND,			    FXMessageList::ID_POPUP_COPY,	FXMessageList::onPopupCopy)
};

FXIMPLEMENT(FXMessageList, FXTreeList, FXMessageListMap, ARRAYNUMBER(FXMessageListMap))

FXMessageList::FXMessageList(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXTreeList(p, tgt,sel, opts|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES, x,y,w,h)
{
	// set styles...
	setNumVisible(7);
}

void FXMessageList::create()
{
	FXTreeList::create();
}

void FXMessageList::setMapFile(std::shared_ptr<datafile>& f)
{
    if (f != mapFile) {
        datablock::itor block, end;
        mapFile = f;
    }
}

void FXMessageList::setClipboard(const FXString& text)
{
    getShell()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), (void*)text.text());
}

void FXMessageList::addMessage(FXTreeItem* group, const datablock& block)
{
    if (block.type() == block_type::TYPE_BATTLE) {
        datablock::itor region;
        if (mapFile->getRegion(region, block)) {
            FXString name = region->value(key_type::TYPE_NAME);
            FXString label;
            if (name.empty()) name.assign("Unbekannt");
            FXTreeItem* item = appendItem(group, label.format("In %s (%d,%d) findet ein Kampf statt.", name.text(), block.x(), block.y()));
            item->setData((void*)&*region);
        }
    }
    else if (block.type() == block_type::TYPE_MESSAGE) {
        FXTreeItem* item = appendItem(group, block.value("rendered"));

        item->setData(mapFile->getMessageTarget(block));
    }
}
long FXMessageList::onMapChange(FXObject* sender, FXSelector sel, void* ptr)
{
    datafile::SelectionState* pstate = (datafile::SelectionState*)ptr;

    selection = *pstate;
    return 1;
}

long FXMessageList::onPopupCopy(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* cmd = static_cast<FXMenuCommand *>(sender);
    FXTreeItem* item = static_cast<FXTreeItem*>(cmd->getUserData());
    if (item) {
        setClipboard(item->getText());
    }
    return 0;
}

long FXMessageList::onDoubleClicked(FXObject* sender, FXSelector sel, void* ptr)
{
	if (!mapFile)
		return 0;

	FXTreeItem* item = (FXTreeItem*)ptr;
	if (!item)
		return FXTreeList::onDoubleClicked(this, sel, ptr);

	// select MESSAGE.unit on double-click
    datablock* select = (datablock*)item->getData();
    if (select != nullptr)
	{
        return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_SETVALUE), select);
	}

	return FXTreeList::onDoubleClicked(this, sel, ptr);
}

long FXMessageList::onRightBtnRelease(FXObject* sender, FXSelector sel, void* ptr)
{
    if (mapFile) {
        FXEvent* event = (FXEvent*)ptr;
        FXTreeItem* item = getCursorItem();
        if (!item) {
            item = getItemAt(event->click_x, event->click_y);
        }
        if (item && item->getData()) {
            showPopup(item, event->root_x, event->root_y);
        }
    }
    return FXTreeList::onRightBtnRelease(this, sel, ptr);
}

long FXMessageList::onKeyPress(FXObject* sender, FXSelector sel, void* ptr)
{
    FXEvent* event = (FXEvent*)ptr;
    if (event->code == KEY_F10 && event->state & SHIFTMASK) {
        FXTreeItem* item = getCurrentItem();
        if (item && item->getData()) {
            showPopup(item, event->root_x, event->root_y);
        }
    }
    return FXTreeList::onKeyPress(sender, sel, ptr);
}

void FXMessageList::showPopup(FXTreeItem* item, FXint x, FXint y)
{
    FXMenuPane pane(this);

    FXMenuCommand* cmd;
    cmd = new FXMenuCommand(&pane, "&Kopieren", nullptr, this, FXMessageList::ID_POPUP_COPY);
    cmd->setUserData(item);

    pane.create();
    pane.popup(nullptr, x, y);
    getApp()->runModalWhileShown(&pane);
}

void FXMessageList::clearChildren(FXTreeItem* parent_item)
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
