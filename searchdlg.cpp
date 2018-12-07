#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "searchdlg.h"
#include "symbols.h"

#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>

// *********************************************************************************************************
// *** FXMessages implementation

FXDEFMAP(FXSearchDlg) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,				FXSearchDlg::ID_CLOSE,					FXSearchDlg::onCmdHide),
	
	FXMAPFUNC(SEL_FOCUSIN,				0,										FXSearchDlg::onFocusIn),

	FXMAPFUNC(SEL_COMMAND,				FXSearchDlg::ID_SEARCH,					FXSearchDlg::onSearch),
	FXMAPFUNC(SEL_CHANGED,				FXSearchDlg::ID_SEARCH,					FXSearchDlg::onChangedSearch),
	FXMAPFUNC(SEL_UPDATE,				FXSearchDlg::ID_SEARCH,					FXSearchDlg::onUpdateSearch),

	FXMAPFUNC(SEL_COMMAND,				FXSearchDlg::ID_RESULTS,				FXSearchDlg::onSelectResults),
};

FXIMPLEMENT(FXSearchDlg,FXDialogBox,MessageMap, ARRAYNUMBER(MessageMap))

FXSearchDlg::FXSearchDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXDialogBox(owner, name, opts, x,y,w,h, 10,10,10,10, 10,10), modifiedText(false)
{
	setIcon(icon);

    // buttons on bottom
	FXHorizontalFrame* buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0);
	new FXButton(buttons, "S&chliessen", NULL, this,ID_CLOSE, BUTTON_DEFAULT|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	info_text = new FXLabel(buttons, "", NULL, LABEL_NORMAL);
	
	new FXHorizontalSeparator(this, SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

    // create layout
	FXVerticalFrame* content = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);

	// search field
	new FXLabel(content, "&Suche nach:", NULL, LABEL_NORMAL);

	FXHorizontalFrame* search_line = new FXHorizontalFrame(content, LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0);
	search = new FXTextField(search_line, 12, this,ID_SEARCH, FRAME_LINE|TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_X);
	search->setBorderColor(getApp()->getShadowColor());
	search_button = new FXButton(search_line, "Suchen", NULL, this,ID_SEARCH, BUTTON_DEFAULT|FRAME_RAISED);
	
	// search mode: Ignore case, regular expression matching
	FXHorizontalFrame* mode_frame = new FXHorizontalFrame(content, LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0);
	options.regardcase = new FXCheckButton(mode_frame, iso2utf("&Groß-/Kleinschreibung beachten"), this,ID_SEARCH, CHECKBUTTON_NORMAL);
	options.regexp = new FXCheckButton(mode_frame, iso2utf("&Regulärer Ausdruck"), this,ID_SEARCH, CHECKBUTTON_NORMAL);

	// search domain: Search for what type, search in descriptions
	FXHorizontalFrame* domain_frame = new FXHorizontalFrame(content, LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0);
	options.descriptions = new FXCheckButton(domain_frame, "&Beschreibungen", this,ID_SEARCH, CHECKBUTTON_NORMAL);
	new FXVerticalSeparator(domain_frame, SEPARATOR_GROOVE|LAYOUT_FILL_Y);
	new FXLabel(domain_frame, "&Durchsuche", NULL, LABEL_NORMAL);
	FXListBox *box = options.domain = new FXListBox(domain_frame, this,ID_SEARCH, LISTBOX_NORMAL|LAYOUT_FILL_X|FRAME_LINE);
	box->setBorderColor(getApp()->getShadowColor());
	
	box->appendItem("Alles");
	box->appendItem("Regionen");
	box->appendItem("Einheiten");
	box->appendItem(iso2utf("Gebäude"));
	box->appendItem("Schiffe");
	box->appendItem("Befehle");
	box->setNumVisible(box->getNumItems());

	// detail options
	FXHorizontalFrame *details = new FXHorizontalFrame(content, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
	detailsTab = new FXToolBarTab(details, NULL,0, TOOLBARTAB_HORIZONTAL, 0,0,0,0);
	detailsTab->setTipText("Weitere Optionen ein- und ausblenden");

	FXVerticalFrame *details_frame = new FXVerticalFrame(details, LAYOUT_FILL_X|FRAME_LINE, 0,0,0,0);
	details_frame->setBorderColor(getApp()->getShadowColor());
	options.searchdirectly = new FXCheckButton(details_frame, "Bei Texteingabe suchen", NULL,0, CHECKBUTTON_NORMAL);
	options.limitresults = new FXCheckButton(details_frame, iso2utf("Auf 1000 Treffer beschränken"), this,ID_SEARCH, CHECKBUTTON_NORMAL);

	// results list
	new FXHorizontalSeparator(content, SEPARATOR_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 0,0,DEFAULT_SPACING,DEFAULT_SPACING);

	FXHorizontalFrame *list_frame = new FXHorizontalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_LINE, 0,0,0,0, 0,0,0,0);
	list_frame->setBorderColor(getApp()->getShadowColor());

	results = new FXFoldingList(list_frame, this,ID_RESULTS, FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	results->getHeader()->setHeaderStyle(HEADER_RESIZE|HEADER_TRACKING);
	
    results->appendHeader("Region");
	results->appendHeader(iso2utf("Einheit/Gebäude/Schiff"));
}

void FXSearchDlg::create()
{
	FXDialogBox::create();

	// resize table headers
	int width = (getWidth() - getPadLeft() - getPadRight() - 20) / results->getNumHeaders();
	for (int i = 0; i < results->getNumHeaders(); i++)
		results->setHeaderSize(i, width);
}

FXSearchDlg::~FXSearchDlg()
{
}

void FXSearchDlg::mapfiles(std::list<datafile> *f)
{
    files = f;
}

void FXSearchDlg::loadState(FXRegistry& reg)
{
	// default size is 400x300.
	width = getApp()->reg().readUnsignedEntry("SEARCHDLG", "WIDTH", 400);
	height = getApp()->reg().readUnsignedEntry("SEARCHDLG", "HEIGHT", 300);

	if (width < 0) width = 400;
	if (height < 0) height = 300;

	setWidth(width);
	setHeight(height);

	// load options
	FXint regardcase = reg.readUnsignedEntry("SEARCHDLG", "REGARDCASE", 0);
	FXint regexp = reg.readUnsignedEntry("SEARCHDLG", "REGEXP", 0);
	FXint descriptions = reg.readUnsignedEntry("SEARCHDLG", "DESCRIPTIONS", 0);
	options.regardcase->setCheck(regardcase);
	options.regexp->setCheck(regexp);
	options.descriptions->setCheck(descriptions);
	
	FXint domain = reg.readUnsignedEntry("SEARCHDLG", "DOMAIN", 0);
	if (domain >= 0 && domain < options.domain->getNumItems())
		options.domain->setCurrentItem(domain);

	FXint hideoptions = reg.readUnsignedEntry("SEARCHDLG", "HIDEOPTIONS", 1);
	detailsTab->collapse(hideoptions);

	FXint searchdirectly = reg.readUnsignedEntry("SEARCHDLG", "SEARCHDIRECTLY", 1);
	FXint limitresults = reg.readUnsignedEntry("SEARCHDLG", "LIMITRESULTS", 1);
	options.searchdirectly->setCheck(searchdirectly);
	options.limitresults->setCheck(limitresults);
}

void FXSearchDlg::saveState(FXRegistry& reg)
{
	// save window size
	if (!isMinimized())
	{		
		reg.writeUnsignedEntry("SEARCHDLG", "WIDTH", getWidth());
		reg.writeUnsignedEntry("SEARCHDLG", "HEIGHT", getHeight());
	}

	// save options
	reg.writeUnsignedEntry("SEARCHDLG", "REGARDCASE", options.regardcase->getCheck());
	reg.writeUnsignedEntry("SEARCHDLG", "REGEXP", options.regexp->getCheck());
	reg.writeUnsignedEntry("SEARCHDLG", "DESCRIPTIONS", options.descriptions->getCheck());
	
	reg.writeUnsignedEntry("SEARCHDLG", "DOMAIN", options.domain->getCurrentItem());

	reg.writeUnsignedEntry("SEARCHDLG", "HIDEOPTIONS", detailsTab->isCollapsed());

	reg.writeUnsignedEntry("SEARCHDLG", "SEARCHDIRECTLY", options.searchdirectly->getCheck());
	reg.writeUnsignedEntry("SEARCHDLG", "LIMITRESULTS", options.limitresults->getCheck());
}

long FXSearchDlg::onFocusIn(FXObject* sender, FXSelector sel, void* ptr)
{
	if (!search->hasFocus())
	{
		search->setFocus();
		search->selectAll();
	}

	return FXDialogBox::onFocusIn(sender, sel, ptr);;
}

// search functors
namespace
{
	// compare functions
	typedef boost::function<bool(const FXString&)> compare_func_t;

	class compare_normal
	{
	public:
		compare_normal() {}
		compare_normal(const FXString& needle_) : needle(needle_) { needle.upper(); }

		bool operator()(const FXString& str) const
		{
			return FXString(str).upper().find(needle) != -1;
		}

	protected:
		FXString needle;
	};

	class compare_regardcase
	{
	public:
		compare_regardcase() {}
		compare_regardcase(const FXString& needle_) : needle(needle_) {}

		bool operator()(const FXString& str) const
		{
			return str.find(needle) != -1;
		}

	protected:
		FXString needle;
	};

	class compare_regexp_normal
	{
	public:
		compare_regexp_normal() {}
		compare_regexp_normal(const FXString& needle_) : compare(needle_, REX_ICASE) {}

		bool operator()(const FXString& str) const
		{
			return compare.match(str) == TRUE;
		}

	protected:
		FXRex compare;
	};

	class compare_regexp_regardcase
	{
	public:
		compare_regexp_regardcase() {}
		compare_regexp_regardcase(const FXString& needle_) : compare(needle_, REX_NORMAL) {}

		bool operator()(const FXString& str) const
		{
			return compare.match(str) == TRUE;
		}

	protected:
		FXRex compare;
	};

	// search functions
	typedef const datablock::itor& itor_ref;
	typedef boost::tuple<itor_ref, itor_ref, itor_ref, itor_ref, itor_ref, const compare_func_t&, const compare_func_t&, bool> block_context;
	// context(search_string, region, building, ship, unit, end, compare_func, compare_func_icase, descriptions);

	typedef boost::function<bool(const datablock::itor&, const block_context&)> search_func_t;

	bool search_region(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != datablock::TYPE_REGION)
			return false;

		const compare_func_t& compare = context.get<5>();

		FXString name = block->value(datakey::TYPE_NAME);
		if (name.empty())
			name = block->terrainString();
		if (name.empty())
			name = "Unbekannt";

		name += " (" + FXStringVal(block->x()) + "," + FXStringVal(block->y());
		if (block->info())
			name += "," + datablock::planeName(block->info());
		name += ")";

		if (compare(name))
			return true;

		if (context.get<7>())		// compare descriptions?
			return compare(block->value(datakey::TYPE_DESCRIPTION));

		return false;
	}
	
	bool search_unit(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != datablock::TYPE_UNIT)
			return false;

		const compare_func_t& compare = context.get<5>();
		const compare_func_t& compare_icase = context.get<6>();

		if (compare(block->value(datakey::TYPE_NAME)) || compare_icase(block->id()))
			return true;

		if (context.get<7>())		// compare descriptions?
			return compare(block->value(datakey::TYPE_DESCRIPTION));

		return false;
	}

	bool search_building(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != datablock::TYPE_BUILDING)
			return false;

		const compare_func_t& compare = context.get<5>();
		const compare_func_t& compare_icase = context.get<6>();

		if (compare(block->value(datakey::TYPE_NAME)) || compare_icase(block->id()))
			return true;

		if (context.get<7>())		// compare descriptions?
			return compare(block->value(datakey::TYPE_DESCRIPTION));

		return false;
	}

	bool search_ship(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != datablock::TYPE_SHIP)
			return false;

		const compare_func_t& compare = context.get<5>();
		const compare_func_t& compare_icase = context.get<6>();

		if (compare(block->value(datakey::TYPE_NAME)) || compare_icase(block->id()))
			return true;

		if (context.get<7>())		// compare descriptions?
			return compare(block->value(datakey::TYPE_DESCRIPTION));

		return false;
	}

	bool search_commands(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != datablock::TYPE_COMMANDS)
			return false;

		const compare_func_t& compare = context.get<5>();

		// search if a command matchs
		if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
		{
			// search in the changed command list
			att_commands::cmdlist_t::iterator end = cmds->commands.end();
			for (att_commands::cmdlist_t::iterator itor = cmds->commands.begin(); itor != end; itor++)
			{
				if (compare(*itor))
					return true;
			}
		}
		else
		{
			// search in default command list
			datakey::itor end = block->data().end();
			for (datakey::itor itor = block->data().begin(); itor != end; itor++)
			{
				if (compare(itor->value()))
					return true;
			}
		}

		return false;
	}

	bool search_all(const datablock::itor& block, const block_context& context)
	{
		return search_region(block, context) || search_unit(block, context) || search_building(block, context)
			 || search_ship(block, context) || search_commands(block, context);
	}
}

long FXSearchDlg::onChangedSearch(FXObject* sender, FXSelector sel, void* ptr)
{
	if (options.searchdirectly->getCheck())
		modifiedText = true;
	return 0;
}

long FXSearchDlg::onUpdateSearch(FXObject* sender, FXSelector sel, void* ptr)
{
	if (modifiedText)
	{
		modifiedText = false;
		return onSearch(sender, sel, ptr);
	}

	return 0;
}

long FXSearchDlg::onSearch(FXObject*, FXSelector sel, void*)
{
	if (!files)
		return 0;

	// clear old results
	results->clearItems();
	info_text->setText("");

	// get search string
	FXString str = search->getText().trim();
	if (str.empty())
		return 1;

	// search options
	bool regardcase = options.regardcase->getCheck() == TRUE;		// ignore case or don't ignore case
	bool regexp = options.regexp->getCheck() == TRUE;				// use regular expressions
	bool descriptions = options.descriptions->getCheck() == TRUE;	// search also in description texts

	bool limitresults = options.limitresults->getCheck() == TRUE;	// stop after 1000 results

	compare_func_t compare_func_icase = compare_normal(str);		// ignores case (for base36 numbers)
	compare_func_t compare_func = compare_normal(str);				// does ignores case if not other specified
	if (regardcase)
		compare_func = compare_regardcase(str);
	if (regexp)
		compare_func_icase = compare_regexp_normal(str),
		compare_func = compare_regexp_normal(str);
	if (regexp && regardcase)
		compare_func = compare_regexp_regardcase(str);

	int domain = options.domain->getCurrentItem();

	search_func_t search_func;

	if (domain == 0)
		search_func = search_all;
	else if (domain == 1)
		search_func = search_region;
	else if (domain == 2)
		search_func = search_unit;
	else if (domain == 3)
		search_func = search_building;
	else if (domain == 4)
		search_func = search_ship;
	else if (domain == 5)
		search_func = search_commands;
	else
	{
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "FXSearchDlg::onSearch Fehler: domain = %d.", domain);
		return 0;
	}

	// do the search
	for (datafile::itor file = files->begin(); file != files->end(); file++)
	{
		datablock::itor end = file->end();

		datablock::itor region = end,			// initialize last-block-of-typ holder
						building = end,
						ship = end,
						unit = end;

		block_context context(region, building, ship, unit, end, compare_func, compare_func_icase, descriptions);

		for (datablock::itor block = file->blocks().begin(); block != end; block++)
		{
			// pass blocks to search functors
			if (block->type() == datablock::TYPE_REGION)
			{
				region = block;
				building = ship = unit = end;
			}
			else if (block->type() == datablock::TYPE_BUILDING)
			{
				building = block;
				ship = unit = end;
			}
			else if (block->type() == datablock::TYPE_SHIP)
			{
				ship = block;
				building = unit = end;
			}
			else if (block->type() == datablock::TYPE_UNIT)
			{
				unit = block;
				building = ship = end;
			}

			if (search_func(block, context))
			{
				FXString region_str, object_str;

				// add region to results-list in first column
				if (region != end)
				{
					FXString terrain = region->terrainString();
					FXString name = region->value(datakey::TYPE_NAME);

					if (name.empty())
						name = terrain;
					if (name.empty())
						name = "Unbekannt";

					if (region->info())
						region_str.format("%s (%d,%d,%s)", name.text(), region->x(), region->y(), datablock::planeName(region->info()).text());
					else
						region_str.format("%s (%d,%d)", name.text(), region->x(), region->y());
				}

				// add block to results-list in second column
				if (building != end)
				{
					FXString name = building->value(datakey::TYPE_NAME);
					FXString id = building->id();

					if (name.empty())
						name = iso2utf("Gebäude ") + id;

					object_str = name + " (" + id + ")";
				}
				else if (ship != end)
				{
					FXString name = ship->value(datakey::TYPE_NAME);
					FXString id = ship->id();

					if (name.empty())
						name = "Schiff " + id;

					object_str = name + " (" + id + ")";
				}
				else if (unit != end)
				{
					FXString name = unit->value(datakey::TYPE_NAME);
					FXString id = unit->id();
					//FXString number = block->value(datakey::TYPE_NUMBER);
					//FXString type = block->value(datakey::TYPE_TYPE);

					if (name.empty())
						name = "Einheit " + id;

					object_str = name + " (" + id + ")";
				}

				// add to list
				datablock::itor link = region;
				if (unit != end)
					link = unit;
				else if (building != end)
					link = building;
				else if (ship != end)
					link = ship;

                results->appendItem(NULL, region_str + "\t" + object_str, NULL,NULL, (void*)&*link);
				if (limitresults && results->getNumItems() >= 1000)
					break;				// list only 1000 results
			}
		}
	}

	if (limitresults && results->getNumItems() >= 1000)
		info_text->setText("Nur die ersten " + FXStringVal(results->getNumItems()) + " Treffer werden angezeigt.");
	else
		info_text->setText(FXStringVal(results->getNumItems()) + " Treffer");

	return 1;
}

long FXSearchDlg::onSelectResults(FXObject*, FXSelector, void*)
{
	if (!files)
		return 0;

	FXFoldingItem *item = results->getCurrentItem();
	if (!item)
		return 0;

	datablock* select = (datablock*)item->getData();

	for (datafile::itor file = files->begin(); file != files->end(); file++)
	{
		datablock::itor end = file->blocks().end();

		datablock::itor region = end;

		for (datablock::itor block = file->blocks().begin(); block != end; block++)
		{
			if (block->type() == datablock::TYPE_REGION)
				region = block;

			if (select != &*block)
				continue;

			// propagate selection
            datafile::SelectionState state;

			state.selected = 0;

			if (region != end)
				state.region = region, state.selected |= state.REGION;

			if (block->type() == datablock::TYPE_UNIT)
				state.unit = block, state.selected |= state.UNIT;
			else if (block->type() == datablock::TYPE_BUILDING)
				state.building = block, state.selected |= state.BUILDING;
			else if (block->type() == datablock::TYPE_SHIP)
				state.ship = block, state.selected |= state.SHIP;

			getOwner()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
			return 1;
		}
	}

	return 1;
}
