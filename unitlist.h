#ifndef UNITLIST_H_
#define UNITLIST_H_

#include <fx.h>

#include "properties.h"

class FXUnitList : public FXProperties
{
	FXDECLARE(FXUnitList)

public:
	FXUnitList(FXComposite* p, FXObject* tgt = nullptr, FXSelector sel = 0, 
        FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    virtual void makeItems() override;

protected:
	FXUnitList() {}
	FXUnitList(const FXUnitList&) {}
};

#endif // UNITLIST_H_
