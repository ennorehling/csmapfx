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

FXExportDlg::FXExportDlg(FXWindow* owner, const FXString& name, FXIcon* icon, FXuint opts, FXuval limitScale, FXint x,FXint y,FXint w,FXint h)
		: FXDialogBox(owner, name, opts, x,y,w,h, 10,10,10,10, 10,10), maxScale(limitScale)
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

	new FXLabel(regionsize, FXString(L"Regionsgr\u00f6\u00dfe"));

	scalebox = new FXComboBox(regionsize, 0, NULL,0, COMBOBOX_STATIC|FRAME_NORMAL);
    for (FXuint i = 1; i <= maxScale; i *= 2) {
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

	new FXLabel(backgrcolor, FXString("Hintergrundfarbe"));

	colorbox = new FXComboBox(backgrcolor, 0, NULL,0, COMBOBOX_STATIC|FRAME_NORMAL);
	colorbox->appendItem(FXString("Schwarz"), (void*)0);
	colorbox->appendItem(FXString(L"Wei\u00df"), (void*)1);
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

void FXExportDlg::loadState(FXRegistry &reg)
{
    shownamesbox->setCheck(reg.readUnsignedEntry("EXPORTDLG", "NAMES", 0) != 0);
    showislandsbox->setCheck(reg.readUnsignedEntry("EXPORTDLG", "ISLANDS", 0) != 0);
    showkoordsbox->setCheck(reg.readUnsignedEntry("EXPORTDLG", "COORDINATES", 0) != 0);
    setScale(reg.readUnsignedEntry("EXPORTDLG", "SCALE", 16));
    setColor(reg.readUnsignedEntry("EXPORTDLG", "COLOR", 0));
}

void FXExportDlg::saveState(FXRegistry &reg)
{
    reg.writeUnsignedEntry("EXPORTDLG", "NAMES", shownamesbox->getCheck());
    reg.writeUnsignedEntry("EXPORTDLG", "ISLANDS", showislandsbox->getCheck());
    reg.writeUnsignedEntry("EXPORTDLG", "COORDINATES", showkoordsbox->getCheck());
    reg.writeUnsignedEntry("EXPORTDLG", "SCALE", getScale());
    reg.writeUnsignedEntry("EXPORTDLG", "COLOR", getColor());
}

FXuval FXExportDlg::getScale() const
{
	int item = scalebox->getCurrentItem();
	if (item < 0)
		return 1;	// no current item?

	return (FXuval)scalebox->getItemData(item);	// item data is scale
}

void FXExportDlg::setScale(FXuval scale)
{
    if (scale > maxScale) scale = maxScale;

    for (FXint i = 0; i != scalebox->getNumItems(); ++i) {
        FXuval val = (FXuval)scalebox->getItemData(i);
        if (val == scale) {
            scalebox->setCurrentItem(i);
            break;
        }
    }    
}

FXuval FXExportDlg::getColor() const
{
	int item = colorbox->getCurrentItem();
	if (item < 0)
		return 0;	// no current item?

	return (FXuval)colorbox->getItemData(item);	// item data is color index (0=black, 1=white)
}

void FXExportDlg::setColor(FXuval color)
{
    for (FXint i = 0; i != colorbox->getNumItems(); ++i) {
        FXuval val = (FXuval)colorbox->getItemData(i);
        if (val == color) {
            colorbox->setCurrentItem(i);
            break;
        }
    }
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
