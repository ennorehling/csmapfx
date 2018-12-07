#ifndef _CSMAP_STATSINFOS
#define _CSMAP_STATSINFOS

#include <fx.h>
#include <list>
#include <vector>
#include "datafile.h"

class FXStatsInfos : public FXVerticalFrame
{
	FXDECLARE(FXStatsInfos)

public:
	FXStatsInfos(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXStatsInfos();

	void mapfiles(std::list<datafile> *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_LAST = FXVerticalFrame::ID_LAST,
	};

protected:
	datafile::SelectionState selection;

	std::list<datafile>	*files;

	// Statsinfos
	struct
	{
		FXHorizontalFrame *matrixframe;
		FXVerticalSeparator *matrixsep;
		FXMatrix *leftmatrix, *rightmatrix;
		
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
	void collectData_Ruby(std::list<Info>& info, datablock::itor region);	// collect by calling ruby
	void updateData();

protected:
	FXStatsInfos(){}
	FXStatsInfos(const FXStatsInfos&) {}
};

#endif //_CSMAP_STATSINFOS
