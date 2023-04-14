#ifndef _CSMAP_STATSINFOS
#define _CSMAP_STATSINFOS

#include "datafile.h"

#include <fx.h>

#include <memory>
#include <vector>

class FXStatsPanel : public FXVerticalFrame
{
	FXDECLARE(FXStatsPanel)

public:
	FXStatsPanel(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXStatsPanel();

    void setMapFile(std::shared_ptr<datafile>& f);

public:
	long onMapChange(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_LAST = FXVerticalFrame::ID_LAST,
	};

protected:
	datafile::SelectionState selection;

    std::shared_ptr<datafile> mapFile;

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
		FXlong value;

		Info(const FXString& n, const FXString& t, FXlong v) : name(n), tip(t), value(v) {}
	};

protected:
	void clearLabels();
	void createLabels(const FXString& name, const FXString& info, const FXString& info2, int column);

	void setInfo(const std::vector<Info>& info);
	void addEntry(std::vector<Info>& info, FXString name, FXlong value, FXString tip = "");

	void collectData(std::vector<Info>& info, const datablock::itor& region);
	void updateData();

protected:
	FXStatsPanel(){}
	FXStatsPanel(const FXStatsPanel&) {}
};

#endif //_CSMAP_STATSINFOS
