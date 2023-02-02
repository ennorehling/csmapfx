#include <vector>
#include "main.h"
#include "fxhelper.h"
#include "building_props.h"

// *********************************************************************************************************
// *** FXStatistics implementation

FXDEFMAP(FXBuildingProperties) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,			FXBuildingProperties::ID_UPDATE,				FXBuildingProperties::onMapChange), 
    FXMAPFUNC(SEL_QUERY_HELP,		0,									FXBuildingProperties::onQueryHelp),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,								FXBuildingProperties::onPopup),
};

FXIMPLEMENT(FXBuildingProperties, FXTreeList, MessageMap, ARRAYNUMBER(MessageMap))

FXBuildingProperties::FXBuildingProperties(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
		: FXProperties(p, tgt, sel, opts, x, y, w, h)
{
}

void FXBuildingProperties::makeItems()
{
    // clear list and build new
    clearItems();

    if (selection.selected & selection.BUILDING)
    {
        datablock::itor building = selection.building;
        datablock::itor end = mapFile->blocks().end();

        FXString name, descr, type;
        FXString aura, auramax, hero;
        FXint ownerId = -1, size = -1;

        std::vector<datakey::list_type::const_iterator> unhandled;

        for (datakey::list_type::const_iterator key = building->data().begin(); key != building->data().end(); ++key)
        {
            switch (key->type()) {
            case TYPE_NAME:
                name = key->value();
                break;
            case TYPE_DESCRIPTION:
                descr = key->value();
                break;
            case TYPE_OWNER:
                ownerId = atoi(key->value().text());
                break;
            case TYPE_TYPE:
                type = key->value();
                break;
            case TYPE_SIZE:
                size = key->getInt();
                break;
            case TYPE_FACTION:
                /* ignore */
                break;
            default:
                if (key->type() != TYPE_LOAD && key->type() != TYPE_MAXLOAD)
                    unhandled.push_back(key);
                break;
            }
        }
        FXString label;

        label.format("%s (%s)", name.text(), building->id().text());
        if (!type.empty()) {
            label += ", " + type;
        }
        if (size >= 0) {
            label += FXString(L", Gr\u00f6\u00dfe ") + FXStringVal(size);
        }
        FXTreeItem* root = appendItem(nullptr, label);
        root->setExpanded(true);

        datablock::itor unit = std::next(selection.region);
        if (ownerId > 0)
        {
            if (mapFile->getUnit(unit, ownerId)) {
                label.assign("Besitzer: ");
                label += mapFile->unitName(*unit, true);
                appendItem(root, makeItem(label, &*unit));
            }
        }

        label = building->value(TYPE_DESCRIPTION);
        if (!label.empty()) {
            appendItem(root, label);
        }

        // Kueste
        const wchar_t* coasts[] = { L"Nordwestk\u00fcste", L"Nordostk\u00fcste", L"Ostk\u00fcste", L"S\u00fcdostk\u00fcste", L"S\u00fcdwestk\u00fcste", L"Westk\u00fcste" };

        // list unhandled keys
        for (std::vector<datakey::list_type::const_iterator>::const_iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
        {
            label.format("%s: %s", (*itag)->translatedKey().text(), (*itag)->value().text());
            appendItem(root, label);
        }

        // find the EFFECTS block, if it exists
        for (datablock::itor block = std::next(building); block != end && block->depth() > building->depth(); ++block)
        {
            if (block->type() == block_type::TYPE_EFFECTS) {
                FXTreeItem* node = makeStringList(root, "Effekte", *block);
                break;
            }
        }
        // List units, if any:
        FXTreeItem* node = makeUnitList(root, "Einheiten", unit, end, TYPE_BUILDING, building->info());
    }
}

