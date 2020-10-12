#include "main.h"
#include "fxhelper.h"
#include "statsinfos.h"

#include <algorithm>
#include <exception>

// *********************************************************************************************************
// *** FXStatsInfos implementation

FXDEFMAP(FXStatsInfos) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,		FXStatsInfos::ID_UPDATE,				FXStatsInfos::onMapChange), 
}; 

FXIMPLEMENT(FXStatsInfos,FXVerticalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXStatsInfos::FXStatsInfos(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXVerticalFrame(p, opts, x,y,w,h, 0,0,0,0, 0,0)
{
	// set target etc.
	setTarget(tgt);
	setSelector(sel);

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	// init variables
	files = NULL;

	// create layout
	tags.matrixframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);

	tags.leftmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
	tags.matrixsep = new FXVerticalSeparator(tags.matrixframe,LAYOUT_FILL_Y|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	tags.matrixsep->setBorderColor(getBorderColor());
	tags.matrixsep->hide();
	tags.rightmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
}

void FXStatsInfos::create()
{
	FXVerticalFrame::create();

	createLabels("Keine Statistik", "", "", 0);
}

FXStatsInfos::~FXStatsInfos()
{
}

void FXStatsInfos::mapfiles(std::list<datafile> *f)
{
    files = f;
}

inline FXString thousandsPoints(FXint value, bool plusSign = false)
{
	FXString str = FXStringVal((value<0)?-value:value);

	int n = 0;
	for (int i = str.length(); i > 0; i--,n++)
		if (n && !(n%3))
			str.insert(i, '.');

	if (value<0)
		str = "-" + str;
	else if (value && plusSign)
		str = "+" + str;

	return str;
}

void FXStatsInfos::clearLabels()
{
	for (std::vector<FXLabel*>::iterator itor = tags.entries.begin(); itor != tags.entries.end(); itor++)
		delete *itor;

	tags.entries.clear();
}

void FXStatsInfos::createLabels(const FXString& name, const FXString& first, const FXString& second, int column)
{
	FXMatrix *matrix = (column==0) ? tags.leftmatrix : tags.rightmatrix;

	// create labels
	FXLabel *lname = new FXLabel(matrix, name, NULL, JUSTIFY_LEFT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	FXLabel *lfirst = new FXLabel(matrix, first, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	FXLabel *lsecond = new FXLabel(matrix, second, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	lsecond->disable();
	lname->create(); lfirst->create(); lsecond->create();

	// put into list
	tags.entries.push_back(lname);
	tags.entries.push_back(lfirst);
	tags.entries.push_back(lsecond);
}

void FXStatsInfos::setInfo(const std::list<Info>& info)
{
	clearLabels();

	int number = info.size();
	int index = 0;
	for (std::list<Info>::const_iterator itor = info.begin(); itor != info.end(); itor++, index++)
	{
		FXString value = thousandsPoints(itor->value);
		FXString offset = "";
		if (itor->offset)		
			offset = thousandsPoints(itor->offset, true);

		if (itor->skill)
			value += " (" + FXStringVal(itor->skill) + ")";

		FXString infotip = "\t" + value;
		if (offset.length())
			infotip += " [" + offset + "]";

		createLabels(itor->name+"\t"+itor->tip, value+infotip, offset+infotip, 2*index>=number);
	}

	if (number)
	{
		tags.matrixsep->show();
	}
	else
	{
		tags.matrixsep->hide();
		createLabels("Keine Statistik", "", "", 0);
	}
}

void FXStatsInfos::addEntry(std::list<Info>& info, FXString name, int value, int skill, int offset, FXString tip)
{
	std::list<Info>::iterator itor;
	for (itor = info.begin(); itor != info.end(); itor++)
		if (itor->name == name)
		{
			itor->value += value;
			itor->skill += skill;
			itor->offset += offset;
			break;
		}

	if (itor == info.end())
		info.push_back(Info(name, tip, value, skill, offset));
}

void FXStatsInfos::collectData(std::list<Info>& info, datablock::itor region)
{
	int WorkPerRegion[] =	// Arbeitsplätze pro Regionstyp
	{
		0,	//TERRAIN_UNKNOWN,
		0,	//TERRAIN_OCEAN,
		2000,	//TERRAIN_SWAMP,
		10000,	//TERRAIN_PLAINS,
		500,	//TERRAIN_DESERT,
		10000,	//TERRAIN_FOREST,
		4000,	//TERRAIN_HIGHLAND,
		1000,	//TERRAIN_MOUNTAIN,
		100,	//TERRAIN_GLACIER,
		500,	//TERRAIN_VOLCANO,
		0,	//TERRAIN_ICEBERG,
		0,	//TERRAIN_FIREWALL,
		0,	//TERRAIN_MAHLSTROM,
		0	//TERRAIN_LAST
	};

	int peasants = region->valueInt("Bauern");

	int workstations = WorkPerRegion[region->terrain()];
	workstations -= 8 * region->valueInt("Baeume");
	workstations -= 4 * region->valueInt("Schoesslinge");
	if (workstations < 0) workstations = 0;

	int Lohn = region->valueInt("Lohn");
	if (Lohn) addEntry(info, "Lohn", Lohn, 0, 0, iso2utf("Lohn für Arbeit"));

	int earnings = (Lohn+1) * std::min(workstations, peasants) - 10 * peasants;
	if (earnings) addEntry(info, "Bauernertrag", earnings, 0, 0, iso2utf("Überschuss der Bauerneinnahmen pro Runde. Kann sicher eingetrieben werden."));

	// search income messages for this region
	datablock::itor faction, block, end = files->front().blocks().end();
	if (selection.map & selection.ACTIVEFACTION)
	{
		block = faction = selection.activefaction;
		if (block != end)
			block++;
	}
	else
		faction = block = end;

	/*
		modes:
		0: misc
		1: Unterhaltung
		2: Steuern
		3: Handel
		4: Handel fremder Parteien
		5: Diebstahl
		6: Zauberei
	*/
	int income[7] = {};
	int learncost = 0;

	for (; block != end && block->depth() > faction->depth(); block++)
	{
		if (block->type() == datablock::TYPE_MESSAGE)
		{
			int type = block->valueInt("type");

			if (type != 771334452 && type != 443066738)
				continue;

			datakey::list_type& list = block->data();

			datakey::itor reg = list.end();
			int amount = 0, mode = 0;

			for (datakey::itor itor = list.begin(); itor != list.end(); itor++)
			{
				if (itor->key() == "region")
					reg = itor;
				else if (itor->key() == "amount")
					amount = itor->getInt();
				else if (itor->key() == "cost")
					amount = itor->getInt();
				else if (itor->key() == "mode")
					mode = itor->getInt();
			}

			if (reg == list.end())
				continue;

			FXString location = reg->value();
			int x = atoi(location.before(' ').text());
			location = location.after(' ');
			int y = atoi(location.before(' ').text());
			location = location.after(' ');
			int z = atoi(location.before(' ').text());
			
			if (region->x() != x || region->y() != y || region->info() != z)
				continue;

			if (mode < 0 || mode > 6)
				mode = 0;

			if (type == 771334452)	// Einnahmen
				income[mode] += amount;
			else if (type == 443066738)	// Ausgaben für teure Talente (+Akademie)
				learncost += amount;
		}
	}

	// display income messages
	if (income[0]) addEntry(info, "Einnahmen", income[0], 0, 0, iso2utf("Einnahmen durch sonstige Tätigkeiten."));
	if (income[1]) addEntry(info, "Unterhaltung", income[1], 0, 0, "Einnahmen durch Unterhaltung");
	if (income[2]) addEntry(info, "Steuern", income[2], 0, 0, "Einnahmen durch Steuern");
	if (income[3]) addEntry(info, "Handel", income[3], 0, 0, "Einnahmen durch Handel");
	if (income[4]) addEntry(info, "Handelsabgaben", income[4], 0, 0, "Einnahmen durch Handel fremder Parteien");
	if (income[5]) addEntry(info, "Diebstahl", income[5], 0, 0, "Einnahmen durch Diebstahl");
	if (income[6]) addEntry(info, "Zauberei", income[6], 0, 0, "Einnahmen durch Zauberei");

	// display learncost messages
	if (learncost) addEntry(info, "Lernkosten", -learncost, 0, 0, "Ausgaben zum Lernen von Talenten");
}

void FXStatsInfos::updateData()
{
	//if (files->empty())
		//return;

	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		std::list<Info> info;

		// collect infos
		datablock::itor notfound = files->begin()->blocks().end();
		for (std::set<datablock*>::iterator itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
		{
			datablock::itor region = files->begin()->region((*itor)->x(), (*itor)->y(), (*itor)->info());

			if (region != notfound)
			{
				// collect
				collectData(info, region);
			}
		}

		// "Lohn" makes no sense here. Remove.
		for (std::list<Info>::iterator itor = info.begin(); itor != info.end(); itor++)
		{
			if (itor->name == "Lohn")
				itor = info.erase(itor);
		}

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else if (selection.selected & selection.REGION)
	{
		// show information of newly selected region
		datablock &region = *selection.region;

		// collect information (Bauern, Silber, Pferde...)
		std::list<Info> info;

		// bla
		collectData(info, selection.region);

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else
	{
		clearLabels();

		tags.matrixsep->hide();
		createLabels("Keine Statistik", "", "", 0);
	}
}

long FXStatsInfos::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *state = (datafile::SelectionState*)ptr;

	// connected to a datafile list?
	if (!files)
		return 0;

	bool needUpdate = false;

	// any data changed, so need to update list?
	if (selection.fileChange != state->fileChange)
	{
		selection.fileChange = state->fileChange;
		selection.map = state->map;
		selection.activefaction = state->activefaction;

		needUpdate = true;
	}

	if (selection.selChange != state->selChange)
	{
		selection.selChange = state->selChange;

		if ((selection.selected & selection.REGION) != (state->selected & selection.REGION)
			|| (selection.selected & selection.REGION && selection.region != state->region))
			needUpdate = true;				// ignore changes that don't change selected region

		if ((selection.selected & selection.UNKNOWN_REGION) != (state->selected & selection.UNKNOWN_REGION))
			needUpdate = true;				// ignore changes that don't change selected region

		if (selection.selected & selection.UNKNOWN_REGION && (selection.sel_x != state->sel_x ||
			selection.sel_y != state->sel_y || selection.sel_plane != state->sel_plane))
			needUpdate = true;				// ignore changes that don't change selected region

		if ((selection.selected & selection.MULTIPLE_REGIONS) != (state->selected & selection.MULTIPLE_REGIONS)
			|| (selection.selected & selection.MULTIPLE_REGIONS && selection.regionsSelected != state->regionsSelected))
			needUpdate = true;

		selection.selected = state->selected;
		
		selection.region = state->region;
		selection.faction = state->faction;
		selection.building = state->building;
		selection.ship = state->ship;
		selection.unit = state->unit;

		selection.sel_x = state->sel_x;
		selection.sel_y = state->sel_y;
		selection.sel_plane = state->sel_plane;

		if (needUpdate) // expensive operation
			selection.regionsSelected = state->regionsSelected;
	}

	if (needUpdate)
		updateData();

	return 1;
}
