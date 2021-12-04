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
    FXExportDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

	void create();
	virtual ~FXExportDlg();

	long onAccept(FXObject* sender, FXSelector sel, void* ptr);

public:
	int getScale() const;
	int getColor() const;

	bool getShowNames() const;
	bool getShowKoords() const;
	bool getShowIslands() const;

protected:
	FXComboBox*		scalebox;
	FXComboBox*		colorbox;

	FXCheckButton*	shownamesbox;
	FXCheckButton*	showkoordsbox;
	FXCheckButton*	showislandsbox;

protected:
	FXExportDlg(){}
	FXExportDlg(const FXExportDlg&) {}
};

#endif //_CSMAP_EXPORTDLG
