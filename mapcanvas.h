#ifndef _CSMAP_MAPCANVAS
#define _CSMAP_MAPCANVAS

#include <fx.h>

// FXCanvas subclasses: Forward Query-help to parent

class FXCSMapCanvas : public FXCanvas
{
	FXDECLARE(FXCSMapCanvas)

public:
	FXCSMapCanvas(FXComposite* p, FXObject *tgt, FXSelector sel, FXuint opts=(FRAME_SUNKEN|FRAME_THICK), FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	long onQueryHelp(FXObject*,FXSelector,void*); 

protected:
	FXCSMapCanvas(){}
	FXCSMapCanvas(const FXCSMapCanvas&) {}
};

#endif //_CSMAP_MAPCANVAS
