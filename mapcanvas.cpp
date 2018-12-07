#include "mapcanvas.h"

FXDEFMAP(FXCSMapCanvas) CanvasMessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_QUERY_HELP,			0,					FXCSMapCanvas::onQueryHelp), 
}; 

FXIMPLEMENT(FXCSMapCanvas,FXCanvas,CanvasMessageMap, ARRAYNUMBER(CanvasMessageMap))

FXCSMapCanvas::FXCSMapCanvas(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXCanvas(p, tgt, sel, opts, x, y, w, h)
{
	// nothing else to do
}

long FXCSMapCanvas::onQueryHelp(FXObject* sender, FXSelector sel, void* ptr)
{	// forward 1:1 to target
	if (target)
		return target->handle(sender, sel, ptr);
	else
		return 0;
}
