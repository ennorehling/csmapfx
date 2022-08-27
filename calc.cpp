#include <sstream>

#include "fxkeys.h"
#include "main.h"
#include "fxhelper.h"
#include "calc.h"
#include "symbols.h"
#undef USE_MJS
#ifdef USE_MJS
#include "mjs/mjs.h"
#else
#define CEVAL_STOICAL
#include "ceval.h"
#endif

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

    new FXLabel(this, "&Rechner ", 0, LAYOUT_CENTER_Y);

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
    datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

    if (selection.fileChange != pstate->fileChange || selection.selChange != pstate->selChange)
    {
        selection = *pstate;

        onChanged(this, 0, NULL);
    }
    
    return 1;
}

static FXString ev_format(double v) {
    FXString x = FXStringFormat("%lf", v);
    FXint pos = x.length();
    while(--pos > 0) {
        if (x[pos] != '0') break;
    }
    if (x[pos] == '.') --pos;
    x.trunc(pos+1);
    return x;
}

#ifdef USE_MJS
static double ev_result;
static void ffi_ev(double v) {
    ev_result = v;
}

static void* my_dlsym(void* handle, const char* name) {
    if (strcmp(name, "ev") == 0) return (void*)ffi_ev;
    return NULL;
}
#endif

static FXString evaluate(const char* expr)
{
#ifdef USE_MJS
    FXString text(expr);
    if (!text.trim().empty()) {
        FXString script = FXStringFormat("let _ = ffi('void ev(double)'); _(%s)", text.text());
        struct mjs* mjs = mjs_create();
        mjs_set_ffi_resolver(mjs, my_dlsym);
        mjs_err_t err = mjs_exec(mjs, script.text(), nullptr);
        mjs_destroy(mjs);
        switch (err) {
        case MJS_OK:
            return ev_format(ev_result);
        case MJS_SYNTAX_ERROR:
            return FXString("Syntax Error");
        case MJS_REFERENCE_ERROR:
            return FXString("Reference Error");
        case MJS_TYPE_ERROR:
            return FXString("Type Error");
        case MJS_OUT_OF_MEMORY:
            return FXString("Out Of Memory");
        case MJS_INTERNAL_ERROR:
            return FXString("Internal Error");
        case MJS_NOT_IMPLEMENTED_ERROR:
            return FXString("Not Implemented");
        case MJS_BAD_ARGS_ERROR:
            return FXString("Bad Arguments");
        default:
            return FXString("Unknown Error");
        }
    }
    return FXString_Empty;
#else
    double f = ceval_result(expr);
    return ev_format(f);
#endif
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

    FXString resultstr = evaluate(exp.text());
    if (resultstr.length() > 15)
    {
        result->setText("");
        longresult->setText(resultstr);
        secondline->show();
    }
    else
    {
        result->setText(resultstr);
        secondline->hide();
    }
    recalc();

    return 1;
}
