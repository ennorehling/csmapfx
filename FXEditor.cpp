#include "FXEditor.h"

FXDEFMAP(FXEditor) MessageMap[]=
{
    FXMAPFUNC(SEL_KEYPRESS, 0, FXEditor::onKeyPress)
};

FXIMPLEMENT(FXEditor, FXText, MessageMap, ARRAYNUMBER(MessageMap))

FXEditor::FXEditor(FXComposite *p, FXObject * tgt, FXSelector sel,
            FXuint opts, FXint x, FXint y, FXint w, FXint h) :
    FXText(p, tgt, sel, opts, x, y, w, h)
{
};

long FXEditor::onKeyPress(FXObject *sender, FXSelector sel, void *ptr)
{
	if (isEnabled()) {
        FXEvent* event=(FXEvent*)ptr;
    }
    return FXText::onKeyPress(sender, sel, ptr);
}
