#include "fxhelper.h"
#include "statistics.h"
#include "FXMenuSeparatorEx.h"

#include <algorithm>

// *********************************************************************************************************
// *** FXStatistics implementation

FXDEFMAP(FXStatistics) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,			FXStatistics::ID_FACTIONBOX,		FXStatistics::onChangeFaction), 
	FXMAPFUNC(SEL_COMMAND,			FXStatistics::ID_FILTERBOX,			FXStatistics::onChangeFilter), 
	FXMAPFUNC(SEL_UPDATE,			FXStatistics::ID_FILTERBOX,			FXStatistics::updChangeFilter), 

	FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, FXStatistics::ID_LIST,			FXStatistics::onPopup), 
	FXMAPFUNC(SEL_COMMAND,			FXStatistics::ID_POPUP_CLICKED,		FXStatistics::onPopupClicked), 

	FXMAPFUNC(SEL_COMMAND,			FXStatistics::ID_UPDATE,			FXStatistics::onMapChange), 
	FXMAPFUNC(SEL_QUERY_HELP,		0,									FXStatistics::onQueryHelp), 
}; 

FXIMPLEMENT(FXStatistics,FXVerticalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXStatistics::FXStatistics(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXVerticalFrame(p, opts, x,y,w,h, 0,0,0,0, 0,0)
{
	// set target
	setTarget(tgt);
	setSelector(sel);

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	select.faction = 0;		// no faction selected
	select.filter = select.FILTER_PERSONS|select.FILTER_ITEMS|select.FILTER_TALENTS;

	// create faction combobox
	factionBox = new FXComboBox(this, 0, this,ID_FACTIONBOX, COMBOBOX_STATIC|LAYOUT_FILL_X|FRAME_RAISED);

	// create filter checkboxes
	FXHorizontalFrame *filterboxes = new FXHorizontalFrame(this, 0, 0,0,0,0, 0,0,0,0, 0,0);

    opTab = new FXToolBarTab(filterboxes, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	opTab->setTipText("Filteroptionen ein- und ausblenden");

	FXMatrix *optionmatrix = new FXMatrix(filterboxes, 3, MATRIX_BY_COLUMNS|PACK_UNIFORM_HEIGHT, 0,0,0,0, 2,2,2,2, 2,2);

	FXToggleButton *btn;		
	btn = new FXToggleButton(optionmatrix, "Personen", "Personen", NULL,NULL, this,ID_FILTERBOX, TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE|FRAME_RAISED|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	btn->setUserData((void*)select.FILTER_PERSONS);
	btn = new FXToggleButton(optionmatrix, FXString(L"Gegenst\u00e4nde"), FXString(L"Gegenst\u00e4nde"), NULL,NULL, this,ID_FILTERBOX, TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE|FRAME_RAISED|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	btn->setUserData((void*)select.FILTER_ITEMS);
	btn = new FXToggleButton(optionmatrix, "Talente", "Talente", NULL,NULL, this,ID_FILTERBOX, TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE|FRAME_RAISED|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	btn->setUserData((void*)select.FILTER_TALENTS);
	btn = new FXToggleButton(optionmatrix, FXString(L"Geb\u00e4ude"), FXString(L"Geb\u00e4ude"), NULL,NULL, this,ID_FILTERBOX, TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE|FRAME_RAISED|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	btn->setUserData((void*)select.FILTER_BUILDINGS);
	btn = new FXToggleButton(optionmatrix, "Schiffe", "Schiffe", NULL,NULL, this,ID_FILTERBOX, TOGGLEBUTTON_TOOLBAR|TOGGLEBUTTON_KEEPSTATE|FRAME_RAISED|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	btn->setUserData((void*)select.FILTER_SHIPS);

	// create list
    list = new FXList(this, this,ID_LIST, LIST_BROWSESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
    // hide the selection:
	list->setSelTextColor(list->getTextColor());
	list->setSelBackColor(list->getBackColor());
}

void FXStatistics::create()
{
	FXVerticalFrame::create();
}

FXStatistics::~FXStatistics()
{
}

void FXStatistics::setMapFile(std::shared_ptr<datafile>& f)
{
    mapFile = f;
}

void FXStatistics::loadState(FXRegistry& reg)
{
	// load options
	select.filter = reg.readUnsignedEntry("STATISTICS", "FILTER", select.filter);

	FXint hidefilters = reg.readUnsignedEntry("STATISTICS", "HIDE_FILTERS", 0);
	opTab->collapse(hidefilters != 0);
}

void FXStatistics::saveState(FXRegistry& reg)
{
	// save options
	reg.writeUnsignedEntry("STATISTICS", "FILTER", select.filter);

	reg.writeUnsignedEntry("STATISTICS", "HIDE_FILTERS", opTab->isCollapsed());
}

void FXStatistics::collectData(std::map<FXString, entry> &persons, std::map<FXString, entry> &items,
							   std::map<std::pair<FXString,int>, entry> &talents, std::map<FXString, entry> &ships,
							   std::map<FXString, entry> &buildings, const datablock::itor& region)
{
	bool unitInFaction = false;
	int unitId = 0, personsInUnit = 0;

    if (!mapFile) return;
    FXint item = factionBox->getCurrentItem();
    FXival selected_faction = (FXival)factionBox->getItemData(item);
    datablock::itor end = mapFile->blocks().end();
	for (datablock::itor block = std::next(region); block != end && block->depth() > region->depth(); block++)
	{
		if (block->type() == block_type::TYPE_SHIP)
		{
			FXString fac = block->value(TYPE_FACTION);
			int faction = -1;
			if (!fac.empty())
				faction = atoi(fac.text());

			if (faction == selected_faction || selected_faction == 0)
			{
				int size = atoi(block->value("Groesse").text());

				// <Shiptype>: <Size>
				FXString type = block->value(TYPE_TYPE);
				if (!type.empty())
					ships[type].list.push_back(std::make_pair(block->info(), size));
			}
		}
		else if (block->type() == block_type::TYPE_BUILDING)
		{
			FXString fac = block->value(TYPE_FACTION);
			int faction = -1;
			if (!fac.empty())
				faction = atoi(fac.text());

			if (faction == selected_faction || selected_faction == 0)
			{
				int size = atoi(block->value("Groesse").text());

				// <Buildingtype>: <Size>
				FXString type = block->value(TYPE_TYPE);
				if (!type.empty())
					buildings[type].list.push_back(std::make_pair(block->info(), size));
			}
		}
		else if (block->type() == block_type::TYPE_UNIT)
		{
			unitId = block->info();

			FXString fac = block->value(TYPE_FACTION);
			int faction = -1;
			if (!fac.empty())
				faction = atoi(fac.text());

			if (faction == selected_faction || selected_faction == 0)
				unitInFaction = true;
			else
				unitInFaction = false;
		}

		if (!unitInFaction)
			continue;
		
		// collect statistics from this block
		if (block->type() == block_type::TYPE_UNIT)
		{
			FXString val, type = block->value(TYPE_TYPE);

			// <Race>: #
			val = block->value(TYPE_NUMBER);
			if (!type.empty() && !val.empty())
			{
				personsInUnit = atoi(val.text());
				persons[type].list.push_back(std::make_pair(unitId, personsInUnit));
			}
			else	
				personsInUnit = 0;

            val = block->value(TYPE_HERO);
            if (!val.empty()) {
                persons["Helden"].list.push_back(std::make_pair(unitId, personsInUnit));
            }

			// Aura: #
			val = block->value(TYPE_AURA);
			if (!val.empty())
				items["Aura"].list.push_back(std::make_pair(unitId, atoi(val.text())));
		}

		// collect statistics from this block
		if (block->type() == block_type::TYPE_ITEMS)
		{
			// <Item>: #
            for (datakey::list_type::const_iterator itor = block->data().begin(); itor != block->data().end(); ++itor) {
                if (!itor->value().empty())
                    items[itor->key()].list.push_back(std::make_pair(unitId, atoi(itor->value().text())));
            }
		}

		// collect statistics from this block
		if (block->type() == block_type::TYPE_TALENTS)
		{
			//stats_talents, numberInUnit

			// <Talent> <Level>: #
			for (datakey::list_type::const_iterator itor = block->data().begin(); itor != block->data().end(); ++itor)
				if (!itor->value().empty())
				{
					FXString talent = itor->value().after(' ');
					int level = atoi(talent.text());

					talents[std::make_pair(itor->key(), level)].list.push_back(std::make_pair(unitId, personsInUnit));
				}
		}
	}
}

void FXStatistics::updateList()
{
	// clear old list	
	list->clearItems();
	entries.clear();

	// generate new list
	std::map<FXString, entry> persons;
	std::map<FXString, entry> items;
	std::map<FXString, entry> ships;
	std::map<FXString, entry> buildings;
	std::map<std::pair<FXString,int>, entry> talents;

	// collect data
	if (!selection.regionsSelected.empty())
	{
		for (datablock* block : selection.regionsSelected)
		{
            datablock::itor region;
            if (mapFile->getRegion(region, *block)) {
				// collect if selected
				collectData(persons, items, talents, ships, buildings, region);
			}
		}
	}
	else if (selection.selected & selection.REGION)
	{
		collectData(persons, items, talents, ships, buildings, selection.region);
	}

	// add statistics to the list
	bool list_persons = (select.filter & select.FILTER_PERSONS) && !persons.empty();
	bool list_items = (select.filter & select.FILTER_ITEMS) && !items.empty();
	bool list_talents = (select.filter & select.FILTER_TALENTS) && !talents.empty();
	bool list_ships = (select.filter & select.FILTER_SHIPS) && !ships.empty();
	bool list_buildings = (select.filter & select.FILTER_BUILDINGS) && !buildings.empty();

	// add persons to list if not filtered out
	if (list_persons)
	{
		for (std::map<FXString, entry>::iterator itor = persons.begin(); itor != persons.end(); itor++)
		{
			const entry& en = itor->second;

			// calculate sum of units persons
			int sum = 0;
			for (std::vector<std::pair<int, int> >::iterator unit = itor->second.list.begin(); unit != itor->second.list.end(); unit++)
				sum += unit->second;

			FXString label;
			label.format("%s: %d", itor->first.text(), sum);
			int idx = list->appendItem(label);
			list->setItemData(idx, (void*)0);		// 0: unit, 1: building, 2: ship

			// save additional data
			entries[idx] = en;
		}

		if (list_items || list_talents || list_buildings || list_ships)
			list->appendItem("--------------------------");
	}

	// add items (and aura) to list
	if (list_items)
	{
		for (std::map<FXString, entry>::iterator itor = items.begin(); itor != items.end(); itor++)
		{
			const entry& en = itor->second;

			// calculate sum of units items
			int sum = 0;
			for (std::vector<std::pair<int, int> >::iterator unit = itor->second.list.begin(); unit != itor->second.list.end(); unit++)
				sum += unit->second;

			FXString label;
			label.format("%s: %d", itor->first.text(), sum);
			int idx = list->appendItem(label);
			list->setItemData(idx, (void*)0);		// 0: unit, 1: building, 2: ship

			// save additional data
			entries[idx] = en;
		}

		if (list_talents || list_buildings || list_ships)
			list->appendItem("--------------------------");
	}

	// add talents to list
	if (list_talents)
	{
		for (std::map<std::pair<FXString,int>, entry>::iterator itor = talents.begin(); itor != talents.end(); itor++)
		{
			const entry& en = itor->second;

			// calculate sum of units items
			int sum = 0;
			for (std::vector<std::pair<int, int> >::iterator unit = itor->second.list.begin(); unit != itor->second.list.end(); unit++)
				sum += unit->second;

			FXString label;
			label.format("%s %d: %d", itor->first.first.text(), itor->first.second, sum);
			int idx = list->appendItem(label);
			list->setItemData(idx, (void*)0);		// 0: unit, 1: building, 2: ship

			// save additional data
			entries[idx] = en;
		}

		if (list_buildings || list_ships)
			list->appendItem("--------------------------");
	}

	// add castles and buildings
	if (list_buildings)
	{
		for (std::map<FXString, entry>::iterator itor = buildings.begin(); itor != buildings.end(); itor++)
		{
			const entry& en = itor->second;

			// calculate sum of units items
			int sum = 0, min_ = -1, max_ = -1;
			for (std::vector<std::pair<int, int> >::iterator building = itor->second.list.begin(); building != itor->second.list.end(); building++)
			{
				sum++;

				if (min_ == -1 || building->second < min_)
					min_ = building->second;
				if (building->second > max_)
					max_ = building->second;
			}

			FXString label;
			if (min_==max_)
				label.format("%s: %d (%d)", itor->first.text(), sum, min_);
			else
				label.format("%s: %d (%d-%d)", itor->first.text(), sum, min_, max_);
			int idx = list->appendItem(label);
			list->setItemData(idx, (void*)1);		// 0: unit, 1: building, 2: ship

			// save additional data
			entries[idx] = en;
		}

		if (list_ships)
			list->appendItem("--------------------------");
	}

	// add castles and buildings
	if (list_ships)
	{
		for (std::map<FXString, entry>::iterator itor = ships.begin(); itor != ships.end(); itor++)
		{
			const entry& en = itor->second;

			FXString label;
			label.format("%s: %zd", itor->first.text(), itor->second.list.size());
			int idx = list->appendItem(label);
			list->setItemData(idx, (void*)2);		// 0: unit, 1: building, 2: ship

			// save additional data
			entries[idx] = en;
		}
	}
}

long FXStatistics::onChangeFaction(FXObject*, FXSelector, void*)
{
	// the faction to list statistics for
	int item = factionBox->getCurrentItem();
    if (item >= 0) {
        FXival factionId = (FXival)factionBox->getItemData(item);
        if (factionId >= 0) {
            select.faction = factionId;
        }
    }

	// change statistics in list
	updateList();

	return 1;
}

long FXStatistics::onChangeFilter(FXObject* sender, FXSelector, void* ptr)
{
	if (!sender || !sender->isMemberOf(&FXId::metaClass))
		return 0;

	FXId *item = (FXId*)sender;
    FXival udata = (FXival)item->getUserData();

	if (ptr)	// new button state
		select.filter |= udata;			// set filter
	else
		select.filter &= ~udata;			// unset filter

	// change statistics in list
	updateList();

	return 1;
}

long FXStatistics::updChangeFilter(FXObject* sender, FXSelector, void*)
{
	if (!sender || !sender->isMemberOf(&FXId::metaClass))
		return 0;

	FXId *item = (FXId*)sender;
    FXival mask = (FXival)item->getUserData();

	sender->handle(this, FXSEL(SEL_COMMAND, (select.filter & mask)?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

long FXStatistics::onPopup(FXObject* sender,FXSelector sel, void* ptr)
{
	FXVerticalFrame::onRightBtnRelease(sender, sel, ptr);

	// no data, no popup:
	if (!mapFile)
		return 0;

    FXEvent* event = (FXEvent*)ptr;
    
    int itemIdx = list->getCursorItem();
    if (itemIdx < 0) {
        itemIdx = list->getItemAt(event->click_x, event->click_y);
        if (itemIdx < 0) {
            return 0;
        }
    }
    FXListItem* item = list->getItem(itemIdx);
	if (!item)
		return 0;

    FXuval entryType = (FXuval)item->getData();	// 0: unit, 1: building, 2: ship

	FXMenuPane menu(this);

	new FXMenuSeparatorEx(&menu, item->getText());

	std::map<int, entry>::iterator en = entries.find(itemIdx);
	if (en != entries.end())
	{
		entry& entry = en->second;

		for (std::vector<std::pair<int, int> >::iterator itor = entry.list.begin(); itor != entry.list.end(); itor++)
		{
			datablock::itor block;

            FXString label, name;
            if (entryType == 0) {
                if (!mapFile->getUnit(block, itor->first)) continue;		// get unit
            }
            else if (entryType == 1) {
                if (!mapFile->getBuilding(block, itor->first)) continue;	// get building/castle
            }
            else if (entryType == 2) {
                if (!mapFile->getShip(block, itor->first)) continue;		// get ship
            }
            else {
                // something done effed up
                continue;
            }
            name = block->getName();
            label.format("%s (%s): ", name.text(), block->id().text());
            if (entryType > 0) {
                label += FXString(L"Gr\u00f6\u00dfe ");
            }
            label += FXStringVal(itor->second);
			FXMenuCommand *menuitem = new FXMenuCommand(&menu, label, NULL, this, ID_POPUP_CLICKED);
			menuitem->setUserData((void*)&*block);
		}
	}

	// show popup
    menu.create();
	menu.popup(NULL, event->root_x,event->root_y);

    getApp()->runModalWhileShown(&menu);
	return 1;
}

long FXStatistics::onPopupClicked(FXObject* sender,FXSelector, void*)
{
	// no data, no popup
	if (!mapFile)
		return 0;

	FXMenuCommand *cmd = static_cast<FXMenuCommand*>(sender);
		
	datablock* dblock = (datablock*)cmd->getUserData();

	datablock::itor main = mapFile->blocks().end();
	datablock::itor iparent = mapFile->blocks().end();

	for (datablock::itor block = mapFile->blocks().begin(); block != mapFile->blocks().end(); block++)
	{
		// nothing to search for
		if (!dblock)
			break;

		// handle only regions, factions (+alliances), buildings, ships and units
		if (block->type() != block_type::TYPE_REGION &&
			block->type() != block_type::TYPE_ALLIANCE &&
			block->type() != block_type::TYPE_FACTION &&
			block->type() != block_type::TYPE_BUILDING &&
			block->type() != block_type::TYPE_SHIP &&
			block->type() != block_type::TYPE_UNIT)
			continue;

		if (dblock && dblock == &*block)
		{
			main = block;
			dblock = NULL;		// to indicate that block was found.
		}
		else if (block->type() == block_type::TYPE_REGION)
		{
			iparent = block;
		}
	}

	if (main == mapFile->blocks().end())
	{
		main = iparent;
		iparent = mapFile->blocks().end();
	}

	if (main != mapFile->blocks().end())
	{
		// send new selection to main window
        selection.selected = 0;
        FXString clipboard;
        if (main->type() == block_type::TYPE_REGION)
		{
            selection.selected = selection.REGION;
            selection.region = main;
		}
		if (main->type() == block_type::TYPE_FACTION)
		{
            selection.selected = selection.FACTION;
            selection.faction = main;

			if (iparent != mapFile->blocks().end())
			{
                selection.selected |= selection.REGION;
                selection.region = iparent;
			}
		}
		if (main->type() == block_type::TYPE_BUILDING)
		{
            selection.selected = selection.BUILDING;
            selection.building = main;
            clipboard = main->id();
            if (iparent != mapFile->blocks().end())
            {
                selection.selected |= selection.REGION;
                selection.region = iparent;
            }
        }
		if (main->type() == block_type::TYPE_SHIP)
		{
            selection.selected = selection.SHIP;
            selection.ship = main;
            clipboard = main->id();
            if (iparent != mapFile->blocks().end())
            {
                selection.selected |= selection.REGION;
                selection.region = iparent;
            }
        }
		if (main->type() == block_type::TYPE_UNIT)
		{
            selection.selected = selection.UNIT;
            selection.unit = main;

			if (iparent != mapFile->blocks().end())
			{
                selection.selected |= selection.FACTION;
                selection.faction = iparent;
			}			
		}
        getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
        if (!clipboard.empty()) {
            getTarget()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), (void *)clipboard.text());
        }
		return 1;
	}

	return 0;
}

bool FXStatistics::collectFactionList(std::set<int> &factions, datablock::itor region)
{
    if (!mapFile) return false;
	// list factions of selected region
    bool bFound = false;
    datablock::itor end = mapFile->blocks().end();
	for (datablock::itor block = std::next(region); block != end && block->depth() > region->depth(); block++)
	{
		if (block->type() == block_type::TYPE_UNIT)
		{
			FXString fac = block->value(TYPE_FACTION);
			FXival factionId = -1;
			if (!fac.empty())
				factionId = atoi(fac.text());

			if (factionId < 0 || factions.find(factionId) == factions.end())
			{
				factions.insert(factionId);

                FXString label;
                datablock::itor faction;
                if (factionId <= 0) {
                    label.assign("Parteigetarnt");
                }
                else if (mapFile->getFaction(faction, factionId)) {
                    FXString name = faction->value(TYPE_FACTIONNAME);

                    if (name.empty())
                        label.assign("Parteigetarnt");
                    else
                        label.format("%s (%s)", name.text(), faction->id().text());

                }
                else {
                    // missing PARTEI block in report? how?
                    label.format("Unbekannt (%s)", FXStringValEx((FXulong)factionId, 36).text());
                }
                FXint index;
                if (mapFile->getActiveFactionId() == factionId) {
                    index = factionBox->prependItem(label, (void *)factionId);
                }
                else {
                    index = factionBox->appendItem(label, (void *)factionId);
                }
                // select previously selected faction again
                if (select.faction == factionId) {
                    factionBox->setCurrentItem(index);
                    bFound = true;
                }
			}
		}
	}
    return bFound;
}

long FXStatistics::onMapChange(FXObject* /*sender*/, FXSelector, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

	bool needUpdate = false;

	// any data changed, so need to update list?
	if (selection.fileChange != pstate->fileChange)
	{
		selection = *pstate;
		needUpdate = true;
	}
	else if (selection.selChange != pstate->selChange)
	{
        if ((selection.selected & selection.REGION) != (pstate->selected & selection.REGION)
            || (selection.selected & selection.REGION && selection.region != pstate->region))
        {
            needUpdate = true;				// ignore changes that don't change selected region
        }
        else if (selection.regionsSelected != pstate->regionsSelected) {
            needUpdate = true;
        }

        selection = *pstate;

		if (needUpdate)
			selection.regionsSelected = pstate->regionsSelected;
	}

	if (needUpdate)
	{
		if ((selection.selected & selection.REGION) || !selection.regionsSelected.empty())
		{
			// clear Box and create new one
			factionBox->clearItems();
            factionBox->setCurrentItem(-1);

			std::set<int> factions;
            bool bSelection = false;
			if (!selection.regionsSelected.empty())
			{
				for (datablock* block : selection.regionsSelected)
				{
                    datablock::itor region;
                    if (mapFile->getRegion(region, *block)) {
                        bSelection |= collectFactionList(factions, region);
					}
				}
			}
			else if (selection.selected & selection.REGION)
			{
                bSelection |= collectFactionList(factions, selection.region);
			}
            FXint index = factionBox->appendItem("Alles");
            // set size of combo box list
            factionBox->setNumVisible(std::min(factionBox->getNumItems(), 10));
            if (!bSelection) {
                factionBox->setCurrentItem(index);
            }

			// change statistics in list
            updateList();
		}
		else
		{
			factionBox->clearItems();
            list->clearItems();
			entries.clear();
		}
	}

	return 1;
}

long FXStatistics::onQueryHelp(FXObject* /*sender*/, FXSelector, void* /*ptr*/)
{ 
	return 0;
}
