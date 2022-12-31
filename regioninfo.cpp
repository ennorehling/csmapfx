#include "regioninfo.h"

FXDEFMAP(FXRegionInfo) MessageMap[] =
{
    //________Message_Type_____________________ID_______________Message_Handler_______
    FXMAPFUNC(SEL_COMMAND,			FXMessageList::ID_UPDATE,		FXRegionInfo::onMapChange),

    FXMAPFUNC(SEL_DOUBLECLICKED,	0,								FXRegionInfo::onDoubleClick),
};

FXIMPLEMENT(FXRegionInfo, FXTreeList, MessageMap, ARRAYNUMBER(MessageMap))


void FXRegionInfo::create()
{
    FXMessageList::create();
}

FXRegionInfo::FXRegionInfo(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
    : FXMessageList(p, tgt, sel, opts, x, y, w, h)
{
    groups.messages = appendItem(nullptr, "Meldungen");
    groups.effects = appendItem(nullptr, "Effekte");
    groups.travel = appendItem(nullptr, "Durchreise");
    groups.battle = nullptr;
    groups.streets = appendItem(nullptr, FXString(L"Stra\u00dfen"));
    groups.guards = appendItem(nullptr, "Bewacher");
}

long FXRegionInfo::onMapChange(FXObject * target, FXSelector sel, void * ptr)
{
    datafile::SelectionState* pstate = (datafile::SelectionState*)ptr;

    // any data changed, so need to update list?
    if (selection.fileChange != pstate->fileChange)
    {
        selection.fileChange = pstate->fileChange;
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

                        for (datakey::list_type::const_iterator msg = block->data().begin(); msg != block->data().end(); msg++)
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

                        for (datakey::list_type::const_iterator msg = block->data().begin(); msg != block->data().end(); msg++)
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
