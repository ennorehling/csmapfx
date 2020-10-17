#include "main.h"
#include "fxhelper.h"
#include "exportdlg.h"

// *********************************************************************************************************
// *** FXMessages implementation

FXDEFMAP(FXExportDlg) MessageMap[]=
{
	//________Message_Type_____________________ID_______________Message_Handler_______
	FXMAPFUNC(SEL_COMMAND,				FXExportDlg::ID_ACCEPT,					FXExportDlg::onAccept),
};

FXIMPLEMENT(FXExportDlg,FXDialogBox,MessageMap, ARRAYNUMBER(MessageMap))

FXExportDlg::FXExportDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXDialogBox(owner, name, opts, x,y,w,h, 10,10,10,10, 10,10)
{
	setIcon(icon);

    // buttons on bottom of the dialog
	FXHorizontalFrame* buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0);
	new FXButton(buttons, "&Abbrechen", NULL, this,ID_CANCEL, BUTTON_DEFAULT|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	new FXButton(buttons, "&Ok", NULL, this,ID_ACCEPT, BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_RIGHT);

	new FXHorizontalSeparator(this, SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

	// options
	FXVerticalFrame* vfoptions = new FXVerticalFrame(this, PACK_UNIFORM_HEIGHT);

	FXHorizontalFrame* regionsize = new FXHorizontalFrame(vfoptions, LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT);

	new FXLabel(regionsize, iso2utf("Regionsgr\u00f6\u00dfe"));

	scalebox = new FXComboBox(regionsize, 0, NULL,0, COMBOBOX_STATIC|FRAME_NORMAL);
    for (FXint i = 1; i <= 128; i *= 2) {
        scalebox->appendItem(FXStringVal(i), (void*)(FXival)i);
    }
	scalebox->setCurrentItem(4);
	scalebox->setNumVisible(scalebox->getNumItems());

	// what to show in the picture?
	shownamesbox = new FXCheckButton(vfoptions, "Regions&namen anzeigen", NULL,0, CHECKBUTTON_NORMAL);
	showkoordsbox = new FXCheckButton(vfoptions, "&Koordinaten anzeigen", NULL,0, CHECKBUTTON_NORMAL);
	showislandsbox = new FXCheckButton(vfoptions, "&Inselnamen anzeigen", NULL,0, CHECKBUTTON_NORMAL);

	// background color
	FXHorizontalFrame* backgrcolor = new FXHorizontalFrame(vfoptions, LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT);

	new FXLabel(backgrcolor, iso2utf("Hintergrundfarbe"));

	colorbox = new FXComboBox(backgrcolor, 0, NULL,0, COMBOBOX_STATIC|FRAME_NORMAL);
	colorbox->appendItem(iso2utf("Schwarz"), (void*)0);
	colorbox->appendItem(iso2utf("Wei\u00df"), (void*)1);
	colorbox->setCurrentItem(0);
	colorbox->setNumVisible(colorbox->getNumItems());
}

void FXExportDlg::create()
{
	FXDialogBox::create();
}

FXExportDlg::~FXExportDlg()
{
}

long FXExportDlg::onAccept(FXObject* sender, FXSelector sel, void* ptr)
{
	return FXDialogBox::onCmdAccept(sender, sel, ptr);
}

int FXExportDlg::getScale() const
{
	int item = scalebox->getCurrentItem();
	if (item < 0)
		return 1;	// no current item?

	return (FXival)scalebox->getItemData(item);	// item data is scale
}

int FXExportDlg::getColor() const
{
	int item = colorbox->getCurrentItem();
	if (item < 0)
		return 1;	// no current item?

	return (FXival)colorbox->getItemData(item);	// item data is color index (0=black, 1=white)
}

bool FXExportDlg::getShowNames() const
{
	return shownamesbox->getCheck() ? true:false;
}

bool FXExportDlg::getShowKoords() const
{
	return showkoordsbox->getCheck() ? true:false;
}

bool FXExportDlg::getShowIslands() const
{
	return showislandsbox->getCheck() ? true:false;
}
