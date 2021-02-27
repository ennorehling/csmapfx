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
	files = NULL;
	show_description = false;

	// create layout
	tags.name = new FXLabel(this, "", terrainIcons[ data::TERRAIN_UNKNOWN], ICON_BEFORE_TEXT|LAYOUT_FILL_X);

	tags.matrixsep = new FXHorizontalSeparator(this, LAYOUT_FILL_X|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	tags.matrixsep->setBorderColor(getBorderColor());
	tags.matrixsep->hide();

	tags.matrixframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	tags.matrixframe->hide();

	tags.leftmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
	FXFrame *sep = new FXVerticalSeparator(tags.matrixframe,LAYOUT_FILL_Y|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	sep->setBorderColor(getBorderColor());
	tags.rightmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);

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

void FXRegionInfos::mapfiles(std::list<datafile> *f)
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

void FXRegionInfos::clearLabels()
{
	for (std::vector<FXLabel*>::iterator itor = tags.entries.begin(); itor != tags.entries.end(); itor++)
		delete *itor;

	tags.entries.clear();
}

void FXRegionInfos::createLabels(const FXString& name, const FXString& first_label, const FXString& second_label, int column)
{
	FXMatrix *matrix = (column==0) ? tags.leftmatrix : tags.rightmatrix;

	// create labels
	FXLabel *lname = new FXLabel(matrix, name, NULL, JUSTIFY_LEFT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
	FXLabel *lfirst = new FXLabel(matrix, first_label, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	FXLabel *lsecond = new FXLabel(matrix, second_label, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	lsecond->disable();
	lname->create(); lfirst->create(); lsecond->create();

	// put into list
	tags.entries.push_back(lname);
	tags.entries.push_back(lfirst);
	tags.entries.push_back(lsecond);
}

void FXRegionInfos::setInfo(const std::list<Info>& info)
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
		tags.matrixframe->show();
	}
	else
	{
		tags.matrixsep->hide();
		tags.matrixframe->hide();
	}
}

void FXRegionInfos::addEntry(std::list<Info>& info, FXString name, int value, int skill, int offset, FXString tip)
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

void FXRegionInfos::collectData(std::list<Info>& info, datablock::itor region)
{
	FXString Bauern = region->value("Bauern");
	FXString Silber = region->value("Silber");
	FXString Unterh = region->value("Unterh");
	FXString Rekruten = region->value("Rekruten");
	FXString Pferde = region->value("Pferde");

	if (!Bauern.empty()) addEntry(info, "Bauern", atoi(Bauern.text()), 0, 0, "Anzahl der Bauern");
	if (!Silber.empty()) addEntry(info, "Silber", atoi(Silber.text()), 0, 0, "Silbervorat der Bauern");
	if (!Unterh.empty()) addEntry(info, "Unterh.max", atoi(Unterh.text()), 0, 0, "Maximale Anzahl Silber, dass per Unterhalung eingenommen werden kann");
	if (!Rekruten.empty()) addEntry(info, "Rekruten", atoi(Rekruten.text()), 0, 0, FXString(L"Anzahl der m\u00f6glichen Rekruten"));
	if (!Pferde.empty()) addEntry(info, "Pferde", atoi(Pferde.text()), 0, 0, "Anzahl Pferde");

	int factionId = -1;
	bool myfaction = false;
	if (selection.map & selection.ACTIVEFACTION)
		factionId = selection.activefaction->info();

	FXint Personen = 0, Parteipersonen = 0;
	FXint Parteisilber = 0;

	datablock::itor end = files->front().blocks().end();
	datablock::itor block = region;
	for (block++; block != end && block->depth() > region->depth(); block++)
	{
		if (block->type() == datablock::TYPE_RESOURCE)
		{
			/*	RESOURCE 235898859
				"Eisen";type
				17;skill
				5;number	*/

			FXString type = block->value("type");
			FXString skillStr = block->value(datakey::TYPE_SKILL);
			FXString numberStr = block->value("number");
			FXString infotip = "Resource ";
				infotip += type;

			// don't double-count resources in REGION block
			if (FXString("Bauern|Silber|Pferde").find(type) != -1)
				continue;

            if (!type.empty())
			{
				FXint number = 0;
				FXint skill = 0;

				if (numberStr.length())
					number = atoi(numberStr.text());
				if (skillStr.length())
					skill = atoi(skillStr.text());

				addEntry(info, type, number, skill, 0, infotip);
			}
		}
		else if (block->type() == datablock::TYPE_UNIT)
		{
			myfaction = false;
			FXString id = block->value(datakey::TYPE_FACTION);
			if (!id.empty())
				if (factionId == atoi(id.text()))
					myfaction = true;

			FXString number = block->value(datakey::TYPE_NUMBER);
            if (!number.empty())
			{
				Personen += atoi(number.text());
				if (myfaction)
					Parteipersonen += atoi(number.text());
			}
		}
		else if (block->type() == datablock::TYPE_ITEMS)
		{
			// Silber der eigenen Partei zum Parteisilber zaehlen
			if (myfaction)
			{
				FXString silber = block->value("Silber");
				if (!silber.empty())
	                Parteisilber += atoi(silber.text());
			}
		}
	}

	if (Personen)
	{
		addEntry(info, "Personen", Personen, 0, 0, "Anzahl (sichtbarer) Personen in dieser Region");
	}
	if (Parteipersonen)
	{
		addEntry(info, L"Anh\u00e4nger", Parteipersonen, 0, 0, "Anzahl eigener Personen");
		addEntry(info, "Parteisilber", Parteisilber, 0, 0, "Das Silber, welches Personen der Partei in dieser Region besitzen");
	}
}

void FXRegionInfos::updateData()
{
	//if (files->empty())
		//return;

	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		FXString label;
		tags.name->setText(label.format("%lu Regionen", selection.regionsSelected.size()));
		tags.name->setIcon(terrainIcons[0]);				// Symbol fuer unbekanntes Terrain, da mehrere Regionen
		tags.name->setHelpText(label + " markiert");
		tags.name->setTipText(label + " markiert");

		// Beschreibungsfeld kann weg
		tags.desc->setText("");

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

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else if (selection.selected & selection.REGION)
	{
		// show information of newly selected region
		datablock &region = *selection.region;

		FXString name = region.value(datakey::TYPE_NAME);

		if (name.empty())
			name = region.terrainString();
		if (name.empty())
			name = "Unbekannt";

		FXString label;
		tags.name->setText(label.format("%s (%d,%d)", name.text(), region.x(), region.y()));

		label.format("%s von %s (%d,%d)", region.terrainString().text(), name.text(), region.x(), region.y());

		// Beschreibung an den Statusleisten-Text anfuegen.
		FXString description = region.value(datakey::TYPE_DESCRIPTION);
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
		std::list<Info> info;

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
		tags.name->setIcon(terrainIcons[ data::TERRAIN_UNKNOWN]);

		tags.matrixsep->hide();
		tags.matrixframe->hide();
		clearLabels();

		tags.desc->setText("");
	}
}

long FXRegionInfos::onMapChange(FXObject*, FXSelector, void* ptr)
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
