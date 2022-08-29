/********************************************************************************
*                                                                               *
*                M e n u   S e p a r a t o r   E x   W i d g e t                *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004 by Thomas J. Gritzan,										*
*	based on FXMenuSeparator Copyright (C) 1997,2002 by Jeroen van der Zijp		*
*********************************************************************************
* $Id: FXMenuSeparatorEx.cpp 100 2006-07-21 17:17:59Z phygon $				*
********************************************************************************/
#include <fx.h>

#include "FXMenuSeparatorEx.h"

#define LEADSPACE   22
#define TRAILSPACE  16

/*******************************************************************************/

// Map
FXDEFMAP(FXMenuSeparatorEx) FXMenuSeparatorExMap[]={
  FXMAPFUNC(SEL_PAINT,0,FXMenuSeparatorEx::onPaint),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETSTRINGVALUE,FXMenuSeparatorEx::onCmdSetStringValue),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_GETSTRINGVALUE,FXMenuSeparatorEx::onCmdGetStringValue),
  };


// Object implementation
FXIMPLEMENT(FXMenuSeparatorEx,FXWindow,FXMenuSeparatorExMap,ARRAYNUMBER(FXMenuSeparatorExMap))


// Deserialization
FXMenuSeparatorEx::FXMenuSeparatorEx() :
  font(nullptr)
{
  flags |= FLAG_SHOWN; 
}

// Separator item
FXMenuSeparatorEx::FXMenuSeparatorEx(FXComposite* p,const FXString& text,FXuint opts,FXObject* tgt,FXSelector sel) :
  FXWindow(p,opts,0,0,0,0)
{
  flags |= FLAG_SHOWN;
  setTarget(tgt);
  setSelector(sel);
  label = text.section('\t',0);
  label = stripHotKey(label);
  font = getApp()->getNormalFont();
  hiliteColor = getApp()->getHiliteColor();
  shadowColor = getApp()->getShadowColor();
  defaultCursor = getApp()->getDefaultCursor(DEF_RARROW_CURSOR);
}

// Handle repaint
long FXMenuSeparatorEx::onPaint(FXObject*,FXSelector,void* ptr)
{
  FXEvent *ev = (FXEvent*)ptr;
  FXDCWindow dc(this,ev);

  dc.setForeground(backColor);
  dc.fillRectangle(ev->rect.x,ev->rect.y,ev->rect.w,ev->rect.h);

  if(label.empty()){
	FXint yy=height/2;

	dc.setForeground(shadowColor);
    dc.fillRectangle(1,yy+0,width-2,1);
    dc.setForeground(hiliteColor);
    dc.fillRectangle(1,yy+1,width-2,1);
    }
  else{
    FXint xx,yy,xleft,xright;
    xx=font->getTextWidth(label.text(),label.length());
    yy=height/2;

	xleft=width/2-xx/2;
	xright=width/2+xx/2+2;

    dc.setForeground(shadowColor);
    dc.fillRectangle(1,yy+0,xleft-3,1);
    dc.fillRectangle(xright,yy+0,width-xright-1,1);
    dc.setForeground(hiliteColor);
    dc.fillRectangle(1,yy+1,xleft-3,1);
    dc.fillRectangle(xright,yy+1,width-xright-1,1);

    dc.setFont(font);
	yy=font->getFontAscent()+(height-font->getFontHeight())/2;
    dc.setForeground(hiliteColor);
    dc.drawText(xleft+1,yy+1,label.text(),label.length());
    dc.setForeground(shadowColor);
    dc.drawText(xleft,yy,label.text(),label.length());
    }
  return 1;
  }

  // Create Window
void FXMenuSeparatorEx::create(){
  FXWindow::create();
  font->create();
  }


  // Detach Window
void FXMenuSeparatorEx::detach(){
  FXWindow::detach();
  font->detach();
}


// Get default size
FXint FXMenuSeparatorEx::getDefaultWidth(){
  FXint tw=0;
  if(!label.empty()) tw=font->getTextWidth(label.text(),label.length());
  return LEADSPACE+tw+TRAILSPACE;
  }

  // Get default height
FXint FXMenuSeparatorEx::getDefaultHeight()
{
  FXint th = 9;
  if (!label.empty()) th = font->getFontHeight() + 5;
  return th;
}


// Update value from a message
long FXMenuSeparatorEx::onCmdSetStringValue(FXObject*,FXSelector, void* ptr)
{
  FXString *text = (FXString*) ptr;
  if (!text) {
     fxerror("%s::onCmdSetStringValue: NULL pointer.\n", getClassName());
  }
  else {
    setText(*text);
  }
  return 1;
}
  
  
// Obtain value from text field
long FXMenuSeparatorEx::onCmdGetStringValue(FXObject*, FXSelector, void* ptr) {
  FXString *text = (FXString*) ptr;
  if (!text) {
    fxerror("%s::onCmdGetStringValue: NULL pointer.\n", getClassName());
  }
  else {
    *text = getText();
  }
  return 1;
}

// Change text, and scan this text to replace accelerators
void FXMenuSeparatorEx::setText(const FXString& text) {
  FXString str = stripHotKey(text);
  if(label != str) {
    label = str;
    recalc();
    update();
  }
}


// Set highlight color
void FXMenuSeparatorEx::setHiliteColor(FXColor clr){
  if(clr!=hiliteColor){
    hiliteColor=clr;
    update();
    }
  }


// Set shadow color
void FXMenuSeparatorEx::setShadowColor(FXColor clr){
  if(clr!=shadowColor){
    shadowColor=clr;
    update();
    }
  }


// Save object to stream
void FXMenuSeparatorEx::save(FXStream& store) const {
  FXWindow::save(store);
  store << label;
  store << font;
  store << hiliteColor;
  store << shadowColor;
  }


// Load object from stream
void FXMenuSeparatorEx::load(FXStream& store){
  FXWindow::load(store);
  store >> label;
  store >> font;
  store >> hiliteColor;
  store >> shadowColor;
  }


// Zap it
FXMenuSeparatorEx::~FXMenuSeparatorEx(){
  font=(FXFont*)-1;
  }
