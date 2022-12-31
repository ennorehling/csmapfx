#ifndef CSMAP_REGIONINFO_H
#define CSMAP_REGIONINFO_H
#include "messages.h"

class FXRegionInfo : public FXMessageList
{
    FXDECLARE(FXRegionInfo)

public:
    void create();

    FXRegionInfo() {};
    FXRegionInfo(FXComposite* p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
    virtual ~FXRegionInfo() {}

public:
    virtual long onMapChange(FXObject*, FXSelector, void*);
protected:
    FXTreeItem* messages = nullptr;
    FXTreeItem* effects = nullptr;
    FXTreeItem* streets = nullptr;
    FXTreeItem* travel = nullptr;
    FXTreeItem* guards = nullptr;
    FXTreeItem* battle = nullptr;
};
#endif
