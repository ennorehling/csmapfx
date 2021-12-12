#include <sstream>

#include "fxkeys.h"
#include "main.h"
#include "fxhelper.h"
#include "calc.h"
#include "symbols.h"
#include "tinyjs/TinyJS.h"
#include "tinyjs/TinyJS_MathFunctions.h"
#include "tinyjs/TinyJS_Functions.h"

// *********************************************************************************************************
// *** FXCalculator implementation

FXDEFMAP(FXCalculator) MessageMap[]=
{ 
    //________Message_Type_____________________ID_______________Message_Handler_______ 
    FXMAPFUNC(SEL_COMMAND,           FXCalculator::ID_TOGGLESHOWN,     FXCalculator::onToggleShown), 
    FXMAPFUNC(SEL_UPDATE,            FXCalculator::ID_TOGGLESHOWN,     FXCalculator::onUpdateShown),

    FXMAPFUNC(SEL_FOCUSIN,           0,                                FXCalculator::onFocusIn),
    FXMAPFUNC(SEL_FOCUS_NEXT,        0,                                FXCalculator::onFocusNext),
    FXMAPFUNC(SEL_FOCUS_DOWN,        0,                                FXCalculator::onFocusNext),
    FXMAPFUNC(SEL_FOCUS_RIGHT,       0,                                FXCalculator::onFocusNext),
    FXMAPFUNC(SEL_FOCUS_PREV,        0,                                FXCalculator::onFocusPrev),
    FXMAPFUNC(SEL_FOCUS_UP,          0,                                FXCalculator::onFocusPrev),
    FXMAPFUNC(SEL_FOCUS_LEFT,        0,                                FXCalculator::onFocusPrev),

    FXMAPFUNC(SEL_COMMAND,           FXCalculator::ID_UPDATE,          FXCalculator::onMapChange),

    FXMAPFUNC(SEL_CHANGED,           FXCalculator::ID_FORMULA,         FXCalculator::onChanged)
}; 

FXIMPLEMENT(FXCalculator,FXHorizontalFrame,MessageMap, ARRAYNUMBER(MessageMap))

FXCalculator::FXCalculator(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
        : FXHorizontalFrame(p, opts, x,y,w,h, 3,3,2,2, 0,0), map(NULL)
{
    // set target etc.
    setTarget(tgt);
    setSelector(sel);

    // set layout
    setFrameStyle(FRAME_LINE);
    setBorderColor(getApp()->getShadowColor());

    // close button icon
    if (p && p->getParent()) {
        if (p->getParent()->getParent())
        {
            closeIcon = new FXGIFIcon(getApp(), data::small_x, FXRGB(255, 255, 255), IMAGE_ALPHACOLOR);
            new FXButton(this,
                L"\t\tTaschenrechnerleiste schliessen",
                closeIcon, this, FXCalculator::ID_CLOSE, BUTTON_TOOLBAR);
        }
    }

    new FXLabel(this, "Rechner ", 0, LAYOUT_CENTER_Y);

    FXVerticalFrame *frame = new FXVerticalFrame(this, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);
    firstline = new FXHorizontalFrame(frame, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);

    formula = new FXTextField(firstline, 10, this,ID_FORMULA, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    formula->setHelpText("Taschenrechner");

    result = new FXTextField(firstline, 13, NULL,0, TEXTFIELD_READONLY|JUSTIFY_RIGHT|LAYOUT_FILL_Y);
    result->setBackColor(getBackColor());

    // second line
    secondline = new FXHorizontalFrame(frame, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);
    secondline->hide();

    longresult = new FXTextField(secondline, 13, NULL,0, TEXTFIELD_READONLY|LAYOUT_FILL_X|LAYOUT_FILL_Y);
    longresult->setBackColor(getBackColor());

}

void FXCalculator::create()
{
    FXHorizontalFrame::create();
}

FXCalculator::~FXCalculator()
{
}

void FXCalculator::setMapFile(datafile *f) {
    mapFile = f;
}

void FXCalculator::connectMap(FXCSMap* map_)
{
    map = map_;
}

long FXCalculator::onToggleShown(FXObject*, FXSelector, void*)
{
    if (shown())
        hide();
    else
        show();

    recalc();
    return 1;  
}

long FXCalculator::onUpdateShown(FXObject* sender, FXSelector, void*)
{
    sender->handle(this, FXSEL(SEL_COMMAND, shown()?ID_CHECK:ID_UNCHECK), NULL);
    return 1;
}

long FXCalculator::onCloseButton(FXObject*, FXSelector, void*)
{
    if (shown())
    {
        hide();
        recalc();
    }
    return 1;
}

long FXCalculator::onFocusIn(FXObject* sender, FXSelector sel, void* ptr)
{
    long res = FXHorizontalFrame::onFocusIn(sender, sel, ptr);

    if (formula->hasFocus())
        formula->selectAll();
    else if (!getFocus())
        formula->setFocus();

    return res;
}

long FXCalculator::onFocusNext(FXObject* sender, FXSelector sel, void* ptr)
{
    if (!getFocus())
        return formula->handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
    else
        return 0;
}

long FXCalculator::onFocusPrev(FXObject* sender, FXSelector sel, void* ptr)
{
    if (!getFocus())
        return formula->handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
    else
        return 0;
}

long FXCalculator::onMapChange(FXObject* /*sender*/, FXSelector, void* ptr)
{
    datafile::SelectionState *state = (datafile::SelectionState*)ptr;

    getApp()->beginWaitCursor();

    bool needUpdate = false;

    if (selection.fileChange != state->fileChange)
    {
        selection.fileChange = state->fileChange;
        selection.map = state->map;
        selection.activefaction = state->activefaction;

        needUpdate = true;
    }

    if (selection.selChange != state->selChange)
    {
        selection.selChange = state->selChange;
        selection.selected = state->selected;
        
        selection.region = state->region;
        selection.faction = state->faction;
        selection.building = state->building;
        selection.ship = state->ship;
        selection.unit = state->unit;

        needUpdate = true;
    }

    if (needUpdate)
    {
        onChanged(this, 0, NULL);
    }
    
    getApp()->endWaitCursor();
    return 1;
}

long FXCalculator::onChanged(FXObject*, FXSelector, void*)
{
    FXString exp = formula->getText();

    bool changed = false;
    for (int pos = exp.find_first_of("\t\r\n"); pos != -1; pos = exp.find_first_of("\t\r\n"))
    {
        changed = true;
        exp.replace(pos, 1, " ");
    }

    if (changed)
        formula->setText(exp);

    // if ROUTE or NACH, send to map
    if (map)
    {
        FXString text;

        if (exp.before(' ').upper() == "NACH" || exp.before(' ').upper() == "ROUTE")
        {
            int length = map->sendRouteCmds(exp, 1);

            text = "Regionen: " + FXStringVal(length);
        }
        else
            map->sendRouteCmds("", 1);

        result->setText(text);
        longresult->setText("");
        
        if (text.length())
        {
            if (secondline->shown())
            {
                secondline->hide();
                recalc();
            }

            return 1;
        }
    }

    std::string resultstr;
    CTinyJS s;
    registerFunctions(&s);
    registerMathFunctions(&s);
    try {
        std::string code(exp.text());
        resultstr = s.evaluate(code);
    }
    catch (CScriptException *e) {
        resultstr.assign(e->text.c_str());
        delete e;
    }

    if (resultstr.size() > 15)
    {
        result->setText("");
        longresult->setText( FXString(resultstr.c_str()) );
        secondline->show();
    }
    else
    {
        result->setText( FXString(resultstr.c_str()) );
        secondline->hide();
    }
    recalc();

    return 1;
}
