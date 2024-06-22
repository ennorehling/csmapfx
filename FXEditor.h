#pragma once

#include <fx.h>
#include <FXText.h>

class FXEditor: public FXText {

    FXDECLARE(FXEditor)

    public:
        FXEditor(FXComposite *p, FXObject * tgt=NULL, FXSelector sel=0,
            FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
        virtual ~FXEditor() {}
       
        virtual long onKeyPress(FXObject *sender, FXSelector sel, void *ptr);
    protected:
        FXEditor() {};
        FXEditor(const FXEditor&) {};
};

