#define _CRT_SECURE_NO_WARNINGS // for sscanf

#include "main.h"
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

	// init variables
	mapFile = nullptr;

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

void FXStatsPanel::setMapFile(datafile *f)
{
    if (!f) {
        selection.selected = 0;
    }
    mapFile = f;
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

void FXStatsPanel::addEntry(std::vector<Info>& info, FXString name, int value, FXString tip)
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

void FXStatsPanel::collectData(std::vector<Info>& info, datablock::itor region)
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

    int peasants = region->valueInt("Bauern");

	int workstations = WorkPerRegion[region->terrain()];
	workstations -= 8 * region->valueInt("Baeume");
	workstations -= 4 * region->valueInt("Schoesslinge");
	if (workstations < 0) workstations = 0;

	int Lohn = region->valueInt("Lohn");
    int earnings = std::min(workstations, peasants);
    if (Lohn) {
        addEntry(info, "Lohn", Lohn, FXString(L"Lohn f\u00fcr Arbeit"));
        earnings = Lohn * earnings - 10 * peasants;
    }
	if (earnings) addEntry(info, "Bauernertrag", earnings, FXString(L"\u00dcberschuss der Bauerneinnahmen pro Runde. Kann sicher eingetrieben werden."));

    if (mapFile) {
        // search income messages for this region
        datablock::itor faction, end = mapFile->blocks().end();
        datablock::itor block = faction = mapFile->activefaction();
        if (block != end) {
            block++;
        }
    
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
            if (block->type() == block_type::TYPE_MESSAGE)
            {
                int type = block->valueInt(TYPE_MSG_TYPE);

                if (type != 771334452 && type != 443066738)
                    continue;

                const datakey::list_type &list = block->data();

                datakey::list_type::const_iterator reg = list.end();
                int amount = 0, mode = 0;

                for (datakey::list_type::const_iterator itor = list.begin(); itor != list.end(); itor++)
                {
                    key_type key = itor->type();
                    if (key == key_type::TYPE_MSG_REGION) {
                        reg = itor;
                    }
                    else if (key == key_type::TYPE_MSG_AMOUNT) {
                        amount = itor->getInt();
                    }
                    else if (key == key_type::TYPE_MSG_COST) {
                        amount = itor->getInt();
                    }
                    else if (key == key_type::TYPE_MSG_MODE) {
                        mode = itor->getInt();
                    }
                }

                if (reg == list.end())
                    continue;

                FXString location = reg->value();
                int x, y, z;
                int num = sscanf(location.text(), "%d %d %d", &x, &y, &z);
                if (num < 2) {
                    continue;
                }
                else if (num < 3) {
                    z = 0;
                }

                if (region->x() != x || region->y() != y || region->info() != z)
                    continue;

                if (mode < 0 || mode > 6)
                    mode = 0;

                if (type == 771334452)	// Einnahmen
                    income[mode] += amount;
                else if (type == 443066738)	// Ausgaben fuer teure Talente (+Akademie)
                    learncost += amount;
            }
        }
        // display income messages
        if (income[0]) addEntry(info, "Einnahmen", income[0], FXString(L"Einnahmen durch sonstige T\u00e4tigkeiten."));
        if (income[1]) addEntry(info, "Unterhaltung", income[1], "Einnahmen durch Unterhaltung");
        if (income[2]) addEntry(info, "Steuern", income[2], "Einnahmen durch Steuern");
        if (income[3]) addEntry(info, "Handel", income[3], "Einnahmen durch Handel");
        if (income[4]) addEntry(info, "Handelsabgaben", income[4], "Einnahmen durch Handel fremder Parteien");
        if (income[5]) addEntry(info, "Diebstahl", income[5], "Einnahmen durch Diebstahl");
        if (income[6]) addEntry(info, "Zauberei", income[6], "Einnahmen durch Zauberei");

        // display learncost messages
        if (learncost) addEntry(info, "Lernkosten", -learncost, "Ausgaben zum Lernen von Talenten");
    }
}

void FXStatsPanel::updateData()
{
	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		std::vector<Info> info;

		// collect infos
		datablock::itor notfound = mapFile->blocks().end();
		for (std::set<datablock*>::iterator itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
		{
            datablock::itor region;
            if (mapFile->getRegion(region, (*itor)->x(), (*itor)->y(), (*itor)->info())) {
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
        if ((selection.selected & selection.MULTIPLE_REGIONS) != (pstate->selected & selection.MULTIPLE_REGIONS)) {
            /* we went from having a selection to not, or vice versa */
            needUpdate = true;
        }
        else if ((selection.selected & selection.MULTIPLE_REGIONS) || (pstate->selected & selection.MULTIPLE_REGIONS)) {
            if (selection.regionsSelected.size() != pstate->regionsSelected.size()) {
                /* the selection has groen or shrunk */
                needUpdate = true;
            }
        }
        else if ((selection.selected & selection.REGION) != (pstate->selected & selection.REGION)
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

        selection = *pstate;

		if (needUpdate && (selection.selected & selection.MULTIPLE_REGIONS)) // expensive operation
			selection.regionsSelected = pstate->regionsSelected;
	}

    if (needUpdate) {
        if (selection.selected & selection.MULTIPLE_REGIONS) {
            getApp()->beginWaitCursor();
        }
        updateData();
        if (selection.selected & selection.MULTIPLE_REGIONS) {
            getApp()->endWaitCursor();
        }
    }
	return 1;
}
