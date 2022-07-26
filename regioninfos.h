#ifndef _CSMAP_REGIONINFOS
#define _CSMAP_REGIONINFOS

#include "datafile.h"
#include "terrain.h"

#include <fx.h>

#include <list>
#include <vector>

class FXRegionInfos : public FXVerticalFrame
{
	FXDECLARE(FXRegionInfos)

public:
	FXRegionInfos(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionInfos();

    void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onToggleDescription(FXObject*,FXSelector,void*);
	long onUpdateToggleDescription(FXObject*,FXSelector,void*);
	long onUpdateDescription(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_TOGGLEDESCRIPTION = FXVerticalFrame::ID_LAST,
		ID_DESCRIPTION,
		ID_LAST
	};

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

    FXIcon* terrainIcons[data::TERRAIN_LAST] = { 0 };

	bool show_description = false;

	// Regionsinfos
	struct
	{
		FXLabel *name;
        
		FXFrame *matrixsep;
		FXHorizontalFrame *matrixframe;
		FXMatrix *leftmatrix, *rightmatrix;
		
		FXFrame *descsep;
		FXText *desc;

		std::vector<FXLabel*> entries;

	}tags;

	struct Info
	{
		FXString name, tip;
		FXint value, skill;

		Info(const FXString& n, const FXString& t, FXint v, FXint s) : name(n), tip(t), value(v), skill(s) {}
	};

protected:
	void clearLabels();
	void createLabels(const FXString& name, const FXString& label, int column);

	void setInfo(const std::list<Info>& info);
	void addEntry(std::list<Info>& info, FXString name, int value, int skill, FXString tip = "");

	void collectData(std::list<Info>& info, datablock::itor region);
	void updateData();

protected:
	FXRegionInfos() {}
	FXRegionInfos(const FXRegionInfos&) {}
};

#endif
