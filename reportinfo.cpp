#include "main.h"
#include "fxhelper.h"
#include "reportinfo.h"

#include <set>

// *********************************************************************************************************
// *** FXReportInfo implementation

FXReportInfo::FXReportInfo(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXMessageList(p, tgt, sel, opts|TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES, x, y, w, h)
{
	// set styles...
	setNumVisible(7);

	setSelTextColor(getTextColor());
	setSelBackColor(getBackColor());

	messages = appendItem(nullptr, "Meldungen");
	battles = appendItem(nullptr, L"K\u00e4mpfe");
}

void FXReportInfo::create()
{
	FXTreeList::create();
}

FXReportInfo::~FXReportInfo()
{
}

void FXReportInfo::setMapFile(std::shared_ptr<datafile>& f)
{
    if (f != mapFile) {
        datablock::itor it, end;
        mapFile = f;
        clearChildren(messages);
        clearChildren(battles);
        if (mapFile) {
            end = mapFile->blocks().end();
            for (it = mapFile->blocks().begin(); it != end;) {
                const datablock& block = *it;
                if (block.type() == block_type::TYPE_REGION) {
                    break;
                }
                else if (block.type() == block_type::TYPE_BATTLE) {
                    addBattle(it);
                    /* block is already on the next object */
                    continue;
                }
                else if (block.type() == block_type::TYPE_FACTION) {
                    addFaction(it);
                    continue;
                }
                else if (block.type() == block_type::TYPE_MESSAGE && block.depth() == 3) {
                    addMessage(messages, block);
                }
                ++it;
            }
        }
    }
    FXMessageList::setMapFile(f);
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

void FXReportInfo::addMessage(FXTreeItem *group, const datablock& msg)
{
    FXTreeItem* item;
    FXival uid = msg.valueInt("target");
    FXString section = msg.value("section");

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
    item = appendItem(group, msg.value("rendered"));

    item->setData(mapFile->getMessageTarget(msg));
}

void FXReportInfo::addBattle(datablock::itor& block)
{
    FXString name = mapFile->regionName(*block);
    FXString label;
    label.format(" (%d,%d)", block->x(), block->y());
    label = name + label;
    FXTreeItem* group = appendItem(battles, label);

    datablock::itor it;
    if (mapFile->getRegion(it, *block)) {
        group->setData((void*)&*it);
    }
    datablock::itor end = mapFile->blocks().end();
    for (++block; block != end; ++block) {
        if (block->type() != block_type::TYPE_MESSAGE) {
            break;
        }
        addMessage(group, *block);
    }
}

void FXReportInfo::addFaction(datablock::itor& block)
{
    ++block;
}
