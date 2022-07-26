#ifndef _CSMAP_STATSINFOS
#define _CSMAP_STATSINFOS

#include "datafile.h"

#include <fx.h>

#include <list>
#include <vector>

class FXStatsInfos : public FXVerticalFrame
{
	FXDECLARE(FXStatsInfos)

public:
	FXStatsInfos(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXStatsInfos();

    void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_LAST = FXVerticalFrame::ID_LAST,
	};

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

	// Statsinfos
	struct
	{
		FXHorizontalFrame *matrixframe;
		FXVerticalSeparator *matrixsep;
		FXMatrix *leftmatrix, *rightmatrix;
		
		std::vector<FXLabel*> entries;

	} tags;

	struct Info
	{
		FXString name, tip;
		FXint value;

		Info(const FXString& n, const FXString& t, FXint v) : name(n), tip(t), value(v) {}
	};

protected:
	void clearLabels();
	void createLabels(const FXString& name, const FXString& info, const FXString& info2, int column);

	void setInfo(const std::vector<Info>& info);
	void addEntry(std::vector<Info>& info, FXString name, int value, FXString tip = "");

	void collectData(std::vector<Info>& info, datablock::itor region);
	void updateData();

protected:
	FXStatsInfos(){}
	FXStatsInfos(const FXStatsInfos&) {}
};

#endif //_CSMAP_STATSINFOS
