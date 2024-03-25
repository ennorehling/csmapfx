#include <vector>
#include <algorithm>
#include "fxhelper.h"
#include "unitlist.h"

#define NGETTEXT(a, b, n) ((n==1) ? (a) : (b))

#define WEIGHT_PERSON      1000
#define WEIGHT_TROLL       2000
#define WEIGHT_GOBLIN       600
#define WEIGHT_HORSE       5000
#define WEIGHT_CART        4000
#define WEIGHT_CATAPULT   10000
#define CAPACITY_PERSON     540
#define CAPACITY_TROLL     1080
#define CAPACITY_GOBLIN     440
#define CAPACITY_HORSE     2000
#define CAPACITY_CART     10000
#define CAPACITY_CATAPULT     0

// *********************************************************************************************************
// *** FXStatistics implementation

FXDEFMAP(FXUnitList) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,              FXUnitList::ID_UPDATE,  FXUnitList::onMapChange), 
    FXMAPFUNC(SEL_QUERY_HELP,           0,                      FXUnitList::onQueryHelp),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,                      FXUnitList::onPopup),
};

FXIMPLEMENT(FXUnitList, FXTreeList, MessageMap, ARRAYNUMBER(MessageMap))

FXUnitList::FXUnitList(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
	: FXProperties(p, tgt, sel, opts, x, y, w, h)
{
}

struct UnitProperty {
    FXString label;
    int value;
};

static bool CompareProperties(const UnitProperty& a, const UnitProperty& b)
{
    return a.label < b.label;
}

void FXUnitList::makeItems()
{
    // clear list and build new
    clearItems();

    if (selection.selected & selection.UNIT)
    {
        datablock::itor end = mapFile->blocks().end();

        datablock::itor unit = selection.unit;
        datablock::itor items = end;
        datablock::itor talents = end;
        datablock::itor spells = end;
        datablock::itor effects = end;
        std::map<int, datablock::itor> combatspells;

        datablock::itor building = end;
        datablock::itor ship = end;

        for (datablock::itor block = std::next(unit); block != end && block->depth() > unit->depth(); block++)
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

        FXString name, number, prefix, race;
        FXString aura, auramax, hero;
        FXString hp, hungry, combatstatus, guards;
        FXint factionId = -1, familiarMage = -1, group = -1, weight = 0;
        FXint otherFactionId = -2;

        std::vector<datakey::list_type::const_iterator> unhandled;

        for (datakey::list_type::const_iterator key = unit->data().begin(); key != unit->data().end(); ++key)
        {
            switch (key->type()) {
            case TYPE_NAME:
                name = key->value();
                break;
            case TYPE_FACTION:
                factionId = FXIntVal(key->value());
                break;
            case TYPE_OTHER_FACTION:
                otherFactionId = FXIntVal(key->value());
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
                mapFile->getBuilding(building, FXIntVal(key->value()));
                break;
            case TYPE_SHIP:
                mapFile->getShip(ship, FXIntVal(key->value()));
                break;
            case TYPE_WEIGHT:
                weight = FXIntVal(key->value());
                break;
            case TYPE_GROUP:
                group = FXIntVal(key->value());
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
                familiarMage = FXIntVal(key->value());
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
            case TYPE_DESCRIPTION:
            case TYPE_NOTES:
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

        FXString label;

        label.format("%s (%s)", name.text(), unit->id().text());
        FXTreeItem* unititem = appendItem(nullptr, makeItem(label, &*unit));
        unititem->setExpanded(true);

        datablock* factionPtr = nullptr;
        if (factionId < 0 && otherFactionId < 0)
        {
            label.assign("Parteigetarnt");
        }
        else {
            datablock::itor faction;

            if (factionId > 0) {
                if (mapFile->getFaction(faction, factionId)) {
                    factionPtr = &*faction;
                    name = factionPtr->value(TYPE_FACTIONNAME);
                    if (name.empty())
                        label.assign("Parteigetarnt");
                    else
                        label.format("%s (%s)", name.text(), factionPtr->id().text());
                }
                else {
                    label.format("Unbekannt (%s)", FXStringValEx((FXulong)factionId, 36).text());
                }
            }

            if (otherFactionId > 0) {
                datablock::itor anotherfaction;
                if (mapFile->getFaction(anotherfaction, otherFactionId)) {
                    factionPtr = &*anotherfaction;
                    name = factionPtr->value(TYPE_FACTIONNAME);
                    FXString label2;
                    if (name.empty()) {
                        label2.assign(", parteigetarnt");
                    }
                    else {
                        label2.format(", als %s (%s)", name.text(), FXStringValEx(otherFactionId, 36).text());
                    }

                    label += label2;
                }
                else {
                    label.format("Unbekannt (%s)", FXStringValEx(otherFactionId, 36).text());
                }
            }
        }
        FXTreeItem* item = appendItem(unititem, makeItem(label, factionPtr));

        if (group > 0)
        {
            label = "Gruppe: ";
            datablock::itor match;
            if (mapFile->getGroup(match, group)) {
                label += match->value(key_type::TYPE_LOWERCASE_NAME);
            }
            else {
                // TODO: find group name in mapFile
                label += FXStringVal(group);
            }
            item = appendItem(unititem, label);
        }

        if (familiarMage > 0) {
            datablock::itor mage;
            if (mapFile->getUnit(mage, familiarMage)) {
                FXString mage_name = mage->value(TYPE_NAME);
                label.format("Vertrauter von %s (%s)", mage_name.text(), FXStringValEx(familiarMage, 36).text());
                item = appendItem(unititem, label);
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
            label += FXString(race).lower();
        }
        else {
            label += race;
        }
        if (!hero.empty()) {
            label += ", ";
            label += (count == 1) ? "Held" : "Helden";
        }
        item = appendItem(unititem, label);

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

            item = appendItem(unititem, label);
        }

        if (!aura.empty() || !auramax.empty())
        {
            label.format("%s von %s Aura", aura.text(), auramax.text());
            item = appendItem(unititem, label);
        }

        if (weight > 0)
        {
            FXString label;
            if (weight % 100 == 0) {
                label = FXStringVal(weight / 100);
            }
            else {
                label.format("%.2f", weight / 100.f);
            }
            item = appendItem(unititem, "Gewicht: " + label);
        }

        // list unhandled keys
        for (std::vector<datakey::list_type::const_iterator>::const_iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
        {
            label.format("%s: %s", (*itag)->key().text(), (*itag)->value().text());
            appendItem(unititem, label);
        }

        if (spells != end)		// does a SPRUECHE block exist?
        {
            FXTreeItem* node = appendItem(unititem, FXString(L"Zauberspr\u00fcche"));

            for (datakey::list_type::const_iterator key = spells->data().begin(); key != spells->data().end(); ++key)
            {
                label = key->value();
                appendItem(node, makeItem(label, nullptr, &label));
            }
        }

        if (!combatspells.empty())	// do some KAMPFZAUBER blocks exist?
        {
            FXTreeItem* node = appendItem(unititem, "Kampfzauber");
            //node->setExpanded(true);

            for (std::map<int, datablock::itor>::iterator itor = combatspells.begin(); itor != combatspells.end(); itor++)
            {
                FXString type;

                const datablock::itor &block = itor->second;
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
            FXTreeItem* node = appendItem(unititem, "Effekte");
            node->setExpanded(true);

            for (datakey::list_type::const_iterator key = effects->data().begin(); key != effects->data().end(); ++key)
                appendItem(node, key->value());
        }

        FXint riding_skill = 0;
        FXint horses = 0;
        FXint carts = 0;
        FXint catapults = 0;
        if (talents != end)		// does a TALENTE block exist?
        {
            FXTreeItem* node = appendItem(unititem, "Talente");
            node->setExpanded(true);
            std::vector<UnitProperty> properties;
            for (datakey::list_type::const_iterator key = talents->data().begin(); key != talents->data().end(); ++key)
            {
                FXString info = key->key();
                FXint value = key->valueInt(1);
                properties.push_back({ info, value });

                // Talent, f�r Transportkapazit�t
                if (riding_skill == 0) {
                    if (info == "Reiten") {
                        riding_skill = value;
                    }
                }
            }
            std::sort(properties.begin(), properties.end(), CompareProperties);
            for (const UnitProperty& item : properties) {
                label.format("%s %d", item.label.text(), item.value);
                appendItem(node, makeItem(label, nullptr, &item.label));
            }
        }

        if (items != end)		// does a GEGENSTAENDE block exist?
        {
            FXTreeItem* node = appendItem(unititem, FXString(L"Gegenst\u00e4nde"));
            node->setExpanded(true);
            std::vector<UnitProperty> properties;
            for (datakey::list_type::const_iterator key = items->data().begin(); key != items->data().end(); ++key)
            {
                const FXString& info = key->key();
                FXint value = FXIntVal(key->value(), 10);
                properties.push_back({ info, value });

                // Pferde und Wagen, für Transportkapazität
                if (info == "Pferd") {
                    horses += value;
                }
                else if (info == "Elfenpferd") {
                    horses += value;
                }
                else if (info == "Wagen") {
                    carts += value;
                }
                else if (info == "Katapult") {
                    catapults += value;
                }
            }
            std::sort(properties.begin(), properties.end(), CompareProperties);
            for (const UnitProperty& item : properties) {
                label.format("%d %s", item.value, item.label.text());
                appendItem(node, makeItem(label, nullptr, &item.label));
            }
        }

        // walking capacity
        FXint max_horses = (riding_skill * 4 + 1) * count;
        if (max_horses > horses) {
            max_horses = horses;
        }
        FXint max_carts = max_horses / 2;
        FXint troll_carts = 0;
        FXint carry = CAPACITY_PERSON;
        FXint self = WEIGHT_PERSON;
        FXint vehicles = carts;
        FXbool catapult_is_vehicle = datafile::compareVersions(mapFile->getVersion(), "28.4") >= 0;
        if (!catapult_is_vehicle) {
            // catapults are vehicles, too!
            catapults = 0;
        }
        if (race == "Goblins") {
            carry = CAPACITY_GOBLIN;
            self = WEIGHT_GOBLIN;
        }
        else if (race == "Trolle") {
            carry = CAPACITY_TROLL;
            self = WEIGHT_TROLL;
            troll_carts = count / 4;
        }
        max_carts += troll_carts;
        int max_catapults = 0;
        if (max_carts > carts) {
            max_catapults = max_carts - carts;
            max_carts = carts;
        }
        if (max_catapults > catapults) {
            max_catapults = catapults;
        }

        FXint walk_cap = max_carts * CAPACITY_CART + max_horses * CAPACITY_HORSE + max_catapults * CAPACITY_CATAPULT;
        walk_cap += carry * count;
        walk_cap -= weight;
        /* the following are included in weight, but we don't have to carry them: */
        walk_cap += max_catapults * WEIGHT_CATAPULT;
        walk_cap += max_carts * WEIGHT_CART;
        walk_cap += horses * WEIGHT_HORSE;
        walk_cap += self * count;

        if (max_horses < horses) {
            FXint n = horses - max_horses;
            label.format(
                NGETTEXT(
                    "%.2f (%d Pferd zu viel)",
                    "%.2f (%d Pferde zu viel)", n), 
                walk_cap / 100.f, n);
        }
        else {
            label.format("%.2f", walk_cap / 100.f);
        }
        insertItem(item, unititem, L"Kapazit\u00e4t: " + label);
        if (horses > 0) {
            // riding capacity
            max_horses = riding_skill * 2 * count;
            if (max_horses > horses) {
                max_horses = horses;
            }
            max_carts = max_horses / 2;
            int max_catapults = 0;
            if (max_carts > carts) {
                max_catapults = max_carts - carts;
                max_carts = carts;
            }
            if (max_catapults > catapults) {
                max_catapults = catapults;
            }
            FXint ride_cap = max_carts * CAPACITY_CART + max_horses * CAPACITY_HORSE + max_catapults * CAPACITY_CATAPULT;;
            ride_cap -= weight;
            /* the following are included in weight, but we don't have to carry them: */
            ride_cap += max_catapults * WEIGHT_CATAPULT;
            ride_cap += max_carts * WEIGHT_CART;
            ride_cap += horses * WEIGHT_HORSE;
            if (max_horses > 0) {
                if (max_horses < horses) {
                    FXint n = horses - max_horses;
                    label.format(
                        NGETTEXT(
                            "%.2f (%d Pferd zu viel)",
                            "%.2f (%d Pferde zu viel)", n), 
                        ride_cap / 100.f, n);
                }
                else {
                    label.format("%.2f", ride_cap / 100.f);
                }
                insertItem(item, unititem, L"Kapazit\u00e4t beritten: " + label);
            }
        }

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

            FXTreeItem* node = makeItem(label, &*building, nullptr);
            prependItem(nullptr, node)->setExpanded(true);

            // list unhandled keys
            for (std::vector<datakey::list_type::const_iterator>::iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
            {
                const datakey::list_type::const_iterator t = *itag;
                datablock* block = nullptr;

                if (t->type() == TYPE_OWNER) {
                    FXint uid = FXIntVal(t->value());
                    datablock::itor unit_owner;
                    if (mapFile->getUnit(unit_owner, uid)) {
                        datablock& owner_blk = *unit_owner;
                        if (unit_owner != unit) {
                            block = &owner_blk;
                        }
                        label = t->translatedKey() + ": " + mapFile->unitName(owner_blk, true);
                    }
                }
                else {
                    label.format("%s: %s", t->key().text(), t->value().text());
                }
                if (block) {
                    item = appendItem(node, makeItem(label, block, nullptr));
                }
                else {
                    item = appendItem(node, label);
                }
            }

            effects = end;
            for (datablock::itor block = std::next(building); block != end && block->depth() > unit->depth(); block++)
            {
                if (block->type() == block_type::TYPE_EFFECTS)
                    effects = block;
            }

            if (effects != end)		// does a EFFEKTE block exist?
            {
                FXTreeItem* effnode = appendItem(node, "Effekte");
                effnode->setExpanded(true);

                for (datakey::list_type::const_iterator key = effects->data().begin(); key != effects->data().end(); ++key)
                    appendItem(effnode, key->value());
            }
        }

        if (ship != end)	// unit in a ship?
        {
            unhandled.clear();

            FXString type;
            FXint size = -1, damage = -1, coast = -1, cargo = -1, capacity = -1;

            for (datakey::list_type::const_iterator key = ship->data().begin(); key != ship->data().end(); ++key)
            {
                if (key->type() == TYPE_NAME)
                    name = key->value();
                else if (key->type() == TYPE_TYPE)
                    type = key->value();
                else if (key->type() == TYPE_SIZE)
                    size = key->getInt();
                else if (key->type() == TYPE_DAMAGE)
                    damage = key->getInt();
                else if (key->type() == TYPE_COAST)
                    coast = key->getInt();
                else if (key->type() == TYPE_CARGO)
                    cargo = key->getInt();
                else if (key->type() == TYPE_CAPACITY)
                    capacity = key->getInt();
                else if (key->type() != TYPE_LOAD && key->type() != TYPE_MAXLOAD && key->type() != TYPE_FACTION)
                    unhandled.push_back(key);
            }

            if (name.empty())
                name = type;

            label.format("%s (%s)", name.text(), ship->id().text());
            if (!type.empty())
                label += ", " + type;
            if (size > 0)
                label += FXString(L", Gr\u00f6\u00dfe ") + FXStringVal(size);

            FXTreeItem* node = makeItem(label, &*ship, nullptr);
            prependItem(nullptr, node)->setExpanded(true);

            // Schaden
            if (damage > 0)
                appendItem(node, FXStringVal(damage) + FXString(L"% besch\u00e4digt"));

            // Kueste
            if (coast >= 0 && coast < 6) {
                appendItem(node, coastToString(coast));
            }

            // Beladung (cargo/capacity)
            if (cargo > 0 || capacity > 0)
            {
                appendItem(node, "Beladung: " + weightToString(cargo) + " von " + weightToString(capacity) + " GE");
            }

            // list unhandled keys
            for (std::vector<datakey::list_type::const_iterator>::iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
            {
                datakey::list_type::const_iterator t = *itag;
                datablock* block = nullptr;
                if (t->type() == TYPE_CAPTAIN) {
                    FXint uid = FXIntVal(t->value());
                    datablock::itor unit_owner;
                    if (mapFile->getUnit(unit_owner, uid)) {
                        datablock& owner_blk = *unit_owner;
                        if (unit_owner != unit) {
                            block = &owner_blk;
                        }
                        label = t->translatedKey() + ": " + mapFile->unitName(owner_blk, true);
                    }
                }
                else {
                    FXString key = t->translatedKey();
                    label.format("%s: %s", key.text(), t->value().text());
                }
                if (block) {
                    item = appendItem(node, makeItem(label, block, nullptr));
                }
                else {
                    item = appendItem(node, label);
                }
            }

            effects = end;
            for (datablock::itor block = std::next(ship); block != end && block->depth() > unit->depth(); block++)
            {
                if (block->type() == block_type::TYPE_EFFECTS)
                    effects = block;
            }

            if (effects != end)		// does a EFFEKTE block exist?
            {
                FXTreeItem* effnode = appendItem(node, "Effekte");
                effnode->setExpanded(true);

                for (datakey::list_type::const_iterator key = effects->data().begin(); key != effects->data().end(); ++key)
                    appendItem(effnode, key->value());
            }
        }
    }
}
