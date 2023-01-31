#include "contextmenu.h"

#include "datablock.h"
#include "csmap.h"

FXDEFMAP(FXContextMenu) MessageMap[] = {
    FXMAPFUNC(SEL_COMMAND,			FXContextMenu::ID_POPUP_SHOW_INFO,		FXContextMenu::onShowInfo),
    FXMAPFUNC(SEL_COMMAND,			FXContextMenu::ID_POPUP_COPY_TEXT,		FXContextMenu::onCopyText),
    FXMAPFUNC(SEL_COMMAND,			FXContextMenu::ID_POPUP_GOTO,			FXContextMenu::onGotoObject)
};

FXIMPLEMENT(FXContextMenu, FXMenuPane, MessageMap, ARRAYNUMBER(MessageMap))

long FXContextMenu::onCopyText(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* menuitem = static_cast<FXMenuCommand*>(sender);
    const char *text = static_cast<const char*>(menuitem->getUserData());
    setClipboard(text);
    return 0;
}

long FXContextMenu::onShowInfo(FXObject* sender, FXSelector sel, void* ptr)
{
    const char* text = static_cast<const char*>(ptr);
    showInfo(text);
    setClipboard(text);
    return 0;
}

long FXContextMenu::onGotoObject(FXObject* sender, FXSelector sel, void* ptr)
{
    FXMenuCommand* command = static_cast<FXMenuCommand*>(sender);
    datablock* block = static_cast<datablock *>(command->getUserData());
    if (block) {
        gotoObject(block);
    }
    return 0;
}

void FXContextMenu::setClipboard(const char* text)
{
    getOwner()->handle(this, FXSEL(SEL_CLIPBOARD_REQUEST, ID_SETSTRINGVALUE), const_cast<char*>(text));
}

void FXContextMenu::showInfo(const char* text)
{
    getOwner()->handle(this, FXSEL(SEL_QUERY_HELP, ID_SETSTRINGVALUE), const_cast<char*>(text));
}

void FXContextMenu::gotoObject(datablock* block)
{
    getOwner()->handle(this, FXSEL(SEL_COMMAND, ID_SETVALUE), block);
}
