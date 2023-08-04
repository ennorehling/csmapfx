#include <cctype>
#include <iostream>
#include <string>

#include <fx.h>
#include <FX88591Codec.h>

#ifdef WIN32
#include "shlobj.h"		// SHGetSpecialFolderPath
#endif

#ifdef HAVE_PHYSFS
#include <memory>
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

int isUTF8(const char* data, size_t size)
{
    const unsigned char* str = (unsigned char*)data;
    const unsigned char* end = str + size;
    unsigned char byte;
    unsigned int code_length, i;
    uint32_t ch;
    while (str != end) {
        byte = *str;
        if (byte <= 0x7F) {
            /* 1 byte sequence: U+0000..U+007F */
            str += 1;
            continue;
        }

        if (0xC2 <= byte && byte <= 0xDF)
            /* 0b110xxxxx: 2 bytes sequence */
            code_length = 2;
        else if (0xE0 <= byte && byte <= 0xEF)
            /* 0b1110xxxx: 3 bytes sequence */
            code_length = 3;
        else if (0xF0 <= byte && byte <= 0xF4)
            /* 0b11110xxx: 4 bytes sequence */
            code_length = 4;
        else {
            /* invalid first byte of a multibyte character */
            return 0;
        }

        if (str + (code_length - 1) >= end) {
            /* truncated string or invalid byte sequence */
            return 0;
        }

        /* Check continuation bytes: bit 7 should be set, bit 6 should be
         * unset (b10xxxxxx). */
        for (i = 1; i < code_length; i++) {
            if ((str[i] & 0xC0) != 0x80)
                return 0;
        }

        if (code_length == 2) {
            /* 2 bytes sequence: U+0080..U+07FF */
            ch = ((str[0] & 0x1f) << 6) + (str[1] & 0x3f);
            /* str[0] >= 0xC2, so ch >= 0x0080.
               str[0] <= 0xDF, (str[1] & 0x3f) <= 0x3f, so ch <= 0x07ff */
        }
        else if (code_length == 3) {
            /* 3 bytes sequence: U+0800..U+FFFF */
            ch = ((str[0] & 0x0f) << 12) + ((str[1] & 0x3f) << 6) +
                (str[2] & 0x3f);
            /* (0xff & 0x0f) << 12 | (0xff & 0x3f) << 6 | (0xff & 0x3f) = 0xffff,
               so ch <= 0xffff */
            if (ch < 0x0800)
                return 0;

            /* surrogates (U+D800-U+DFFF) are invalid in UTF-8:
               test if (0xD800 <= ch && ch <= 0xDFFF) */
            if ((ch >> 11) == 0x1b)
                return 0;
        }
        else if (code_length == 4) {
            /* 4 bytes sequence: U+10000..U+10FFFF */
            ch = ((str[0] & 0x07) << 18) + ((str[1] & 0x3f) << 12) +
                ((str[2] & 0x3f) << 6) + (str[3] & 0x3f);
            if ((ch < 0x10000) || (0x10FFFF < ch))
                return 0;
        }
        str += code_length;
    }
    return 1;
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

void FXParseCommandLine(const std::vector<FXString>& args, std::vector<FXString>& tokens, std::vector<FXString>& switches, std::vector<FXParam>& params)
{
    for (const FXString& arg : args) {
        if (arg[0] == '-') {
            FXint split = arg.find('=');
            if (split < 0) {
                switches.push_back(arg.right(arg.length() - 1));
            } else {
                FXParam param;
                param.key = arg.mid(1, split - 1);
                param.value = arg.right(arg.length() - split - 1);
                params.push_back(param);
            }
        }
        else {
            tokens.push_back(arg);
        }
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
            auto arr = std::unique_ptr<char[]>(new char[size]);
            char * data = arr.get();
            if (PHYSFS_readBytes(file, data, filesize) == filesize) {
                result.assign(data, size);
            }
        }
        PHYSFS_close(file);
    }
#elif defined(WIN32)
    TCHAR pf[MAX_PATH];
    if (SHGetSpecialFolderPath(0, pf, CSIDL_APPDATA, FALSE))
    {
        FXString filename(pf);
        filename += "\\Eressea\\CsMapFX\\";
        filename += relpath;
        std::ifstream file;
        file.open(filename.text(), std::ios::in | std::ios::binary);
        if (file.is_open())
        {
            std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return str;
        }
    }
#endif
    return result;
}
