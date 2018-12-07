#ifndef _CSMAP_REGIONLIST
#define _CSMAP_REGIONLIST

#include <fx.h>
#include <list>
#include "datafile.h"

class FXRegionList : public FXTreeList
{
	FXDECLARE(FXRegionList)

public:
	FXRegionList(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionList();

	void mapfiles(std::list<datafile> *f);

public:
	long onSelected(FXObject*,FXSelector,void*);

	long onPopup(FXObject*,FXSelector,void*);
	long onPopupClicked(FXObject*,FXSelector,void*);

	long onToggleOwnFactionGroup(FXObject*,FXSelector,void*);
	long onUpdateOwnFactionGroup(FXObject*,FXSelector,void*);

	long onMapChange(FXObject*,FXSelector,void*);
	long onQueryHelp(FXObject*,FXSelector,void*);
	
public: 
	enum
	{
		ID_POPUP_CLICKED = FXTreeList::ID_LAST,
		ID_TOGGLEOWNFACTIONGROUP,
		ID_LAST
	};

protected:
	datafile::SelectionState selection;

	std::list<datafile>	*files;

	FXIcon			*terrainIcons[datablock::TERRAIN_LAST];
	FXIcon			*green, *red, *blue, *cyan, *yellow, *orange, *gray, *black;

	FXFont			*boldfont;

	// rekursivly searches item with userdata=data in treeitem list
	FXTreeItem* findTreeItem(FXTreeItem* first, void* data);

	bool active_faction_group;

protected:
	FXRegionList(){}
	FXRegionList(const FXRegionList&) {}

};

#endif //_CSMAP_REGIONLIST
