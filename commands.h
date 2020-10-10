#ifndef _CSMAP_COMMANDS
#define _CSMAP_COMMANDS

#include <fx.h>
#include <list>
#include <vector>
#include "datafile.h"
#include "map.h"

class FXCommands : public FXText
{
	FXDECLARE(FXCommands)

public:
	FXCommands(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXCommands();

	void mapfiles(std::list<datafile> *f);

	void connectMap(FXCSMap* map);

    long onNextUnit(FXObject *, FXSelector, void *); // goto next unconfirmed unit
    long onPrevUnit(FXObject *, FXSelector, void *); // goto previous

    void makeTemp();
	FXString getFreeTemp();

	void setConfirmed(bool confirmed);		// set if unit's commands are confirmed
	int getConfirmed();						// 0 or 1: commands are confirmed or not, -1: no commandable unit selected

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onKeyPress(FXObject*,FXSelector,void*);
	long onUpdate(FXObject*,FXSelector,void*);

	long onRowCol(FXObject*,FXSelector,void*);	// set row and col for this command
	
public: 
	enum
	{
		ID_ROWCOL = FXText::ID_LAST,
		ID_LAST
	};
   
protected:
	datafile::SelectionState selection;

	std::list<datafile>	*files;

	FXCSMap				*map;				// map to send ROUTE commands to
	int					routeLength;		// length of ROUTE

	std::vector<FXHiliteStyle>	textStyles;

protected:
	FXCommands(){}
	FXCommands(const FXCommands&) {}

	void saveCommands();		// save commands back to unit
	void highlightText();
	void mapShowRoute();

};

#endif //_CSMAP_COMMANDS
