#ifndef _CSMAP_CALCULATOR
#define _CSMAP_CALCULATOR

#include "datafile.h"
#include "map.h"

#include <fx.h>

#include <list>
#include <memory>

class FXCalculator : public FXHorizontalFrame
{
	FXDECLARE(FXCalculator)

public:
	FXCalculator(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXCalculator();

    void setMapFile(std::shared_ptr<datafile> &f);

	void connectMap(FXCSMap* map);

public:
	long onToggleShown(FXObject*, FXSelector, void*);
	long onUpdateShown(FXObject*, FXSelector, void*);
	long onCloseButton(FXObject*, FXSelector, void*);
	long onChanged(FXObject*, FXSelector, void*);
	long onFocusIn(FXObject*, FXSelector, void*);
	long onFocusNext(FXObject*, FXSelector, void*);
	long onFocusPrev(FXObject*, FXSelector, void*);
	long onMapChange(FXObject*, FXSelector, void*);

public: 
	enum
	{
		ID_FORMULA = FXHorizontalFrame::ID_LAST,
        ID_CLOSE,
		ID_LAST
	};

protected:
	datafile::SelectionState selection;

	std::shared_ptr<datafile> mapFile;

	FXHorizontalFrame	*firstline;
	FXHorizontalFrame	*secondline;

	FXTextField			*formula;
	FXTextField			*result;
	FXTextField			*longresult;

	FXIcon				*closeIcon;

	FXCSMap				*map;				// map to send ROUTE commands to

protected:
	FXCalculator(){}
	FXCalculator(const FXCalculator&) {}

};

#endif //_CSMAP_CALCULATOR
