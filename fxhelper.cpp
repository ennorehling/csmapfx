#include <iostream>
#include <string>
#include <cctype>
#include <fx.h>
#include <FX88591Codec.h>

#ifdef WIN32
#include "shlobj.h"		// SHGetSpecialFolderPath
#endif

#ifdef HAVE_PHYSFS
#include <physfs.h>
#elif defined(WIN32)
#include <shlobj_core.h>
#include <fstream>
#include <vector>
#endif

#include "version.h"
#include "fxhelper.h"

FX::FXString FXString_Empty("");

FX::FXString utf2iso(const FX::FXString& s)
{
	static FX::FX88591Codec codec;

	return codec.utf2mb(s);
}

FX::FXString iso2utf(const FX::FXString& s)
{
	static FX::FX88591Codec codec;

	return codec.mb2utf(s);
}

FX::FXString iso2utf(const FXchar * src, FXint nsrc)
{
	static FX::FX88591Codec codec;

	return codec.mb2utf(src, nsrc);
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

FX::FXString flatten(const FX::FXString& str)
{
    FX::FXString out;

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
void showError(const FX::FXString& str)
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
	showError((FXString)FXString(str.c_str()));	// caution: cross forwarding!
#else
	std::cerr << str << std::endl;
#endif
}

FXString FXStringValEx(FXulong num, unsigned int base)
{
    if (base <= 16)
    {
        return FXStringVal(num, base);
    }
    else {
        char buf[40], b36[] = "0123456789abcdefghijkLmnopqrstuvwxyz";
        char* p = buf + sizeof(buf) - 1;
        *p = '\0';
        do {
            *--p = b36[num % 36];
            num /= 36;
        } while (num);

        return FXString(p, buf + sizeof(buf) - 1 - p);
    }
}

std::string loadResourceFile(const char *relpath)
{
    std::string result;
#ifdef HAVE_PHYSFS
    PHYSFS_File* file;
    file = PHYSFS_openRead(relpath);
    if (file) {
        PHYSFS_sint64 filesize = PHYSFS_fileLength(file);

        if (filesize > 0) {
            size_t size = (size_t)filesize;
            char* data = new char[size];
            if (data) {
                if (PHYSFS_readBytes(file, data, filesize) == filesize) {
                    result.assign(data, size);
                }
                delete[] data;
            }
        }
        PHYSFS_close(file);
    }
#elif defined(WIN32)
    TCHAR pf[MAX_PATH];
    if (SHGetSpecialFolderPath(0, pf, CSIDL_APPDATA, FALSE))
    {
        std::string filename(pf);
        filename += "\\Eressea\\CsMapFX\\";
        filename += relpath;
        std::ifstream file;
        file.open(filename.c_str(), std::ios::in | std::ios::binary);
        if (file.is_open())
        {
            std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return str;
        }
    }
#endif
    return result;
}
