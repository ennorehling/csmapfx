#ifndef _CSMAP_SEARCHDLG
#define _CSMAP_SEARCHDLG

#include "datafile.h"

#include <fx.h>

#include <map>
#include <vector>

class FXSearchDlg : public FXDialogBox
{
	FXDECLARE(FXSearchDlg)

public:
	FXSearchDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts=DECOR_TITLE|DECOR_BORDER, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXSearchDlg();

    void setMapFile(datafile *f);

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
    datafile *mapFile;

	FXTextField		*search;				// textfield for search
	FXButton		*search_button;			// "do the search!"
	bool			modifiedText;

	FXFoldingList	*results;				// list of results
	FXLabel			*info_text = nullptr;				// info about search

	struct
	{
		FXCheckButton	*regardcase;			// ignore case or don't
		FXCheckButton	*regexp;				// regular expression match
		FXCheckButton	*descriptions;			// search also in descriptions
		FXCheckButton	*factions;			// search also in descriptions

		FXListBox		*domain;				// where to search
	} options;

protected:
	FXSearchDlg(){}
	FXSearchDlg(const FXSearchDlg&) {}

private:
    class FXFont* boldFont = nullptr;
};

#endif //_CSMAP_SEARCHDLG
