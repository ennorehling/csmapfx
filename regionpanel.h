#pragma once

#include "datafile.h"
#include "terrain.h"

#include <fx.h>

#include <vector>

class FXRegionPanel : public FXVerticalFrame
{
	FXDECLARE(FXRegionPanel)

public:
	FXRegionPanel(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionPanel();

    void setMapFile(datafile *f);

public:
	long onMapChange(FXObject*,FXSelector,void*);
	long onUpdateDescription(FXObject*,FXSelector,void*);

public:

	enum
	{
		ID_DESCRIPTION = FXVerticalFrame::ID_LAST,
		ID_LAST
	};

protected:
	datafile::SelectionState selection;

    datafile *mapFile = nullptr;

    FXIcon* terrainIcons[data::TERRAIN_LAST] = { 0 };

	// Regionsinfos
	struct
	{
		FXLabel *name = nullptr;
        
		FXFrame *matrixsep = nullptr;
		FXHorizontalFrame *matrixframe = nullptr;
		FXMatrix *leftmatrix = nullptr, *rightmatrix = nullptr;
		
		FXFrame *descsep = nullptr;
		FXText *desc = nullptr;

		std::vector<FXLabel*> entries;

	} tags;

	struct Info
	{
		FXString name, tip;
        FXulong value;
		FXint skill;

		Info(const FXString& n, const FXString& t, FXulong v, FXint s) : name(n), tip(t), value(v), skill(s) {}
	};

protected:
	void clearLabels();
	void createLabels(const FXString& name, const FXString& label, int column);

	void setInfo(const std::vector<Info>& info);
	void addEntry(std::vector<Info>& info, FXString name, FXulong value, int skill, FXString tip = "");

	void collectData(std::vector<Info>& info, datablock::itor region);
	void updateData();

protected:
	FXRegionPanel() {}
	FXRegionPanel(const FXRegionPanel&) {}
};
