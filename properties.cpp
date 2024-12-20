#include "properties.h"
#include "csmap.h"
#include "FXMenuSeparatorEx.h"

FXProperties::FXProperties(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
    : FXTreeList(p, tgt, sel, opts | TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES, x, y, w, h)
{
}

FXTreeItem* FXProperties::makeItem(const FXString& label, datablock* block, const FXString* info)
{
    FXTreeItem* item = new FXTreeItem(label);
    if (block) {
        FXProperty* popup = new FXProperty(label);
        popup->block = block;
        if (info) {
            popup->info = *info;
        }
        item->setData(popup);	// show "info" popup cmd
    }
    return item;
}

FXTreeItem* FXProperties::makeStringList(FXTreeItem* p, const FXString& label, const datablock& block)
{
    FXTreeItem* node = appendItem(p, label);
    node->setExpanded(true);

    for (const datakey& key : block.data()) {
        appendItem(node, key.value());
    }
    return node;
}

FXTreeItem* FXProperties::makeUnitList(FXTreeItem* p, const FXString& label, datablock::itor begin, datablock::itor end, key_type key, int value)
{
    std::vector<FXTreeItem*> units;

    for (datablock::itor block = begin; block != end && block->type() != block_type::TYPE_REGION; ++block)
    {
        if (block->type() == block_type::TYPE_UNIT) {
            datablock* unitPtr = &*block;
            if (unitPtr->valueInt(key) == value) {
                units.push_back(makeItem(unitPtr->getLabel() + ": " + unitPtr->value(TYPE_NUMBER), unitPtr));
            }
        }
    }
    if (!units.empty()) {
        FXTreeItem* node = appendItem(p, label);
        node->setExpanded(true);
        for (FXTreeItem* item : units) {
            appendItem(node, item);
        }
        return node;
    }

    return nullptr;
}

void FXProperties::setMapFile(std::shared_ptr<datafile>& f)
{
    mapFile = f;
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

long FXProperties::onDoubleClicked(FXObject *sender, FXSelector sel, void *ptr)
{
    if (!mapFile)
        return 0;

    FXTreeItem *item = static_cast<FXTreeItem*>(ptr);
    if (item) {
        FXProperty* prop = static_cast<FXProperty*>(item->getData());
        if (prop) {
            return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_SETVALUE), prop->block);
        }
    }

    return FXTreeList::onDoubleClicked(this, sel, ptr);
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

long FXProperties::onPopup(FXObject* sender, FXSelector sel, void* ptr)
{
    onRightBtnRelease(sender, sel, ptr);

    // no datafile, no popup
    if (!mapFile)
        return 0;

    // find item to create popup for
    FXEvent* event = (FXEvent*)ptr;
    FXTreeItem* item = getCursorItem();
    if (!item) {
        item = getItemAt(event->click_x, event->click_y);
    }
    if (item) {
        FXProperty* popup = static_cast<FXProperty*>(item->getData());
        if (popup) {
            CSMap* csmap = static_cast<CSMap*>(getShell());
            FXMenuPane pane(this);
            FXMenuPane paneCascade(this);
            if (popup->block) {
                datablock* block = popup->block;
                FXString label = block->getName();
                if (label.length() <= 20) {
                    new FXMenuSeparatorEx(&pane, label);
                }
                else {
                    FXString str = label.left(17) + "...";
                    new FXMenuSeparatorEx(&pane, str);
                }
                if (block->type() == block_type::TYPE_UNIT) {
                    label.assign("&Zeige Einheit");
                }
                else if (block->type() == block_type::TYPE_SHIP) {
                    label.assign("Zeige &Schiff");
                }
                else if (block->type() == block_type::TYPE_BUILDING) {
                    label.assign(L"&Zeige Geb\u00e4ude");
                }
                if (!label.empty()) {
                    FXMenuCommand* command = new FXMenuCommand(&pane, label, nullptr, csmap, CSMap::ID_POPUP_GOTO);
                    command->setUserData(block);
                }
                csmap->addClipboardPane(&pane, block);
            }
            else {
                const FXString& label = item->getText();
                const FXString& info = popup->info;
                FXMenuCommand* command = new FXMenuCommand(&pane, "&Kopieren", nullptr, csmap, CSMap::ID_POPUP_COPY_TEXT);
                command->setUserData(const_cast<char*>(label.text()));
                (new FXMenuCommand(&pane, "Zeige &Info", nullptr, csmap, CSMap::ID_POPUP_SHOW_INFO))->setUserData(const_cast<char*>(info.text()));
            }

            pane.create();
            pane.popup(nullptr, event->root_x, event->root_y);
            return getApp()->runModalWhileShown(&pane);
        }
    }
    return 0;
}
