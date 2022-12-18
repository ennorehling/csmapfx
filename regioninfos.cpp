#include "main.h"
#include "fxhelper.h"
#include "regioninfos.h"
#include "symbols.h"

#include <algorithm>

// *********************************************************************************************************
// *** FXRegionInfos implementation

FXDEFMAP(FXRegionInfos) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,		FXRegionInfos::ID_UPDATE,				FXRegionInfos::onMapChange),

	FXMAPFUNC(SEL_COMMAND,		FXRegionInfos::ID_TOGGLEDESCRIPTION,	FXRegionInfos::onToggleDescription),
	FXMAPFUNC(SEL_UPDATE,		FXRegionInfos::ID_TOGGLEDESCRIPTION,	FXRegionInfos::onUpdateToggleDescription),

	FXMAPFUNC(SEL_UPDATE,		FXRegionInfos::ID_DESCRIPTION,			FXRegionInfos::onUpdateDescription),
};

FXIMPLEMENT(FXRegionInfos,FXVerticalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXRegionInfos::FXRegionInfos(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXVerticalFrame(p, opts, x,y,w,h, 0,0,0,0, 0,0)
{
	// set target etc.
	setTarget(tgt);
	setSelector(sel);

	// create/load images for terrain types
    for (int i = 0; i < data::TERRAIN_LAST; i++) {
        terrainIcons[i] = new FXGIFIcon(getApp(), data::terrain_icon(i), 0, IMAGE_ALPHAGUESS);
    }

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	// init variables
	mapFile = nullptr;
	show_description = false;

	// create layout
	tags.name = new FXLabel(this, "", terrainIcons[ data::TERRAIN_UNKNOWN], ICON_BEFORE_TEXT|LAYOUT_FILL_X);

	tags.matrixsep = new FXHorizontalSeparator(this, LAYOUT_FILL_X|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	tags.matrixsep->setBorderColor(getBorderColor());
	tags.matrixsep->hide();

	tags.matrixframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	tags.matrixframe->hide();

	tags.leftmatrix = new FXMatrix(tags.matrixframe,2,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
	FXFrame *sep = new FXVerticalSeparator(tags.matrixframe,LAYOUT_FILL_Y|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	sep->setBorderColor(getBorderColor());
	tags.rightmatrix = new FXMatrix(tags.matrixframe,2,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);

	tags.descsep = new FXHorizontalSeparator(this, LAYOUT_FILL_X|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	tags.descsep->setBorderColor(getBorderColor());

	tags.desc = new FXText(this, this,ID_DESCRIPTION, LAYOUT_FILL_X|TEXT_READONLY|TEXT_WORDWRAP|HSCROLLER_NEVER);
	tags.desc->disable();
}

void FXRegionInfos::create()
{
	FXVerticalFrame::create();

	// create terrain icons for region list
	for (int i = 0; i <  data::TERRAIN_LAST; i++)
		terrainIcons[i]->create();
}

FXRegionInfos::~FXRegionInfos()
{
	// terrain icons for region list
	for (int i = 0; i <  data::TERRAIN_LAST; i++)
		delete terrainIcons[i];
}

void FXRegionInfos::setMapFile(datafile *f)
{
    mapFile = f;
}

inline FXString thousandsPoints(FXulong value, bool plusSign = false)
{
	FXString str = FXStringVal(value);

	int n = 0;
	for (int i = str.length(); i > 0; i--,n++)
		if (n && !(n%3))
			str.insert(i, '.');

    if (plusSign) {
        str.prepend("+");
    }

	return str;
}

void FXRegionInfos::clearLabels()
{
	for (std::vector<FXLabel*>::iterator itor = tags.entries.begin(); itor != tags.entries.end(); itor++)
		delete *itor;

	tags.entries.clear();
}

void FXRegionInfos::createLabels(const FXString& name, const FXString& label, int column)
{
	FXMatrix *matrix = (column==0) ? tags.leftmatrix : tags.rightmatrix;

	// create labels
	FXLabel *lname = new FXLabel(matrix, name, NULL, JUSTIFY_LEFT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	FXLabel *lfirst = new FXLabel(matrix, label, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	lname->create();
    lfirst->create();

	// put into list
	tags.entries.push_back(lname);
	tags.entries.push_back(lfirst);
}

void FXRegionInfos::setInfo(const std::vector<Info>& info)
{
	clearLabels();

	int number = info.size();
	int index = 0;
	for (std::vector<Info>::const_iterator itor = info.begin(); itor != info.end(); itor++, index++)
	{
		FXString value = thousandsPoints(itor->value);
		FXString offset = "";
		if (itor->skill)
			value += " (" + FXStringVal(itor->skill) + ")";

		FXString infotip = "\t" + value;
		createLabels(itor->name+"\t"+itor->tip, value+infotip, 2*index>=number);
	}

	if (number)
	{
		tags.matrixsep->show();
		tags.matrixframe->show();
	}
	else
	{
		tags.matrixsep->hide();
		tags.matrixframe->hide();
	}
}

void FXRegionInfos::addEntry(std::vector<Info>& info, FXString name, FXulong value, int skill, FXString tip)
{
    std::vector<Info>::iterator itor;
	for (itor = info.begin(); itor != info.end(); itor++)
		if (itor->name == name)
		{
			itor->value += value;
            if (skill < itor->skill) {
                itor->skill = skill;
            }
			break;
		}

	if (itor == info.end())
		info.push_back(Info(name, tip, value, skill));
}

void FXRegionInfos::collectData(std::vector<Info>& info, datablock::itor region)
{
	FXint Bauern = region->valueInt("Bauern", -1);
	FXint Silber = region->valueInt("Silber", -1);
    FXint Unterh = region->valueInt("Unterh", -1);
    FXint Rekruten = region->valueInt("Rekruten", -1);
    FXint Pferde = region->valueInt("Pferde", -1);

	if (Bauern >= 0) addEntry(info, "Bauern", Bauern, 0, "Anzahl der Bauern");
	if (Silber >= 0) addEntry(info, "Silber", Silber, 0, "Silbervorrat der Bauern");
	if (Unterh >= 0) addEntry(info, "Unterh.max", Unterh, 0, "Maximale Anzahl Silber, dass per Unterhaltung eingenommen werden kann");
	if (Rekruten >= 0) addEntry(info, "Rekruten", Rekruten, 0, FXString(L"Anzahl der m\u00f6glichen Rekruten"));
	if (Pferde >= 0) addEntry(info, "Pferde", Pferde, 0, "Anzahl Pferde");

	int factionId = 0;
	bool myfaction = false;
	if (mapFile)
		factionId = mapFile->getFactionId();

	FXint Personen = 0, Parteipersonen = 0;
	FXint Parteisilber = 0;

	datablock::itor end = mapFile->blocks().end();
	datablock::itor block = region;
	for (block++; block != end && block->depth() > region->depth(); block++)
	{
		if (block->type() == block_type::TYPE_RESOURCE)
		{
			/*	RESOURCE 235898859
				"Eisen";type
				17;skill
				5;number	*/

			const FXString& type = block->value(TYPE_RESOURCE_TYPE);

			// don't double-count resources in REGION block
			if (FXString("Bauern|Silber|Pferde").find(type) != -1)
				continue;

            if (!type.empty())
			{
				FXint skill = block->valueInt(TYPE_RESOURCE_SKILL);
				FXint number = block->valueInt(TYPE_RESOURCE_COUNT);
                FXString infotip = "Resource " + type;

				addEntry(info, type, number, skill, infotip);
			}
		}
		else if (block->type() == block_type::TYPE_UNIT)
		{
            const datablock* unitPtr = &*block;
			myfaction = false;
            int id = mapFile->getFactionIdForUnit(unitPtr);
            if (factionId == id) {
                myfaction = true;
            }

			int number = unitPtr->valueInt(TYPE_NUMBER, -1);
            if (number > 0)
			{
				Personen += number;
				if (myfaction)
					Parteipersonen += number;
			}
		}
		else if (block->type() == block_type::TYPE_ITEMS)
		{
			// Silber der eigenen Partei zum Parteisilber zaehlen
			if (myfaction)
			{
				int silber = block->valueInt(TYPE_SILVER, -1);
				if (silber > 0)
	                Parteisilber += silber;
			}
		}
	}

	if (Personen)
	{
		addEntry(info, "Personen", Personen, 0, "Anzahl (sichtbarer) Personen in dieser Region");
	}
	if (Parteipersonen)
	{
		addEntry(info, L"Anh\u00e4nger", Parteipersonen, 0, "Anzahl eigener Personen");
		addEntry(info, "Parteisilber", Parteisilber, 0, "Das Silber, welches Personen der Partei in dieser Region besitzen");
	}
}

void FXRegionInfos::updateData()
{
	//if (files->empty())
		//return;

	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		FXString label;
		tags.name->setText(label.format("%zu Regionen", selection.regionsSelected.size()));
		tags.name->setIcon(terrainIcons[0]);				// Symbol fuer unbekanntes Terrain, da mehrere Regionen
		tags.name->setHelpText(label + " markiert");
		tags.name->setTipText(label + " markiert");

		// Beschreibungsfeld kann weg
		tags.desc->setText("");

        std::vector<Info> info;

		// collect infos
		for (std::set<datablock*>::iterator itor = selection.regionsSelected.begin(); itor != selection.regionsSelected.end(); itor++)
		{
            datablock::itor region;
            if (mapFile->getRegion(region, (*itor)->x(), (*itor)->y(), (*itor)->info()))
            {
				collectData(info, region);
			}
		}

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else if (selection.selected & selection.REGION)
	{
		// show information of newly selected region
		datablock &region = *selection.region;

		FXString name = region.value(TYPE_NAME);

		if (name.empty())
			name = region.terrainString();
		if (name.empty())
			name = "Unbekannt";

		FXString label;
		tags.name->setText(label.format("%s (%d,%d)", name.text(), region.x(), region.y()));

		label.format("%s von %s (%d,%d)", region.terrainString().text(), name.text(), region.x(), region.y());

		// Beschreibung an den Statusleisten-Text anfuegen.
		FXString description = region.value(TYPE_DESCRIPTION);
		if (!description.empty())
		{
			FXchar lastc = description.right(1)[0];
			if (lastc != '.' && lastc != '!' && lastc != '?' && lastc != ')')
				description += ".";

			label += ". " + description;
		}
		tags.name->setHelpText(label);

		// Beschreibung ins Beschreibungsfeld
		if (show_description)
			tags.desc->setText(description);
		else
			tags.desc->setText("");

		// Beschreibung als Tooltip
		for (label.clear(); description.length();)
		{
			if (label.length())
				label += "\n";
			label += description.before(' ', 7);
			description = description.after(' ', 7);
		}
		tags.name->setTipText(label);

		// select terrain image
		FXint terrain = region.terrain();
		tags.name->setIcon(terrainIcons[terrain]);

		// collect information (Bauern, Silber, Pferde...)
        std::vector<Info> info;

		// Bauern, Silber, Unterhaltung (Unterh), Rekruten, Parteisilber,
		// Pferde, Laen, Eisen, Baeume, Schoesslinge
		collectData(info, selection.region);

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else
	{
		if (selection.selected & selection.UNKNOWN_REGION)
		{
			FXString label;
			tags.name->setText(label.format("Unbekannt (%d,%d)", selection.sel_x, selection.sel_y));
		}
		else
			tags.name->setText("");

		tags.name->setHelpText("");
		tags.name->setIcon(terrainIcons[data::TERRAIN_UNKNOWN]);

		tags.matrixsep->hide();
		tags.matrixframe->hide();
		clearLabels();

		tags.desc->setText("");
	}
}

long FXRegionInfos::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

	bool needUpdate = false;

	// any data changed, so need to update list?
	if (selection.fileChange != pstate->fileChange)
	{
		selection.fileChange = pstate->fileChange;
		needUpdate = true;
	}

	if (selection.selChange != pstate->selChange)
	{
		if ((selection.selected & selection.REGION) != (pstate->selected & selection.REGION)
			|| (selection.selected & selection.REGION && selection.region != pstate->region))
			needUpdate = true;				// ignore changes that don't change selected region

		if ((selection.selected & selection.UNKNOWN_REGION) != (pstate->selected & selection.UNKNOWN_REGION))
			needUpdate = true;				// ignore changes that don't change selected region

		if (selection.selected & selection.UNKNOWN_REGION && (selection.sel_x != pstate->sel_x ||
			selection.sel_y != pstate->sel_y || selection.sel_plane != pstate->sel_plane))
			needUpdate = true;				// ignore changes that don't change selected region

		if ((selection.selected & selection.MULTIPLE_REGIONS) != (pstate->selected & selection.MULTIPLE_REGIONS)
			|| (selection.selected & selection.MULTIPLE_REGIONS && selection.regionsSelected != pstate->regionsSelected))
			needUpdate = true;

		selection = *pstate;
        if (needUpdate) // expensive operation
			selection.regionsSelected = pstate->regionsSelected;
	}

	if (needUpdate)
		updateData();

	return 1;
}

long FXRegionInfos::onToggleDescription(FXObject*,FXSelector,void*)
{
	show_description = !show_description;
	updateData();
	return 1;
}

long FXRegionInfos::onUpdateToggleDescription(FXObject* sender,FXSelector,void*)
{
	sender->handle(this, FXSEL(SEL_COMMAND, show_description?ID_CHECK:ID_UNCHECK), NULL);
	return 1;
}

long FXRegionInfos::onUpdateDescription(FXObject* sender,FXSelector,void*)
{
	if (tags.desc->getText().empty())
	{
		if (tags.desc->shown())
		{
			tags.desc->hide();
			tags.descsep->hide();
			tags.matrixframe->recalc();
		}
	}
	else
	{
		tags.desc->setCursorPos(tags.desc->getText().length());
		tags.desc->setVisibleRows(1+std::min(4,tags.desc->getCursorRow()));
		if (!tags.desc->shown())
		{
			tags.desc->show();
			tags.descsep->show();
			tags.matrixframe->recalc();
		}
	}
	return 1;
}
