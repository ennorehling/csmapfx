#pragma once

#include "datafile.h"

#include <fx.h>

class FXProperties : public FXTreeList
{
public:
    FXProperties(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    enum
    {
        ID_POPUP_SHOW_INFO = FXTreeList::ID_LAST,
        ID_POPUP_COPY_TEXT,
        ID_POPUP_SELECT,
        ID_LAST
    };

    void setMapFile(datafile* f);

    long onPopup(FXObject*, FXSelector, void*);
    long onCopyText(FXObject*, FXSelector, void*);
    long onShowInfo(FXObject*, FXSelector, void*);
    long onGotoItem(FXObject*, FXSelector, void*);
    long onQueryHelp(FXObject*, FXSelector, void*);
    long onMapChange(FXObject*, FXSelector, void*);

    static FXString weightToString(int prop);
    static FXString coastToString(int prop);

protected:
    virtual void makeItems() = 0;
    FXTreeItem* makeItem(const FXString& label, datablock* block, const FXString* info = nullptr);
    FXTreeItem* makeStringList(FXTreeItem* parent, const FXString& label, const datablock& block);
    FXTreeItem* makeUnitList(FXTreeItem* parent, const FXString& label, datablock::itor begin, datablock::itor end, key_type matchKey, int matchValue);
    void setClipboard(const FXString& text);
    void showInfo(const FXString& text);

    FXProperties() {}
	FXProperties(const FXProperties&) {}

    datafile::SelectionState selection;
    datafile* mapFile = nullptr;
};
