#ifndef _CSMAP_UNITLIST
#define _CSMAP_UNITLIST

#include "datafile.h"

#include <fx.h>

#include <memory>

class FXUnitList : public FXVerticalFrame
{
	FXDECLARE(FXUnitList)

public:
	FXUnitList(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXUnitList();

    void setMapFile(std::shared_ptr<datafile> &f);

	void setClipboard(const FXString& text);
	void showInfo(const FXString& text);
	FXString trimNumbers(const FXString& str) const;
	FXString getSubTreeText(const FXTreeItem* item) const;

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onQueryHelp(FXObject*,FXSelector,void*);

	long onPopup(FXObject*,FXSelector,void*);
	long onCopyTree(FXObject*,FXSelector,void*);
	long onCopyText(FXObject*,FXSelector,void*);
	long onShowInfo(FXObject*,FXSelector,void*);

public: 
	enum
	{
		ID_LIST = FXVerticalFrame::ID_LAST,		// tree list events
        ID_POPUP_SHOW_INFO,
        ID_POPUP_COPY_TREE,
        ID_POPUP_COPY_TEXT,
		ID_LAST
	};

protected:
	datafile::SelectionState selection;

    std::shared_ptr<datafile> mapFile;

	FXTreeList *list;

protected:
	FXUnitList(){}
	FXUnitList(const FXUnitList&) {}

};

#endif //_CSMAP_UNITLIST
