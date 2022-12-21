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
    return out << utf2iso(str).text();
}

FXString FXStringValEx(FXulong num, unsigned int base = 10);

unsigned char* loadResourceFile(const FXString& relpath);
#endif //_CSMAP_FXHELPER_H
