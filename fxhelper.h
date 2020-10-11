#ifndef _CSMAP_FXHELPER_H
#define _CSMAP_FXHELPER_H

#include <vector>
#include <ostream>
#include <fx.h>

#include <boost/signal.hpp>
#include <boost/bind.hpp>

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
FX::FXString iso2utf(const FX::FXString& s);
FX::FXString utf2iso(const FX::FXString& s);

// flatten strings: Removed spaces,
// german umlauts to ae,oe,ue,ss and
// all letters to lower case.
// ---------------------------------
std::string flatten(const std::string& str);
FXString flatten(const FXString& str);

// Small error functions
// ---------------------
void showError(const FXString& str);
void showError(const std::string& str);

// get search path for app config data
// -----------------------------------
std::vector<FXString> getSearchPath();

// FXString operator<<
// -------------------
inline std::ostream& operator<<(std::ostream& out, const FX::FXString& str)
{
    return out << utf2iso(str).text();
}

// command signals
// ---------------
typedef boost::signal<void ()> command_signal_t;
typedef boost::signal<int ()> update_signal_t;		// allowed: true, false, -1

typedef boost::signals::connection connection_t;

#endif //_CSMAP_FXHELPER_H
