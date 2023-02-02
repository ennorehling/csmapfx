#include "properties.h"
#include "csmap.h"
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
    if (item) {
        void* udata = item->getData();
        if (udata) {
            FXProperty* popup = static_cast<FXProperty*>(udata);
            CSMap* app = static_cast<CSMap*>(getShell());
            FXMenuPane menu(this);
            if (popup->block) {
                datablock* block = popup->block;
                app->createPopup(&menu, app, static_cast<const datablock*>(block), item->getText());

                FXString label;
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
                    FXMenuCommand* command = new FXMenuCommand(&menu, label, nullptr, app, CSMap::ID_POPUP_GOTO);
                    command->setUserData(block);
                }
            }
            else {
                const FXString& label = item->getText();
                const FXString& info = popup->info;
                app->createPopup(&menu, app, nullptr, label);
                FXMenuCommand* command = new FXMenuCommand(&menu, "&Kopieren", nullptr, app, CSMap::ID_POPUP_COPY_TEXT);
                command->setUserData(const_cast<char*>(label.text()));
                (new FXMenuCommand(&menu, "Zeige &Info", nullptr, app, CSMap::ID_POPUP_SHOW_INFO))->setUserData(const_cast<char*>(info.text()));
            }

            menu.create();
            menu.popup(nullptr, event->root_x, event->root_y);
            return getApp()->runModalWhileShown(&menu);
        }
    }
    return 0;
}
