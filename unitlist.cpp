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
    
    FXMAPFUNC(SEL_QUERY_HELP,		0,									FXUnitList::onQueryHelp),

	FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	FXUnitList::ID_LIST,			FXUnitList::onPopup),
}; 

FXIMPLEMENT(FXUnitList,FXVerticalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXUnitList::FXUnitList(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXVerticalFrame(p, opts, x,y,w,h, 0,0,0,0, 0,0)
{
	// set target
	setTarget(tgt);
	setSelector(sel);

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	// init variables
	mapFile = NULL;

	// create layout
	list = new FXTreeList(this, this,ID_LIST, LAYOUT_FILL_X|LAYOUT_FILL_Y|
			TREELIST_SINGLESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES);
}

void FXUnitList::create()
{
	FXVerticalFrame::create();
}

FXUnitList::~FXUnitList()
{
}

void FXUnitList::setMapFile(datafile *f)
{
    mapFile = f;
}

long FXUnitList::onMapChange(FXObject* /*sender*/, FXSelector, void* ptr)
{
	datafile::SelectionState *state = (datafile::SelectionState*)ptr;

	// any data changed, so need to update list?
	if (selection.fileChange != state->fileChange)
	{
		selection.fileChange = state->fileChange;
		selection.map = state->map;
		selection.activefaction = state->activefaction;

		// clear list and build a new one from data in this->files
		//clearItems();
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

		list->clearItems();		// clear list and build new

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
                if (block->type() == datablock::TYPE_ITEMS)
					items = block;
				else if (block->type() == datablock::TYPE_TALENTS)
					talents = block;
				else if (block->type() == datablock::TYPE_SPELLS)
					spells = block;
				else if (block->type() == datablock::TYPE_EFFECTS)
					effects = block;
				else if (block->type() == datablock::TYPE_COMBATSPELL)
					combatspells[block->info()] = block;
			}

			FXString name, descr, number, group, prefix, race, weight;
			FXString aura, auramax, hero;
			FXString hp, hungry, combatstatus, guards;
			FXint factionId = -1;
			FXint AnotherfactionId = -2;

			std::vector<datakey::itor> unhandled;

			for (datakey::itor key = unit->data().begin(); key != unit->data().end(); key++)
			{
				if (key->type() == datakey::TYPE_NAME)
					name = key->value();
				else if (key->type() == datakey::TYPE_DESCRIPTION)
					descr = key->value();
				else if (key->type() == datakey::TYPE_FACTION)
					factionId = atoi(key->value().text());
				else if (key->key() == "Anderepartei")
					AnotherfactionId = atoi(key->value().text());
				else if (key->type() == datakey::TYPE_NUMBER)
					number = key->value();
				else if (key->type() == datakey::TYPE_TYPE)
					race = key->value();
				else if (key->type() == datakey::TYPE_AURA)
					aura = key->value();
				else if (key->type() == datakey::TYPE_AURAMAX)
					auramax = key->value();
				else if (key->key() == "Burg")
					building = mapFile->building(atoi(key->value().text()));
				else if (key->key() == "Schiff")
					ship = mapFile->ship(atoi(key->value().text()));
				else if (key->key() == "weight")
					weight = key->value();
				else if (key->key() == "gruppe")
					group = key->value();
				else if (key->key() == "typprefix")
					prefix = key->value();
				else if (key->key() == "Kampfstatus")
					combatstatus = key->value();
				else if (key->key() == "hp")
					hp = key->value();
				else if (key->key() == "bewacht")
					guards = key->value();
				else if (key->key() == "hunger")
					hungry = key->value();
				else if (key->key() == "hero")
					hero = key->value();
				else
					unhandled.push_back(key);
			}

			/*
				Phygon (phyg)
				* 1 Nebelmeermensch
				* 500 von 700 Aura
			*/

			// prefix+race
			if (!prefix.empty() && !race.empty())
				race = prefix + race.lower();

			FXTreeItem *item;

			FXString label;
			label.format("%s (%s)", name.text(), unit->id().text());
			FXTreeItem *unititem = list->appendItem(NULL, label);
			unititem->setExpanded(true);

			if (factionId != -1 || AnotherfactionId != -1 || !group.empty())
			{
				datablock::itor faction = mapFile->faction(factionId);
				datablock::itor anotherfaction = mapFile->faction(AnotherfactionId);

				label = "";

				if (faction != end)
				{
					name = faction->value(datakey::TYPE_FACTIONNAME);
					if (name.empty())
						name = "Partei";

					label.format("%s (%s)", name.text(), faction->id().text());
				}

				if (anotherfaction != end)
				{
					if (!label.empty())
						label += ", ";

					name = anotherfaction->value(datakey::TYPE_FACTIONNAME);
					if (name.empty())
						name = "Partei";

					FXString label2;
					label2.format("%s (%s)", name.text(), faction->id().text());

					label += label2;
				}

				if (!group.empty())
				{
					if (!label.empty())
						label += ", ";

					label += "Gruppe " + group;
				}

				item = list->appendItem(unititem, label.text());
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

				item = list->appendItem(unititem, "Gewicht: " + weight);
			}

			label.format("%s %s", number.text(), race.text());
			if (!hero.empty())
				label += ", Held";
			item = list->appendItem(unititem, label.text());

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

				item = list->appendItem(unititem, label.text());
			}

			if (!aura.empty() || !auramax.empty())
			{
				label.format("%s von %s Aura", aura.text(), auramax.text());
				item = list->appendItem(unititem, label.text());
			}

			// list unhandled keys
			for (std::vector<datakey::itor>::iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
			{
				label.format("%s: %s", (*itag)->key().text(), (*itag)->value().text());
				item = list->appendItem(unititem, label.text());
			}

			if (spells != end)		// does a SPRUECHE block exist?
			{
				FXTreeItem *node = list->appendItem(unititem, FXString(L"Zauberspr\u00fcche"));
				//node->setExpanded(true);

				for (datakey::itor key = spells->data().begin(); key != spells->data().end(); ++key)
				{
					item = list->appendItem(node, key->value());
					item->setData((void*)1);	// show "info" popup cmd
				}
			}

			if (combatspells.size())	// do some KAMPFZAUBER blocks exist?
			{
				FXTreeItem *node = list->appendItem(unititem, "Kampfzauber");
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
					list->appendItem(node, label);
				}
			}

			if (effects != end)		// does a EFFEKTE block exist?
			{
				FXTreeItem *node = list->appendItem(unititem, "Effekte");
				node->setExpanded(true);

				for (datakey::itor key = effects->data().begin(); key != effects->data().end(); key++)
					list->appendItem(node, key->value());
			}

			if (talents != end)		// does a TALENTE block exist?
			{
				FXTreeItem *node = list->appendItem(unititem, "Talente");
				node->setExpanded(true);

				for (datakey::itor key = talents->data().begin(); key != talents->data().end(); key++)
				{
					item = list->appendItem(node, key->key()+" "+key->value().section(' ',1));
					item->setData((void*)1);	// show "info" popup cmd
				}

			}

			if (items != end)		// does a GEGENSTAENDE block exist?
			{
				FXTreeItem *node = list->appendItem(unititem, FXString(L"Gegenst\u00e4nde"));
				node->setExpanded(true);

				for (datakey::itor key = items->data().begin(); key != items->data().end(); key++)
				{
					item = list->appendItem(node, key->value()+" "+key->key());
					item->setData((void*)1);	// show "info" popup cmd
				}
			}

			if (building != end)	// unit in a building?
			{
				unhandled.clear();

				FXString size;
				FXString type = FXString(L"Geb\u00e4ude");
				factionId = -1;

				for (datakey::itor key = building->data().begin(); key != building->data().end(); key++)
				{
					if (key->type() == datakey::TYPE_NAME)
						name = key->value();
					else if (key->type() == datakey::TYPE_DESCRIPTION)
						descr = key->value();
					else if (key->type() == datakey::TYPE_FACTION)
						factionId = atoi(key->value().text());
					else if (key->type() == datakey::TYPE_TYPE)
						type = key->value();
					else if (key->key() == "Groesse")
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

				FXTreeItem *node = list->prependItem(NULL, label);
				node->setExpanded(true);

				// list unhandled keys
				for (std::vector<datakey::itor>::iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
				{
					label.format("%s: %s", (*itag)->key().text(), (*itag)->value().text());
					item = list->appendItem(node, label.text());
				}

				effects = end;
				block = building;
				for (block++; block != end && block->depth() > unit->depth(); block++)
				{
					if (block->type() == datablock::TYPE_EFFECTS)
						effects = block;
				}

				if (effects != end)		// does a EFFEKTE block exist?
				{
					FXTreeItem *effnode = list->appendItem(node, "Effekte");
					effnode->setExpanded(true);

					for (datakey::itor key = effects->data().begin(); key != effects->data().end(); key++)
						list->appendItem(effnode, key->value());
				}
			}

			if (ship != end)	// unit in a ship?
			{
				unhandled.clear();

				FXString size, captain;
				FXString damage, coast, cargo, capacity;
				FXString type = "Schiff";
				factionId = -1;

				for (datakey::itor key = ship->data().begin(); key != ship->data().end(); key++)
				{
					if (key->type() == datakey::TYPE_NAME)
						name = key->value();
					else if (key->type() == datakey::TYPE_DESCRIPTION)
						descr = key->value();
					else if (key->type() == datakey::TYPE_FACTION)
						factionId = atoi(key->value().text());
					else if (key->type() == datakey::TYPE_TYPE)
						type = key->value();
					else if (key->key() == "Groesse")
						size = key->value();
					else if (key->key() == "Schaden")
						damage = key->value();
					else if (key->key() == "Kapitaen")
						captain = key->value();
					else if (key->key() == "Kueste")
						coast = key->value();
					else if (key->key() == "cargo")
						cargo = key->value();
					else if (key->key() == "capacity")
						capacity = key->value();
					else if (key->key() == "Ladung" || key->key() == "MaxLadung")
					{} // ignore
					else
						unhandled.push_back(key);
				}

				if (name.empty())
					name = type;

				label.format("An Bord von %s (%s)", name.text(), ship->id().text());
				if (!type.empty())
					label += ", " + type;
				if (!size.empty())
					label += FXString(L", Gr\u00f6\u00dfe ") + size;

				FXTreeItem *node = list->prependItem(NULL, label);
				node->setExpanded(true);

				// Schaden
				if (!damage.empty())

					list->appendItem(node, damage + FXString(L"% besch\u00e4digt"));
				// Kueste
				const wchar_t* coasts[] = { L"Nordwest", L"Nordost", L"Ost", L"S\u00fcdost", L"S\u00fcdwest", L"West", L"-unknown-" };

				if (!coast.empty())
				{
					FXint dir = 0;
					dir = atoi(coast.text());

					if (dir < 0 || dir > 6)
						dir = 6;

					list->appendItem(node, FXString(coasts[dir]) + FXString(L"k\u00fcste"));
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

					list->appendItem(node, "Beladung: " + cargo + " von " + capacity + " GE");
				}

				// list unhandled keys
				for (std::vector<datakey::itor>::iterator itag = unhandled.begin(); itag != unhandled.end(); ++itag)
				{
					label.format("%s: %s", (*itag)->key().text(), (*itag)->value().text());
					item = list->appendItem(node, label.text());
				}

				effects = end;
				block = ship;
				for (block++; block != end && block->depth() > unit->depth(); block++)
				{
					if (block->type() == datablock::TYPE_EFFECTS)
						effects = block;
				}

				if (effects != end)		// does a EFFEKTE block exist?
				{
					FXTreeItem *effnode = list->appendItem(node, "Effekte");
					effnode->setExpanded(true);

					for (datakey::itor key = effects->data().begin(); key != effects->data().end(); key++)
						list->appendItem(effnode, key->value());
				}
			}
		}
	}

	return 1;
}

long FXUnitList::onCopyTree(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender && sender->isMemberOf(&FXMenuCommand::metaClass))
    {
        FXMenuCommand *cmd = (FXMenuCommand*)sender;
        FXTreeItem *item = (FXTreeItem *)cmd->getUserData();
        setClipboard(getSubTreeText(item));
        return 1;
    }
    return 0;
}

long FXUnitList::onCopyText(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender && sender->isMemberOf(&FXMenuCommand::metaClass))
    {
        FXMenuCommand *cmd = (FXMenuCommand*)sender;
        FXTreeItem *item = (FXTreeItem *)cmd->getUserData();
        if (item) {
            setClipboard(item->getText());
            return 1;
        }
    }
    return 0;
}

long FXUnitList::onShowInfo(FXObject* sender, FXSelector sel, void* ptr)
{
    if (sender && sender->isMemberOf(&FXMenuCommand::metaClass))
    {
        FXMenuCommand *cmd = (FXMenuCommand*)sender;
        FXTreeItem *item = (FXTreeItem *)cmd->getUserData();
        showInfo(item->getText());
        if (item) {
            setClipboard(item->getText());
            return 1;
        }
    }
    return 0;
}

long FXUnitList::onPopup(FXObject* sender, FXSelector sel, void* ptr)
{ 
	FXVerticalFrame::onRightBtnRelease(sender, sel, ptr);

	FXEvent *event = (FXEvent*)ptr;

	// dont't show popup if mouse has moved
	if (event->last_x != event->click_x || event->last_y != event->click_y)
		return 0;
	
	// create popup
	FXTreeItem *item = list->getItemAt(event->click_x, event->click_y);
	if (!item)
		return 0;

	FXMenuPane popup(this);
	FXMenuPane *menu = &popup;

	FXString title = item->getText();
	if (title.length() > 20)
		title = title.left(17) + "...";

	new FXMenuSeparatorEx(menu, title);

	FXMenuPane *clipboard = new FXMenuPane(this);
	new FXMenuCascade(menu, "&Zwischenablage", NULL, clipboard, 0);

    (new FXMenuCommand(clipboard, L"Alles", NULL, this, ID_POPUP_COPY_TREE))->setUserData(NULL);
    (new FXMenuCommand(clipboard, L"Baum", NULL, this, ID_POPUP_COPY_TREE))->setUserData(item);
    (new FXMenuCommand(clipboard, L"Eintrag", NULL, this, ID_POPUP_COPY_TEXT))->setUserData(item);

    if (item->getData()) {
        (new FXMenuCommand(menu, "Zeige Info", NULL, this, ID_POPUP_SHOW_INFO))->setUserData(item);
    }
	// show popup
	menu->create();
	menu->popup(NULL, event->root_x,event->root_y);

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
		item = list->getFirstItem();
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
