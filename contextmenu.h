#pragma once

#include <fx.h>
#include "datafile.h"

class FXContextMenu : public FXMenuPane
{
    FXDECLARE(FXContextMenu)
public:
    FXContextMenu(FXWindow* owner, FXuint opts = 0) : FXMenuPane(owner, 0) {}
    enum
    {
        ID_POPUP_GOTO = FXMenuPane::ID_LAST,
        ID_POPUP_SHOW_INFO,
        ID_POPUP_COPY_TEXT,
        ID_LAST
    };

    long onCopyText(FXObject* sender, FXSelector sel, void* ptr);
    long onShowInfo(FXObject* sender, FXSelector sel, void* ptr);
    long onGotoObject(FXObject* sender, FXSelector sel, void* ptr);

private:
    datafile::SelectionState selection;

    void setClipboard(const char *text);
    void showInfo(const char* text);

    void gotoObject(class datablock* block);
    FXContextMenu() : FXMenuPane() {}
    FXContextMenu(const FXContextMenu&) = delete;
};
