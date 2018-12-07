/********************************************************************************
*                                                                               *
*                M e n u   S e p a r a t o r   E x   W i d g e t                *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004 by Thomas J. Gritzan,										*
*	based on FXMenuSeparator Copyright (C) 1997,2002 by Jeroen van der Zijp		*
*********************************************************************************
* $Id: FXMenuSeparatorEx.h 92 2006-02-08 00:41:54Z phygon $				*
********************************************************************************/
#ifndef FXMenuSeparatorEx_H
#define FXMenuSeparatorEx_H

#ifndef FXWINDOW_H
#include "FXWindow.h"
#endif



/**
* The menu separator ex. is a separator line
* with optional text to group menu commands in a
* popup menu.
*/
class FXAPI FXMenuSeparatorEx : public FXWindow {
  FXDECLARE(FXMenuSeparatorEx)
protected:
  FXString	label;
  FXFont	*font;
  FXColor	hiliteColor;
  FXColor	shadowColor;
protected:
  FXMenuSeparatorEx();
private:
  FXMenuSeparatorEx(const FXMenuSeparatorEx&);
  FXMenuSeparatorEx &operator=(const FXMenuSeparatorEx&);
public:
  long onPaint(FXObject*,FXSelector,void*);
  long onCmdGetStringValue(FXObject*,FXSelector,void*);
  long onCmdSetStringValue(FXObject*,FXSelector,void*);
public:

  /// Construct a menu separator
  FXMenuSeparatorEx(FXComposite* p,const FXString& text="",FXuint opts=0,FXObject* tgt=NULL,FXSelector sel=0);

  /// create the window
  void create();

  // Detach Window
  void detach();

  /// Return default width
  virtual FXint getDefaultWidth();

  /// Return default height
  virtual FXint getDefaultHeight();

  /// Set the text for this menu
  void setText(const FXString& text);

  /// Get the text for this menu
  FXString getText() const { return label; }

  /// Set the text font
  void setFont(FXFont* fnt);

  /// Return the text font
  FXFont* getFont() const { return font; }

  /// Change highlight color
  void setHiliteColor(FXColor clr);

  /// Get highlight color
  FXColor getHiliteColor() const { return hiliteColor; }

  /// Change shadow color
  void setShadowColor(FXColor clr);

  /// Get shadow color
  FXColor getShadowColor() const { return shadowColor; }

  /// Save menu to a stream
  virtual void save(FXStream& store) const;

  /// Load menu from a stream
  virtual void load(FXStream& store);

  /// destructor
  virtual ~FXMenuSeparatorEx();
  };


#endif
