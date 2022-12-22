#include "main.h"
#include "fxhelper.h"
#include "reportinfo.h"

#include <set>

// *********************************************************************************************************
// *** FXReportInfo implementation

FXDEFMAP(FXReportInfo) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,			FXReportInfo::ID_UPDATE,	FXReportInfo::onMapChange),

	FXMAPFUNC(SEL_DOUBLECLICKED,	0,							FXReportInfo::onDoubleClick),
};

FXIMPLEMENT(FXReportInfo,FXTreeList,MessageMap, ARRAYNUMBER(MessageMap))

FXReportInfo::FXReportInfo(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXTreeList(p, tgt, sel, opts|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES, x, y, w, h)
{
	// init variables
	mapFile = nullptr;

	// set styles...
	setNumVisible(7);

	setSelTextColor(getTextColor());
	setSelBackColor(getBackColor());

	groups.messages = appendItem(nullptr, "Meldungen");
	groups.factions = appendItem(nullptr, "Parteien");
	groups.battles = appendItem(nullptr, L"K\u00e4mpfe");
}

void FXReportInfo::create()
{
	FXTreeList::create();
}

FXReportInfo::~FXReportInfo()
{
}

void FXReportInfo::setMapFile(datafile *f)
{
    if (f != mapFile) {
        datablock::itor it, end;
        mapFile = f;
        clearSiblings(groups.messages);
        clearSiblings(groups.factions);
        clearSiblings(groups.battles);
        if (mapFile) {
            end = mapFile->blocks().end();
            for (it = mapFile->blocks().begin(); it != end;) {
                datablock* block = &*it;
                if (block->type() == block_type::TYPE_REGION) {
                    break;
                }
                else if (block->type() == block_type::TYPE_BATTLE) {
                    addBattle(it);
                    /* block is already on the next object */
                    continue;
                }
                else if (block->type() == block_type::TYPE_FACTION) {
                    addFaction(it);
                    continue;
                }
                else if (block->type() == block_type::TYPE_MESSAGE && block->depth() == 3) {
                    addMessage(groups.messages, block);
                }
                ++it;
            }
        }
    }
}

const char *FXReportInfo::messageSection(const FXString& section)
{
    if (section == "errors") return "Fehler";
    if (section == "magic") return "Magie";
    if (section == "production") return "Produktion";
    if (section == "movement") return "Bewegungen";
    if (section == "economy") return "Wirtschaft";
    if (section == "events") return "Ereignisse";
    if (section == "study") return "Ausbildung";
    return nullptr;
}

void FXReportInfo::addMessage(FXTreeItem *group, datablock * msg)
{
    FXTreeItem* item;
    FXival uid = msg->valueInt("target");
    FXString section = msg->value("section");

    if (!section.empty()) {
        const char *text = messageSection(section);
        if (text != nullptr) {
            FXTreeItem* child;
            FXString label(text);
            for (child = group->getFirst(); child != nullptr; child = child->getNext()) {
                if (child->getText() == label) {
                    group = child;
                    break;
                }
            }
            if (child == nullptr) {
                group = prependItem(group, label);
            }
        }
    }
    item = appendItem(group, msg->value("rendered"));

    if (uid <= 0)
        uid = msg->valueInt("unit");
    if (uid <= 0)
        uid = msg->valueInt("mage");
    if (uid <= 0)
        uid = msg->valueInt("spy");
    if (uid <= 0)
        uid = msg->valueInt("teacher");

    datablock::itor select;
    if (uid > 0 && mapFile->getUnit(select, uid)) {
        item->setData((void*)&*select);
    }
    else {
        FXString loc = msg->value("region");
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

void FXReportInfo::addBattle(datablock::itor& block)
{
    FXString name = mapFile->regionName(*block);
    FXString label;
    label.format(" (%d,%d)", block->x(), block->y());
    label = name + label;
    FXTreeItem* group = appendItem(groups.battles, label);

    datablock::itor it;
    if (mapFile->getRegion(it, block->x(), block->y(), block->info())) {
        group->setData((void*)&*it);
    }
    datablock::itor end = mapFile->blocks().end();
    for (++block; block != end; ++block) {
        if (block->type() != block_type::TYPE_MESSAGE) {
            break;
        }
        addMessage(group, &*block);
    }
}

void FXReportInfo::addFaction(datablock::itor& block)
{
    ++block;
}

long FXReportInfo::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

    selection = *pstate;
	return 1;
}

long FXReportInfo::onDoubleClick(FXObject* sender, FXSelector sel, void* ptr)
{
    if (this != sender) {
        return 0;
    }
    if (!mapFile)
		return 0;

	FXTreeItem* item = (FXTreeItem*)ptr;
	if (!item)
		return FXTreeList::onDoubleClicked(sender, sel, ptr);

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

void FXReportInfo::clearSiblings(FXTreeItem* parent_item)
{
	FXTreeItem *item = parent_item->getFirst();
	while (item)
	{
        FXTreeItem *next_item = item->getNext();
		removeItem(item);
		item = next_item;
	}
}
