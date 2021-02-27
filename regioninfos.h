#ifndef _CSMAP_REGIONINFOS
#define _CSMAP_REGIONINFOS

#include <fx.h>
#include <list>
#include <vector>
#include "datafile.h"
#include "terrain.h"

class FXRegionInfos : public FXVerticalFrame
{
	FXDECLARE(FXRegionInfos)

public:
	FXRegionInfos(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionInfos();

	void mapfiles(std::list<datafile> *f);

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

	std::list<datafile>	*files;

	FXIcon			*terrainIcons[data::TERRAIN_LAST];

	bool show_description;

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
		FXint value, skill, offset;

		Info(const FXString& n, const FXString& t, FXint v, FXint s, FXint o) : name(n), tip(t), value(v), skill(s), offset(o) {}
	};

protected:
	void clearLabels();
	void createLabels(const FXString& name, const FXString& info, const FXString& info2, int column);

	void setInfo(const std::list<Info>& info);
	void addEntry(std::list<Info>& info, FXString name, int value, int skill, int offset, FXString tip = "");
	void clearInfo();

	void collectData(std::list<Info>& info, datablock::itor region);
	void updateData();

protected:
	FXRegionInfos(){}
	FXRegionInfos(const FXRegionInfos&) {}
};

#endif //_CSMAP_REGIONINFOS
