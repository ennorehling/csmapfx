#include "properties.h"
#include "FXMenuSeparatorEx.h"

class FXProperty : public FXTreeItem
{
public:
    FXProperty(const FXString& text, FXIcon* oi = NULL, FXIcon* ci = NULL, void* ptr = NULL)
        : FXTreeItem(text, oi, ci, ptr)
    {
    }
    datablock* block = nullptr;
    FXString info;
};

class FXPopupMenuCommand : public FXMenuCommand
{
public:
    FXPopupMenuCommand(FXComposite* p, const FXString& text, const FXString& info_key, FXIcon* ic = NULL, FXObject* tgt = NULL, FXuint opts = 0)
        : FXMenuCommand(p, text, ic, tgt, FXProperties::ID_POPUP_SHOW_INFO, opts), info(info_key) {}

    const FXString& getInfo() const { return info; }
private:
    FXString info;
};


FXProperties::FXProperties(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
    : FXTreeList(p, tgt, sel, opts, x, y, w, h)
{
}

FXTreeItem* FXProperties::makeItem(const FXString& label, datablock* block, const FXString* info)
{
    FXProperty* popup = new FXProperty(label);
    popup->block = block;
    if (info) {
        popup->info = *info;
    }
    popup->setData(popup);	// show "info" popup cmd
    return popup;
}

FXTreeItem* FXProperties::makeStringList(FXTreeItem* parent, const FXString& label, const datablock& block)
{
    FXTreeItem* node = appendItem(parent, label);
    node->setExpanded(true);

    for (const datakey& key : block.data()) {
        appendItem(node, key.value());
    }
    return node;
}

FXTreeItem* FXProperties::makeUnitList(FXTreeItem* parent, const FXString& label, datablock::itor begin, datablock::itor end, key_type key, int value)
{
    std::vector<FXTreeItem*> units;

    for (datablock::itor block = begin; block != end && block->type() != block_type::TYPE_REGION; ++block)
    {
        if (block->type() == block_type::TYPE_UNIT) {
            datablock* unitPtr = &*block;
            int unitId = unitPtr->info();
            if (unitPtr->valueInt(key) == value) {
                FXString label = unitPtr->getLabel() + ": " + unitPtr->value(TYPE_NUMBER);
                units.push_back(makeItem(label, unitPtr));
            }
        }
    }
    if (!units.empty()) {
        FXTreeItem* node = appendItem(parent, label);
        node->setExpanded(true);
        for (FXTreeItem* item : units) {
            appendItem(node, item);
        }
        return node;
    }

    return nullptr;
}

void FXProperties::setMapFile(datafile* f)
{
    mapFile = f;
}

void FXProperties::setClipboard(const FXString& text)
{
    getShell()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), (void*)text.text());
}

void FXProperties::showInfo(const FXString& text)
{
    getShell()->handle(this, FXSEL(SEL_QUERY_HELP, ID_SETSTRINGVALUE), (void*)text.text());
}

long FXProperties::onQueryHelp(FXObject* /*sender*/, FXSelector, void* /*ptr*/)
{
    return 0;
}
long FXProperties::onMapChange(FXObject*, FXSelector, void* ptr)
{
    datafile::SelectionState* pstate = (datafile::SelectionState*)ptr;

    // any data changed, so need to update list?
    if (selection.fileChange != pstate->fileChange)
    {
        selection.fileChange = pstate->fileChange;

        // clear list and build a new one from data in this->files
        //clearItems();
    }

    if (selection.selChange != pstate->selChange)
    {
        selection = *pstate;
        makeItems();
    }
    return 0;
}

FXString FXProperties::weightToString(int prop)
{
    int full = prop / 100;
    int remain = prop % 100;
    FXString result = FXStringVal(full);
    if (remain >= 10) {
        result += "." + FXStringVal(remain);
    }
    else if (remain > 0) {
        result += ".0" + FXStringVal(remain);
    }
    return result;
}

static const wchar_t* coasts[] = { L"Nordwestk\u00fcste", L"Nordostk\u00fcste", L"Ostk\u00fcste", L"S\u00fcdostk\u00fcste", L"S\u00fcdwestk\u00fcste", L"Westk\u00fcste" };

FXString FXProperties::coastToString(int prop)
{
    if (prop >= 0 && prop < 6) {
        return FXString(coasts[prop]);
    }
    return FXString("Unbekannt");
}

long FXProperties::onShowInfo(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender)
    {
        FXPopupMenuCommand* command = static_cast<FXPopupMenuCommand*>(sender);
        showInfo(command->getInfo());
        setClipboard(command->getText());
        return 1;
    }
    return 0;
}

long FXProperties::onGotoItem(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* command = static_cast<FXMenuCommand*>(sender);
    const datablock* block = static_cast<const datablock*>(command->getUserData());
    datafile::SelectionState sel_state = selection;
    sel_state.selChange++;
    if (sel_state.regionsSelected.empty()) {
        sel_state.selected = 0;
    }
    else {
        sel_state.regionsSelected = selection.regionsSelected;
    }
    if (block->type() == block_type::TYPE_UNIT)
    {
        if (mapFile->getUnit(sel_state.unit, block->info())) {
            sel_state.selected |= selection.UNIT;
        }
    }
    else if (block->type() == block_type::TYPE_REGION)
    {
        if (mapFile->getRegion(sel_state.region, block->x(), block->y(), block->info())) {
            sel_state.selected |= selection.REGION;
        }
    }
    else if (block->type() == block_type::TYPE_BUILDING)
    {
        if (mapFile->getBuilding(sel_state.building, block->info())) {
            sel_state.selected |= selection.BUILDING;
        }
    }
    else if (block->type() == block_type::TYPE_SHIP)
    {
        if (mapFile->getBuilding(sel_state.ship, block->info())) {
            sel_state.selected |= selection.SHIP;
        }
    }
    else if (block->type() == block_type::TYPE_FACTION)
    {
        if (mapFile->getFaction(sel_state.faction, block->info())) {
            sel_state.selected |= selection.FACTION;
        }
    }
    return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &sel_state);
}

long FXProperties::onPopup(FXObject* sender, FXSelector sel, void* ptr)
{
    onRightBtnRelease(sender, sel, ptr);

    // no datafile, no popup
    if (!mapFile)
        return 0;

    FXEvent* event = (FXEvent*)ptr;

    // dont't show popup if mouse has moved
    if (event->last_x != event->click_x || event->last_y != event->click_y)
        return 0;

    // find item to create popup for
    FXTreeItem* item = getItemAt(event->click_x, event->click_y);
    if (!item)
        return 0;

    FXMenuPane* menu = new FXMenuPane(this);
    FXMenuSeparatorEx* sep = nullptr;

    FXString title = item->getText();
    if (title.length() > 20)
        title = title.left(17) + "...";

    void* udata = item->getData();
    if (udata) {
        FXProperty* popup = static_cast<FXProperty*>(item);
        if (popup->block) {
            datablock* block = popup->block;
            FXMenuCommand* command = nullptr;
            if (!sep) {
                sep = new FXMenuSeparatorEx(menu, title);
            }
            if (block->type() == block_type::TYPE_UNIT) {
                command = new FXMenuCommand(menu, "Zeige &Einheit", nullptr, this, ID_POPUP_SELECT);
            }
            else if (block->type() == block_type::TYPE_SHIP) {
                command = new FXMenuCommand(menu, "Zeige &Schiff", nullptr, this, ID_POPUP_SELECT);
            }
            else if (block->type() == block_type::TYPE_BUILDING) {
                command = new FXMenuCommand(menu, L"Zeige &Geb\u00e4ude", nullptr, this, ID_POPUP_SELECT);
            }
            if (command) {
                command->setUserData(block);
            }
            command = new FXMenuCommand(menu, "&Kopieren", nullptr, this, ID_POPUP_COPY_TEXT);
            command->setUserData(block);
        }
        else {
            new FXPopupMenuCommand(menu, "Zeige Info", popup->info, nullptr, this);
        }
    }
    // show popup
    if (menu->numChildren()) {
        menu->create();
        menu->popup(nullptr, event->root_x, event->root_y);
        getApp()->runModalWhileShown(menu);
    }
    delete menu;
    return 1;
}

long FXProperties::onCopyText(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender)
    {
        FXMenuCommand* cmd = static_cast<FXMenuCommand*>(sender);
        const datablock* block = static_cast<const datablock*>(cmd->getUserData());
        if (block) {
            setClipboard(block->id());
            return 1;
        }
    }
    return 0;
}

