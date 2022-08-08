#pragma once
#ifndef _CSMAP_REPORTINFO
#define _CSMAP_REPORTINFO

#include "datafile.h"

#include <fx.h>

#include <list>
#include <vector>

class FXReportInfo : public FXTreeList
{
	FXDECLARE(FXReportInfo)

public:
	FXReportInfo(FXComposite* p, FXObject* tgt = nullptr, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

	void create();
	virtual ~FXReportInfo();

    void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onDoubleClick(FXObject*,FXSelector,void*);

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

    void addMessage(FXTreeItem* group, class datablock * msg);
    void addBattle(datablock::itor& block);
    void addFaction(datablock::itor& block);

    struct
    {
        FXTreeItem* messages;
        FXTreeItem* factions;
        FXTreeItem* battles;
    } groups = { 0 };

	void clearSiblings(FXTreeItem* parent);

protected:
	FXReportInfo() {}
	FXReportInfo(const FXReportInfo&) {}

private:
    static const char *messageSection(const FXString& section);
};

#endif //_CSMAP_MESSAGES

