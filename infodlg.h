#ifndef _CSMAP_INFODLG
#define _CSMAP_INFODLG

#include <fx.h>
#include <map>
#include <list>
#include <vector>
#include <istream>
#include "datafile.h"
#include "linked_ptr.h"

class FXInfoDlg : public FXDialogBox
{
	FXDECLARE(FXInfoDlg)

public:
	FXInfoDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts=DECOR_TITLE|DECOR_BORDER, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXInfoDlg();

	void loadState(FXRegistry& reg);
	void saveState(FXRegistry& reg);

	void setGame(const FXString& game);

	void setSearchText(const FXString& text);

	FXString getTableText(FXFoldingList* list) const;
	void setClipboard(const FXString& text);

public:
	long onSearch(FXObject*, FXSelector, void*);		// hit enter in search textbox
public:
	enum
	{
		ID_SEARCH = FXDialogBox::ID_LAST,
		ID_LIST,
		ID_LAST
	};

protected:
	FXTextField *search;				// textfield for search

	FXTabBook	*tabbook;

	// table of information to show
	struct infoblock
	{
		typedef std::vector<FXString>	row;

		row								header;
		std::list<row>					lines;

		linked_ptr<FXTabItem>			tab;
		linked_ptr<FXHorizontalFrame>	frame;
		FXFoldingList*					list;
	};

	std::map<FXString, infoblock>		blocks;

	// name of current game, i.e. "default", "Eressea", "E3"
	FXString							current_game;

	// parse text to table and create widgets from table
	bool parseTableData(std::istream& input);
	void createTable();

protected:
	FXInfoDlg(){}
	FXInfoDlg(const FXInfoDlg&) {}
};

#endif //_CSMAP_INFODLG
