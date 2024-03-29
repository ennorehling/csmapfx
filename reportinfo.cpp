#include "fxhelper.h"
#include "reportinfo.h"

#include <set>

// *********************************************************************************************************
// *** FXReportInfo implementation

FXReportInfo::FXReportInfo(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h) :
    FXMessageList(p, tgt, sel, opts, x, y, w, h)
{
	// set styles...
	setNumVisible(7);

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
                }
                else if (block.type() == block_type::TYPE_FACTION) {
                    addFaction(it);
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

void FXReportInfo::addBattle(const datablock::itor& block)
{
    datablock::itor region;
    if (mapFile->getRegion(region, *block)) {
        datablock::itor message;
        datablock* regionPtr = &*region;
        FXString label = regionPtr->getLabel();
        FXTreeItem* group = appendItem(battles, label);
        group->setData(regionPtr);
        if (mapFile->getChild(message, block, block_type::TYPE_MESSAGE)) {
            do {
                addMessage(group, *message);
            }
            while (mapFile->getNext(message, block_type::TYPE_MESSAGE));
        }
    }
}

void FXReportInfo::addFaction(const datablock::itor& block)
{
}
