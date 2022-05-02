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
	FXSearchDlg(FXWindow* owner, FXFoldingList* resultList, const FXString& name, FXIcon* icon, FXuint opts=DECOR_TITLE|DECOR_BORDER, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

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
    long onCopyResults(FXObject* sender, FXSelector sel, void* ptr);
public:
	enum
	{
		ID_SEARCH = FXDialogBox::ID_LAST,
		ID_RESULTS,
		ID_LAST
	};

protected:
	FXTextField		*txtSearch;				// textfield for search
	FXButton		*btnSearch;	 	    	// "do the search!"
	FXButton		*btnTransfer;			// "transfer to results"

	FXFoldingList	*matches;				// list of results
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

    datafile* mapFile;
    datafile::SelectionState selection;
    bool modifiedText;

protected:
	FXSearchDlg(){}
	FXSearchDlg(const FXSearchDlg&) {}
};

#endif //_CSMAP_SEARCHDLG
