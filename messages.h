#pragma once
#ifndef _CSMAP_MESSAGES
#define _CSMAP_MESSAGES

#include "datafile.h"

#include <fx.h>

#include <list>
#include <vector>

class FXMessageList : public FXTreeList
{
	FXDECLARE(FXMessageList)

public:
	FXMessageList(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXMessageList();

    void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onDoubleClick(FXObject*,FXSelector,void*);

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

    void addMessage(FXTreeItem*, datablock * block);

    struct
    {
        FXTreeItem* messages = nullptr;
        FXTreeItem* effects = nullptr;
        FXTreeItem* streets = nullptr;
        FXTreeItem* travel = nullptr;
        FXTreeItem* guards = nullptr;
        FXTreeItem* battle = nullptr;
    } groups;

	void clearSiblings(FXTreeItem* parent);

protected:
	FXMessageList() {}
	FXMessageList(const FXMessageList&) {}

};

#endif //_CSMAP_MESSAGES

