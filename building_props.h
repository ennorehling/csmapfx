#pragma once

#include <fx.h>

#include "properties.h"

class FXBuildingProperties : public FXProperties
{
	FXDECLARE(FXBuildingProperties)

public:
    FXBuildingProperties(FXComposite* p, FXObject* tgt = nullptr, FXSelector sel = 0,
        FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    virtual void makeItems() override;

protected:
    FXBuildingProperties() {}
    FXBuildingProperties(const FXBuildingProperties&) {}
};
