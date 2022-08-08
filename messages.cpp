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

FXMessages::FXMessages(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXTreeList(p, tgt,sel, opts|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES, x,y,w,h)
{
	// init variables
	mapFile = nullptr;

	// set styles...
	setNumVisible(7);

	setSelTextColor(getTextColor());
	setSelBackColor(getBackColor());

	groups.messages = appendItem(nullptr, "Meldungen");
	groups.effects = appendItem(nullptr, "Effekte");
	groups.travel = appendItem(nullptr, "Durchreise");
    groups.battle = nullptr;
	groups.streets = appendItem(nullptr, FXString(L"Stra\u00dfen"));
	groups.guards = appendItem(nullptr, "Bewacher");
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
    if (f != mapFile) {
        datablock::itor block, end;
        mapFile = f;
    }
}

void FXMessages::addMessage(FXTreeItem* group, datablock * block)
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
        FXival uid = block->valueInt("target");

        if (uid <= 0)
            uid = block->valueInt("unit");
        if (uid <= 0)
            uid = block->valueInt("mage");
        if (uid <= 0)
            uid = block->valueInt("spy");
        if (uid <= 0)
            uid = block->valueInt("teacher");

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

long FXMessages::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

	// any data changed, so need to update list?
	if (selection.fileChange != pstate->fileChange)
	{
		selection.fileChange = pstate->fileChange;
		selection.map = pstate->map;
		selection.activefaction = pstate->activefaction;
	}

	if (selection.selChange != pstate->selChange)
	{
        selection = *pstate;

		clearSiblings(groups.effects);
		clearSiblings(groups.streets);
		clearSiblings(groups.travel);
		clearSiblings(groups.messages);
        if (groups.battle) {
            clearSiblings(groups.battle);
            removeItem(groups.battle);
            groups.battle = nullptr;
        }
		clearSiblings(groups.guards);

        if (mapFile) {
            if (selection.selected & selection.REGION)
            {
                datablock::itor region = selection.region;
                datablock::itor end = mapFile->blocks().end();
                datablock::itor block;
                std::set<FXint> guard_ids;
                if (mapFile->getBattle(block, selection.sel_x, selection.sel_y, selection.sel_plane)) {
                    int depth = block->depth();
                    groups.battle = appendItem(nullptr, "Kampf");
                    for (++block; block != end && block->depth() > depth; block++)
                    {
                        if (block->type() == block_type::TYPE_MESSAGE) {
                            addMessage(groups.battle, &*block);
                        }
                    }
                }
                block = region;
                for (block++; block != end && block->depth() > region->depth(); block++)
                {
                    if (block->type() == block_type::TYPE_MESSAGE) {
                        /*
                        MESSAGE 324149248
                        "von Figo (g351): 'KABUMM *kicher*'";rendered
                        */
                        addMessage(groups.messages, &*block);
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
                            appendItem(groups.travel, prefix + msg->value());
                    }
                }
                if (!guard_ids.empty()) {
                    for (FXint id : guard_ids) {
                        datablock::itor faction;
                        if (mapFile->getFaction(faction, id)) {
                            FXString label = faction->value("Parteiname") + " (" + faction->id() + ")";
                            appendItem(groups.guards, label);
                        }
                    }
                }
            }
        }
	}

	return 1;
}

long FXMessages::onDoubleClick(FXObject* sender, FXSelector sel, void* ptr)
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
        datafile::SelectionState sel_state;
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
