#pragma once

#include "datafile.h"

#include <fx.h>

class FXProperties : public FXTreeList
{
public:
    FXProperties(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    void setMapFile(std::shared_ptr<datafile>& f);

    long onPopup(FXObject*, FXSelector, void*);
    long onMapChange(FXObject*, FXSelector, void*);

    static FXString weightToString(int prop);
    static FXString coastToString(int prop);

protected:
    virtual void makeItems() = 0;
    FXTreeItem* makeItem(const FXString& label, datablock* block, const FXString* info = nullptr);
    FXTreeItem* makeStringList(FXTreeItem* parent, const FXString& label, const datablock& block);
    FXTreeItem* makeUnitList(FXTreeItem* parent, const FXString& label, datablock::itor begin, datablock::itor end, key_type matchKey, int matchValue);

    FXProperties() {}
	FXProperties(const FXProperties&) {}

    datafile::SelectionState selection;
    std::shared_ptr<datafile> mapFile;
};
