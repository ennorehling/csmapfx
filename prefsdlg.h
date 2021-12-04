#pragma once

#ifndef _CSMAP_PrefsDLG
#define _CSMAP_PrefsDLG

#include <fx.h>

class FXPrefsDlg : public FXDialogBox
{
    FXDECLARE(FXPrefsDlg)

public:
    FXPrefsDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
    virtual ~FXPrefsDlg() {}

    void create();

    long onAccept(FXObject* sender, FXSelector sel, void* ptr);

protected:
    FXPrefsDlg() {}
    FXPrefsDlg(const FXPrefsDlg&) {}
};

#endif // _CSMAP_PrefsDLG
