/***********************************-*- mode: c++; tab-width: 2 -*-*\
 *
 * NAME:     
 *   FXFileDialogEx - File dialog using native dialog
 *
 * AUTHOR:
 *   Daniel Gehriger (gehriger@linkcad.com)
 *
 * Copyright (c) 1999 by Daniel Gehriger.  All Rights Reserved.
 *
 * PUPROSE:
 *   File dialog using native Common Controls dialog
 *
 * NOTE
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Library General Public   
 * License as published by the Free Software Foundation; either  
 * version 2 of the License, or (at your option) any later version.
 *                                                                 
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.                 
 *                                                                  
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free       
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * $Id: FXFileDialogEx.h 101 2006-07-23 02:57:31Z phygon $
 *
 * HISTORY:
 *        dgehrige - Dec, 10 1999: Created.
 *
\*******************************************************************/
#ifndef FXFILEDIALOGEX_H
#define FXFILEDIALOGEX_H

// use commctl32 dialog on Win32, or normal FXFileDialog for other platforms
#if !defined(WIN32)
#include <fx.h>
#define FXFileDialogEx FXFileDialog
#define DLGEX_SAVE			0
#else
#include <FXObject.h>
using namespace FX;

namespace FXEX {}
using namespace FXEX;

#ifndef _INC_COMMDLG
struct OPENFILENAMEW;
#endif

namespace FXEX {

#define FXFileDialog FXFileDialogEx

/// File dialog options
#define DLGEX_SAVE              0x00100000  // "Save" dialog box
#define DLGEX_CREATEPROMPT      0x00400000  // -> OFN_CREATEPROMPT
#define DLGEX_READONLY          0x00800000  // -> OFN_READONLY
#define DLGEX_PATHMUSTEXIST     0x01000000  // -> OFN_PATHMUSTEXIST
#define DLGEX_FILEMUSTEXIST     0x02000000  // -> OFN_FILEMUSTEXIST
#define DLGEX_HIDEREADONLY      0x04000000  // -> OFN_HIDEREADONLY
#define DLGEX_OVERWRITEPROMPT   0x08000000  // -> OFN_OVERWRITEPROMPT

/**
 * Use comctl32 dialog under WIN32 for file dialog cases
 */

class FXAPI FXFileDialogEx : public FXObject {
  FXDECLARE(FXFileDialogEx)

protected:
    FXString m_title;
    IFileOpenDialog *m_pfod = NULL;
    IFileDialog *m_pfd = NULL;
    COMDLG_FILTERSPEC *m_rgFilterSpec = NULL;

    OPENFILENAMEW *m_ofn = NULL;   // low-level access to OPENFILENAMEW
    FXuint         m_opts = 0;
    FXchar **m_pszFoxPats = NULL;
    FXchar *m_pszFoxCustomPat = NULL;
    FXint          m_nFoxPats = 0;

    FXString *m_filenames = NULL;

    FXWindow *m_owner = NULL;

private:
  FXFileDialogEx(const FXFileDialogEx&);
  FXFileDialogEx& operator=(const FXFileDialogEx&);

  // get FOX patterns into Win32 patterns
  void getWinPattern(FXchar*& pszWinPat, const FXchar* pszFoxPat) const;
  void deleteFilters(void);
protected:
  FXFileDialogEx();

public:
  // ctor
  FXFileDialogEx(FXWindow* owner,const FXString& title,FXuint opts=0,FXint x=0,FXint y=0,FXint w=500,FXint h=300);

  /// Run modal invocation of the dialog
  FXuint execute(FXuint placement = PLACEMENT_CURSOR);	// parameter is for compatibility

  /// set the initial file name/path
  void setFilename(const FXString& path);

  /// set the initial directory
  void setDirectory(const FXString& path);

  /// set the inital pattern
  void setPattern(const FXString& ptrn);

  /// set the pattern list
  void setPatternList(const FXString& ptrns);

  /// select the inital pattern from the list  (zero based)
  void setCurrentPattern(FXint n);

  /// modify pattern list
  void setPatternText(FXint patno,const FXString& text);

  /// get the selected file path
  FXString getFilename() const;

  /// get the selected file extension 
  FXString getPattern() const;
  
  /// get the selected directory
  FXString getDirectory() const;

  /// get index of selected pattern in the list (zero based)
  FXint getCurrentPattern() const;

  /// get a pattern by index
  FXString getPatternText(FXint patno) const;

  /// get multiple files
  FXString* getFilenames();

  /// set the title of the dialog
  void setTitle(const FXString& name);

  /// get the title of the dialog
  FXString getTitle() const;

  /// set icon of dialog (no-op)
  void setIcon(FXIcon* icon);

  /// set select mode
  void setSelectMode(int mode);

  // dtor
  virtual ~FXFileDialogEx();
  };

} // namespace FXEX
#endif /* use commctl32 dialog */
#endif /* FXFILEDIALOGEX_H */
