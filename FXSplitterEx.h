/********************************************************************************
*                                                                               *
*              S p l i t t e r  E x  W i n d o w   O b j e c t                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004 by Thomas J. Gritzan,										*
*	based on FXSplitter Copyright (C) 1997,2002 by Jeroen van der Zijp	        *
*********************************************************************************
* $Id: FXSplitterEx.h 92 2006-02-08 00:41:54Z phygon $	    	            *
********************************************************************************/

#ifndef FXSplitterEx_H
#define FXSplitterEx_H

#include <fx.h>
#include "FXSplitter.h"

/// Splitter options
/*enum {
  SPLITTER_HORIZONTAL = 0,                  /// Split horizontally
  SPLITTER_VERTICAL   = 0x00008000,         /// Split vertically
  SPLITTER_REVERSED   = 0x00010000,         /// Reverse-anchored
  SPLITTER_TRACKING   = 0x00020000,         /// Track continuous during split
  SPLITTER_NORMAL     = SPLITTER_HORIZONTAL
  };*/



/**
* Splitter window is used to interactively repartition
* two or more subpanes.
* Space may be subdivided horizontally or vertically.
* When the splitter is itself resized, the right-most (bottom-most)
* child window will be resized unless the splitter window is reversed;
* if the splitter is reversed, the left-most (top-most) child window
* will be resized instead.
* The splitter widget sends a SEL_CHANGED to its target
* during the resizing of the panes; at the end of the resize interaction,
* it sends a SEL_COMMAND to signify that the resize operation is complete.
* Normally, children are resizable from 0 upwards; however, if the child
* in a horizontally oriented splitter has LAYOUT_FILL_X in combination with
* LAYOUT_FIX_WIDTH, it will not be made smaller than its default width,
* except when the child is the last visible widget (or first when the option
* SPLITTER_REVERSED has been passed to the splitter).
* In a vertically oriented splitter, children with LAYOUT_FILL_Y and
* LAYOUT_FIX_HEIGHT behave analogously.
* These options only affect interactive resizing.
*/
class FXAPI FXSplitterEx : public FXComposite {
  FXDECLARE(FXSplitterEx)
private:
  FXWindow *window;         // Window being resized
  FXint     split;          // Split value
  FXint     offset;         // Mouse offset
  FXint     barsize;        // Size of the splitter bar

  /// FXSplitterEx: the stretcher window
  FXWindow *stretcher;

protected:
  FXSplitterEx();
  virtual void layout();
  void adjustHLayout();
  void adjustVLayout();
  void moveHSplit(FXint amount);
  void moveVSplit(FXint amount);
  void drawHSplit(FXint pos);
  void drawVSplit(FXint pos);
  FXWindow* findHSplit(FXint pos);
  FXWindow* findVSplit(FXint pos);
private:
  FXSplitterEx(const FXSplitterEx&);
  FXSplitterEx& operator=(const FXSplitterEx&);
public:
  long onLeftBtnPress(FXObject*,FXSelector,void*);
  long onLeftBtnRelease(FXObject*,FXSelector,void*);
  long onMotion(FXObject*,FXSelector,void*);
  long onFocusNext(FXObject*,FXSelector,void*);
  long onFocusPrev(FXObject*,FXSelector,void*);
  long onFocusUp(FXObject*,FXSelector,void*);
  long onFocusDown(FXObject*,FXSelector,void*);
  long onFocusLeft(FXObject*,FXSelector,void*);
  long onFocusRight(FXObject*,FXSelector,void*);
public:

  /// FXSplitterEx: sets and gets the stretcher window
  void setStretcher(FXWindow* s);
  FXWindow* getStretcher();

  /// Construct new splitter widget
  FXSplitterEx(FXComposite* p,FXuint opts=SPLITTER_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Construct new splitter widget, which will notify target about size changes
  FXSplitterEx(FXComposite* p,FXObject* tgt,FXSelector sel,FXuint opts=SPLITTER_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Get default width
  virtual FXint getDefaultWidth();

  /// Get default height
  virtual FXint getDefaultHeight();

  /// Change splitter style
  void setSplitterStyle(FXuint style);

  /// Return current splitter style
  FXuint getSplitterStyle() const;

  /// Change splitter bar size
  void setBarSize(FXint bs);

  /// Return current bar size
  FXint getBarSize() const { return barsize; }

  /// Save to stream
  virtual void save(FXStream& store) const;

  /// Load from stream
  virtual void load(FXStream& store);

  /// Destroy splitter
  virtual ~FXSplitterEx();
  };


#endif
