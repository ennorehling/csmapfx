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
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "fxwin.h"

#include <windows.h>
#include <commdlg.h>

#include <fx.h>
#include "fxhelper.h"
#include "FXFileDialogEx.h"
using namespace FX;


static LPWSTR utf2mbs(LPCCH lpMultiByteStr)
{
    int chars = MultiByteToWideChar(CP_UTF8, 0, lpMultiByteStr, -1, NULL, 0);
    if (chars == 0) {
        return NULL;
    }
    LPWSTR lpWideCharStr = new WCHAR[chars];
    if (MultiByteToWideChar(CP_UTF8, 0, lpMultiByteStr, -1, lpWideCharStr, chars) > 0) {
        return lpWideCharStr;
    }
    return NULL;
}

namespace FXEX {

    // maps
    FXIMPLEMENT(FXFileDialogEx, FXObject, NULL, 0);

    // de-serialisation
    FXFileDialogEx::FXFileDialogEx() : FXObject(), m_owner(0) {}

    // ctor
    FXFileDialogEx::FXFileDialogEx(FXWindow *owner, const FXString &title, FXuint opts, FXint x, FXint y, FXint w, FXint h) :
        FXObject(), m_owner(owner), m_title(title)
    {
        HRESULT hr;
        if (opts & DLGEX_SAVE) {
            hr = CoCreateInstance(CLSID_FileSaveDialog,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_pfd));
        }
        else {
            hr = CoCreateInstance(CLSID_FileOpenDialog,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_pfod));
            m_pfd = m_pfod;
            m_options |= FOS_FILEMUSTEXIST;
        }
        if (opts & DLGEX_CREATEPROMPT) m_options |= FOS_CREATEPROMPT;
        if (opts & DLGEX_PATHMUSTEXIST) m_options |= FOS_PATHMUSTEXIST;
        if (opts & DLGEX_FILEMUSTEXIST) m_options |= FOS_FILEMUSTEXIST;
        if (opts & DLGEX_OVERWRITEPROMPT) m_options |= FOS_OVERWRITEPROMPT;
        if (opts & DLGEX_DONTADDTORECENT) m_options |= FOS_DONTADDTORECENT;
        if (opts & DLGEX_ALLOWMULTISELECT) m_options |= FOS_ALLOWMULTISELECT;
    }

    FXFileDialogEx::~FXFileDialogEx() {
        if (m_pfd) {
            m_pfd->Release();
        }
        delete[] m_filenames;
        deleteFilters();
    }

    /*-----------------------------------------------------------------*\
     *
     * Set / get the dialog title
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setTitle(const FXString &name) {
        m_title = name;
        WCHAR szFilePath[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, m_title.text(), -1, szFilePath, MAX_PATH);
        m_pfd->SetTitle(szFilePath);
    }

    FXString FXFileDialogEx::getTitle() const {
        return m_title;
    }

    /// set icon of dialog (no-op)
    void FXFileDialogEx::setIcon(FXIcon *icon) {
        return;
    }

    void FXFileDialogEx::setSelectMode(int mode)
    {
        if (mode == SELECTFILE_MULTIPLE)
            m_options |= FOS_ALLOWMULTISELECT;
        else
            m_options &= ~FOS_ALLOWMULTISELECT;
    }

    /*-----------------------------------------------------------------*\
     *
     * Set / get the file name
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setFilename(const FXString &path) {
        if (m_pfd) {
            LPWSTR pszName = utf2mbs(path.text());
            HRESULT hr = m_pfd->SetFileName(pszName);
            delete[] pszName;
        }
    }

    FXString FXFileDialogEx::getFilename() const {
        FXString result;
        if (m_pfd) {
            // Obtain the result of the user interaction.
            IShellItem *psiResult = NULL;
            HRESULT hr = m_pfd->GetResult(&psiResult);

            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = NULL;
                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
                    &pszFilePath);
                if (SUCCEEDED(hr)) {
                    result.assign(pszFilePath);
                }
                psiResult->Release();
            }
        }
        return result;
    }

    /*-----------------------------------------------------------------*\
     *
     * Set the initial directory
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setDirectory(const FXString &path) {
        if (m_pfd && !path.empty()) {
            IShellItem *psi = NULL;
            HRESULT hr = S_OK;
            WCHAR szPath[MAX_PATH];
            if (MultiByteToWideChar(CP_UTF8, 0, path.text(), path.length(), szPath, MAX_PATH) > 0) {
                hr = SHCreateItemFromParsingName(szPath, NULL, IID_IShellItem, (void **)&psi);
            }
            if (SUCCEEDED(hr)) {
                hr = m_pfd->SetDefaultFolder(psi);
            }
        }
    }

    /*-----------------------------------------------------------------*\
     *
     * Get the selected directory
     *
    \*-----------------------------------------------------------------*/
    FXString FXFileDialogEx::getDirectory() const {
        FXString result;
        if (m_pfod) {
            IShellItem *psiResult;
            HRESULT hr = m_pfod->GetFolder(&psiResult);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = NULL;
                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
                    &pszFilePath);
                if (SUCCEEDED(hr)) {
                    result.assign(pszFilePath);
                }
            }
        }
        return result;
    }

    /*-----------------------------------------------------------------*\
     *
     * Select the file pattern
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setPattern(const FXString &ptrn) {
        for (FXint i = 0; i != m_cFileTypes; ++i) {
            if (ptrn == m_filterPatterns[i]) {
                m_pfd->SetFileTypeIndex(i + 1);
                return;
            }
        }
    }

    /*-----------------------------------------------------------------*\
     *
     * Get the selected file extension
     *
    \*-----------------------------------------------------------------*/
    FXString FXFileDialogEx::getPattern() const {
        UINT nIndex = (UINT)getCurrentPattern();
        if (nIndex < m_cFileTypes) {
            return m_filterPatterns[nIndex];
        }
        FXASSERT(FALSE);
        return FXString::null;
    }

    /*-----------------------------------------------------------------*\
     *
     * Set the pattern list
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setPatternList(const FXString &ps) {
        /*
        ** Remove old items
        */
        bool bHasAll = false;

        /*
        ** Count patterns && and allocate memory
        */
        FXString patterns = ps;
        FXint nPat = 0;
        if (patterns.empty()) {
            nPat = 0;
        }
        else {
            nPat = 1;
            for (FXint i = 0; i != patterns.length(); i++)
                if (patterns[i] == '\n')
                    nPat++;
        }
        m_filterNames = new FXString[nPat + 1];
        m_filterPatterns = new FXString[nPat + 1];
        for (FXint i = 0; i != nPat; i++)
        {
            FXString pat = patterns.before('\n');
            patterns = patterns.after('\n');

            m_filterNames[i] = pat.before('(').trimEnd();
            m_filterPatterns[i] = FXFileSelector::patternFromText(pat);
            if (m_filterPatterns[i] == "*") bHasAll = true;
        }

        if (!bHasAll) {
            ++nPat;
            m_filterNames[nPat] = "All Files";
            m_filterPatterns[nPat] = "*";
        }

        if (m_pfd) {
            m_cFileTypes = nPat;
            m_rgFilterSpec = new COMDLG_FILTERSPEC[m_cFileTypes];
            for (UINT nIndex = 0; nIndex != m_cFileTypes; ++nIndex) {
                m_rgFilterSpec[nIndex].pszName = utf2mbs(m_filterNames[nIndex].text());
                m_rgFilterSpec[nIndex].pszSpec = utf2mbs(m_filterPatterns[nIndex].text());
            }
        }
    }

    /*-----------------------------------------------------------------*\
     *
     * Get index of selected pattern in the list (zero based)
     *
    \*-----------------------------------------------------------------*/
    FXint FXFileDialogEx::getCurrentPattern() const {
        UINT nIndex = 0;
        HRESULT hr = m_pfd->GetFileTypeIndex(&nIndex);
        FXASSERT(SUCCEEDED(hr));
        return nIndex - 1;
    }

    /*-----------------------------------------------------------------*\
     *
     * Select the inital pattern from the list (zero based)
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setCurrentPattern(FXint nPat) {
        UINT nIndex = nPat + 1;
        HRESULT hr = m_pfd->SetFileTypeIndex(nIndex);
        FXASSERT(SUCCEEDED(hr));
    }

    /*-----------------------------------------------------------------*\
     *
     * Get the selected pattern in the list
     *
    \*-----------------------------------------------------------------*/
    FXString FXFileDialogEx::getPatternText(FXint nPat) const {
        UINT nIndex = (UINT)nPat;
        if (nIndex < m_cFileTypes) {
            return m_filterNames[nIndex];
        }
        FXASSERT(FALSE);
        return FXString::null;
    }

    /*-----------------------------------------------------------------*\
     *
     * Modify pattern list
     *
    \*-----------------------------------------------------------------*/
    void FXFileDialogEx::setPatternText(FXint nPat, const FXString &text) {
        UINT nIndex = (UINT)nPat;
        if (nIndex < m_cFileTypes) {
            m_filterNames[nIndex] = text;
        }
    }

    /*-----------------------------------------------------------------*\
     *
     * Get multiple files
     *
    \*-----------------------------------------------------------------*/
    FXString *FXFileDialogEx::getFilenames()
    {
        delete[] m_filenames;
        m_filenames = NULL;
        if (m_pfod) {
            IShellItemArray *psiaResults = NULL;
            HRESULT hr = m_pfod->GetResults(&psiaResults);
            if (SUCCEEDED(hr))
            {
                DWORD dwFiles = 0;
                hr = psiaResults->GetCount(&dwFiles);
                if (SUCCEEDED(hr)) {
                    m_filenames = new FXString[dwFiles + 1];
                    for (DWORD dwIndex = 0; dwIndex != dwFiles; ++dwIndex) {
                        IShellItem *psiResult = NULL;
                        psiaResults->GetItemAt(dwIndex, &psiResult);
                        if (SUCCEEDED(hr)) {
                            PWSTR pszFilePath = NULL;
                            hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
                                &pszFilePath);
                            if (SUCCEEDED(hr)) {
                                m_filenames[dwIndex].assign(pszFilePath);
                            }
                            psiResult->Release();
                        }
                    }
                    psiaResults->Release();
                }
            }
        }
        return m_filenames;
    }

    /*-----------------------------------------------------------------*\
     *
     * Display the file dialog
     *
    \*-----------------------------------------------------------------*/
    FXuint FXFileDialogEx::execute(FXuint /*placement*/)
    {
        WCHAR szFilePath[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, m_title.text(), -1, szFilePath, MAX_PATH);
        HRESULT hr = m_pfd->SetTitle(szFilePath);
        if (SUCCEEDED(hr)) {
            hr = m_pfd->SetOptions(m_options);
        }
        if (SUCCEEDED(hr)) {
            hr = m_pfd->SetFileTypes(m_cFileTypes, m_rgFilterSpec);
        }
        if (SUCCEEDED(hr)) {
            HWND hwndOwner = (HWND)m_owner->id();
            hr = m_pfd->Show(hwndOwner);
        }
        return SUCCEEDED(hr);
    }

    void FXFileDialogEx::deleteFilters(void)
    {
        if (m_rgFilterSpec) {
            for (UINT i = 0; i != m_cFileTypes; ++i) {
                delete[] m_rgFilterSpec[i].pszName;
                delete[] m_rgFilterSpec[i].pszSpec;
            }
            delete[] m_rgFilterSpec;
            m_rgFilterSpec = NULL;
        }
        delete[] m_filterNames;
        delete[] m_filterPatterns;
        m_cFileTypes = 0;
    }

}
#endif
