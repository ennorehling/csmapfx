#pragma once

#include <fx.h>

#include "properties.h"

class FXShipProperties : public FXProperties
{
	FXDECLARE(FXShipProperties)

public:
    FXShipProperties(FXComposite* p, FXObject* tgt = nullptr, FXSelector sel = 0,
        FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    virtual void makeItems() override;

protected:
    FXShipProperties() {}
    FXShipProperties(const FXShipProperties&) {}
};
