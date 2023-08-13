#pragma once

#ifndef _CSMAP_EXPORTDLG
#define _CSMAP_EXPORTDLG

#include <fx.h>
#include <list>
#include <vector>
#include "datafile.h"

class FXExportDlg : public FXDialogBox
{
	FXDECLARE(FXExportDlg)

public:
    FXExportDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXuval limitScale = 128, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

	void create();
	virtual ~FXExportDlg();

	long onAccept(FXObject* sender, FXSelector sel, void* ptr);

    void loadState(FXRegistry &reg);
    void saveState(FXRegistry &reg);

public:
    FXuval getScale() const;
    void setScale(FXuval scale);
    FXuval getColor() const;
    void setColor(FXuval color);

	bool getShowNames() const;
	bool getShowKoords() const;
	bool getShowIslands() const;

protected:
    FXuval maxScale = 64;
	FXComboBox*		scalebox = nullptr;
	FXComboBox*		colorbox = nullptr;

	FXCheckButton*	shownamesbox = nullptr;
	FXCheckButton*	showkoordsbox = nullptr;
	FXCheckButton*	showislandsbox = nullptr;

protected:
	FXExportDlg(){}
	FXExportDlg(const FXExportDlg&) {}
};

#endif //_CSMAP_EXPORTDLG
