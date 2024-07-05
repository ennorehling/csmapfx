#ifndef _CSMAP_COMMANDS
#define _CSMAP_COMMANDS

#include "datafile.h"
#include "map.h"

#include <fx.h>

#include <vector>

class FXCommands : public FXText
{
	FXDECLARE(FXCommands)

public:
	FXCommands(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXCommands();

	void setMapFile(std::shared_ptr<datafile>& f);

	void connectMap(FXCSMap* map);

    void makeTemp();
	FXString getFreeTemp();

	void setConfirmed(bool confirmed);		// set if unit's commands are confirmed
	int getConfirmed();						// 0 or 1: commands are confirmed or not, -1: no commandable unit selected

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onKeyPress(FXObject*,FXSelector,void*);
	long onUpdate(FXObject*,FXSelector,void*);
	long onConfirmUnit(FXObject*,FXSelector,void*);
	long onCreateUnit(FXObject*,FXSelector,void*);

	long updRowCol(FXObject*,FXSelector,void*);	// set row and col for this command
	long updCommandable(FXObject*,FXSelector,void*);
	long updConfirmed(FXObject*,FXSelector,void*);
	
public: 
	enum
	{
		ID_ROWCOL = FXText::ID_LAST,
        ID_UNIT_CONFIRM,
        ID_UNIT_ADD,
        ID_LAST
	};
   
protected:
	datafile::SelectionState selection;
    std::shared_ptr<datafile> mapFile;

	FXCSMap				*map = nullptr;				// map to send ROUTE commands to
	int					routeLength = 0;		// length of ROUTE

	std::vector<FXHiliteStyle>	textStyles;

protected:
	FXCommands(){}
	FXCommands(const FXCommands&) {}

	void saveCommands();		// save commands back to unit
	void loadCommands(const datablock::itor &block);		// save commands back to unit
	void highlightText();
	void mapShowRoute();
    void updateSelection();
};

#endif //_CSMAP_COMMANDS
