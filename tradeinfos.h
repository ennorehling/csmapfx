#ifndef _CSMAP_TRADEINFOS
#define _CSMAP_TRADEINFOS

#include "datafile.h"

#include <fx.h>

#include <vector>

class FXTradeInfos : public FXVerticalFrame
{
	FXDECLARE(FXTradeInfos)

public:
	FXTradeInfos(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXTradeInfos();

	void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_LAST = FXVerticalFrame::ID_LAST
	};

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

	// Tradeinfos
	struct
	{
		FXLabel *luxury, *luxuryVolume;
		FXLabel *selled, *selledPrice;
        
		FXFrame *matrixsep;
		FXHorizontalFrame *matrixframe;
		FXMatrix *topmatrix, *leftmatrix, *rightmatrix;
		
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
	void createLabels(const FXString& name, const FXString& info, int column);

	void setInfo(const std::list<Info>& info);
	void addEntry(std::list<Info>& info, FXString name, int value, FXString tip = "");

	void collectData(std::list<Info>& info, datablock::itor region);
	void updateData();

protected:
	FXTradeInfos() {}
	FXTradeInfos(const FXTradeInfos&) {}
};

#endif //_CSMAP_TRADEINFOS
