#ifndef _CSMAP_STATISTICS
#define _CSMAP_STATISTICS

#include <fx.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "datafile.h"

class FXStatistics : public FXVerticalFrame
{
	FXDECLARE(FXStatistics)

public:
	FXStatistics(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXStatistics();

	void mapfiles(std::list<datafile> *f);

	void loadState(FXRegistry& reg);
	void saveState(FXRegistry& reg);

public:
	long onChangeFaction(FXObject*,FXSelector,void*);
	long onChangeFilter(FXObject*,FXSelector,void*);
	long updChangeFilter(FXObject*,FXSelector,void*);

	long onPopup(FXObject*,FXSelector,void*);
	long onPopupClicked(FXObject*,FXSelector,void*);

	long onMapChange(FXObject*,FXSelector,void*);
	long onQueryHelp(FXObject*,FXSelector,void*);

public: 
	enum
	{
		ID_FACTIONBOX = FXVerticalFrame::ID_LAST,
		ID_FILTERBOX,
		ID_LIST,
		ID_POPUP_CLICKED,

		ID_LAST
	};

protected:
	datafile::SelectionState selection;

	std::list<datafile>	*files;

	// control box
	FXComboBox		*factionBox;

	FXToolBarTab	*opTab;

	// the list
	FXList			*list;

	struct
	{
		// ID of selected faction (in box)
		int			faction;

		// selected filter
		int			filter;

		enum
		{
            FILTER_PERSONS	= (1<<0),
			FILTER_ITEMS	= (1<<1),
			FILTER_TALENTS	= (1<<2),
			FILTER_SHIPS	= (1<<3),
			FILTER_BUILDINGS= (1<<4)
		};

	} select;

	struct entry
	{
		std::vector<std::pair<int, int> > list;			// saves: (UnitID, Number) or (shipId, Number)
	};

	std::map<int, entry> entries;						// maps ListItem-Idx to entry

protected:

	void updateList();

	void collectFactionList(std::set<int> &factions, datablock::itor region);
	void collectData(std::map<FXString, entry> &persons, std::map<FXString, entry> &items,
				std::map<std::pair<FXString,int>, entry> &talents, std::map<FXString, entry> &ships,
				std::map<FXString, entry> &buildings, datablock::itor region);

protected:
	FXStatistics(){}
	FXStatistics(const FXStatistics&) {}
};

#endif //_CSMAP_STATISTICS
