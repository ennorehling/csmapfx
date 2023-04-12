#pragma once

#include "datafile.h"
#include "terrain.h"

#include <fx.h>

#include <memory>
#include <vector>

class FXRegionPanel : public FXVerticalFrame
{
	FXDECLARE(FXRegionPanel)

public:
	FXRegionPanel(FXComposite* p, FXObject* tgt=NULL,FXSelector sel=0, FXuint opts=0, FXint x=0,FXint y=0,FXint w=0,FXint h=0);

	void create();
	virtual ~FXRegionPanel();

    void setMapFile(std::shared_ptr<datafile>& f);

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

    std::shared_ptr<datafile> mapFile;

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

protected:
	void clearLabels();
	void createLabels(const FXString& name, const FXString& label, int column);

	void setInfo(const std::vector<RegionInfo>& info);
	void addEntry(std::vector<RegionInfo>& info, FXString name, FXulong value, int skill, FXString tip = "");

	void collectData(std::vector<RegionInfo>& info, const datablock::itor& region);
	void updateData();

protected:
	FXRegionPanel() {}
	FXRegionPanel(const FXRegionPanel&) {}
};
