#ifndef _CSMAP_FXHELPER_H
#define _CSMAP_FXHELPER_H

#include <vector>
#include <ostream>
#include <fx.h>

extern FXString FXString_Empty;
// array begin() / end() template
// ------------------------------
template <typename T, std::size_t size>
T* begin(T (&array)[size])
{
	return array;
}

template <typename T, std::size_t size>
T* end(T (&array)[size])
{
	return array + size;
}

// FXString converter: iso8859-1 <-> utf8
// --------------------------------------
FX::FXString utf2iso(const FX::FXString& s);
FX::FXString iso2utf(const FX::FXString& s);
FX::FXString iso2utf(const FXchar* src, FXint nsrc);

int isUTF8(const char* data, size_t size);

// flatten strings: Removed spaces,
// german umlauts to ae,oe,ue,ss and
// all letters to lower case.
// ---------------------------------
std::string flatten(const std::string& str);
FX::FXString flatten(const FX::FXString& str);

// Small error functions
// ---------------------
void showError(const FX::FXString& str);
void showError(const std::string& str);

// FXString operator<<
// -------------------
inline std::ostream& operator<<(std::ostream& out, const FX::FXString& str)
{
    return out << str.text();
}

FXString FXStringValEx(FXulong num, unsigned int base = 10);

struct FXParam {
    FXString key;
    FXString value;
};
void FXParseCommandLine(const std::vector<FXString>& args, std::vector<FXString>& tokens, std::vector<FXString>& switches, std::vector<FXParam>& params);

std::string loadResourceFile(const char * relpath);
#endif //_CSMAP_FXHELPER_H
