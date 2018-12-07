#ifndef _CSMAP_SEARCHDLG
#define _CSMAP_SEARCHDLG

#include <fx.h>
#include <map>
#include <list>
#include <vector>
#include "datafile.h"

class FXSearchDlg : public FXDialogBox
{
	FXDECLARE(FXSearchDlg)

public:
	FXSearchDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts=DECOR_TITLE|DECOR_BORDER, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXSearchDlg();

	void mapfiles(std::list<datafile> *f);

	void loadState(FXRegistry& reg);
	void saveState(FXRegistry& reg);

public:
	long onFocusIn(FXObject*, FXSelector, void*);		// got focus. set focus to search box

	long onSearch(FXObject*, FXSelector, void*);		// hit enter in search textbox
	long onChangedSearch(FXObject*, FXSelector, void*);	// change text in textbox
	long onUpdateSearch(FXObject*, FXSelector, void*);	// update event on textbox
	long onSelectResults(FXObject*, FXSelector, void*);

public:
	enum
	{
		ID_SEARCH = FXDialogBox::ID_LAST,
		ID_RESULTS,
		ID_LAST
	};

protected:
	datafile::SelectionState selection;
	std::list<datafile>	*files;

	FXTextField		*search;				// textfield for search
	FXButton		*search_button;			// "do the search!"
	bool			modifiedText;

	FXFoldingList	*results;				// list of results
	FXLabel			*info_text;				// info about search

	FXToolBarTab	*detailsTab;			// tab of detailed options

	struct
	{
		FXCheckButton	*regardcase;			// ignore case or don't
		FXCheckButton	*regexp;				// regular expression match
		FXCheckButton	*descriptions;			// search also in descriptions

		FXListBox		*domain;				// where to search

		FXCheckButton	*searchdirectly;
		FXCheckButton	*limitresults;

	} options;

protected:
	FXSearchDlg(){}
	FXSearchDlg(const FXSearchDlg&) {}
};

#endif //_CSMAP_SEARCHDLG
