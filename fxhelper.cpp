#include <iostream>
#include <string>
#include <cctype>
#include <fx.h>
#include <FX88591Codec.h>

#ifdef WIN32
#include "shlobj.h"		// SHGetSpecialFolderPath
#else
#include <iconv.h>
#endif

#include "version.h"
#include "fxhelper.h"

FX::FXString display(const wchar_t *wstr)
{
    char buffer[256];
    int n = 0;
#ifdef WIN32
    n = WideCharToMultiByte(CP_ACP, 0, wstr, -1, buffer, sizeof(buffer), NULL, NULL);
#else
    char * inbuf = (char *)wstr;
    char * outbuf = buffer;
    size_t outlen = sizeof(buffer);
    size_t inlen = wcslen(wstr) * sizeof(wchar_t);
    iconv_t ic = iconv_open("", "WCHAR_T");
    size_t bytes = iconv(ic, &inbuf, &inlen, &outbuf, &outlen);
    n = (int)bytes;
    /* really terrible solution, use ICU or iconv
    for (int i = 0; wstr[i]; ++i) {
        wchar_t wc = wstr[i];
        if (wc < 128) {
            buffer[n++] = (char)wc;
        }
    }*/
    iconv_close(ic);
#endif
    buffer[n] = 0;
    return FXString(buffer, n);
}

// FXString converter: iso8859-1 <-> utf8
// --------------------------------------
FX::FXString iso2utf(const FX::FXString& s)
{
	/*
	static FX::FX88591Codec codec;

	return codec.mb2utf(s);
	 */
	 return s;
}

FX::FXString utf2iso(const FX::FXString& s)
{
	static FX::FX88591Codec codec;

	return codec.utf2mb(s);
}

// flatten strings: Removed spaces,
// german umlauts to ae,oe,ue,ss and
// all letters to lower case.
// ---------------------------------
std::string flatten(const std::string& str)
{
	std::string out;

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		unsigned char c = str[i];

		if (c == 0xdf)
			out += "ss";
		else if (c == 0xef || c == 0xc4)
			out += "ae";
		else if (c == 0xd6 || c == 0xf6)
			out += "oe";
		else if (c == 0xdc || c == 0xfc)
			out += "ue";
		else if (isalnum(c))
			out += (char)std::tolower(c);
	}

	return out;
}

FXString flatten(const FXString& str)
{
	FXString out;

	for (int i = 0; i < str.length(); i = str.inc(i))
	{
		FXwchar wc = str.wc(i);

		if (wc == L"\u00df"[0])
			out += "ss";
		else if (wc == L"\u00c4"[0] || wc == L"\u00e4"[0])
			out += "ae";
		else if (wc == L"\u00d6"[0] || wc == L"\u00f6"[0])
			out += "oe";
		else if (wc == L"\u00dc"[0] || wc == L"\u00fc"[0])
			out += "ue";
		else
			out += (char)std::tolower(wc);
	}

	return out;
}

// Small error function
// --------------------
void showError(const FXString& str)
{
#ifdef WIN32
	FXMessageBox::information(FXApp::instance(), MBOX_OK, CSMAP_APP_TITLE, str.text());
#else
	showError((std::string)utf2iso(str).text());	// caution: cross forwarding!
#endif
}

void showError(const std::string& str)
{
#ifdef WIN32
	showError((FXString)iso2utf(str.c_str()));	// caution: cross forwarding!
#else
	std::cerr << str << std::endl;
#endif
}

// get search path for app config data
// -----------------------------------
std::vector<FXString> getSearchPath()
{
	std::vector<FXString> searchPath;

	// add path to .exe file
#ifdef WIN32
	char pathName[MAX_PATH];
	DWORD length = ::GetModuleFileName(NULL, pathName, MAX_PATH);

	FXString exePath = FXString(pathName, length).rbefore('\\');

	if (!exePath.empty())
		searchPath.push_back(exePath + "\\");
#endif

	// add app path / home directory
#ifdef WIN32
	BOOL res = SHGetSpecialFolderPath(NULL, pathName, CSIDL_LOCAL_APPDATA, false);
	if (res)
	{
		// Z.B. "C:\Users\Phygon\AppData\Local\CSMap\"
		FXString appPath(pathName);
		searchPath.push_back(appPath + "\\CSMap\\");
	}
#else
	searchPath.push_back("~/.");
#endif

	// add current directory
	searchPath.push_back(".\\");

	return searchPath;
}

