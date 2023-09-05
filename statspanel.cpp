#include "fxhelper.h"
#include "statspanel.h"

#include <cstdio>

#include <algorithm>
#include <exception>

// *********************************************************************************************************
// *** FXStatsInfos implementation

FXDEFMAP(FXStatsPanel) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,		FXStatsPanel::ID_UPDATE,				FXStatsPanel::onMapChange), 
}; 

FXIMPLEMENT(FXStatsPanel,FXVerticalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXStatsPanel::FXStatsPanel(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXVerticalFrame(p, opts, x,y,w,h, 0,0,0,0, 0,0)
{
	// set target etc.
	setTarget(tgt);
	setSelector(sel);

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	// create layout
	tags.matrixframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);

	tags.leftmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
	tags.matrixsep = new FXVerticalSeparator(tags.matrixframe,LAYOUT_FILL_Y|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	tags.matrixsep->setBorderColor(getBorderColor());
	tags.matrixsep->hide();
	tags.rightmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
}

void FXStatsPanel::create()
{
	FXVerticalFrame::create();

	createLabels("Keine Statistik", "", "", 0);
}

FXStatsPanel::~FXStatsPanel()
{
}

void FXStatsPanel::setMapFile(std::shared_ptr<datafile>& f)
{
    if (!f) {
        selection.selected = 0;
    }
    mapFile = f;
}

inline FXString thousandsPoints(FXlong value, bool plusSign = false)
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

void FXStatsPanel::clearLabels()
{
	for (std::vector<FXLabel*>::iterator itor = tags.entries.begin(); itor != tags.entries.end(); itor++)
		delete *itor;

	tags.entries.clear();
}

void FXStatsPanel::createLabels(const FXString& name, const FXString& one, const FXString& two, int column)
{
	FXMatrix *matrix = (column==0) ? tags.leftmatrix : tags.rightmatrix;

	// create labels
	FXLabel *lname = new FXLabel(matrix, name, NULL, JUSTIFY_LEFT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	FXLabel *lfirst = new FXLabel(matrix, one, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	FXLabel *lsecond = new FXLabel(matrix, two, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	lsecond->disable();
	lname->create(); lfirst->create(); lsecond->create();

	// put into list
	tags.entries.push_back(lname);
	tags.entries.push_back(lfirst);
	tags.entries.push_back(lsecond);
}

void FXStatsPanel::setInfo(const std::vector<Info>& info)
{
	clearLabels();

	int number = info.size();
	int index = 0;
	for (std::vector<Info>::const_iterator itor = info.begin(); itor != info.end(); itor++, index++)
	{
		FXString value = thousandsPoints(itor->value);
		FXString offset = "";
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

void FXStatsPanel::addEntry(std::vector<Info>& info, FXString name, FXlong value, FXString tip)
{
    std::vector<Info>::iterator itor;
	for (itor = info.begin(); itor != info.end(); itor++)
		if (itor->name == name)
		{
			itor->value += value;
			break;
		}

	if (itor == info.end())
		info.push_back(Info(name, tip, value));
}

void FXStatsPanel::collectData(std::vector<Info>& info, const datablock::itor& region)
{
	int WorkPerRegion[] =	// Arbeitsplaetze pro Regionstyp
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

    int peasants = region->valueInt(TYPE_PEASANTS);
    int trees = region->valueInt(TYPE_TREES);
    int saplings = region->valueInt(TYPE_SAPLINGS);
    int salary = region->valueInt(TYPE_SALARY);

    datablock::itor end = mapFile->blocks().end();
    for (datablock::itor res = std::next(region); res != end && res->depth() > region->depth(); ++res)
    {
        if (res->type() != block_type::TYPE_RESOURCE) continue;
        const FXString& value = res->value(TYPE_RESOURCE_TYPE);
        key_type type = static_cast<key_type>(datakey::parseType(value, block_type::TYPE_RESOURCE) & TYPE_MASK);
        if (type == TYPE_PEASANTS) {
            peasants = res->valueInt(TYPE_RESOURCE_COUNT);
        }
        else if (type == TYPE_TREES) {
            trees = res->valueInt(TYPE_RESOURCE_COUNT);
        }
        else if (type == TYPE_SAPLINGS) {
            saplings = res->valueInt(TYPE_RESOURCE_COUNT);
        }
    }

	int workstations = WorkPerRegion[region->terrain()];
	workstations -= 8 * trees;
	workstations -= 4 * saplings;
	if (workstations < 0) workstations = 0;

    int earnings = std::min(workstations, peasants);
    if (salary) {
        addEntry(info, "Lohn", salary, FXString(L"Lohn f\u00fcr Arbeit"));
        earnings = salary * earnings - 10 * peasants;
    }
	if (earnings) addEntry(info, "Bauernertrag", earnings, FXString(L"\u00dcberschuss der Bauerneinnahmen pro Runde. Kann sicher eingetrieben werden."));

    att_region* att = static_cast<att_region*>(region->attachment());
    if (att) {
        // display income messages
        if (att->income[0]) addEntry(info, "Einnahmen", att->income[0], FXString(L"Einnahmen durch sonstige T\u00e4tigkeiten."));
        if (att->income[1]) addEntry(info, "Unterhaltung", att->income[1], "Einnahmen durch Unterhaltung");
        if (att->income[2]) addEntry(info, "Steuern", att->income[2], "Einnahmen durch Steuern");
        if (att->income[3]) addEntry(info, "Handel", att->income[3], "Einnahmen durch Handel");
        if (att->income[4]) addEntry(info, "Handelsabgaben", att->income[4], "Einnahmen durch Handel fremder Parteien");
        if (att->income[5]) addEntry(info, "Diebstahl", att->income[5], "Einnahmen durch Diebstahl");
        if (att->income[6]) addEntry(info, "Zauberei", att->income[6], "Einnahmen durch Zauberei");

        // display learncost messages
        if (att->learncost) addEntry(info, "Lernkosten", -att->learncost, "Ausgaben zum Lernen von Talenten");
    }
}

void FXStatsPanel::updateData()
{
	if (!selection.regionsSelected.empty())
	{
		std::vector<Info> info;

		// collect infos
		for (auto selected : selection.regionsSelected)
		{
            datablock::itor region;
            if (mapFile->getRegion(region, *selected)) {
                collectData(info, region);
            }
		}

		// "Lohn" makes no sense here. Remove.
		for (std::vector<Info>::iterator itor = info.begin(); itor != info.end(); itor++)
		{
            if (itor->name == "Lohn") {
                info.erase(itor);
                break;
            }
		}

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else if (selection.selected & selection.REGION)
	{
		// collect information (Bauern, Silber, Pferde...)
        std::vector<Info> info;

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

long FXStatsPanel::onMapChange(FXObject*, FXSelector, void* ptr)
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
            || (selection.selected & selection.REGION && selection.region != pstate->region)) {
            /* the cursor has moved */
            needUpdate = true;				// ignore changes that don't change selected region
        }
        else if ((selection.selected & selection.UNKNOWN_REGION) != (pstate->selected & selection.UNKNOWN_REGION)) {
            /* we did not, or do now, have a cursor position off the map */
            needUpdate = true;				// ignore changes that don't change selected region
        }
        else if (selection.selected & selection.UNKNOWN_REGION && (selection.sel_x != pstate->sel_x ||
            selection.sel_y != pstate->sel_y || selection.sel_plane != pstate->sel_plane))
        {
            /* our off-map cursor position has changed. */
            needUpdate = true;				// ignore changes that don't change selected region
        }
        else if (selection.regionsSelected != pstate->regionsSelected) {
            /* the selection has groen or shrunk */
            needUpdate = true;
        }

        selection = *pstate;
	}

    if (needUpdate) {
        if (!selection.regionsSelected.empty()) {
            getApp()->beginWaitCursor();
        }
        updateData();
        if (!selection.regionsSelected.empty()) {
            getApp()->endWaitCursor();
        }
    }
	return 1;
}
