#include "main.h"
#include "fxhelper.h"
#include "messages.h"

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
	files = NULL;

	// set styles...
	setNumVisible(7);

	setSelTextColor(getTextColor());
	setSelBackColor(getBackColor());
	
	groups.effects = appendItem(NULL, "Effekte");
	groups.travel = appendItem(NULL, "Durchreise");
	groups.other = appendItem(NULL, "Sonstiges");
	groups.streets = appendItem(NULL, iso2utf("Straßen"));
}

void FXMessages::create()
{
	FXTreeList::create();
}

FXMessages::~FXMessages()
{
}

void FXMessages::mapfiles(std::list<datafile> *f)
{
    files = f;
}

long FXMessages::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *state = (datafile::SelectionState*)ptr;

	// connected to a datafile list?
	if (!files)
		return 0;

	// any data changed, so need to update list?
	if (selection.fileChange != state->fileChange)
	{
		selection.fileChange = state->fileChange;
		selection.map = state->map;
		selection.activefaction = state->activefaction;
	}

	if (selection.selChange != state->selChange)
	{
		selection.selChange = state->selChange;
		selection.selected = state->selected;
		
		selection.region = state->region;
		selection.faction = state->faction;
		selection.building = state->building;
		selection.ship = state->ship;
		selection.unit = state->unit;

		clearSiblings(groups.effects);
		clearSiblings(groups.streets);
		clearSiblings(groups.travel);
		clearSiblings(groups.other);

		if (selection.selected & selection.REGION)
		{
			datablock::itor region = selection.region;

			datablock::itor end = files->front().blocks().end();
			datablock::itor block = region;
			for (block++; block != end && block->depth() > region->depth(); block++)
			{
				if (block->type() == datablock::TYPE_UNIT || block->type() == datablock::TYPE_SHIP
						|| block->type() == datablock::TYPE_BUILDING)
					break;
				else if (block->type() == datablock::TYPE_EFFECTS)
				{
					/*
					EFFECTS
					"Untote schrecken vor dieser Region zurück. (njr4)"
					*/

					for (datakey::itor msg = block->data().begin(); msg != block->data().end(); msg++)
						appendItem(groups.effects, msg->value());
				}
				else if (block->type() == datablock::TYPE_BORDER)
				{
					/*
					GRENZE 1
					"Straße";typ
					1;richtung
					100;prozent
					*/

					char* directions[] = { "Nordwesten", "Nordosten", "Osten", "Südosten", "Südwesten", "Westen", "-unknown-" };

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
					label = FXString("Im ") + iso2utf(directions[dir]) + " befindet sich ";

					if (procent == 100)
						label += artikel + typ + ".";
					else
						label += artikel + "zu " + FXStringVal(procent) + "% vollendete " + typ + ".";

					appendItem(groups.streets, label);
				}
				else if (block->type() == datablock::TYPE_DURCHREISE || block->type() == datablock::TYPE_DURCHSCHIFFUNG)
				{
					/*
					DURCHREISE
					"Fuhrwagenlenker zu Xorlosch (g5te)"
					*/
					FXString prefix;

					if (block->type() == datablock::TYPE_DURCHSCHIFFUNG)
						prefix = "Die ";	// für Schiffe

					for (datakey::itor msg = block->data().begin(); msg != block->data().end(); msg++)
						appendItem(groups.travel, prefix+msg->value());
				}
				else if (block->type() == datablock::TYPE_MESSAGE)
				{
					/*
					MESSAGE 324149248
					"von Figo (g351): 'KABUMM *kicher*'";rendered
					*/
					
					FXTreeItem* item = appendItem(groups.other, block->value("rendered"));
					
					FXival unit = block->valueInt("unit");
					if (!unit)
						unit = block->valueInt("mage");
					if (!unit)
						unit = block->valueInt("teacher");

					item->setData((void*)unit);
				}
			}
		}
	}

	return 1;
}

long FXMessages::onDoubleClick(FXObject* sender, FXSelector sel, void* ptr)
{
	if (!files || !files->size())
		return 0;

	FXTreeItem* item = (FXTreeItem*)ptr;
	if (!item)
		return FXTreeList::onDoubleClicked(sender, sel, ptr);

	// select MESSAGE.unit on double-click
    if (int id = (FXival)item->getData())
	{
        datafile::SelectionState state;

		state.selected = selection.selected & selection.REGION;
		state.region = selection.region;

		datablock::itor unit = files->front().unit(id);
		if (unit != files->front().blocks().end())
		{		
			state.unit = unit;
			state.selected |= state.UNIT;
		}

		getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
		return 1;
	}

	return FXTreeList::onDoubleClicked(sender, sel, ptr);
}

void FXMessages::clearSiblings(FXTreeItem* parent)
{
	FXTreeItem *next, *item = parent->getFirst();
	while (item)
	{
		next = item->getNext();
		removeItem(item);
		item = next;
	}
}
