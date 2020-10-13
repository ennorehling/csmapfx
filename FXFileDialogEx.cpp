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
 *
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
 * $Id: FXFileDialogEx.cpp 130 2010-01-13 19:36:37Z phygon $
 *
 * HISTORY:
 *        dgehrige - Dec, 10 1999: Created.
 *
\*******************************************************************/

#if defined(WIN32)

#include "fxwin.h"

#include <windows.h>
#include <commdlg.h>

#include <fx.h>
#include "fxhelper.h"
#include "FXFileDialogEx.h"
using namespace FX;

// Length of narrow character string
static inline FXint strlen(const FXnchar *src){
  FXint i=0;
  while(src[i]) i++;
  return i;
  }

static inline void strcpy(FXnchar *dst, const FXnchar *src){
	for (; *src; dst++, src++)
		*dst = *src;
	*dst = *src;
  }

using namespace FXEX;
namespace FXEX {

// maps
FXIMPLEMENT(FXFileDialogEx, FXObject, NULL, 0);

// de-serialisation
FXFileDialogEx::FXFileDialogEx() : FXObject(),m_ofn(new OPENFILENAMEW),m_filenames(0),m_owner(0) {}

// ctor
FXFileDialogEx::FXFileDialogEx(FXWindow* owner, const FXString& title, FXuint opts, FXint x, FXint y, FXint w, FXint h) : FXObject(),m_ofn(new OPENFILENAMEW),m_opts(opts),m_pszFoxPats(NULL),m_pszFoxCustomPat(NULL),m_nFoxPats(0),m_filenames(0),m_owner(owner) {
  /*
  ** Initialize OPENFILENAMEW
  */
  const int max_filenames_length = 1024*4;

  memset(m_ofn, 0, sizeof(*m_ofn));
  m_ofn->lStructSize = sizeof(OPENFILENAMEW);
  m_ofn->hwndOwner = (HWND)owner->id();
  m_ofn->hInstance = (HINSTANCE)owner->getApp()->getDisplay();

  m_ofn->lpstrFile = new FXnchar[max_filenames_length];
  m_ofn->lpstrFile[0] = '\0';
  m_ofn->nMaxFile = max_filenames_length;

  setTitle(title);
  setPatternList(""); // Init with "All Files"

  opts |= DLGEX_HIDEREADONLY;

  if (!(opts & DLGEX_SAVE))
	  opts |= DLGEX_FILEMUSTEXIST;

  if (opts & DLGEX_CREATEPROMPT) m_ofn->Flags |= OFN_CREATEPROMPT;
  if (opts & DLGEX_READONLY) m_ofn->Flags |= OFN_READONLY;
  if (opts & DLGEX_PATHMUSTEXIST) m_ofn->Flags |= OFN_PATHMUSTEXIST;
  if (opts & DLGEX_FILEMUSTEXIST) m_ofn->Flags |= OFN_FILEMUSTEXIST;
  if (opts & DLGEX_HIDEREADONLY) m_ofn->Flags |= OFN_HIDEREADONLY;
  if (opts & DLGEX_OVERWRITEPROMPT) m_ofn->Flags |= OFN_OVERWRITEPROMPT; 

  m_ofn->Flags |= OFN_EXPLORER;
  }

FXFileDialogEx::~FXFileDialogEx() {
  delete [] (FXchar*)m_ofn->lpstrInitialDir;
  delete [] (FXchar*)m_ofn->lpstrFilter;
  delete [] (FXchar*)m_ofn->lpstrCustomFilter;
  delete [] (FXchar*)m_ofn->lpstrFile;
  delete [] (FXchar*)m_ofn->lpstrTitle;
  for (FXint i = 0; i < m_nFoxPats; i++) {
    delete [] m_pszFoxPats[i];
    }
  delete [] m_pszFoxPats;
  delete [] m_pszFoxCustomPat;

  delete m_ofn;
  }

/*-----------------------------------------------------------------*\
 * 
 * Set / get the dialog title
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setTitle(const FXString& name) {
  delete [] (FXnchar*)m_ofn->lpstrTitle;
  m_ofn->lpstrTitle = NULL;

  if (!name.empty()) {
	FXint size = name.count();
	FXnchar* ptr = new FXnchar[size + 1];
	utf2ncs(ptr,name.text(),name.length()+1);
	m_ofn->lpstrTitle = ptr;
    }
  }

FXString FXFileDialogEx::getTitle() const {
  return FXString(m_ofn->lpstrTitle);
  }

  /// set icon of dialog (no-op)
void FXFileDialogEx::setIcon(FXIcon* icon) {
	return;
}

void FXFileDialogEx::setSelectMode(int mode)
{
	if (mode == SELECTFILE_MULTIPLE)
		m_ofn->Flags |= OFN_ALLOWMULTISELECT; 
	else
		m_ofn->Flags &= ~OFN_ALLOWMULTISELECT; 
}

/*-----------------------------------------------------------------*\
 * 
 * Set / get the file name
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setFilename(const FXString& path) {
  if (path.length() > _MAX_PATH + 1) {
    fxerror("File path exceeds maxmimum path length: %d\n", path.length());
    return;
    }

	utf2ncs(m_ofn->lpstrFile,path.text(),path.length()+1);
  }

FXString FXFileDialogEx::getFilename() const {
  return FXString(m_ofn->lpstrFile);
  }

/*-----------------------------------------------------------------*\
 * 
 * Set the initial directory
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setDirectory(const FXString& path) {
  delete [] (FXnchar*)m_ofn->lpstrInitialDir;
  m_ofn->lpstrInitialDir = NULL;

  if (!path.empty()) {
	FXint size = path.count();
	FXnchar* ptr = new FXnchar[size + 1];
	utf2ncs(ptr,path.text(),path.length()+1);
	m_ofn->lpstrInitialDir = ptr;
    }
  }

/*-----------------------------------------------------------------*\
 * 
 * Get the selected directory
 *
\*-----------------------------------------------------------------*/
FXString FXFileDialogEx::getDirectory() const {
	 for (int i = 0; m_ofn->lpstrFile[i]; i++)
	 {
		 if (i > m_ofn->nFileOffset)
			return FXPath::directory(m_ofn->lpstrFile);	// only one file
	 }
	
	return m_ofn->lpstrFile;	// more files. first entry is directory
  }

/*-----------------------------------------------------------------*\
 * 
 * Select the file pattern
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setPattern(const FXString& ptrn) {
  FXint i, len;
  FXchar* pszWinPat;
  /*
  ** Pattern in the pattern list?
  */
  for (i = 0; i < m_nFoxPats; i++) {
    if (ptrn == m_pszFoxPats[i]) {
      m_ofn->nFilterIndex = i + 1;
      return;
      }
    }

  // translate pattern
  delete [] m_ofn->lpstrCustomFilter;  // delete old filter
  getWinPattern(pszWinPat, ptrn.text());
  len = FXMAX(ptrn.length() + 6 + strlen(pszWinPat) + 1 + 1, 40 /* magic value from MS doc*/);
  m_ofn->nMaxCustFilter = len;
  
  FXString filter = ptrn + " Files" + pszWinPat;
  FXnchar* ptr = new FXnchar[len];  
  m_ofn->lpstrCustomFilter = ptr;
  utf2ncs(ptr,filter.text(),filter.length()+1);
  
  m_ofn->nFilterIndex = 0;
  delete [] pszWinPat;
  
  // save pattern for getPattern();
  delete [] m_pszFoxCustomPat;
  m_pszFoxCustomPat = NULL;
  m_pszFoxCustomPat = new FXchar[ptrn.length()];
  strcpy(m_pszFoxCustomPat, ptrn.text());
  }

/*-----------------------------------------------------------------*\
 * 
 * Get the selected file extension
 *
\*-----------------------------------------------------------------*/
FXString FXFileDialogEx::getPattern() const {
  if (m_ofn->nFilterIndex == 0 && m_pszFoxCustomPat != NULL) {
    return FXString(m_pszFoxCustomPat);
    }
  else if (m_pszFoxPats != NULL && m_pszFoxPats[m_ofn->nFilterIndex - 1] != NULL) {
    return m_pszFoxPats[m_ofn->nFilterIndex -1];
    }
  FXASSERT(FALSE);
  return FXString::null;
  }

/*-----------------------------------------------------------------*\
 * 
 * Set the pattern list
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setPatternList(const FXString& ps) {
  FXint i = 0, len = 0, nPat = 0;
  FXbool bHasAll = FALSE;
  
  /*
  ** Remove old items
  */
  for (i = 0; i < m_nFoxPats; i++) {
    delete [] m_pszFoxPats[i];
    }
  delete [] m_pszFoxPats;
  delete [] (FXnchar*)m_ofn->lpstrFilter; 
  m_ofn->lpstrFilter = NULL;

  /*
  ** Count patterns && and allocate memory
  */
  FXString patterns = ps;

  nPat = 1;
  for (i = 0; i < patterns.length(); i++)
	  if (patterns[i] == '\n')
		  nPat++;
  //nPat = patterns.count('\n') + 1;
  if (patterns.empty()) nPat = 0;
  FXchar** ptrns = new FXchar*[2*nPat+1];
  for (i = 0; i < nPat; i++)
  {
	  FXString pat = patterns.before('\n');
	  patterns = patterns.after('\n');

	  ptrns[2*i] = new FXchar[pat.length()+1];
      strcpy(ptrns[2*i], pat.text());

	  pat = pat.after('(').before(')');

	  ptrns[2*i+1] = new FXchar[pat.length()+1];
	  strcpy(ptrns[2*i+1], pat.text());
  }
  ptrns[2*i] = NULL;

  for (nPat = 0; ptrns && ptrns[2*nPat] != NULL; nPat++) {
    if (strcmp(ptrns[2*nPat+1], "*") == 0 || strcmp(ptrns[2*nPat+1], "*.*") == 0) bHasAll=TRUE;
    }
  if (!bHasAll) nPat++;
  m_nFoxPats = nPat;
  m_pszFoxPats = new FXchar*[nPat];
  FXchar const** pszWinDesc = new FXchar const*[nPat];
  FXchar** pszWinPats = new FXchar*[nPat];

  /*
  ** Copy translated patterns
  */
  for (i = 0; ptrns && ptrns[2*i] != NULL; i++) {
    pszWinDesc[i] = ptrns[2*i];

	for (char* ptr = ptrns[2*i+1]; *ptr; ptr++)
	   if (*ptr == ',')
          *ptr = ';';

	getWinPattern(pszWinPats[i], ptrns[2*i+1]);

	len += strlen(pszWinDesc[i])+1 + strlen(pszWinPats[i])+1;
    
    // save fox pattern
    m_pszFoxPats[i] = new FXchar[strlen(ptrns[2*i+1])+1];
    strcpy(m_pszFoxPats[i], ptrns[2*i+1]); 
    }

  if (!bHasAll) {
    pszWinDesc[i] = "All Files";
    pszWinPats[i] = new FXchar[2];
    strcpy(pszWinPats[i], "*");
    len += strlen(pszWinDesc[i])+ 1 + strlen(pszWinPats[i]) + 1;

    // save fox pattern
    m_pszFoxPats[i] = new FXchar[strlen(pszWinPats[i])+1];
    strcpy(m_pszFoxPats[i], pszWinPats[i]); 
    }

  FXnchar* ptr = new FXnchar[len + 1 /* terminate with "\0\0" */];
  m_ofn->lpstrFilter = ptr;
  for (i = 0, len = 0; i < nPat; i++) {
    len += utf2ncs(ptr + len, pszWinDesc[i]);
    len += utf2ncs(ptr + len, pszWinPats[i]);
    }
  ptr[len] = '\0';

  /*
  ** A lot of stuff to delete...
  */
  for (i = 0; i < nPat; i++) {
    delete [] pszWinPats[i];
    }
  delete [] pszWinPats;
  delete [] pszWinDesc;

  for (i = 0; ptrns[i]; i++)
	  delete[] ptrns[i];
  delete[] ptrns;

  delete[] m_filenames;
}

/*-----------------------------------------------------------------*\
 * 
 * Get index of selected pattern in the list (zero based)
 *
\*-----------------------------------------------------------------*/
FXint FXFileDialogEx::getCurrentPattern() const {
  return m_ofn->nFilterIndex - 1;
  }

/*-----------------------------------------------------------------*\
 * 
 * Select the inital pattern from the list (zero based)
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setCurrentPattern(FXint n) {
  m_ofn->nFilterIndex = n + 1;
  }

/*-----------------------------------------------------------------*\
 * 
 * Get the selected pattern in the list 
 *
\*-----------------------------------------------------------------*/
FXString FXFileDialogEx::getPatternText(FXint patno) const {
  const FXnchar* pFilter;
  FXint i;

  // skip over patterns
  for (pFilter = m_ofn->lpstrFilter, i = 0; i != patno && pFilter && *pFilter; i++) {
    pFilter += strlen(pFilter) + 1;
    pFilter += strlen(pFilter) + 1;
    }

  if (pFilter && *pFilter) return FXString(pFilter);
  return FXString::null;
  }

/*-----------------------------------------------------------------*\
 * 
 * Modify pattern list 
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::setPatternText(FXint patno, const FXString& text) {
  FXnchar* pNewFilter;
  const FXnchar* pOldFilter;
  FXnchar* pOldList;
  FXint i, len = 0;

  FXASSERT(m_ofn->lpstrFilter != NULL);

  // calculate new size
  pOldFilter = m_ofn->lpstrFilter;
  for (len = 0, i = 0; pOldFilter && *pOldFilter; i++) {
    if (i != patno) len += strlen(pOldFilter) + 1;
    else len += text.length();
	pOldFilter += strlen(pOldFilter) + 1;
    len += strlen(pOldFilter) + 1;
    pOldFilter += strlen(pOldFilter) + 1;
    }
  
  // change pattern list
  pOldList = (FXnchar*)m_ofn->lpstrFilter;
  m_ofn->lpstrFilter = new FXnchar[len + 1];
  
  pNewFilter = (FXnchar*)m_ofn->lpstrFilter;
  pOldFilter = pOldList;
  for (i = 0; pOldFilter && *pOldFilter; i++) {
    if (i != patno) {
      strcpy(pNewFilter, pOldFilter);
      pNewFilter += strlen(pNewFilter) + 1;
      }
    else {
	  FXint size = text.count();
	  utf2ncs(pNewFilter, text.text(), text.length()+1);
      pNewFilter += size;
      }
    
    pOldFilter += strlen(pOldFilter) + 1;
    strcpy(pNewFilter, pOldFilter);
    pNewFilter += strlen(pNewFilter) + 1;
    pOldFilter += strlen(pOldFilter) + 1;
    }
  
  *pNewFilter = '\0';
  delete [] pOldList;
  }

/*-----------------------------------------------------------------*\
 * 
 * Get multiple files
 *
\*-----------------------------------------------------------------*/
 FXString* FXFileDialogEx::getFilenames()
 {
	 delete[] m_filenames;
	 m_filenames = NULL;

	 int i, files = 0;
	 for (i = 0; m_ofn->lpstrFile[i]; i++)
	 {
		 if (i > m_ofn->nFileOffset)
		 {
			 files = 1;
			 break;
		 }
	 }

	 if (i > 0 && files != 1)
	 {
		 i++;

		 // count files
		 for (; m_ofn->lpstrFile[i]; i++)
		 {
			 for (; m_ofn->lpstrFile[i]; i++)
			 {}

			 files++;
		 }				 
	 }

	 m_filenames = new FXString[files+1];

	 if (files == 1)
		 m_filenames[0] = m_ofn->lpstrFile;
	 else if (files > 1)
	 {
		 // split
		 FXString dir = m_ofn->lpstrFile;

		 files = 0;
		 for (i = m_ofn->nFileOffset; m_ofn->lpstrFile[i]; i++)
		 {
			 m_filenames[files] = dir + "/" + (m_ofn->lpstrFile+i);
			 files++;

			 for (; m_ofn->lpstrFile[i]; i++)
			 {}
		 }
	 }

     return m_filenames;
 }

/*-----------------------------------------------------------------*\
 * 
 * Display the file dialog
 *
\*-----------------------------------------------------------------*/
FXuint FXFileDialogEx::execute(FXuint /*placement*/) {
  if (m_opts & DLGEX_SAVE) return ::GetSaveFileNameW(m_ofn);
  
  if (!::GetOpenFileNameW(m_ofn))
  {
	  if (::CommDlgExtendedError() != FNERR_BUFFERTOOSMALL)
		  return FALSE;

	  FXMessageBox::error(m_owner, MBOX_OK, FXString(m_ofn->lpstrTitle).text(), iso2utf("Zuviele Dateien ausgewählt.").text());

      return FALSE;
  }

  return TRUE;
  }

/*-----------------------------------------------------------------*\
 * 
 * Translate a fox pattern
 *
\*-----------------------------------------------------------------*/
void FXFileDialogEx::getWinPattern(FXchar*& pszWinPat, const FXchar* pszFoxPat) const {
  FXint   nPerms = 1, nLen = 0, nSubPerms;
  FXint   i, j, k;
  FXchar* pszRedFoxPat;
  FXchar* pTgt = NULL;
  const FXchar* pSrc = NULL;
  const FXchar* pMark = NULL;
  FXbool  bIgnore = FALSE;
  pszRedFoxPat = new FXchar[strlen(pszFoxPat) + 1];
  memset(pszRedFoxPat, 0, strlen(pszFoxPat) + 1);

  /*
  ** Calculate number of permutations and reduce source pattern
  */
  pMark = NULL;
  for (pSrc = pszFoxPat, pTgt = pszRedFoxPat; *pSrc != '\0'; pSrc++) {

    if (pMark == NULL && *pSrc == '[' && *(pSrc+1) != '\0') {
      // start of character range
      if (*(pSrc+1) == '!' || *(pSrc+1) == '^') {
        bIgnore = TRUE; // ignore negations
        *(pTgt++) = '?';
        nLen++;
        FXASSERT(FALSE);
        }
      else {
        *(pTgt++) = '[';
        }
      pMark = pTgt;
      }

    else if (pMark != NULL && *pSrc != ']') {
      // only copy unique characters (ignoring case)
      if (!bIgnore && strchr(pMark, tolower(*pSrc)) == 0) *(pTgt++) = tolower(*pSrc);
      }

    else if (pMark != NULL && *pSrc == ']') {
      // end of character range
      if (bIgnore) bIgnore = FALSE;
      else if (pTgt - pMark == 0) *(--pTgt) = '\0'; // ignore []
      else if (pTgt - pMark == 1) {
        *(pTgt-2) = *(pTgt-1);
        *(--pTgt) = '\0'; // single character
        nLen ++;
        }
      else {
        nPerms *= (pTgt - pMark); // calculate permutations
        nLen ++;
        *(pTgt++) = ']';
        }
      pMark = NULL;
      }

    else {
      *(pTgt++) = tolower(*pSrc);
      nLen++;
      }
    }

  /*
  ** At this point we have the number of permutations in nPerms, 
  ** the length of each pattern in nLen,
  ** and pszRedFoxPat contains a reduced version of the original pattern
  ** (all lowercase, removed duplicates & negated ranges)
  */
  pszWinPat = new FXchar[nPerms * (nLen+1)];    // nLen + 1 because of 
                                                // separator (';') or terminating null
  /*
  ** Generate Windoze pattern
  */
  nSubPerms = nPerms;
  pMark = NULL;
  for (pSrc = pszRedFoxPat, pTgt = pszWinPat; *pSrc != '\0'; pSrc++) {

    if (pMark == NULL && *pSrc == '[' && *(pSrc+1) != '\0') pMark = pSrc + 1;

    else if (pMark != NULL && *pSrc == ']') {
      nSubPerms /= (pSrc - pMark);                // remaining permutations
      for (i = 0; i < nPerms / (nSubPerms * (pSrc - pMark)); i++) {
        for (j = 0; j < (pSrc - pMark); j++) {
          // copy as many times as there are remaining perm's
          for (k = 0; k < nSubPerms; k++) {
            pTgt[(i*(nSubPerms * (pSrc - pMark))+j*nSubPerms+k) * (nLen+1)] = pMark[j];
            }
          }
        }
      pTgt++;
      pMark = NULL;
      }

    else if (pMark == NULL) {
      for (i = 0; i < nPerms; i++) {
        pTgt[i *(nLen + 1)] = *pSrc;
        }
      pTgt++;
      }
    }

  /*
  ** That's it. Insert the separators and we're done.
  */
  for (i = 0; i < nPerms - 1; i++) {
    pTgt[i *(nLen + 1)] = ';';
    }
  pTgt[i *(nLen + 1)] = '\0';
  delete [] pszRedFoxPat;
  }

}
#endif /* WIN32_COMDLG */

