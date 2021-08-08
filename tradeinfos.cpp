#include "main.h"
#include "fxhelper.h"
#include "tradeinfos.h"
#include "symbols.h"

// *********************************************************************************************************
// *** FXTradeInfos implementation

FXDEFMAP(FXTradeInfos) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,		FXTradeInfos::ID_UPDATE,				FXTradeInfos::onMapChange), 
}; 

FXIMPLEMENT(FXTradeInfos,FXVerticalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXTradeInfos::FXTradeInfos(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXVerticalFrame(p, opts, x,y,w,h, 0,0,0,0, 0,0)
{
	// set target etc.
	setTarget(tgt);
	setSelector(sel);

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	// init variables
	mapFile = NULL;

	// create layout
	tags.topmatrix = new FXMatrix(this,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);

	tags.matrixsep = new FXHorizontalSeparator(this, LAYOUT_FILL_X|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	tags.matrixsep->setBorderColor(getBorderColor());
	tags.matrixsep->hide();

	tags.matrixframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	tags.matrixframe->hide();

	tags.leftmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
	FXFrame *sep = new FXVerticalSeparator(tags.matrixframe,LAYOUT_FILL_Y|SEPARATOR_LINE, 0,0,0,0, 0,0,0,0);
	sep->setBorderColor(getBorderColor());
	tags.rightmatrix = new FXMatrix(tags.matrixframe,3,MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,2, 0,0);
}

void FXTradeInfos::create()
{
	FXVerticalFrame::create();

	createLabels("Kein Handel", "", "", -1);	// -1 == topmatrix
}

FXTradeInfos::~FXTradeInfos()
{
}

void FXTradeInfos::setMapFile(datafile *f)
{
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

void FXTradeInfos::clearLabels()
{
	for (std::vector<FXLabel*>::iterator itor = tags.entries.begin(); itor != tags.entries.end(); itor++)
		delete *itor;

	tags.entries.clear();
}

void FXTradeInfos::createLabels(const FXString& name, const FXString& one, const FXString& two, int column)
{
	FXMatrix *matrix = NULL;
	
	if (column == -1)
		matrix = tags.topmatrix;
	else if (column == 0)
		matrix = tags.leftmatrix;
	else if (column == 1)
		matrix = tags.rightmatrix;

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

void FXTradeInfos::setInfo(const std::list<Info>& info)
{
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

void FXTradeInfos::addEntry(std::list<Info>& info, FXString name, int value, int skill, int offset, FXString tip)
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

void FXTradeInfos::collectData(std::list<Info>& info, datablock::itor region)
{
	// search prices block of this region
	datablock::itor end = mapFile->blocks().end();
	datablock::itor block = region;
	for (block++; block != end && block->depth() > region->depth(); block++)
		if (block->type() == datablock::TYPE_PRICES)
			break;				// found

	// found PREISE block?
	if (block != end && block->type() == datablock::TYPE_PRICES)
	{
		/*	PREISE
			96;Balsam		// wird angekauft
			-5;Seide		// wird verkauft
		*/

		for (datakey::itor goods = block->data().begin(); goods != block->data().end(); goods++)
			if (goods->key().length() && goods->value().length())
			{
				FXint price = atoi(goods->value().text());
				FXint offset = 0;

				if (price >= 0)
					addEntry(info, goods->key(), price, 0, offset, "Verkaufspreis " + goods->key());
				else
				{
					price = -price;
					createLabels("Einkaufspreis "+goods->key(), thousandsPoints(price), "", -1);	// -1 == topmatrix
				}
			}
	}
}

void FXTradeInfos::updateData()
{
	//if (files->empty())
		//return;

	if (selection.selected & selection.MULTIPLE_REGIONS)
	{
		tags.matrixsep->hide();
		tags.matrixframe->hide();
		clearLabels();

		createLabels("Kein Handel", "", "", -1);	// -1 == topmatrix
	}
	else if (selection.selected & selection.REGION)
	{
		clearLabels();

		// show information of newly selected region
		datablock &region = *selection.region;
		
		FXString peasants = region.value("Bauern");
		FXint goods_at_price = 0;

		if (!peasants.empty())
			goods_at_price = atoi(peasants.text()) / 100;	// one unit of goods for every 100 peasants

		// -1 == topmatrix
		createLabels(FXString(L"Luxusg\u00fcter zum angegebenen Preis"), thousandsPoints(goods_at_price), "", -1);

		// collect information about luxury goods
		std::list<Info> info;
		collectData(info, selection.region);

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else
	{
		tags.matrixsep->hide();
		tags.matrixframe->hide();
		clearLabels();

		createLabels("Kein Handel", "", "", -1);	// -1 == topmatrix
	}
}

long FXTradeInfos::onMapChange(FXObject*, FXSelector, void* ptr)
{
	datafile::SelectionState *state = (datafile::SelectionState*)ptr;

	// connected to a datafile list?
	if (!mapFile)
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

		if ((selection.selected & selection.MULTIPLE_REGIONS) != (state->selected & selection.MULTIPLE_REGIONS)
			|| (selection.selected & selection.MULTIPLE_REGIONS && selection.regionsSelected != state->regionsSelected))
			needUpdate = true;

		selection.selected = state->selected;
		
		selection.region = state->region;
		selection.faction = state->faction;
		selection.building = state->building;
		selection.ship = state->ship;
		selection.unit = state->unit;

		if (needUpdate) // expensive operation
			selection.regionsSelected = state->regionsSelected;
	}

	if (needUpdate)
		updateData();

	return 1;
}
