#ifndef _CSMAP_MESSAGES
#define _CSMAP_MESSAGES

#include "datafile.h"

#include <fx.h>

#include <list>
#include <vector>

class FXMessages : public FXTreeList
{
	FXDECLARE(FXMessages)

public:
	FXMessages(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXMessages();

    void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onDoubleClick(FXObject*,FXSelector,void*);

protected:
	datafile::SelectionState selection;

    datafile *mapFile;

	struct
	{
		FXTreeItem *effects;
		FXTreeItem *streets;
		FXTreeItem *travel;
		FXTreeItem *other;
		FXTreeItem *guards;

	}groups;

	void clearSiblings(FXTreeItem* parent);

protected:
	FXMessages(){}
	FXMessages(const FXMessages&) {}

};

#endif //_CSMAP_MESSAGES

