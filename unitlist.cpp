#include <vector>
#include "main.h"
#include "fxhelper.h"
#include "unitlist.h"
#include "FXMenuSeparatorEx.h"

// *********************************************************************************************************
// *** FXStatistics implementation

FXDEFMAP(FXUnitList) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,			FXUnitList::ID_UPDATE,				FXUnitList::onMapChange), 
    FXMAPFUNC(SEL_COMMAND,			FXUnitList::ID_POPUP_COPY_TREE,		FXUnitList::onCopyTree),
    FXMAPFUNC(SEL_COMMAND,			FXUnitList::ID_POPUP_COPY_TEXT,	    FXUnitList::onCopyText),
    FXMAPFUNC(SEL_COMMAND,			FXUnitList::ID_POPUP_SHOW_INFO,	    FXUnitList::onShowInfo),
    FXMAPFUNC(SEL_COMMAND,			FXUnitList::ID_POPUP_SELECT,	    FXUnitList::onGotoItem),
    
    FXMAPFUNC(SEL_QUERY_HELP,		0,									FXUnitList::onQueryHelp),

    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,								FXUnitList::onPopup),
};

FXIMPLEMENT(FXUnitList, FXTreeList, MessageMap, ARRAYNUMBER(MessageMap))

class FXPopupTreeItem : public FXTreeItem
{
public:
    FXPopupTreeItem(const FXString& text, FXIcon* oi = NULL, FXIcon* ci = NULL, void* ptr = NULL)
        : FXTreeItem(text, oi, ci, ptr)
    {
    }
    datablock* block = nullptr;
    FXString info;
};

class FXPopupMenuCommand : public FXMenuCommand
{
public:
    FXPopupMenuCommand(FXComposite* p, const FXString& text, const FXString& info_key, FXIcon* ic = NULL, FXObject* tgt = NULL, FXuint opts = 0)
        : FXMenuCommand(p, text, ic, tgt, FXUnitList::ID_POPUP_SHOW_INFO, opts), info(info_key) {}

    const FXString& getInfo() const { return info; }
private:
    FXString info;
};

FXUnitList::FXUnitList(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
		: FXTreeList(p, tgt, sel, opts, x, y, w, h)
{
}

void FXUnitList::setMapFile(datafile *f)
{
    mapFile = f;
}

long FXUnitList::onMapChange(FXObject* /*sender*/, FXSelector, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

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

		clearItems();		// clear list and build new

		if (selection.selected & selection.UNIT)
		{
			datablock::itor end = mapFile->blocks().end();
			
			datablock::itor unit = selection.unit;
			datablock::itor items = end;
			datablock::itor talents = end;
			datablock::itor spells = end;
			datablock::itor effects = end;
			std::map<int,datablock::itor> combatspells;

			datablock::itor building = end;
			datablock::itor ship = end;

			datablock::itor block = unit;
			for (block++; block != end && block->depth() > unit->depth(); block++)
			{
                if (block->type() == block_type::TYPE_ITEMS)
					items = block;
				else if (block->type() == block_type::TYPE_TALENTS)
					talents = block;
				else if (block->type() == block_type::TYPE_SPELLS)
					spells = block;
				else if (block->type() == block_type::TYPE_EFFECTS)
					effects = block;
				else if (block->type() == block_type::TYPE_COMBATSPELL)
					combatspells[block->info()] = block;
			}

			FXString name, descr, number, prefix, race, weight;
			FXString aura, auramax, hero;
			FXString hp, hungry, combatstatus, guards;
			FXint factionId = -1, familiarMage = -1, group = -1;
			FXint otherFactionId = -2;

			std::vector<datakey::list_type::const_iterator> unhandled;

			for (datakey::list_type::const_iterator key = unit->data().begin(); key != unit->data().end(); ++key)
			{
                switch (key->type()) {
                case TYPE_NAME:
                    name = key->value();
                    break;
                case TYPE_DESCRIPTION:
                    descr = key->value();
                    break;
                case TYPE_FACTION:
                    factionId = atoi(key->value().text());
                    break;
                case TYPE_OTHER_FACTION:
                    otherFactionId = atoi(key->value().text());
                    break;
                case TYPE_NUMBER:
                    number = key->value();
                    break;
                case TYPE_TYPE:
                    race = key->value();
                    break;
                case TYPE_AURA:
                    aura = key->value();
                    break;
                case TYPE_AURAMAX:
                    auramax = key->value();
                    break;
                case TYPE_BUILDING:
                    mapFile->getBuilding(building, atoi(key->value().text()));
                    break;
                case TYPE_SHIP:
                    mapFile->getShip(ship, atoi(key->value().text()));
                    break;
                case TYPE_WEIGHT:
                    weight = key->value();
                    break;
                case TYPE_GROUP:
                    group = atoi(key->value().text());
                    break;
                case TYPE_PREFIX:
                    prefix = key->value();
                    break;
                case TYPE_STATUS:
                    combatstatus = key->value();
                    break;
                case TYPE_HITPOINTS:
                    hp = key->value();
                    break;
                case TYPE_FAMILIARMAGE:
                    familiarMage = atoi(key->value().text());
                    break;
                case TYPE_GUARDING:
                    guards = key->value();
                    break;
                case TYPE_HUNGER:
                    hungry = key->value();
                    break;
                case TYPE_HERO:
                    hero = key->value();
                    break;
                case TYPE_ORDERS_CONFIRMED:
                case TYPE_FACTIONSTEALTH:
                    // do not show
                    break;
                default:
                    unhandled.push_back(key);
                    break;
                }
			}

			/*
				Phygon (phyg)
				* 1 Nebelmeermensch
				* 500 von 700 Aura
			*/

			FXTreeItem *item;
            FXString label;

			label.format("%s (%s)", name.text(), unit->id().text());
			FXTreeItem *unititem = appendItem(nullptr, label);
			unititem->setExpanded(true);

            if (factionId < 0 && otherFactionId < 0)
            {
                label.assign("Parteigetarnt");
            }
            else {
                datablock::itor faction;

                if (factionId > 0) {
                    if (mapFile->getFaction(faction, factionId)) {
                        name = faction->value(TYPE_FACTIONNAME);
                        if (name.empty())
                            label.assign("Parteigetarnt");
                        else
                            label.format("%s (%s)", name.text(), faction->id().text());
                    }
                    else {
                        label.format("Unbekannt (%s)", FXStringValEx((FXulong)factionId, 36).text());
                    }
                }

                if (otherFactionId > 0) {
                    datablock::itor anotherfaction;
                    if (mapFile->getFaction(anotherfaction, otherFactionId)) {
                        if (!label.empty())
                            label += ", ";

                        name = anotherfaction->value(TYPE_FACTIONNAME);
                        FXString label2;
                        if (name.empty())
                            label2.assign("Parteigetarnt");
                        else
                            label2.format("%s (%s)", name.text(), FXStringValEx(otherFactionId, 36).text());

                        label += label2;
                    }
                    else {
                        label.format("Unbekannt (%s)", FXStringValEx(otherFactionId, 36).text());
                    }
                }
            }
            item = appendItem(unititem, label.text());

            if (group > 0)
            {
                label = "Gruppe: ";
                datablock::itor match;
                if (mapFile->getGroup(match, group)) {
                    label += match->value(key_type::TYPE_NAME);
                } else {
                    // TODO: find group name in mapFile
                    label += FXStringVal(group);
                }
                item = appendItem(unititem, label.text());
            }

            if (familiarMage > 0) {
                datablock::itor mage;
                if (mapFile->getUnit(mage, familiarMage)) {
                    FXString mage_name = mage->value(TYPE_NAME);
                    label.format("Vertrauter von %s (%s)", mage_name.text(), FXStringValEx(familiarMage, 36).text());
                    item = appendItem(unititem, label.text());
                    item->setData(&*mage);
                }
            }

            FXint count = FXIntVal(number);

            // prefix+race
            label = number + " ";
            if (race.empty()) {
                label += (count == 1) ? "Person" : "Personen";
            }
            else if (!prefix.empty()) {
                label += prefix;
                label += race.lower();
            }
            else {
                label += race;
            }
            if (!hero.empty()) {
                label += ", ";
                label += (count == 1) ? "Held" : "Helden";
            }
			item = appendItem(unititem, label.text());

			if (!combatstatus.empty() || !hp.empty() || !hungry.empty() || !guards.empty())
			{
				if (combatstatus == "0")
					label = "aggressiv";
				else if (combatstatus == "1")
					label = "vorne";
				else if (combatstatus == "2")
					label = "hinten";
				else if (combatstatus == "3")
					label = "defensiv";
				else if (combatstatus == "4")
					label = FXString(L"k\u00e4mpft nicht");
				else if (combatstatus == "5")
					label = "flieht";
				else if (!combatstatus.empty())
					label = "Kampfstatus " + combatstatus;
				else
					label = "";

				if (!hp.empty() || !hungry.empty())
				{
					label += " (";

					if (!hp.empty())
					{
						label += hp;

						if (!hungry.empty())
							label += ", ";
					}

					if (!hungry.empty())
						label += "hungert";

					label += ")";
				}

				if (!guards.empty())
				{
					if (!label.empty())
						label += ", ";
					label += "bewacht";
				}

				item = appendItem(unititem, label.text());
			}

			if (!aura.empty() || !auramax.empty())
			{
				label.format("%s von %s Aura", aura.text(), auramax.text());
				item = appendItem(unititem, label.text());
			}

            if (!weight.empty())
            {
                // fill to 3 numbers
                while (weight.length() < 3)
                    weight = "0" + weight;

                // cut up
                if (weight.right(2) == "00")
                    weight = weight.left(weight.length() - 2);
                else
                    weight = weight.left(weight.length() - 2) + "." + weight.right(2);

                item = appendItem(unititem, "Gewicht: " + weight);
            }
            
            // list unhandled keys
			for (std::vector<datakey::list_type::const_iterator>::const_iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
			{
				label.format("%s: %s", (*itag)->key().text(), (*itag)->value().text());
				appendItem(unititem, label.text());
			}

			if (spells != end)		// does a SPRUECHE block exist?
			{
				FXTreeItem *node = appendItem(unititem, FXString(L"Zauberspr\u00fcche"));

				for (datakey::list_type::const_iterator key = spells->data().begin(); key != spells->data().end(); ++key)
				{
                    label = key->value();
                    appendItem(node, makePopupTreeItem(label, nullptr, &label));
				}
			}

			if (!combatspells.empty())	// do some KAMPFZAUBER blocks exist?
			{
				FXTreeItem *node = appendItem(unititem, "Kampfzauber");
				//node->setExpanded(true);

				for (std::map<int,datablock::itor>::iterator itor = combatspells.begin(); itor != combatspells.end(); itor++)
				{
					FXString type;

                    block = itor->second;
                    if (itor->first == 0)
						type = FXString(L"Pr\u00e4kampfzauber");
					else if (itor->first == 1)
						type = "Kampfzauber";
					else if (itor->first == 2)
						type = "Postkampfzauber";
					else
						type.format("%d. Kampfzauber", itor->first);

					FXString spell = block->value("name");
					FXString level = block->value("level"); 

					label.format("%s: %s (%s)", type.text(), spell.text(), level.text());
					appendItem(node, label);
				}
			}

			if (effects != end)		// does a EFFEKTE block exist?
			{
				FXTreeItem *node = appendItem(unititem, "Effekte");
				node->setExpanded(true);

				for (datakey::list_type::const_iterator key = effects->data().begin(); key != effects->data().end(); ++key)
					appendItem(node, key->value());
			}

            FXint riding_skill = 0;
            FXint horses = 0;
            FXint carts = 0;
			if (talents != end)		// does a TALENTE block exist?
			{
				FXTreeItem *node = appendItem(unititem, "Talente");
				node->setExpanded(true);

				for (datakey::list_type::const_iterator key = talents->data().begin(); key != talents->data().end(); ++key)
				{
                    FXString info = key->key();
                    FXString value = key->value().section(' ', 1);
                    label = key->key() + " " + value;
                    appendItem(node, makePopupTreeItem(label, nullptr, &info));

                    // Talent, für Transportkapazität
                    if (riding_skill == 0) {
                        if (info == "Reiten") {
                            riding_skill = FXIntVal(value, 10);
                        }
                    }
                }
			}

			if (items != end)		// does a GEGENSTAENDE block exist?
			{
				FXTreeItem *node = appendItem(unititem, FXString(L"Gegenst\u00e4nde"));
				node->setExpanded(true);

				for (datakey::list_type::const_iterator key = items->data().begin(); key != items->data().end(); ++key)
				{
                    const FXString& info = key->key();
                    const FXString& value = key->value();
                    label = value + " " + info;
                    appendItem(node, makePopupTreeItem(label, nullptr, &info));

                    // Pferde und Wagen, für Transportkapazität
                    if (info == "Pferd" || info == "Elfenpferd") {
                        horses += FXIntVal(value, 10);
                    }
                    else if (info == "Wagen") {
                        carts += FXIntVal(value, 10);
                    }
                }
			}

            // Kapazität berechnen
            FXint carry = 540;
            FXint self = 10;
            if (race == "Goblin") {
                carry = 440;
                self = 6;
            }
            else if (race == "Troll") {
                carry = 1040;
                self = 20;
            }
            // walking capacity
            FXint walk_cap = carry * count;
            FXint max_horses = (riding_skill * 4 + 1) * count;
            if (max_horses > horses) {
                max_horses = horses;
            }
            FXint max_carts = max_horses / 2;
            if (max_carts > carts) {
                max_carts = carts;
            }
            walk_cap += max_carts * 10000 + max_horses * 2000;

            FXString tail;
            label = L"Kapazit\u00e4t: ";
            if (horses > 0) {
                // riding capacity
                max_horses = riding_skill * 2 * count;
                if (max_horses > horses) {
                    max_horses = horses;
                }
                max_carts = max_horses / 2;
                if (max_carts > carts) {
                    max_carts = carts;
                }
                FXint ride_cap = max_carts * 100 + max_horses * 20 - count * self;
                if (ride_cap > 0) {
                    tail.format("%.2f (%d)", walk_cap / 100.0f, ride_cap);
                }
            }
            if (tail.empty()) {
                tail.format("%.2f", walk_cap / 100.0f);
            }
            insertItem(item, unititem, label + tail);

			if (building != end)	// unit in a building?
			{
				unhandled.clear();

				FXString size;
				FXString type = FXString(L"Geb\u00e4ude");
				factionId = -1;

				for (datakey::list_type::const_iterator key = building->data().begin(); key != building->data().end(); ++key)
				{
					if (key->type() == TYPE_NAME)
						name = key->value();
					else if (key->type() == TYPE_DESCRIPTION)
						descr = key->value();
					else if (key->type() == TYPE_FACTION)
						factionId = atoi(key->value().text());
					else if (key->type() == TYPE_TYPE)
						type = key->value();
					else if (key->type() == TYPE_SIZE)
						size = key->value();
					else
						unhandled.push_back(key);
				}

				if (name.empty())
					name = type;

				label.format("In %s (%s)", name.text(), building->id().text());
				if (!type.empty())
					label += ", " + type;
				if (!size.empty())
					label += FXString(L", Gr\u00f6\u00dfe ") + size;

                FXTreeItem* node = makePopupTreeItem(label, &*building, nullptr);
                prependItem(nullptr, node)->setExpanded(true);

				// list unhandled keys
				for (std::vector<datakey::list_type::const_iterator>::iterator& itag = unhandled.begin(); itag != unhandled.end(); ++itag)
				{
                    const datakey::list_type::const_iterator t = *itag;
                    datablock* owner_block = nullptr;

                    if (t->type() == TYPE_OWNER) {
                        FXint uid = FXIntVal(t->value());
                        datablock::itor unit_owner;
                        if (mapFile->getUnit(unit_owner, uid)) {
                            FXint fid = FXIntVal(unit_owner->value(TYPE_FACTION));
                            owner_block = &*unit_owner;
                            datablock::itor faction_owner;
                            if (mapFile->getFaction(faction_owner, fid)) {
                                label.format("%s: %s (%s), %s (%s)",
                                    t->key().text(),
                                    unit_owner->value(TYPE_NAME).text(),
                                    FXStringValEx(uid, 36).text(),
                                    faction_owner->value(TYPE_FACTIONNAME).text(),
                                    FXStringValEx(fid, 36).text()
                                );
                            }
                            else {
                                label.format("%s: %s (%s), Unbekannt (%s)",
                                    t->key().text(),
                                    unit_owner->value(TYPE_NAME).text(),
                                    FXStringValEx(uid, 36).text(),
                                    FXStringValEx(fid, 36).text()
                                );
                            }
                        }
                    }
                    else {
                        label.format("%s: %s", t->key().text(), t->value().text());
                    }

                    if (owner_block) {
                        item = makePopupTreeItem(label, owner_block, nullptr);
                        item = appendItem(node, item);
                    }
                    else {
                        item = appendItem(node, label.text());
                    }
                }

				effects = end;
				block = building;
				for (block++; block != end && block->depth() > unit->depth(); block++)
				{
					if (block->type() == block_type::TYPE_EFFECTS)
						effects = block;
				}

				if (effects != end)		// does a EFFEKTE block exist?
				{
					FXTreeItem *effnode = appendItem(node, "Effekte");
					effnode->setExpanded(true);

					for (datakey::list_type::const_iterator key = effects->data().begin(); key != effects->data().end(); ++key)
						appendItem(effnode, key->value());
				}
			}

			if (ship != end)	// unit in a ship?
			{
				unhandled.clear();

				FXString size;
				FXString damage, coast, cargo, capacity;
				FXString type = "Schiff";
				factionId = -1;

				for (datakey::list_type::const_iterator key = ship->data().begin(); key != ship->data().end(); ++key)
				{
					if (key->type() == TYPE_NAME)
						name = key->value();
					else if (key->type() == TYPE_DESCRIPTION)
						descr = key->value();
					else if (key->type() == TYPE_FACTION)
						factionId = atoi(key->value().text());
					else if (key->type() == TYPE_TYPE)
						type = key->value();
                    else if (key->type() == TYPE_SIZE)
						size = key->value();
                    else if (key->type() == TYPE_DAMAGE)
						damage = key->value();
                    else if (key->type() == TYPE_COAST)
						coast = key->value();
                    else if (key->type() == TYPE_CARGO)
						cargo = key->value();
                    else if (key->type() == TYPE_CAPACITY)
						capacity = key->value();
					else if (key->type() != TYPE_LOAD && key->type() != TYPE_MAXLOAD)
						unhandled.push_back(key);
				}

				if (name.empty())
					name = type;

				label.format("An Bord von %s (%s)", name.text(), ship->id().text());
				if (!type.empty())
					label += ", " + type;
				if (!size.empty())
					label += FXString(L", Gr\u00f6\u00dfe ") + size;

                FXTreeItem* node = makePopupTreeItem(label, &*ship, nullptr);
                prependItem(nullptr, node)->setExpanded(true);

                // Schaden
				if (!damage.empty())
					appendItem(node, damage + FXString(L"% besch\u00e4digt"));

                // Kueste
				const wchar_t* coasts[] = { L"Nordwest", L"Nordost", L"Ost", L"S\u00fcdost", L"S\u00fcdwest", L"West", L"-unknown-" };

				if (!coast.empty())
				{
					FXint dir = 0;
					dir = atoi(coast.text());

					if (dir < 0 || dir > 6)
						dir = 6;

					appendItem(node, FXString(coasts[dir]) + FXString(L"k\u00fcste"));
				}

				// Beladung (cargo/capacity)
				if (!cargo.empty() || !capacity.empty())
				{
					// fill up to 3 numbers
					while (cargo.length() < 3)
						cargo = "0" + cargo;
					while (capacity.length() < 3)
						capacity = "0" + capacity;

					// cut up
					if (cargo.right(2) == "00")
						cargo = cargo.left(cargo.length() - 2);
					else
						cargo = cargo.left(cargo.length() - 2) + "." + cargo.right(2);

					if (capacity.right(2) == "00")
						capacity = capacity.left(capacity.length() - 2);
					else
						capacity = capacity.left(capacity.length() - 2) + "." + capacity.right(2);

					appendItem(node, "Beladung: " + cargo + " von " + capacity + " GE");
				}

				// list unhandled keys
				for (std::vector<datakey::list_type::const_iterator>::iterator& itag = unhandled.begin(); itag != unhandled.end(); ++itag)
				{
                    datakey::list_type::const_iterator t = *itag;
                    if (t->type() == TYPE_CAPTAIN) {
                        FXint uid = FXIntVal(t->value());
                        datablock::itor unit_owner;
                        if (mapFile->getUnit(unit_owner, uid)) {
                            FXint fid = FXIntVal(unit_owner->value(TYPE_FACTION));
                            datablock::itor faction_owner;
                            if (mapFile->getFaction(faction_owner, fid)) {
                                label.format("%s: %s (%s), %s (%s)",
                                    FXString(L"Kapit\u00e4n").text(),
                                    unit_owner->value(TYPE_NAME).text(),
                                    FXStringValEx(uid, 36).text(),
                                    faction_owner->value(TYPE_FACTIONNAME).text(),
                                    FXStringValEx(fid, 36).text()
                                );
                            }
                            else {
                                label.format("%s: %s (%s), Unbekannt (%s)",
                                    t->key().text(),
                                    unit_owner->value(TYPE_NAME).text(),
                                    FXStringValEx(uid, 36).text(),
                                    FXStringValEx(fid, 36).text()
                                );
                            }
                        }
                    }
                    else {
                        label.format("%s: %s", t->key().text(), t->value().text());
                    }
                    item = appendItem(node, label.text());
                }

				effects = end;
				block = ship;
				for (block++; block != end && block->depth() > unit->depth(); block++)
				{
					if (block->type() == block_type::TYPE_EFFECTS)
						effects = block;
				}

				if (effects != end)		// does a EFFEKTE block exist?
				{
					FXTreeItem *effnode = appendItem(node, "Effekte");
					effnode->setExpanded(true);

					for (datakey::list_type::const_iterator key = effects->data().begin(); key != effects->data().end(); ++key)
						appendItem(effnode, key->value());
				}
			}
		}
	}

	return 1;
}

long FXUnitList::onCopyTree(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender)
    {
        FXMenuCommand *cmd = static_cast<FXMenuCommand*>(sender);
        FXTreeItem *item = static_cast<FXTreeItem *>(cmd->getUserData());
        setClipboard(getSubTreeText(item));
        return 1;
    }
    return 0;
}

long FXUnitList::onCopyText(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender)
    {
        FXMenuCommand *cmd = static_cast<FXMenuCommand*>(sender);
        FXTreeItem* item = static_cast<FXTreeItem*>(cmd->getUserData());
        if (item) {
            setClipboard(item->getText());
            return 1;
        }
    }
    return 0;
}

long FXUnitList::onShowInfo(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender)
    {
        FXPopupMenuCommand * command = static_cast<FXPopupMenuCommand*>(sender);
        showInfo(command->getInfo());
        setClipboard(command->getText());
        return 1;
    }
    return 0;
}

long FXUnitList::onGotoItem(FXObject* sender, FXSelector sel, void *ptr)
{
    FXMenuCommand* command = static_cast<FXMenuCommand*>(sender);
    const datablock * block = static_cast<const datablock*>(command->getUserData());
    datafile::SelectionState sel_state;
    sel_state.selChange = selection.selChange + 1;
    if (sel_state.regionsSelected.empty()) {
        sel_state.selected = 0;
    }
    else {
        sel_state.regionsSelected = selection.regionsSelected;
        sel_state.selected = sel_state.MULTIPLE_REGIONS;
    }
    if (block->type() == block_type::TYPE_UNIT)
    {
        if (mapFile->getUnit(sel_state.unit, block->info())) {
            sel_state.selected |= selection.UNIT;
        }
    }
    else if (block->type() == block_type::TYPE_REGION)
    {
        if (mapFile->getRegion(sel_state.region, block->x(), block->y(), block->info())) {
            sel_state.selected |= selection.REGION;
        }
    }
    else if (block->type() == block_type::TYPE_BUILDING)
    {
        if (mapFile->getBuilding(sel_state.building, block->info())) {
            sel_state.selected |= selection.BUILDING;
        }
    }
    else if (block->type() == block_type::TYPE_SHIP)
    {
        if (mapFile->getBuilding(sel_state.ship, block->info())) {
            sel_state.selected |= selection.SHIP;
        }
    }
    else if (block->type() == block_type::TYPE_FACTION)
    {
        if (mapFile->getFaction(sel_state.faction, block->info())) {
            sel_state.selected |= selection.FACTION;
        }
    }
    return getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &sel_state);
}

FXTreeItem* FXUnitList::makePopupTreeItem(const FXString& label, datablock* block, const FXString * info)
{
    FXPopupTreeItem * popup = new FXPopupTreeItem(label);
    popup->block = block;
    if (info) {
        popup->info = *info;
    }
    popup->setData(popup);	// show "info" popup cmd
    return popup;
}

long FXUnitList::onPopup(FXObject* sender, FXSelector sel, void* ptr)
{ 
    onRightBtnRelease(sender, sel, ptr);

    // no datafile, no popup
    if (!mapFile)
        return 0;

    FXEvent *event = (FXEvent*)ptr;

	// dont't show popup if mouse has moved
	if (event->last_x != event->click_x || event->last_y != event->click_y)
		return 0;
	
	// find item to create popup for
	FXTreeItem *item = getItemAt(event->click_x, event->click_y);
	if (!item)
		return 0;

	FXMenuPane pane(this);
	FXMenuPane *menu = &pane;

	FXString title = item->getText();
	if (title.length() > 20)
		title = title.left(17) + "...";

    new FXMenuSeparatorEx(menu, title);

    void* udata = item->getData();
    if (udata) {
        FXPopupTreeItem* popup = static_cast<FXPopupTreeItem*>(item);
        if (popup->block) {
            datablock* block = popup->block;
            FXMenuCommand* command = nullptr;
            if (block->type() == block_type::TYPE_UNIT) {
                command = new FXMenuCommand(menu, "Zeige Einheit", nullptr, this, ID_POPUP_SELECT);
            }
            else if (block->type() == block_type::TYPE_SHIP) {
                command = new FXMenuCommand(menu, "Zeige Schiff", nullptr, this, ID_POPUP_SELECT);
            }
            else if (block->type() == block_type::TYPE_BUILDING) {
                command = new FXMenuCommand(menu, L"Zeige Geb\u00e4ude", nullptr, this, ID_POPUP_SELECT);
            }
            if (command) {
                command->setUserData(block);
            }
        }
        else {
            new FXPopupMenuCommand(menu, "Zeige Info", popup->info, nullptr, this);
        }
    }
    
	FXMenuPane *clipboard = new FXMenuPane(this);
    new FXMenuCascade(menu, "&Zwischenablage", nullptr, clipboard, 0);
    (new FXMenuCommand(clipboard, L"Alles", nullptr, this, ID_POPUP_COPY_TREE))->setUserData(nullptr);
    (new FXMenuCommand(clipboard, L"Baum", nullptr, this, ID_POPUP_COPY_TREE))->setUserData(item);
    (new FXMenuCommand(clipboard, L"Eintrag", nullptr, this, ID_POPUP_COPY_TEXT))->setUserData(item);

	// show popup
	menu->create();
	menu->popup(nullptr, event->root_x,event->root_y);

    getApp()->runModalWhileShown(menu);
	return 1;
}

void FXUnitList::setClipboard(const FXString& text)
{
	getShell()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), (void*)text.text());
}

void FXUnitList::showInfo(const FXString& text)
{
	getShell()->handle(this, FXSEL(SEL_QUERY_HELP, ID_SETSTRINGVALUE), (void*)text.text());
}

FXString FXUnitList::trimNumbers(const FXString& str) const
{
	FXString text = str;

	FXString l = text.left(1);
	FXString r = text.right(1);
	if (l[0] >= '0' && l[0] <= '9')
	{
		int pos = text.find(' ');
        if (pos > 0)
			text = text.erase(0, pos+1);
	}
	else if (r[0] >= '0' && r[0] <= '9')
	{
		int pos = text.rfind(' ');
        if (pos > 0)
			text = text.erase(pos, text.length()-pos);
	}

	return text;
}

FXString FXUnitList::getSubTreeText(const FXTreeItem* item) const
{
	int mindepth = 0, depth = 0;

	if (!item)
	{
		item = getFirstItem();
		mindepth--;
	}

	FXString text;

	if (!item)
		return text;
	
	do
	{
		text += FXString('\t', depth);
		text += item->getText();
		text += '\n';

		if (item->getFirst())
		{
			item = item->getFirst();
			depth++;
		}
		else
		{
			while (item && !item->getNext())
			{
				item = item->getParent();
				depth--;
			}
			if (item)
				item = item->getNext();
		}
	} while (item && depth > mindepth);

	return text;
}

long FXUnitList::onQueryHelp(FXObject* /*sender*/, FXSelector, void* /*ptr*/)
{ 
	return 0;
}
