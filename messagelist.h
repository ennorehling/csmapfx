#pragma once
#ifndef _CSMAP_MESSAGES
#define _CSMAP_MESSAGES

#include "datafile.h"

#include <fx.h>

#include <list>
#include <memory>
#include <vector>

class FXMessageList : public FXTreeList
{
    FXDECLARE(FXMessageList)

public:
	FXMessageList(FXComposite* p, FXObject* tgt=NULL, FXSelector sel=0, FXuint opts=0, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

	void create();
    virtual ~FXMessageList() {}

    virtual void setMapFile(std::shared_ptr<datafile>& f);

public:
    virtual long onMapChange(FXObject*, FXSelector, void*);
    long onPopupCopy(FXObject*, FXSelector, void*);

    long onDoubleClicked(FXObject*, FXSelector, void*);
    long onRightBtnRelease(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);

    void showPopup(FXTreeItem* item, FXint x, FXint y);

    enum
    {
        ID_MESSAGELIST = FXTreeList::ID_LAST,
        ID_POPUP_COPY,
        ID_LAST
    };

protected:
    void setClipboard(const FXString& text);

    datafile::SelectionState selection;

    std::shared_ptr<datafile> mapFile;

    void addMessage(FXTreeItem*, const datablock& block);

	void clearChildren(FXTreeItem* parent);

protected:
	FXMessageList() {}
	FXMessageList(const FXMessageList&) {}

};

#endif //_CSMAP_MESSAGES

