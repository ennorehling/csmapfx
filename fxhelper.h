#ifndef _CSMAP_FXHELPER_H
#define _CSMAP_FXHELPER_H

#include <vector>
#include <ostream>
#include <fx.h>

#include <boost/signal.hpp>
#include <boost/bind.hpp>

#define ITEM_INT(vptr) (0xffffffff & (long)(vptr))

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
typedef boost::signal<void (bool)> flag_signal_t;

typedef boost::signals::connection connection_t;

// menu command with signals
// -------------------------
class FXAPI FXMenuCommandEx : public FXMenuCommand
{
	FXDECLARE(FXMenuCommandEx)
protected:
	FXMenuCommandEx();
private:
	FXMenuCommandEx(const FXMenuCommandEx&);
	FXMenuCommandEx &operator=(const FXMenuCommandEx&);
public:
	long onInvokeCommand(FXObject*,FXSelector,void*);
	long onInvokeUpdate(FXObject*,FXSelector,void*);

	enum
	{
		ID_SELFTARGET = FXMenuCommand::ID_LAST,
		ID_LAST
	};

public:
    void onCommand(command_signal_t::slot_function_type slot) { m_onCommand.connect(slot); }
	void onUpdate(update_signal_t::slot_function_type slot) { m_onUpdate.connect(slot); }

	void disconnectCommand() { m_onCommand.disconnect_all_slots(); }
	void disconnectUpdate() { m_onUpdate.disconnect_all_slots(); }

public:
	/// Construct a menu command
	FXMenuCommandEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,update_signal_t::slot_function_type upd,FXIcon* ic=NULL,FXuint opts=0);
	FXMenuCommandEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,FXIcon* ic=NULL,FXuint opts=0);

private:
	command_signal_t m_onCommand;
	update_signal_t m_onUpdate;
};

// toolbar button with signals
// ---------------------------
class FXAPI FXButtonEx : public FXButton
{
	FXDECLARE(FXButtonEx)
protected:
	FXButtonEx();
private:
	FXButtonEx(const FXButtonEx&);
	FXButtonEx &operator=(const FXButtonEx&);
public:
	long onInvokeCommand(FXObject*,FXSelector,void*);
	long onInvokeUpdate(FXObject*,FXSelector,void*);

	enum
	{
		ID_SELFTARGET = FXButton::ID_LAST,
		ID_LAST
	};

public:
    void onCommand(command_signal_t::slot_function_type slot) { m_onCommand.connect(slot); }
	void onUpdate(update_signal_t::slot_function_type slot) { m_onUpdate.connect(slot); }

	void disconnectCommand() { m_onCommand.disconnect_all_slots(); }
	void disconnectUpdate() { m_onUpdate.disconnect_all_slots(); }

public:
	/// Construct a menu command
	FXButtonEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,update_signal_t::slot_function_type upd,FXIcon* ic=NULL,FXuint opts=0);
	FXButtonEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,FXIcon* ic=NULL,FXuint opts=0);

private:
	command_signal_t m_onCommand;
	update_signal_t m_onUpdate;
};

// check button with signals
// -------------------------
class FXAPI FXCheckButtonEx : public FXCheckButton
{
	FXDECLARE(FXCheckButtonEx)
protected:
	FXCheckButtonEx();
private:
	FXCheckButtonEx(const FXCheckButtonEx&);
	FXCheckButtonEx &operator=(const FXCheckButtonEx&);
public:
	long onInvokeCommand(FXObject*,FXSelector,void*);
	long onInvokeUpdate(FXObject*,FXSelector,void*);
	long onAccelCmd(FXObject*,FXSelector,void*);

	enum
	{
		ID_SELFTARGET = FXButton::ID_LAST,
		ID_LAST
	};

public:
    void onCommand(flag_signal_t::slot_function_type slot) { m_onCommand.connect(slot); }
	void onUpdate(update_signal_t::slot_function_type slot) { m_onUpdate.connect(slot); }

	void disconnectCommand() { m_onCommand.disconnect_all_slots(); }
	void disconnectUpdate() { m_onUpdate.disconnect_all_slots(); }

public:
	/// Construct a menu command
	FXCheckButtonEx(FXComposite* p,const FXString& text,flag_signal_t::slot_function_type slot,update_signal_t::slot_function_type upd,FXuint opts=0);
	FXCheckButtonEx(FXComposite* p,const FXString& text,flag_signal_t::slot_function_type slot,FXuint opts=0);

private:
	flag_signal_t m_onCommand;
	update_signal_t m_onUpdate;
};

#endif //_CSMAP_FXHELPER_H
