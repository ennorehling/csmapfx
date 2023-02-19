#ifndef _CSMAP_TRADEINFOS
#define _CSMAP_TRADEINFOS

#include "datafile.h"

#include <fx.h>

#include <memory>
#include <vector>

class FXTradePanel : public FXVerticalFrame
{
	FXDECLARE(FXTradePanel)

public:
	FXTradePanel(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXTradePanel();

	void setMapFile(std::shared_ptr<datafile>& f);

public:
	long onMapChange(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_LAST = FXVerticalFrame::ID_LAST
	};

protected:
	datafile::SelectionState selection;

    std::shared_ptr<datafile> mapFile;

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

	void setInfo(const std::vector<Info>& info);
	void addEntry(std::vector<Info>& info, FXString name, int value, FXString tip = "");

	void collectData(std::vector<Info>& info, datablock::itor region);
	void updateData();

protected:
	FXTradePanel() {}
	FXTradePanel(const FXTradePanel&) {}
};

#endif //_CSMAP_TRADEINFOS
