#pragma once
#ifndef _CSMAP_REPORTINFO
#define _CSMAP_REPORTINFO

#include "messagelist.h"

#include <fx.h>

#include <list>
#include <vector>

class FXReportInfo : public FXMessageList
{
	FXDECLARE(FXReportInfo)

public:
    void create();

	FXReportInfo(FXComposite* p, FXObject* tgt = nullptr, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
    virtual ~FXReportInfo();


    virtual void setMapFile(std::shared_ptr<datafile>& f);

public:
    virtual long onMapChange(FXObject*, FXSelector, void*);

protected:
    void addMessage(FXTreeItem* group, class datablock * msg);
    void addBattle(datablock::itor& block);
    void addFaction(datablock::itor& block);

protected:
	FXReportInfo() {}
    FXReportInfo(const FXReportInfo&) = delete;

private:
    static const char *messageSection(const FXString& section);

    FXTreeItem* messages;
    FXTreeItem* battles;
};

#endif //_CSMAP_MESSAGES

