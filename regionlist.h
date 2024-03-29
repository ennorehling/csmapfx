#ifndef _CSMAP_REGIONLIST
#define _CSMAP_REGIONLIST

#include "datafile.h"
#include "terrain.h"

#include <fx.h>

#include <list>

class FXRegionItem;

class FXRegionList : public FXTreeList
{
	FXDECLARE(FXRegionList)

public:
	FXRegionList(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionList();

	void setMapFile(std::shared_ptr<datafile>& f);

public:
	long onSelected(FXObject*,FXSelector,void*);

    long onPopup(FXObject*, FXSelector, void*);
    long showPopup(const FXString& label, datablock* block, FXint root_x, FXint root_y);

	long onToggleOwnFactionGroup(FXObject*, FXSelector, void*);
	long onUpdateOwnFactionGroup(FXObject*, FXSelector, void*);

	long onToggleUnitColors(FXObject*,FXSelector,void*);
	long onUpdateUnitColors(FXObject*,FXSelector,void*);

	long onMapChange(FXObject*,FXSelector,void*);
	long onQueryHelp(FXObject*,FXSelector,void*);
	
    bool colorized() const {
        return colorized_units;
    }

    bool isConfirmed(const datablock* block) const;
    FXFont* getBoldFont() const { return boldfont; }
public: 
	enum
	{
        ID_TOGGLE_UNITCOLORS = FXTreeList::ID_LAST,
		ID_TOGGLE_OWNFACTIONGROUP,
        ID_LAST
	};

protected:
	datafile::SelectionState selection;

    std::shared_ptr<datafile> mapFile;

    FXIcon* terrainIcons[data::TERRAIN_LAST] = {};
	FXIcon *green = nullptr, *red = nullptr, *blue = nullptr, *cyan = nullptr, *yellow = nullptr, *orange = nullptr, *gray = nullptr, *black = nullptr;

	FXFont *boldfont = nullptr;

    bool isConfirmed(const datablock& unit) const;
	// rekursivly searches item with userdata=data in treeitem list
	FXTreeItem* findTreeItem(FXTreeItem* first, void* data);

	bool active_faction_group = false;
	bool colorized_units = true;

private:
	FXRegionList() {}
    FXRegionList(const FXRegionList&) = delete;

    void rebuildTree();
    FXRegionItem* appendRegion(const datablock *regionPtr);
    FXColor getItemColor(const datablock& unit) const;
    friend class FXRegionItem;
};

#endif //_CSMAP_REGIONLIST
