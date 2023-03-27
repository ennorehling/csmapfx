#include "main.h"
#include "fxhelper.h"
#include "infodlg.h"
#include "symbols.h"
#include "FXMenuSeparatorEx.h"

#ifdef HAVE_PHYSFS
#include <physfs.h>
#endif

#include <cctype>
#include <climits>
#include <sstream>
#include <fstream>
#include <algorithm>

// *********************************************************************************************************
// *** FXMessages implementation

FXDEFMAP(FXInfoDlg) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,				FXInfoDlg::ID_CLOSE,					FXInfoDlg::onCmdHide),

	FXMAPFUNC(SEL_COMMAND,				FXInfoDlg::ID_SEARCH,					FXInfoDlg::onSearch),
	FXMAPFUNC(SEL_CHANGED,				FXInfoDlg::ID_SEARCH,					FXInfoDlg::onSearch),
};

FXIMPLEMENT(FXInfoDlg,FXDialogBox,MessageMap, ARRAYNUMBER(MessageMap))

FXInfoDlg::FXInfoDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts, FXint x, FXint y, FXint w, FXint h)
		: FXDialogBox(owner, name, opts, x, y, w, h)
{
	setIcon(icon);

    // buttons on bottom
	FXHorizontalFrame* buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0);
	new FXButton(buttons, "S&chliessen", NULL, this,ID_CLOSE, BUTTON_DEFAULT|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	new FXVerticalSeparator(buttons, SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	search = new FXTextField(buttons, 17, this,ID_SEARCH, FRAME_LINE|TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	search->setBorderColor(getApp()->getShadowColor());
	new FXLabel(buttons, "&Suche:", NULL, LAYOUT_FILL_Y|LAYOUT_RIGHT);

	new FXHorizontalSeparator(this, SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

	// tabbook
	tabbook = new FXTabBook(this, NULL,0, TABBOOK_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);

	//bindings::register_module_method("Info", "load", boost::bind(&infodlg_parseTableFile, this, _1) );
}

void FXInfoDlg::create()
{
	FXDialogBox::create();
	createTable();
}

FXInfoDlg::~FXInfoDlg()
{
}

void FXInfoDlg::show(FXuint placement)
{
    FXDialogBox::show(placement);
    search->setFocus();
}

void FXInfoDlg::loadState(FXRegistry& reg)
{
	// default size is 640x480.
	width = getApp()->reg().readUnsignedEntry("INFODLG", "WIDTH", 640);
	height = getApp()->reg().readUnsignedEntry("INFODLG", "HEIGHT", 480);

	if (width < 0) width = 640;
	if (height < 0) height = 480;

	setWidth(width);
	setHeight(height);
}

void FXInfoDlg::saveState(FXRegistry& reg)
{
	if (!isMinimized())
	{
		reg.writeUnsignedEntry("INFODLG", "WIDTH", getWidth());
		reg.writeUnsignedEntry("INFODLG", "HEIGHT", getHeight());
	}
}

void FXInfoDlg::setGame(const FXString& game)
{
    if (currentGame == game)
        return;

    // internal data storage
    struct GameInfo {
        const char* game;
        const char* data;
        size_t		size;
    };

    static GameInfo infoData[] = {
        { "default", (const char*)data::infodlg_data, data::infodlg_data_size },
        { "E3", (const char*)data::infodlg_data_e3, data::infodlg_data_e3_size },
    };

    // clear current data
    blocks.clear();

    if (currentGame != "default") {
        // load internal data for specific game
        GameInfo* loadInfo = &infoData[0];		// load "default" info by default
        for (GameInfo* info = begin(infoData); info != end(infoData); ++info) {
            if (game == info->game) {
                loadInfo = info;
                break;
            }
        }
	    if (loadInfo && loadInfo->data) {
		    std::istringstream stream( std::string(loadInfo->data, loadInfo->size) );
		    parseTableData(stream);
	    }
    }

    currentGame = game;
    // load additional info from file
	// try game specific file first ("csmapfx.Eressea.info"),
	// then try default file name ("csmapfx.info").
	// use all files in getSearchPath().
	std::vector<std::string> fileNames;
    std::string prefix("csmapfx.");
    if (game != "default") {
        fileNames.push_back(prefix + std::string(game.text()) + ".info");
    }
	fileNames.push_back("csmapfx.info");

    for (auto i = fileNames.begin(); i != fileNames.end(); ++i) {
        const std::string& filename = *i;
        std::string str = loadResourceFile(filename.c_str());
        if (!str.empty()) {
            std::istringstream input(str);
            if (parseTableData(input)) break;
        }
	}
}

void FXInfoDlg::setSearchText(const FXString& text)
{
	search->setText(text);
    search->setFocus();
    onSearch(this, 0, (void*)search->getText().text());
}

void FXInfoDlg::setClipboard(const FXString& text)
{
	getOwner()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), (void*)text.text());
}

long FXInfoDlg::onSearch(FXObject*, FXSelector, void* ptr)
{
	FXString str( ptr ? (const FXchar*)ptr : "" );
	str = flatten(str.trim());

	if (str.empty())
		return 0;

	FXFoldingList* found_list = NULL;
	FXival found_tab = 0, found_line = 0, found_start = 0, found_length = 0;

	FXint tab = 0;
	for (std::map<FXString,infoblock>::iterator itor = blocks.begin(); itor != blocks.end(); itor++)
	{
		infoblock& block = itor->second;

		if (block.header.empty() && block.lines.empty())
			continue;

		FXint line = 0;
		for (std::list<infoblock::row>::iterator it = block.lines.begin(); it != block.lines.end(); ++it, line++)
		{
			for (infoblock::row::iterator entry = it->begin(); entry != it->end(); entry++)
			{
				FXint pos = flatten(*entry).find(str);
				if (pos > -1 && (!found_length || pos < found_start || (pos == found_start && entry->length() < found_length)))
				{
					found_list = block.list.get();
					found_tab = tab;
					found_line = line;
					found_start = pos;
					found_length = entry->length();
				}
			}
		}

		tab++;		// should not be incremented by continue;
	}

	if (found_length)
	{
		tabbook->setCurrent(found_tab);

		FXFoldingItem *item = found_list ? found_list->findItemByData((void*)found_line) : NULL;
		if (!item)
			return 0;

		found_list->setCurrentItem(item);
		found_list->selectItem(item);
		found_list->makeItemVisible(item);
	}

	return 1;
}

FXString FXInfoDlg::getTableText(FXFoldingList* list) const
{
    FXString text;
	if (!list)
		return text;

	int headers = list->getNumHeaders();
	for (int i = 0; i < headers; i++)
	{
		if (i)
			text += '\t';
		text += list->getHeaderText(i);
	}

	text += '\n';

	for (FXFoldingItem* item = list->getFirstItem(); item; item = item->getNext())
        text += item->getText() + '\n';

	return text;
}

// synch table view with infoblock data
void FXInfoDlg::createTable()
{
	if (!xid)		// window must be created
		return;

	blocks.erase("- Fehler -");
	if (blocks.empty())
	{
		blocks["- Fehler -"] = infoblock();
		infoblock& block = blocks.begin()->second;

		block.header.push_back("Informationsdatei fehlerhaft oder nicht vorhanden.");
	}

	for (std::map<FXString,infoblock>::iterator itor = blocks.begin(); itor != blocks.end(); itor++)
	{
		infoblock& block = itor->second;

		if (block.header.empty() && block.lines.empty())
			continue;

        block.tab = std::make_unique<FXTabItem>(tabbook, itor->first);
        block.tab->create();

        block.frame = std::make_unique<FXHorizontalFrame>(tabbook, LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_LINE, 0,0,0,0, 0,0,0,0);
        block.frame->setBorderColor(getApp()->getShadowColor());
        block.frame->create();

        block.list = std::make_unique<FXFoldingList>(block.frame.get(), this,ID_LIST, FOLDINGLIST_SINGLESELECT|FOLDINGLIST_SHOWS_LINES|FOLDINGLIST_SHOWS_BOXES|LAYOUT_FILL_X|LAYOUT_FILL_Y);
        block.list->getHeader()->setHeaderStyle(HEADER_RESIZE|HEADER_TRACKING);
        block.list->create();

		
		FXFont* font = block.list->getHeader()->getFont();		// font for width calculation
		FXint indent = 2 + block.list->getIndent() + font->getTextHeight("A")/2;	// parent to child indent

		// create header
		std::vector<FXint> widths;

		for (infoblock::row::iterator header = block.header.begin(); header != block.header.end(); header++)
		{
            block.list->appendHeader(*header, NULL);
			widths.push_back(font->getTextWidth(*header));
		}

		// create items
		std::vector<FXFoldingItem*> fathers;
		FXival line = 0;

		for (std::list<infoblock::row>::iterator it = block.lines.begin(); it != block.lines.end(); it++)
		{
			FXString text;

			// count subitem depth
			infoblock::row::iterator entry = it->begin();
			if (entry->left(1) == ">" && entry->find_first_not_of('>') == -1)
			{
				fathers.resize(std::min((FXint)fathers.size(), entry->length()));
				entry++;
			}
			else
				fathers.clear();

			// add width to first column for indentation
			FXint indentAdd = indent * fathers.size();

			// append item to list
			FXuint col = 0;
			for (; entry != it->end() && col < widths.size(); entry++)
			{
				text += *entry + "\t";
				widths[col] = std::max(widths[col], font->getTextWidth(*entry) + indentAdd);
				col++;

				indentAdd = 0;		// counts for first column only. set to 0 after that.
			}

			FXFoldingItem *father = fathers.size() ? fathers.back() : NULL;
			if (father)
				father->setExpanded(true);

			FXFoldingItem *item = block.list->appendItem(father, text, NULL, NULL, (void*)line);
			fathers.push_back(item);
			line++;
		}

		for (FXint col = 0; col < block.list->getNumHeaders(); col++)
            block.list->setHeaderSize(col, 10+widths[col]);
	}
}

bool FXInfoDlg::parseTableData(std::istream& input)
{
    infoblock* block = NULL;
	std::string sline;
	while (std::getline(input, sline))
	{
		FXString line = FXString(sline.c_str()).trim();

		// parse line
		if (line.left(1) == ";")		// comment lines start with a semikolon
			continue;

		if (line.left(1) == "[")		// new tab lines start with [
		{
			/*
				[Group1]	// new group, clear old with same name
				[Group2]+	// append to old group
			*/

			FXString name = line.after('[').before(']').trim();
            if (name.empty())
				name = "Sonstiges";

			// select block by name
			block = &blocks[name];

			// clear old values, if the append form "[...]+" is not used
			if (line.after(']').left(1) != "+")
			{
				block->header.clear();
				block->lines.clear();
			}
		}
		else
		{
			infoblock::row row;

			for (; !line.empty(); line = line.after('\t'))
			{
				FXString entry = line.before('\t').trim();

				if (entry == "_")	// Ein Unterstrich "_" erzeugt eine leere Spalte.
					row.push_back("");
				else if (!entry.empty())
					row.push_back(entry);
			}

			if (row.empty())		// Keine leeren Zeilen
				continue;

			// append row to active block.
			// first row becomes the header
			if (!block)
				block = &blocks["Sonstiges"];	// if no block specified, append to block "Sonstiges"

			if (block->header.empty())
				block->header = row;
			else
				block->lines.push_back(row);
		}
	}
	createTable();
    return true;
}
