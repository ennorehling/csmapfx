#include <iostream>
#include <string>
#include <cctype>
#include <fx.h>
#include <FX88591Codec.h>

#ifdef WIN32
#include "shlobj.h"		// SHGetSpecialFolderPath
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
