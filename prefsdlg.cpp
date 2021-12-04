#include "prefsdlg.h"

FXDEFMAP(FXPrefsDlg) MessageMap[] =
{
    //________Message_Type_____________________ID_______________Message_Handler_______
    FXMAPFUNC(SEL_COMMAND,                FXPrefsDlg::ID_ACCEPT,                    FXPrefsDlg::onAccept),
};

FXIMPLEMENT(FXPrefsDlg, FXDialogBox, MessageMap, ARRAYNUMBER(MessageMap))

FXPrefsDlg::FXPrefsDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts, FXint x, FXint y, FXint w, FXint h)
    : FXDialogBox(owner, name, opts, opts, x, y, w, h)
{
    setIcon(icon);

    // buttons on bottom of the dialog
    FXHorizontalFrame* buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    new FXButton(buttons, "&Abbrechen", NULL, this, ID_CANCEL, BUTTON_DEFAULT | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_RIGHT);
    new FXButton(buttons, "&Ok", NULL, this, ID_ACCEPT, BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_RIGHT);

    // new FXHorizontalSeparator(this, SEPARATOR_GROOVE | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X);
}

long FXPrefsDlg::onAccept(FXObject* sender, FXSelector sel, void* ptr)
{
    return FXDialogBox::onCmdAccept(sender, sel, ptr);
}
