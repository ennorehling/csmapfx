#ifndef CSMAP_REGIONINFO_H
#define CSMAP_REGIONINFO_H
#include "messagelist.h"

class FXRegionInfo : public FXMessageList
{
public:
    void create();

    FXRegionInfo(FXComposite* p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
    virtual ~FXRegionInfo() {}

public:
    virtual long onMapChange(FXObject*, FXSelector, void*);

protected:
    FXRegionInfo() {}
    FXRegionInfo(const FXRegionInfo&) = delete;

    FXTreeItem* regionMessages = nullptr;
    FXTreeItem* unitMessages = nullptr;
    FXTreeItem* effects = nullptr;
    FXTreeItem* streets = nullptr;
    FXTreeItem* travel = nullptr;
    FXTreeItem* guards = nullptr;
    FXTreeItem* battle = nullptr;
};
#endif
