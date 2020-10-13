/********************************************************************************
*                                                                               *
*              S p l i t t e r  E x  W i n d o w   O b j e c t                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004 by Thomas J. Gritzan,										*
*	based on FXSplitter Copyright (C) 1997,2002 by Jeroen van der Zijp	        *
*********************************************************************************
* $Id: FXSplitterEx.cpp 92 2006-02-08 00:41:54Z phygon $		            *
********************************************************************************/

#include <fx.h>

#include "FXSplitterEx.h"

/*
  FXSplitterEx modified by Thomas J. Gritzan:
  - added private member: FXWindow *stretcher;
  - stretcher initialized to NULL in every constructor
  - added public getter and setter for this member
  - removed all local variables *stretcher

  - layout() und adjustHLayout(), adjustVLayout() geändert
*/

/*
  Notes:
  - Reversed split option starts parting out from right [bottom].
  - Minimal partition of a split is 0 pixels.
  - Minimum width of horizontal splitter is determined by sum of
    all visible children's default widths, and height by maximum
    of all visible children's default heights; analogous for vertical
    splitter of course.
    [This can be done because the default width (height) does not depend
    on the widget's current user-specified actual width (height)].
  - For convenience, width/height of <=1 replaced by minimum width/height,
    but only if both width and height of child is <=1 at the same time.
  - Should we send SEL_CHANGED and SEL_COMMAND also when splitter arrangement
    was changed programmatically?  When bar size changed?
  - FXSplitterEx interprets layout hints on some of the children so that certain
    children have a minimum width (height) constraint on them.
    Note that this rule must have one exception: the last (or first if the option
    SPLITTER_REVERSED has been passed) must necessarily be allowed to be
    any size.

*/

// Splitter styles
#define SPLITTER_MASK     (SPLITTER_REVERSED|SPLITTER_VERTICAL|SPLITTER_TRACKING)


/*******************************************************************************/

// Map
FXDEFMAP(FXSplitterEx) FXSplitterExMap[]={
  FXMAPFUNC(SEL_MOTION,0,FXSplitterEx::onMotion),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,FXSplitterEx::onLeftBtnPress),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,FXSplitterEx::onLeftBtnRelease),
  FXMAPFUNC(SEL_FOCUS_NEXT,0,FXSplitterEx::onFocusNext),
  FXMAPFUNC(SEL_FOCUS_PREV,0,FXSplitterEx::onFocusPrev),
  FXMAPFUNC(SEL_FOCUS_UP,0,FXSplitterEx::onFocusUp),
  FXMAPFUNC(SEL_FOCUS_DOWN,0,FXSplitterEx::onFocusDown),
  FXMAPFUNC(SEL_FOCUS_LEFT,0,FXSplitterEx::onFocusLeft),
  FXMAPFUNC(SEL_FOCUS_RIGHT,0,FXSplitterEx::onFocusRight),
  };


// Object implementation
FXIMPLEMENT(FXSplitterEx,FXComposite,FXSplitterExMap,ARRAYNUMBER(FXSplitterExMap))


// Make a splitter
FXSplitterEx::FXSplitterEx(){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  window=NULL;
  split=0;
  offset=0;
  barsize=4;
  
  stretcher=NULL;
  }


// Make a splitter; it has no interior padding, and no borders
FXSplitterEx::FXSplitterEx(FXComposite* p,FXuint opts,FXint x,FXint y,FXint w,FXint h):
  FXComposite(p,opts,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  defaultCursor=(options&SPLITTER_VERTICAL) ? getApp()->getDefaultCursor(DEF_VSPLIT_CURSOR) : getApp()->getDefaultCursor(DEF_HSPLIT_CURSOR);
  dragCursor=defaultCursor;
  window=NULL;
  split=0;
  offset=0;
  barsize=4;

  stretcher=NULL;
  }


// Make a splitter; it has no interior padding, and no borders
FXSplitterEx::FXSplitterEx(FXComposite* p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
  FXComposite(p,opts,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  defaultCursor=(options&SPLITTER_VERTICAL) ? getApp()->getDefaultCursor(DEF_VSPLIT_CURSOR) : getApp()->getDefaultCursor(DEF_HSPLIT_CURSOR);
  dragCursor=defaultCursor;
  target=tgt;
  message=sel;
  window=NULL;
  split=0;
  offset=0;
  barsize=4;

  stretcher=NULL;
  }

// FXSplitterEx: setStretcher, getStretcher implementation
void FXSplitterEx::setStretcher(FXWindow* s)
{
	stretcher = s;
}

FXWindow* FXSplitterEx::getStretcher()
{
    return stretcher;
}

// Get default width
FXint FXSplitterEx::getDefaultWidth(){
  FXWindow* child;
  FXint wmax,w,numc;
  wmax=numc=0;
  if(options&SPLITTER_VERTICAL){
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        w=child->getDefaultWidth();
        if(wmax<w) wmax=w;
        }
      }
    }
  else{
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        wmax+=child->getDefaultWidth();
        numc++;
        }
      }
    if(numc>1) wmax+=(numc-1)*barsize;
    }
  return wmax;
  }


// Get default height
FXint FXSplitterEx::getDefaultHeight(){
  FXWindow* child;
  FXint hmax,h,numc;
  hmax=numc=0;
  if(options&SPLITTER_VERTICAL){
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        hmax+=child->getDefaultHeight();
        numc++;
        }
      }
    if(numc>1) hmax+=(numc-1)*barsize;
    }
  else{
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        h=child->getDefaultHeight();
        if(hmax<h) hmax=h;
        }
      }
    }
  return hmax;
  }


// Recompute layout
void FXSplitterEx::layout(){
  FXint pos,w,h,stretch;
  FXWindow *child;

	/*
	all windows left of the "stretcher" are computed left-to-right,
	all windows right of this window are computed right-to-left.
	*/
  
  if (!stretcher || ! stretcher->shown())
	stretcher = (options&SPLITTER_REVERSED)?getFirst():getLast();
  while(stretcher && !stretcher->shown())
	stretcher = stretcher->getPrev();
  
  if(options&SPLITTER_VERTICAL){          // Vertical

	pos = 0;
	for (child=getFirst(); child && child!=stretcher; child=child->getNext())
	{
		if (!child->shown())
			continue;

		w = child->getWidth();
		h = child->getHeight();
		if(w<=1 && h<=1)
			h = child->getDefaultHeight();

		if (pos+h+barsize > height)
			h = height-pos-barsize;

		child->position(0,pos, width,h);
		pos=pos+h+barsize;
	}

	stretch = pos;

	pos = height;
	for (child=getLast(); child && child!=stretcher; child=child->getPrev())
	{
		if (!child->shown())
			continue;

		w = child->getWidth();
		h = child->getHeight();
		if(w<=1 && h<=1)
			h = child->getDefaultHeight();

		if (height && pos-h-barsize < stretch)
			h = pos-barsize-stretch;

		child->position(0,pos-h, width,h);
		pos=pos-h-barsize;
	}

	h = pos-stretch;
	if (h < 0) h = 0;
	stretcher->position(0,stretch, width,h);

  } // vert
  else{                                   // Horizontal

	pos = 0;
	for (child=getFirst(); child && child!=stretcher; child=child->getNext())
	{
		if (!child->shown())
			continue;

		w = child->getWidth();
		h = child->getHeight();
		if(w<=1 && h<=1)
			w = child->getDefaultWidth();

		if (width && pos+w+barsize > width)
			w = width-pos-barsize;

		child->position(pos,0, w,height);
		pos=pos+w+barsize;
	}

	stretch = pos;

	pos = width;
	for (child=getLast(); child && child!=stretcher; child=child->getPrev())
	{
		if (!child->shown())
			continue;

		w = child->getWidth();
		h = child->getHeight();
		if(w<=1 && h<=1)
			w = child->getDefaultWidth();

		if (width && pos-w-barsize < stretch)
			w = pos-barsize-stretch;

		child->position(pos-w,0, w,height);
		pos=pos-w-barsize;
	}

	w = pos-stretch;
	if (w < 0) w = 0;
	stretcher->position(stretch,0, w,height);

  } // horiz
  flags&=~FLAG_DIRTY;
  }


// Adjust horizontal layout
void FXSplitterEx::adjustHLayout(){
  FXWindow *child;
  FXint w,h,pos,stretch;
  FXASSERT(window);

  for(child=window->getNext(); child; child=child->getNext())
	  if (child == stretcher)
		  break;

  if (child == stretcher)	// left-to-right ~= SPLITTER_REVERSED not set
  {
	  pos = window->getX();
	  window->position(pos,0, split-pos,height);
	  pos = split+barsize;

	  for(child=window->getNext(); child && child!=stretcher; child=child->getNext())
	  {
		  if (!child->shown())
			  continue;

		  w = child->getWidth();
		  h = child->getHeight();
          if (w<=1 && h<=1)
			  w = child->getDefaultWidth();

		  child->position(pos,0, w,height);
		  pos=pos+w+barsize;
	  }

	  stretch = pos;

	  pos = width;
	  for (child=getLast(); child && child!=stretcher; child=child->getPrev())
	  {
		  if (!child->shown())
			  continue;

		  w = child->getWidth();
		  h = child->getHeight();
		  if (w<=1 && h<=1)
			  w = child->getDefaultWidth();

		  child->position(pos-w,0, w,height);
		  pos=pos-w-barsize;
	  }

	  w = pos-stretch;
	  if (w < 0) w = 0;
	  stretcher->position(stretch,0, w,height);
  }
  else						// right-to-left ~= SPLITTER_REVERSED
  {
	  for (child = window->getNext(); child && !child->shown(); child=child->getNext());
	  FXASSERT(child);

	  pos = child->getX() + child->getWidth();
	  child->position(split+barsize,0, pos-split-barsize,height);
	  pos = split;

	  for (child=window; child && child!=stretcher; child=child->getPrev())
	  {
		  if (!child->shown())
			  continue;

		  w = child->getWidth();
		  h = child->getHeight();
		  if (w<=1 && h<=1)
			  w = child->getDefaultWidth();

		  child->position(pos-w,0, w,height);
		  pos=pos-w-barsize;
	  }

	  stretch = stretcher->getX();
	  w = pos-stretch;
	  if (w < 0) w = 0;
	  stretcher->position(stretch,0, w,height);
  }
}


// Adjust vertical layout
void FXSplitterEx::adjustVLayout(){
  FXWindow *child;
  FXint w,h,pos;
  FXASSERT(window);
  if(options&SPLITTER_REVERSED){
    pos=window->getY()+window->getHeight();
    window->position(0,split,width,pos-split);
    pos=split-barsize;
    if (!stretcher)
		stretcher=getFirst();
	for(; stretcher && !stretcher->shown(); stretcher=stretcher->getNext());
    for(child=window->getPrev(); child; child=child->getPrev()){
      if(child->shown()){
        w=child->getWidth();
        h=child->getHeight();
        if(w<=1 && h<=1) h=child->getDefaultHeight();
        if(child==stretcher){ h=pos; if(h<0) h=0; }
        child->position(0,pos-h,width,h);
        pos=pos-h-barsize;
        }
      }
    }
  else{
    pos=window->getY();
    window->position(0,pos,width,split-pos);
    pos=split+barsize;
	if (!stretcher)
		stretcher=getLast();
    for(; stretcher && !stretcher->shown(); stretcher=stretcher->getPrev());
    for(child=window->getNext(); child; child=child->getNext()){
      if(child->shown()){
        w=child->getWidth();
        h=child->getHeight();
        if(w<=1 && h<=1) h=child->getDefaultHeight();
        if(child==stretcher){ h=height-pos; if(h<0) h=0; }
        child->position(0,pos,width,h);
        pos=pos+h+barsize;
        }
      }
    }
  }


// Find child just before split
FXWindow* FXSplitterEx::findHSplit(FXint pos){
  FXWindow* child=getFirst();
  if(options&SPLITTER_REVERSED){
    while(child){
      if(child->shown()){
        if(child->getX()-barsize<=pos && pos<child->getX()) return child;
        }
      child=child->getNext();
      }
    }
  else{
    while(child){
      if(child->shown()){
        if(child->getX()+child->getWidth()<=pos && pos<child->getX()+child->getWidth()+barsize) return child;
        }
      child=child->getNext();
      }
    }
  return NULL;
  }


// Find child just before split
FXWindow* FXSplitterEx::findVSplit(FXint pos){
  FXWindow* child=getFirst();
  if(options&SPLITTER_REVERSED){
    while(child){
      if(child->shown()){
        if(child->getY()-barsize<=pos && pos<child->getY()) return child;
        }
      child=child->getNext();
      }
    }
  else{
    while(child){
      if(child->shown()){
        if(child->getY()+child->getHeight()<=pos && pos<child->getY()+child->getHeight()+barsize) return child;
        }
      child=child->getNext();
      }
    }
  return NULL;
  }


// Move the horizontal split intelligently
void FXSplitterEx::moveHSplit(FXint pos){
  FXint smin,smax;
  FXuint hints;
  FXASSERT(window);
  hints=window->getLayoutHints();
  if(options&SPLITTER_REVERSED){
    smin=barsize;
    smax=window->getX()+window->getWidth();
    if((hints&LAYOUT_FILL_X)&&(hints&LAYOUT_FIX_WIDTH)) smax-=window->getDefaultWidth();
    }
  else{
    smin=window->getX();
    smax=width-barsize;

    // FXSplitterEx fix
	for (FXWindow *child = window->getNext(); child; child=child->getNext())
		if (child->shown())
		{
			smax = child->getX() + child->getWidth() - barsize;
			break;
		}

    if((hints&LAYOUT_FILL_X)&&(hints&LAYOUT_FIX_WIDTH)) smin+=window->getDefaultWidth();
    }
  split=pos;
  if(split<smin) split=smin;
  if(split>smax) split=smax;
  }


// Move the vertical split intelligently
void FXSplitterEx::moveVSplit(FXint pos){
  FXint smin,smax;
  FXuint hints;
  FXASSERT(window);
  hints=window->getLayoutHints();
  if(options&SPLITTER_REVERSED){
    smin=barsize;
    smax=window->getY()+window->getHeight();
    if((hints&LAYOUT_FILL_Y)&&(hints&LAYOUT_FIX_HEIGHT)) smax-=window->getDefaultHeight();
    }
  else{
    smin=window->getY();
    smax=height-barsize;

    // FXSplitterEx fix
	for (FXWindow *child = window->getNext(); child; child=child->getNext())
		if (child->shown())
		{
			smax = child->getY() + child->getHeight() - barsize;
			break;
		}

    if((hints&LAYOUT_FILL_Y)&&(hints&LAYOUT_FIX_HEIGHT)) smin+=window->getDefaultHeight();
    }
  split=pos;
  if(split<smin) split=smin;
  if(split>smax) split=smax;
  }


// Button being pressed
long FXSplitterEx::onLeftBtnPress(FXObject*,FXSelector,void* ptr){
  FXEvent* ev=(FXEvent*)ptr;
  if(isEnabled()){
    grab();
    if(target && target->handle(this, FXSEL(SEL_LEFTBUTTONPRESS, message), ptr)) return 1;
    if(options&SPLITTER_VERTICAL){
      window=findVSplit(ev->win_y);
      if(window){
        if(options&SPLITTER_REVERSED)
          split=window->getY();
        else
          split=window->getY()+window->getHeight();
        offset=ev->win_y-split;
        if(!(options&SPLITTER_TRACKING)){
          drawVSplit(split);
          }
        flags|=FLAG_PRESSED;
        flags&=~FLAG_UPDATE;
        }
      }
    else{
      window=findHSplit(ev->win_x);
      if(window){
        if(options&SPLITTER_REVERSED)
          split=window->getX();
        else
          split=window->getX()+window->getWidth();
        offset=ev->win_x-split;
        if(!(options&SPLITTER_TRACKING)){
          drawHSplit(split);
          }
        flags|=FLAG_PRESSED;
        flags&=~FLAG_UPDATE;
        }
      }
    return 1;
    }
  return 0;
  }


// Button being released
long FXSplitterEx::onLeftBtnRelease(FXObject*,FXSelector,void* ptr){
  FXuint flgs=flags;
  if(isEnabled()){
    ungrab();
    flags|=FLAG_UPDATE;
    flags&=~FLAG_CHANGED;
    flags&=~FLAG_PRESSED;
    if(target && target->handle(this, FXSEL(SEL_LEFTBUTTONRELEASE, message), ptr)) return 1;
    if(flgs&FLAG_PRESSED){
      if(!(options&SPLITTER_TRACKING)){
        if(options&SPLITTER_VERTICAL){
          drawVSplit(split);
          adjustVLayout();
          }
        else{
          drawHSplit(split);
          adjustHLayout();
          }
        if(flgs&FLAG_CHANGED){
          if(target) target->handle(this, FXSEL(SEL_CHANGED, message), NULL);
          }
        }
      if(flgs&FLAG_CHANGED){
        if(target) target->handle(this, FXSEL(SEL_COMMAND, message), NULL);
        }
      }
    return 1;
    }
  return 0;
  }


// Button being released
long FXSplitterEx::onMotion(FXObject*,FXSelector,void* ptr){
  FXEvent* ev=(FXEvent*)ptr;
  FXint oldsplit;
  if(flags&FLAG_PRESSED){
    oldsplit=split;
    if(options&SPLITTER_VERTICAL){
      moveVSplit(ev->win_y-offset);
      if(split!=oldsplit){
        if(!(options&SPLITTER_TRACKING)){
          drawVSplit(oldsplit);
          drawVSplit(split);
          }
        else{
          adjustVLayout();
          if(target) target->handle(this, FXSEL(SEL_CHANGED, message), NULL);
          }
        flags|=FLAG_CHANGED;
        }
      }
    else{
      moveHSplit(ev->win_x-offset);
      if(split!=oldsplit){
        if(!(options&SPLITTER_TRACKING)){
          drawHSplit(oldsplit);
          drawHSplit(split);
          }
        else{
          adjustHLayout();
          if(target) target->handle(this, FXSEL(SEL_CHANGED, message), NULL);
          }
        flags|=FLAG_CHANGED;
        }
      }
    return 1;
    }
  return 0;
  }


// Focus moved to next
long FXSplitterEx::onFocusNext(FXObject* sender,FXSelector sel,void* ptr){
  return (options&SPLITTER_VERTICAL) ? onFocusDown(sender,sel,ptr) : onFocusRight(sender,sel,ptr);
  }


// Focus moved to previous
long FXSplitterEx::onFocusPrev(FXObject* sender,FXSelector sel,void* ptr){
  return (options&SPLITTER_VERTICAL) ? onFocusUp(sender,sel,ptr) : onFocusLeft(sender,sel,ptr);
  }


// Focus moved up
long FXSplitterEx::onFocusUp(FXObject*,FXSelector sel,void* ptr){
  FXWindow *child;
  if(options&SPLITTER_VERTICAL){
    if(getFocus())
      child=getFocus()->getPrev();
    else
      child=getLast();
    while(child){
      if(child->isEnabled() && child->canFocus()){
        child->handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
        return 1;
        }
      if(child->isComposite() && child->handle(this,sel,ptr)) return 1;
      child=child->getPrev();
      }
    }
  return 0;
  }


// Focus moved down
long FXSplitterEx::onFocusDown(FXObject*,FXSelector sel,void* ptr){
  FXWindow *child;
  if(options&SPLITTER_VERTICAL){
    if(getFocus())
      child=getFocus()->getNext();
    else
      child=getFirst();
    while(child){
      if(child->isEnabled() && child->canFocus()){
        child->handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
        return 1;
        }
      if(child->isComposite() && child->handle(this,sel,ptr)) return 1;
      child=child->getNext();
      }
    }
  return 0;
  }


// Focus moved to left
long FXSplitterEx::onFocusLeft(FXObject*,FXSelector sel,void* ptr){
  FXWindow *child;
  if(!(options&SPLITTER_VERTICAL)){
    if(getFocus())
      child=getFocus()->getPrev();
    else
      child=getLast();
    while(child){
      if(child->isEnabled() && child->canFocus()){
        child->handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
        return 1;
        }
      if(child->isComposite() && child->handle(this,sel,ptr)) return 1;
      child=child->getPrev();
      }
    }
  return 0;
  }


// Focus moved to right
long FXSplitterEx::onFocusRight(FXObject*,FXSelector sel,void* ptr){
  FXWindow *child;
  if(!(options&SPLITTER_VERTICAL)){
    if(getFocus())
      child=getFocus()->getNext();
    else
      child=getFirst();
    while(child){
      if(child->isEnabled() && child->canFocus()){
        child->handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
        return 1;
        }
      if(child->isComposite() && child->handle(this,sel,ptr)) return 1;
      child=child->getNext();
      }
    }
  return 0;
  }


// Draw the horizontal split
void FXSplitterEx::drawHSplit(FXint pos){
  FXDCWindow dc(this);
  dc.clipChildren(FALSE);
  dc.setFunction(BLT_NOT_DST);     // Does this always show up?
  dc.fillRectangle(pos,0,barsize,height);
  }


// Draw the vertical split
void FXSplitterEx::drawVSplit(FXint pos){
  FXDCWindow dc(this);
  dc.clipChildren(FALSE);
  dc.setFunction(BLT_NOT_DST);     // Does this always show up?
  dc.fillRectangle(0,pos,width,barsize);
  }


// Return splitter style
FXuint FXSplitterEx::getSplitterStyle() const {
  return (options&SPLITTER_MASK);
  }


// Set horizontal or vertical
void FXSplitterEx::setSplitterStyle(FXuint style){
  FXuint opts=(options&~SPLITTER_MASK) | (style&SPLITTER_MASK);
  if(options!=opts){

    // Split direction changed; need re-layout of everything
    if((opts&SPLITTER_VERTICAL)!=(options&SPLITTER_VERTICAL)){
      for(FXWindow *child=getFirst(); child; child=child->getNext()){
        if(child->shown()){
          child->setWidth(0);
          child->setHeight(0);
          }
        }
      setDefaultCursor((opts&SPLITTER_VERTICAL) ? getApp()->getDefaultCursor(DEF_VSPLIT_CURSOR) : getApp()->getDefaultCursor(DEF_HSPLIT_CURSOR));
      setDragCursor(getDefaultCursor());
      recalc();
      }

    // Split mode reversal; re-layout first and last only
    if((opts&SPLITTER_REVERSED)!=(options&SPLITTER_REVERSED)){
      if(getFirst()){
        getFirst()->setWidth(0);
        getFirst()->setHeight(0);
        getLast()->setWidth(0);
        getLast()->setHeight(0);
        }
      recalc();
      }
    options=opts;
    }
  }


// Change bar size
void FXSplitterEx::setBarSize(FXint bs){
  if(bs!=barsize){
    barsize=bs;
    recalc();
    }
  }


// Save object to stream
void FXSplitterEx::save(FXStream& store) const {
  FXComposite::save(store);
  store << barsize;
  }


// Load object from stream
void FXSplitterEx::load(FXStream& store){
  FXComposite::load(store);
  store >> barsize;
  }


// Zap it
FXSplitterEx::~FXSplitterEx(){
  window=(FXWindow*)-1;
  }
