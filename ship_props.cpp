#include <vector>
#include "fxhelper.h"
#include "ship_props.h"

// *********************************************************************************************************
// *** FXStatistics implementation

FXDEFMAP(FXShipProperties) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
    FXMAPFUNC(SEL_DOUBLECLICKED,	    0,							    FXShipProperties::onDoubleClicked),
    FXMAPFUNC(SEL_COMMAND,			    FXShipProperties::ID_UPDATE,    FXShipProperties::onMapChange),
    FXMAPFUNC(SEL_QUERY_HELP,           0,                              FXShipProperties::onQueryHelp),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,                              FXShipProperties::onPopup),
};

FXIMPLEMENT(FXShipProperties, FXProperties, MessageMap, ARRAYNUMBER(MessageMap))

FXShipProperties::FXShipProperties(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
    : FXProperties(p, tgt, sel, opts, x, y, w, h)
{
}

void FXShipProperties::makeItems()
{
    // clear list and build new
    clearItems();

    if (selection.selected & selection.SHIP)
    {
        datablock::itor ship = selection.ship;
        datablock::itor end = mapFile->blocks().end();

        FXString name, type;
        FXString aura, auramax, hero;
        FXint captainId = -1, coast = -1, size = -1, cargo = -1, damage = -1, capacity = -1;

        std::vector<datakey::list_type::const_iterator> unhandled;

        for (datakey::list_type::const_iterator key = ship->data().begin(); key != ship->data().end(); ++key)
        {
            switch (key->type()) {
            case TYPE_NAME:
                name = key->value();
                break;
            case TYPE_CAPTAIN:
                captainId = atoi(key->value().text());
                break;
            case TYPE_TYPE:
                type = key->value();
                break;
            case TYPE_SIZE:
                size = key->getInt();
                break;
            case TYPE_DAMAGE:
                damage = key->getInt();
                break;
            case TYPE_COAST:
                coast = key->getInt();
                break;
            case TYPE_CARGO:
                cargo = key->getInt();
                break;
            case TYPE_CAPACITY:
                capacity = key->getInt();
                break;
            case TYPE_FACTION:
            case TYPE_DESCRIPTION:
            case TYPE_NOTES:
                /* ignore */
                break;
            default:
                if (key->type() != TYPE_LOAD && key->type() != TYPE_MAXLOAD)
                    unhandled.push_back(key);
                break;
            }
        }
        FXString label;

        label.format("%s (%s)", name.text(), ship->id().text());
        if (!type.empty()) {
            label += ", " + type;
        }
        if (size >= 0) {
            label += FXString(L", Gr\u00f6\u00dfe ") + FXStringVal(size);
        }
        FXTreeItem* root = appendItem(nullptr, label);
        root->setExpanded(true);

        datablock::itor unit = std::next(selection.region);
        if (captainId > 0)
        {
            if (mapFile->getUnit(unit, captainId)) {
                label.assign(L"Kapit\u00e4n: ");
                label += mapFile->unitName(*unit, true);
                FXTreeItem* item = appendItem(root, makeItem(label, &*unit));
            }
        }

        // Kueste
        const wchar_t* coasts[] = { L"Nordwestk\u00fcste", L"Nordostk\u00fcste", L"Ostk\u00fcste", L"S\u00fcdostk\u00fcste", L"S\u00fcdwestk\u00fcste", L"Westk\u00fcste" };

        if (coast >= 0 && coast < 6) {
            appendItem(root, coastToString(coast));
        }

        // Beladung (cargo/capacity)
        if (cargo > 0 || capacity > 0)
        {
            FXString car = weightToString(cargo);
            FXString cap = weightToString(capacity);
            appendItem(root, "Beladung: " + car + " von " + cap + " GE");
        }

        // Schaden
        if (damage >= 0)
            appendItem(root, FXStringVal(damage) + FXString(L"% besch\u00e4digt"));

        // list unhandled keys
        for (std::vector<datakey::list_type::const_iterator>::const_iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
        {
            label.format("%s: %s", (*itag)->translatedKey().text(), (*itag)->value().text());
            appendItem(root, label);
        }

        // find the EFFECTS block, if it exists
        for (datablock::itor block = std::next(ship); block != end && block->depth() > ship->depth(); ++block)
        {
            if (block->type() == block_type::TYPE_EFFECTS) {
                makeStringList(root, "Effekte", *block);
                break;
            }
        }
        // List units, if any:
        FXTreeItem *unitList = makeUnitList(root, "Einheiten", unit, end, TYPE_SHIP, ship->info());
        if (unitList) {
            int totalSkill = 0;
            for (FXTreeItem *item = unitList->getFirst(); item; item = item->getNext())
            {
                FXProperty *prop = static_cast<FXProperty *>(item->getData());
                if (prop && prop->block)
                {
                    int id = prop->block->info();
                    datablock::itor unit;
                    if (mapFile->getUnit(unit, id)) {
                        datablock::itor block;
                        int number = unit->valueInt(key_type::TYPE_NUMBER);
                        if (number > 0 && mapFile->getChild(block, unit, block_type::TYPE_TALENTS))
                        {
                            int skill = block->valueSkill("Segeln");
                            totalSkill += skill * number;
                        }
                    }
                }
            }
            if (totalSkill > 0) {
                insertItem(unitList, root, label.format("Segeltalent: %d", totalSkill));
            }
        }
    }
}

