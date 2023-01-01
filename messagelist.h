#pragma once
#ifndef _CSMAP_MESSAGES
#define _CSMAP_MESSAGES

#include "datafile.h"

#include <fx.h>

#include <list>
#include <vector>

class FXMessageList : public FXTreeList
{
public:
	FXMessageList(FXComposite* p, FXObject* tgt=NULL, FXSelector sel=0, FXuint opts=0, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

	void create();
    virtual ~FXMessageList() {}

    virtual void setMapFile(datafile *f);

public:
    virtual long onMapChange(FXObject*, FXSelector, void*);
	long onDoubleClick(FXObject*, FXSelector, void*);

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

    void addMessage(FXTreeItem*, datablock * block);

	void clearSiblings(FXTreeItem* parent);

protected:
	FXMessageList() {}
	FXMessageList(const FXMessageList&) {}

};

#endif //_CSMAP_MESSAGES

