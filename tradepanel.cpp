#include "fxhelper.h"
#include "tradepanel.h"
#include "symbols.h"

// *********************************************************************************************************
// *** FXTradeInfos implementation

FXDEFMAP(FXTradePanel) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND, 		FXTradePanel::ID_UPDATE, 				FXTradePanel::onMapChange), 
}; 

FXIMPLEMENT(FXTradePanel, FXVerticalFrame, MessageMap, ARRAYNUMBER(MessageMap))

FXTradePanel::FXTradePanel(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h)
		: FXVerticalFrame(p, opts, x, y, w, h, 0, 0, 0, 0, 0, 0)
{
	// set target etc.
	setTarget(tgt);
	setSelector(sel);

	// set style
	setBorderColor(getApp()->getShadowColor());
	setFrameStyle(FRAME_LINE);

	// create layout
	tags.topmatrix = new FXMatrix(this, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0);

	tags.matrixsep = new FXHorizontalSeparator(this, LAYOUT_FILL_X|SEPARATOR_LINE, 0, 0, 0, 0, 0, 0, 0, 0);
	tags.matrixsep->setBorderColor(getBorderColor());
	tags.matrixsep->hide();

	tags.matrixframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	tags.matrixframe->hide();

	tags.leftmatrix = new FXMatrix(tags.matrixframe, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0);
	FXFrame *sep = new FXVerticalSeparator(tags.matrixframe, LAYOUT_FILL_Y|SEPARATOR_LINE, 0, 0, 0, 0, 0, 0, 0, 0);
	sep->setBorderColor(getBorderColor());
	tags.rightmatrix = new FXMatrix(tags.matrixframe, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0);
}

void FXTradePanel::create()
{
	FXVerticalFrame::create();

	createLabels("Kein Handel", "", -1);	// -1 == topmatrix
}

FXTradePanel::~FXTradePanel()
{
    clearLabels();
}

void FXTradePanel::setMapFile(std::shared_ptr<datafile>& f)
{
    if (!f) selection.selected = 0;
    mapFile = f;
}

inline FXString thousandsPoints(FXint value, bool plusSign = false)
{
	FXString str = FXStringVal((value<0)?-value:value);

	int n = 0;
	for (int i = str.length(); i > 0; i--, n++)
		if (n && !(n%3))
			str.insert(i, '.');

	if (value<0)
		str = "-" + str;
	else if (value && plusSign)
		str = "+" + str;

	return str;
}

void FXTradePanel::clearLabels()
{
	for (std::vector<FXLabel*>::iterator itor = tags.entries.begin(); itor != tags.entries.end(); itor++)
		delete *itor;

	tags.entries.clear();
}

void FXTradePanel::createLabels(const FXString& name, const FXString& info, int column)
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
	FXLabel *lfirst = new FXLabel(matrix, info, NULL, JUSTIFY_RIGHT|LAYOUT_RIGHT);
	lname->create(); lfirst->create();

	// put into list
	tags.entries.push_back(lname);
	tags.entries.push_back(lfirst);
}

void FXTradePanel::setInfo(const std::vector<Info>& info)
{
	int number = info.size();
	int index = 0;
	for (std::vector<Info>::const_iterator itor = info.begin(); itor != info.end(); itor++, index++)
	{
		FXString value = thousandsPoints(itor->value);
		FXString offset = "";
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

void FXTradePanel::addEntry(std::vector<Info>& info, FXString name, int value, FXString tip)
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

void FXTradePanel::collectData(std::vector<Info>& info, const datablock::itor& region)
{
	// search prices block of this region
	datablock::itor block;
	if (mapFile->getChild(block, region, block_type::TYPE_PRICES))
	{
		/*	PREISE
			96;Balsam		// wird angekauft
			-5;Seide		// wird verkauft
		*/

		for (datakey::list_type::const_iterator goods = block->data().begin(); goods != block->data().end(); ++goods)
			if (goods->key().length() && goods->value().length())
			{
				FXint price = atoi(goods->value().text());
				if (price >= 0)
					addEntry(info, goods->key(), price, "Verkaufspreis " + goods->key());
				else
				{
					price = -price;
					createLabels("Einkaufspreis " + goods->key(), thousandsPoints(price), -1);	// -1 == topmatrix
				}
			}
	}
}

void FXTradePanel::updateData()
{
    if (!selection.regionsSelected.empty())
	{
		tags.matrixsep->hide();
		tags.matrixframe->hide();
		clearLabels();

		createLabels("Kein Handel", "", -1);	// -1 == topmatrix
	}
	else if (selection.selected & selection.REGION)
	{
		clearLabels();

		// show information of newly selected region
		datablock &region = *selection.region;
		
		FXString peasants = region.value(key_type::TYPE_PEASANTS);
		FXint goods_at_price = 0;

		if (!peasants.empty())
			goods_at_price = atoi(peasants.text()) / 100;	// one unit of goods for every 100 peasants

		// -1 == topmatrix
		createLabels(FXString(L"Luxusg\u00fcter zum angegebenen Preis"), thousandsPoints(goods_at_price), -1);

		// collect information about luxury goods
		std::vector<Info> info;
		collectData(info, selection.region);

		// apply information entries (Bauern, Silber, Pferde...)
		setInfo(info);
	}
	else
	{
		tags.matrixsep->hide();
		tags.matrixframe->hide();
		clearLabels();

		createLabels("Kein Handel", "", -1);	// -1 == topmatrix
	}
}

long FXTradePanel::onMapChange(FXObject*, FXSelector, void* ptr)
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
			needUpdate = true;				// ignore changes that don't change selected region

		else if (selection.regionsSelected != pstate->regionsSelected)
			needUpdate = true;

		selection = *pstate;
	}

	if (needUpdate)
		updateData();

	return 1;
}
