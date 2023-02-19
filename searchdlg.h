#pragma once

#include "datafile.h"

#include <fx.h>

#include <map>
#include <vector>

class FXSearchResults : public FXFoldingList
{
    FXDECLARE(FXSearchResults)
public:
    FXSearchResults(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
    virtual ~FXSearchResults();

    virtual void create();

    class FXFont* getBoldFont() const { return boldFont; }

    void setActiveFactionId(FXint id);
    FXint getActiveFactionId() const { return activeFaction;  }

private:
    FXSearchResults() {};

    class FXFont* boldFont = nullptr;
    FXint activeFaction = 0;

};

class FXSearchDlg : public FXDialogBox
{
	FXDECLARE(FXSearchDlg)

public:
	FXSearchDlg(FXWindow* owner, FXObject* tgt, FXSelector sel, FXSearchResults* resultList, const FXString& name, FXIcon* icon, FXuint opts=DECOR_TITLE|DECOR_BORDER, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	virtual void create();

    void setMapFile(std::shared_ptr<datafile>& f);

	void loadState(FXRegistry& reg);
	void saveState(FXRegistry& reg);

public:
	long onFocusIn(FXObject*, FXSelector, void*);		// got focus. set focus to search box

	long onSearch(FXObject*, FXSelector, void*);		// hit enter in search textbox
	long onChangedSearch(FXObject*, FXSelector, void*);	// change text in textbox
	long onUpdateSearch(FXObject*, FXSelector, void*);	// update event on textbox
    long onSelectResults(FXObject*, FXSelector, void*);
    long onCopyResults(FXObject* sender, FXSelector sel, void* ptr);

    long onMapChange(FXObject* sender, FXSelector sel, void* ptr);

public:
	enum
	{
		ID_SEARCH = FXDialogBox::ID_LAST,
		ID_TRANSFER,
		ID_RESULTS,
		ID_LAST
	};


protected:
	datafile::SelectionState selection;
    std::shared_ptr<datafile> mapFile;

	FXTextField		*txtSearch = nullptr;			// textfield for search
	FXButton		*btnSearch = nullptr;		    // do the search
	FXButton		*btnTransfer = nullptr;		    // transfer to global search list
	bool			modifiedText = false;

    FXSearchResults *matches = nullptr;				// list of results
    FXSearchResults *results = nullptr;				// list of results
	FXLabel			*info_text = nullptr;			// info about search

	struct
	{
		FXCheckButton	*regardcase = nullptr;      // ignore case or don't
		FXCheckButton	*regexp = nullptr;			// regular expression match
		FXCheckButton	*selection = nullptr;		// search only in selection
		FXCheckButton	*descriptions = nullptr;	// search also in descriptions
		FXCheckButton	*factions = nullptr;		// search also in descriptions

		FXListBox		*domain = nullptr;			// where to search
    } options;

protected:
	FXSearchDlg(){}
	FXSearchDlg(const FXSearchDlg&) {}

    class FXFoldingItem *addMatch(const datablock::itor &region, const datablock::itor& building, const datablock::itor& ship, const datablock::itor& unit);

};
