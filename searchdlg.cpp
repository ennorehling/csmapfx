#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "searchdlg.h"
#include "symbols.h"

#include <functional>
#include <tuple>

#define MAX_RESULTS 1000
#define DOMAIN_ALL 0
#define DOMAIN_REGION 1
#define DOMAIN_UNIT 2
#define DOMAIN_BLDG 3
#define DOMAIN_SHIP 4
#define DOMAIN_CMDS 5

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

    FXMAPFUNC(SEL_COMMAND,			    FXSearchDlg::ID_UPDATE,			        FXSearchDlg::onMapChange),

    FXMAPFUNC(SEL_COMMAND,				FXSearchDlg::ID_TRANSFER,				FXSearchDlg::onCopyResults),
    FXMAPFUNC(SEL_COMMAND,				FXSearchDlg::ID_RESULTS,				FXSearchDlg::onSelectResults),
};

FXIMPLEMENT(FXSearchDlg, FXDialogBox, MessageMap, ARRAYNUMBER(MessageMap))

class FXSearchItem : public FXFoldingItem {
public:
    FXSearchItem(const FXString& text, FXIcon* oi = NULL, FXIcon* ci = nullptr, void* ptr = nullptr)
        : FXFoldingItem(text, oi, ci, ptr), m_font(nullptr) {}

    void setFont(FXFont* font) { m_font = font; }
    virtual void draw(const FXFoldingList* list, FXDC& dc, FXint x, FXint y, FXint w, FXint h) const override;

protected:
    FXFont* m_font;
};

FXSearchDlg::FXSearchDlg(FXWindow* owner, FXObject* tgt, FXSelector sel, FXFoldingList* resultList, const FXString& name, FXIcon* icon, FXuint opts, FXint x, FXint y, FXint w, FXint h)
		: FXDialogBox(owner, name, opts, x, y, w, h, 10, 10, 10, 10, 10, 10), results(resultList), matches(nullptr), modifiedText(false), boldFont(nullptr), mapFile(nullptr)
{
	setIcon(icon);
    setTarget(tgt);
    setSelector(sel);

    // buttons on bottom
    FXHorizontalFrame* buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    info_text = new FXLabel(buttons, "", NULL, LABEL_NORMAL);
    new FXButton(buttons, "S&chliessen", NULL, this, ID_CLOSE, BUTTON_DEFAULT | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_RIGHT);
    btnTransfer = new FXButton(buttons, L"In Such&ergebnissen \u00f6ffnen", NULL, this, ID_TRANSFER, BUTTON_DEFAULT | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_RIGHT);
	
	new FXHorizontalSeparator(this, SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

    // create layout
	FXVerticalFrame* content = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);

	// search field
	new FXLabel(content, "&Suche nach:", NULL, LABEL_NORMAL);

	FXHorizontalFrame* search_line = new FXHorizontalFrame(content, LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
	txtSearch = new FXTextField(search_line, 12, this,ID_SEARCH, FRAME_LINE|TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_X);
	txtSearch->setBorderColor(getApp()->getShadowColor());
	btnSearch = new FXButton(search_line, "&Suchen", NULL, this, ID_SEARCH, BUTTON_DEFAULT|FRAME_RAISED);
	// search mode: Ignore case, regular expression matching
	FXHorizontalFrame* mode_frame = new FXHorizontalFrame(content, LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
	options.regardcase = new FXCheckButton(mode_frame, FXString(L"&Gro\u00df-/Kleinschreibung beachten"), this, ID_SEARCH, CHECKBUTTON_NORMAL);
	options.regexp = new FXCheckButton(mode_frame, FXString(L"&Regul\u00e4rer Ausdruck"), this, ID_SEARCH, CHECKBUTTON_NORMAL);
    options.selection = new FXCheckButton(mode_frame, FXString(L"&Ausgew\u00e4hlte Regionen"), this, ID_SEARCH, CHECKBUTTON_NORMAL);

	// search domain: Search for what type, search in descriptions
	FXHorizontalFrame* domain_frame = new FXHorizontalFrame(content, LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
	options.descriptions = new FXCheckButton(domain_frame, "&Beschreibungen", this, ID_SEARCH, CHECKBUTTON_NORMAL);
	options.factions = new FXCheckButton(domain_frame, "&Parteien", this,ID_SEARCH, CHECKBUTTON_NORMAL);
	new FXVerticalSeparator(domain_frame, SEPARATOR_GROOVE|LAYOUT_FILL_Y);
	new FXLabel(domain_frame, "&Durchsuche", NULL, LABEL_NORMAL);
	FXListBox *box = options.domain = new FXListBox(domain_frame, this, ID_SEARCH, LISTBOX_NORMAL|LAYOUT_FILL_X|FRAME_LINE);
	box->setBorderColor(getApp()->getShadowColor());
	
	box->appendItem("Alles");
	box->appendItem("Regionen");
	box->appendItem("Einheiten");
	box->appendItem(FXString(L"Geb\u00e4ude"));
	box->appendItem("Schiffe");
	box->appendItem("Befehle");
	box->setNumVisible(box->getNumItems());

	// results list
	new FXHorizontalSeparator(content, SEPARATOR_GROOVE|LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, DEFAULT_SPACING, DEFAULT_SPACING);

	FXHorizontalFrame *list_frame = new FXHorizontalFrame(content, LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_LINE, 0, 0, 0, 0, 0, 0, 0, 0);
	list_frame->setBorderColor(getApp()->getShadowColor());

    matches = new FXFoldingList(list_frame, this, ID_RESULTS, FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
    matches->getHeader()->setHeaderStyle(HEADER_RESIZE|HEADER_TRACKING);
	
    matches->appendHeader("Region");
    matches->appendHeader(FXString(L"Einheit/Geb\u00e4ude/Schiff"));
    matches->appendHeader("Partei");
}

void FXSearchDlg::create()
{
	FXDialogBox::create();

    FXFontDesc fontdesc;
    matches->getFont()->getFontDesc(fontdesc);
    fontdesc.weight = FXFont::Bold;
    boldFont = new FXFont(getApp(), fontdesc);
    boldFont->create();

	// resize table headers
	int w = (getWidth() - getPadLeft() - getPadRight() - 20) / matches->getNumHeaders();
	for (int i = 0; i < matches->getNumHeaders(); i++)
        matches->setHeaderSize(i, w);
}

FXSearchDlg::~FXSearchDlg()
{
    delete boldFont;
}

void FXSearchDlg::setMapFile(datafile *f)
{
    mapFile = f;
    matches->clearItems();
    info_text->setText("");
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
	FXint factions = reg.readUnsignedEntry("SEARCHDLG", "FACTIONS", 0);
	FXint selected = reg.readUnsignedEntry("SEARCHDLG", "SELECTION", 0);
	options.regardcase->setCheck(regardcase!=0);
	options.regexp->setCheck(regexp != 0);
	options.descriptions->setCheck(descriptions != 0);
	options.factions->setCheck(factions != 0);
	options.selection->setCheck(selected != 0);
	
	FXint domain = reg.readUnsignedEntry("SEARCHDLG", "DOMAIN", 0);
	if (domain >= 0 && domain < options.domain->getNumItems())
		options.domain->setCurrentItem(domain);
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
	reg.writeUnsignedEntry("SEARCHDLG", "FACTIONS", options.factions->getCheck());
	reg.writeUnsignedEntry("SEARCHDLG", "SELECTION", options.selection->getCheck());
	
	reg.writeUnsignedEntry("SEARCHDLG", "DOMAIN", options.domain->getCurrentItem());
}

long FXSearchDlg::onFocusIn(FXObject* sender, FXSelector sel, void* ptr)
{
	if (!txtSearch->hasFocus())
	{
		txtSearch->setFocus();
		txtSearch->selectAll();
	}

	return FXDialogBox::onFocusIn(sender, sel, ptr);;
}

// search functors
namespace
{
	// compare functions
	typedef std::function<bool(const FXString&)> compare_func_t;

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
    struct block_context {
        datafile* report;
        itor_ref region;
        itor_ref building;
        itor_ref ship;
        itor_ref unit;
        const compare_func_t& compare;
        const compare_func_t& compare_icase;
        bool searchDescriptions;
        bool searchFactions;

        block_context(datafile* report, itor_ref region, itor_ref building, itor_ref ship, itor_ref unit,
            const compare_func_t& compare, const compare_func_t& compare_icase, bool searchDescriptions, bool searchFactions)
            : report(report), region(region), building(building), ship(ship), unit(unit), compare(compare), compare_icase(compare_icase),
            searchDescriptions(searchDescriptions), searchFactions(searchFactions) {}
    };
//	typedef std::tuple<itor_ref, itor_ref, itor_ref, itor_ref, itor_ref, const compare_func_t&, const compare_func_t&, bool, bool> block_context;
	// context(mapfile, region, building, ship, unit, end, compare_func, compare_func_icase, descriptions, faction);

	typedef std::function<bool(const datablock::itor&, const block_context&)> search_func_t;

	bool search_region(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != block_type::TYPE_REGION)
			return false;

		const compare_func_t& compare = context.compare;

		FXString name = block->value(TYPE_NAME);
		if (name.empty())
			name = block->terrainString();

		name += " (" + FXStringVal(block->x()) + ", " + FXStringVal(block->y());
		if (block->info())
			name += ", " + datablock::planeName(block->info());
		name += ")";

		if (compare(name))
			return true;

		if (context.searchDescriptions)		// compare descriptions?
			return compare(block->value(TYPE_DESCRIPTION));

		return false;
	}
	
	bool search_unit(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != block_type::TYPE_UNIT)
			return false;

		const compare_func_t& compare = context.compare;

		if (compare(block->value(TYPE_NAME)))
			return true;

        // compare descriptions?
		if (context.searchDescriptions && compare(block->value(TYPE_DESCRIPTION)))
			return true;

        // compare faction names?
        if (context.searchFactions) {
            datablock::itor faction;
            int fac_id = block->valueInt(TYPE_FACTION);
            if (fac_id > 0 && context.report->getFaction(faction, fac_id)) {
                return compare(faction->value(TYPE_FACTIONNAME)) || context.compare_icase(FXStringValEx(fac_id, 36));
            }
        }
		return false;
	}

	bool search_building(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != block_type::TYPE_BUILDING)
			return false;

		const compare_func_t& compare = context.compare;

		if (compare(block->value(TYPE_NAME)))
			return true;

		if (context.searchDescriptions)		// compare descriptions?
			return compare(block->value(TYPE_DESCRIPTION));

		return false;
	}

	bool search_ship(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != block_type::TYPE_SHIP)
			return false;

        const compare_func_t& compare = context.compare;

		if (compare(block->value(TYPE_NAME)))
			return true;

        if (context.searchDescriptions)		// compare descriptions?
            return compare(block->value(TYPE_DESCRIPTION));

		return false;
	}

	bool search_commands(const datablock::itor& block, const block_context& context)
	{
		if (block->type() != block_type::TYPE_COMMANDS)
			return false;

        const compare_func_t& compare = context.compare;

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
            datakey::list_type::const_iterator end = block->data().end();
			for (datakey::list_type::const_iterator itor = block->data().begin(); itor != end; itor++)
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

long FXSearchDlg::onSelectResults(FXObject*, FXSelector, void*)
{
    if (!mapFile)
        return 0;

    FXFoldingItem* item = matches->getCurrentItem();
    if (!item)
        return 0;

    datablock* select = (datablock*)item->getData();

    datablock::itor region, block, end = mapFile->blocks().end();
    mapFile->findSelection(select, block, region);

    // propagate selection
    datafile::SelectionState state;
    state.selected = 0;
    if (selection.selected & selection.MULTIPLE_REGIONS) {
        state.selected = selection.MULTIPLE_REGIONS;
        state.regionsSelected = selection.regionsSelected;
    }
    if (region != end) {
        state.region = region;
        state.selected |= state.REGION;
    }
    if (block != end) {
        if (block->type() == block_type::TYPE_UNIT) {
            state.unit = block;
            state.selected |= state.UNIT;
        }
        else if (block->type() == block_type::TYPE_BUILDING) {
            state.building = block;
            state.selected |= state.BUILDING;
        }
        else if (block->type() == block_type::TYPE_SHIP) {
            state.ship = block;
            state.selected |= state.SHIP;
        }
    }
    getOwner()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &state);
    return 1;
}

long FXSearchDlg::onSearch(FXObject*, FXSelector sel, void*)
{
	if (!mapFile)
		return 0;

	// clear old results
    matches->clearItems();
	info_text->setText("");

	// get search string
	FXString str = txtSearch->getText().trim();
	if (str.empty())
		return 1;

	// search options
	bool regardcase = options.regardcase->getCheck() != FALSE;		// ignore case or don't ignore case
	bool regexp = options.regexp->getCheck() != FALSE;				// use regular expressions
	bool descriptions = options.descriptions->getCheck() != FALSE;	// search also in description texts
	bool factions = options.factions->getCheck() != FALSE;	        // search also in faction names
    bool selected = options.selection->getCheck() != FALSE;         // search only in selected regions

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

    if (domain == DOMAIN_ALL)
		search_func = search_all;
	else if (domain == DOMAIN_REGION)
		search_func = search_region;
	else if (domain == DOMAIN_UNIT)
		search_func = search_unit;
	else if (domain == DOMAIN_BLDG)
		search_func = search_building;
	else if (domain == DOMAIN_SHIP)
		search_func = search_ship;
	else if (domain == DOMAIN_CMDS)
		search_func = search_commands;
	else
	{
		FXMessageBox::error(this, MBOX_OK, CSMAP_APP_TITLE, "FXSearchDlg::onSearch Fehler: domain = %d.", domain);
		return 0;
	}

	// do the search
	datablock::itor end = mapFile->blocks().end();

	datablock::itor region = end,			// initialize last-block-of-type holder
					building = end,
					ship = end,
					unit = end;

	block_context context(mapFile, region, building, ship, unit, compare_func, compare_func_icase, descriptions, factions);

	for (datablock::itor block = mapFile->blocks().begin(); block != end; block++)
	{
        FXString id;
        // pass blocks to search functors
		if (block->type() == block_type::TYPE_REGION)
		{
            if (selected) {
                bool found = false;
                for (const datablock* sregion : selection.regionsSelected) {
                    if (sregion == &*block) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    building = ship = unit = region = end;
                    continue;
                }
            }
            region = block;
            building = ship = unit = end;
		}
		else if (block->type() == block_type::TYPE_BUILDING)
		{
            building = block;
			ship = unit = end;
            id = block->id().lower();
		}
		else if (block->type() == block_type::TYPE_SHIP)
		{
            ship = block;
			building = unit = end;
            id = block->id().lower();
		}
		else if (block->type() == block_type::TYPE_UNIT)
		{
            unit = block;
			building = ship = end;
            id = block->id().lower();
		}

        if (region != end) {
            if (!id.empty() && compare_func_icase(id)) {
                FXFoldingItem* item = addMatch(region, building, ship, unit);
                FXRex compare(str, REX_ICASE);
                FXint m_beg, m_end;
                if (compare.match(id, &m_beg, &m_end)) {
                    if (m_beg == 0 && m_end == id.length()) {
                        // exact match: add to top of results.
                        matches->prependItem(nullptr, item);
                    }
                    else {
                        matches->appendItem(nullptr, item);
                    }
                }
                else {
                    matches->appendItem(nullptr, item);
                }
            }
            else if (search_func(block, context))
            {
                FXFoldingItem* item = addMatch(region, building, ship, unit);
                matches->appendItem(nullptr, item);
            }
            else continue;
            if (matches->getNumItems() >= MAX_RESULTS)
                break;				// list only 1000 results
        }
    }

	if (matches->getNumItems() >= MAX_RESULTS)
		info_text->setText("Nur die ersten " + FXStringVal(matches->getNumItems()) + " Treffer werden angezeigt.");
	else
		info_text->setText(FXStringVal(matches->getNumItems()) + " Treffer");

	return 1;
}

long FXSearchDlg::onCopyResults(FXObject* sender, FXSelector sel, void* ptr)
{
    FXFoldingItem* item;
    results->clearItems();
    for (item = matches->getFirstItem(); item; item = item->getNext()) {
        FXFoldingItem* copy = new FXFoldingItem(item->getText(), item->getOpenIcon(), item->getClosedIcon(), item->getData());
        results->appendItem(NULL, copy);
    }
    FXTabBook* outputTabs = (FXTabBook*)results->getParent();
    outputTabs->setCurrent(outputTabs->indexOfChild(results) / 2);

    return onCmdHide(sender, sel, ptr);
}

long FXSearchDlg::onMapChange(FXObject* sender, FXSelector sel, void* ptr)
{
    datafile::SelectionState* pstate = (datafile::SelectionState*)ptr;
    selection = *pstate;
    return 1;
}

FXFoldingItem *
FXSearchDlg::addMatch(const datablock::itor& region, const datablock::itor& building, const datablock::itor& ship, const datablock::itor& unit)
{
    const datablock::itor end = mapFile->blocks().end();
    FXString region_str, object_str, faction_str;
    bool bold = false;
    // add region to results-list in first column
    if (region != end)
    {
        FXString terrain = region->terrainString();
        FXString name = region->value(TYPE_NAME);

        if (name.empty())
            name = terrain;
        if (name.empty())
            name = "Unbekannt";

        if (region->info())
            region_str.format("%s (%d, %d, %s)", name.text(), region->x(), region->y(), datablock::planeName(region->info()).text());
        else
            region_str.format("%s (%d, %d)", name.text(), region->x(), region->y());
    }

    // add block to results-list in second column
    if (building != end)
    {
        FXString name = building->value(TYPE_NAME);
        FXString id = building->id();

        if (name.empty())
            name = FXString(L"Geb\u00e4ude ") + id;

        object_str = name + " (" + id + ")";
    }
    else if (ship != end)
    {
        FXString name = ship->value(TYPE_NAME);
        FXString id = ship->id();

        if (name.empty())
            name = "Schiff " + id;

        object_str = name + " (" + id + ")";
    }
    else if (unit != end)
    {
        FXString name = unit->value(TYPE_NAME);
        FXString id = unit->id();
        const datablock* unitPtr = &*unit;
        int factionId = mapFile->getFactionIdForUnit(unitPtr);

        if (name.empty())
            name = "Einheit " + id;

        object_str = name + " (" + id + ")";
        faction_str = mapFile->getFactionName(factionId);

        datablock::itor block = unit;
        for (block++; block != end && block->depth() > unit->depth(); block++)
        {
            if (block->type() != block_type::TYPE_COMMANDS)
                continue;

            if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment())) {
                bold = !cmds->confirmed;
                break;
            }
        }

        if (att_commands* cmds = dynamic_cast<att_commands*>(unit->attachment())) {
            bold = !cmds->confirmed;
        }
    }

    // add to list
    datablock::itor link = region;
    if (unit != end)
        link = unit;
    else if (building != end)
        link = building;
    else if (ship != end)
        link = ship;

    FXSearchItem* item = new FXSearchItem(region_str + "\t" + object_str + "\t" + faction_str, nullptr, nullptr, (void*)&*link);
    if (bold) item->setFont(boldFont);
    return item;
}

static const int ICON_SPACING = 4;	// Spacing between parent and child in x direction
static const int TEXT_SPACING = 4;	// Spacing between icon and text
static const int SIDE_SPACING = 4;	// Spacing between side and item

void FXSearchItem::draw(const FXFoldingList* list, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) const
{
    FXFont* dcfont = dc.getFont();
    FXFont* font = dcfont;
    if (m_font) {
        dc.setFont(font = m_font);
    }
    FXFoldingItem::draw(list, dc, xx, yy, ww, hh);
    if (m_font) dc.setFont(dcfont);
}
