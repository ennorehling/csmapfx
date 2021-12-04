#include "main.h"
#include "fxhelper.h"
#include "messages.h"

#include <set>

// *********************************************************************************************************
// *** FXMessages implementation

FXDEFMAP(FXMessages) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,			FXMessages::ID_UPDATE,			FXMessages::onMapChange),

	FXMAPFUNC(SEL_DOUBLECLICKED,	0,								FXMessages::onDoubleClick),
};

FXIMPLEMENT(FXMessages,FXTreeList,MessageMap, ARRAYNUMBER(MessageMap))

FXMessages::FXMessages(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXTreeList(p, tgt,sel, opts|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES, x,y,w,h)
{
	// init variables
	mapFile = NULL;

	// set styles...
	setNumVisible(7);

	setSelTextColor(getTextColor());
	setSelBackColor(getBackColor());

	groups.effects = appendItem(NULL, "Effekte");
	groups.travel = appendItem(NULL, "Durchreise");
	groups.other = appendItem(NULL, "Sonstiges");
	groups.streets = appendItem(NULL, FXString(L"Stra\u00dfen"));
	groups.guards = appendItem(NULL, "Bewacher");
}

void FXMessages::create()
{
	FXTreeList::create();
}

FXMessages::~FXMessages()
{
}

void FXMessages::setMapFile(datafile *f)
{
    mapFile = f;
}

long FXMessages::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *sel_state = (datafile::SelectionState*)ptr;

	// any data changed, so need to update list?
	if (selection.fileChange != sel_state->fileChange)
	{
		selection.fileChange = sel_state->fileChange;
		selection.map = sel_state->map;
		selection.activefaction = sel_state->activefaction;
	}

	if (selection.selChange != sel_state->selChange)
	{
		selection.selChange = sel_state->selChange;
		selection.selected = sel_state->selected;

		selection.region = sel_state->region;
		selection.faction = sel_state->faction;
		selection.building = sel_state->building;
		selection.ship = sel_state->ship;
		selection.unit = sel_state->unit;

		clearSiblings(groups.effects);
		clearSiblings(groups.streets);
		clearSiblings(groups.travel);
		clearSiblings(groups.other);
		clearSiblings(groups.guards);

		if (mapFile && selection.selected & selection.REGION)
		{
			datablock::itor region = selection.region;
			datablock::itor end = mapFile->blocks().end();
			datablock::itor block = region;
            std::set<FXint> guard_ids;
			for (block++; block != end && block->depth() > region->depth(); block++)
			{
                if (block->type() == block_type::TYPE_MESSAGE) {
                    /*
                    MESSAGE 324149248
                    "von Figo (g351): 'KABUMM *kicher*'";rendered
                    */

                    FXTreeItem *item = appendItem(groups.other, block->value("rendered"));

                    FXival unit = block->valueInt("unit");
                    if (!unit)
                        unit = block->valueInt("mage");
                    if (!unit)
                        unit = block->valueInt("teacher");

                    item->setData((void *)unit);
                }
                else if (block->depth() > region->depth() + 1) {
                    continue;
                }
                else if (block->type() == block_type::TYPE_UNIT) {
                    int guard = block->valueInt("bewacht");
                    if (guard) {
                        FXint faction = block->valueInt("Partei");
                        guard_ids.insert(faction);
                    }
                }
				else if (block->type() == block_type::TYPE_EFFECTS)
				{
					/*
					EFFECTS
					"Untote schrecken vor dieser Region zurueck. (njr4)"
					*/

					for (datakey::itor msg = block->data().begin(); msg != block->data().end(); msg++)
						appendItem(groups.effects, msg->value());
				}
				else if (block->type() == block_type::TYPE_BORDER)
				{
					/*
					GRENZE 1
					"Stra\u00dfe";typ
					1;richtung
					100;prozent
					*/

					const wchar_t* directions[] = { L"Nordwesten", L"Nordosten", L"Osten", L"S\u00fcdosten", L"S\u00fcdwesten", L"Westen", L"-unknown-" };

					FXint dir = 0;
					if (block->value("richtung").length())
						dir = atoi(block->value("richtung").text());

					if (dir < 0 || dir > 6)
						dir = 6;

					FXString typ = block->value("typ");

					FXint procent = 0;
					if (block->value("prozent").length())
						procent = atoi(block->value("prozent").text());

					FXString artikel = "eine ";

					FXString label;
					label = FXString("Im ") + FXString(directions[dir]) + " befindet sich ";

					if (procent == 100)
						label += artikel + typ + ".";
					else
						label += artikel + "zu " + FXStringVal(procent) + "% vollendete " + typ + ".";

					appendItem(groups.streets, label);
				}
				else if (block->type() == block_type::TYPE_DURCHREISE || block->type() == block_type::TYPE_DURCHSCHIFFUNG)
				{
					/*
					DURCHREISE
					"Fuhrwagenlenker zu Xorlosch (g5te)"
					*/
					FXString prefix;

					if (block->type() == block_type::TYPE_DURCHSCHIFFUNG)
						prefix = "Die ";	// fuer Schiffe

					for (datakey::itor msg = block->data().begin(); msg != block->data().end(); msg++)
						appendItem(groups.travel, prefix+msg->value());
				}
			}
            if (!guard_ids.empty()) {
                for (FXint id : guard_ids) {
                    datablock::itor faction = mapFile->faction(id);
                    if (faction != mapFile->end()) {
                        FXString label = faction->value("Parteiname") + " (" + faction->id() + ")";
                        appendItem(groups.guards, label);
                    }
                }
            }
		}
	}

	return 1;
}

long FXMessages::onDoubleClick(FXObject* sender, FXSelector sel, void* ptr)
{
	if (!mapFile)
		return 0;

	FXTreeItem* item = (FXTreeItem*)ptr;
	if (!item)
		return FXTreeList::onDoubleClicked(sender, sel, ptr);

	// select MESSAGE.unit on double-click
    if (int id = (FXival)item->getData())
	{
        datafile::SelectionState sel_state;

		sel_state.selected = selection.selected & selection.REGION;
		sel_state.region = selection.region;

		datablock::itor unit = mapFile->unit(id);
		if (unit != mapFile->blocks().end())
		{
			sel_state.unit = unit;
			sel_state.selected |= sel_state.UNIT;
		}

		getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &sel_state);
		return 1;
	}

	return FXTreeList::onDoubleClicked(sender, sel, ptr);
}

void FXMessages::clearSiblings(FXTreeItem* parent_item)
{
	FXTreeItem *item = parent_item->getFirst();
	while (item)
	{
        FXTreeItem *next_item = item->getNext();
		removeItem(item);
		item = next_item;
	}
}
