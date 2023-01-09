#ifndef _CSMAP_REGIONLIST
#define _CSMAP_REGIONLIST

#include "datafile.h"
#include "terrain.h"

#include <fx.h>

#include <list>

class FXRegionList : public FXTreeList
{
	FXDECLARE(FXRegionList)

public:
	FXRegionList(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionList();

	void setMapFile(datafile *f);

public:
	long onSelected(FXObject*,FXSelector,void*);

	long onPopup(FXObject*, FXSelector, void*);
	long onPopupClicked(FXObject*, FXSelector, void*);

	long onToggleOwnFactionGroup(FXObject*, FXSelector, void*);
	long onUpdateOwnFactionGroup(FXObject*, FXSelector, void*);

	long onToggleUnitColors(FXObject*,FXSelector,void*);
	long onUpdateUnitColors(FXObject*,FXSelector,void*);

	long onMapChange(FXObject*,FXSelector,void*);
	long onQueryHelp(FXObject*,FXSelector,void*);
	
    bool colorized() const {
        return colorized_units;
    }
public: 
	enum
	{
		ID_POPUP_CLICKED = FXTreeList::ID_LAST,
		ID_TOGGLEOWNFACTIONGROUP,
        ID_TOGGLEUNITCOLORS,
        ID_LAST
	};

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

    FXIcon* terrainIcons[data::TERRAIN_LAST] = {};
	FXIcon *green = nullptr, *red = nullptr, *blue = nullptr, *cyan = nullptr, *yellow = nullptr, *orange = nullptr, *gray = nullptr, *black = nullptr;

	FXFont *boldfont = nullptr;

    bool isConfirmed(const datablock::itor& unit) const;
	// rekursivly searches item with userdata=data in treeitem list
	FXTreeItem* findTreeItem(FXTreeItem* first, void* data);

	bool active_faction_group = false;
	bool active_regions_only = true;
	bool colorized_units = true;

private:
	FXRegionList() {}
    FXRegionList(const FXRegionList&) = delete;

    static FXColor getUnitColor(const datablock* unitPtr);
};

#endif //_CSMAP_REGIONLIST
