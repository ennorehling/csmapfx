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
		char c = str[i];

		if (c == 'ß')
			out += "ss";
		else if (c == 'Ä' || c == 'ä')
			out += "ae";
		else if (c == 'Ö' || c == 'ö')
			out += "oe";
		else if (c == 'Ü' || c == 'ü')
			out += "ue";
		else if (c != ' ')
			out += std::tolower(c);
	}

	return out;
}

FXString flatten(const FXString& str)
{
	FXString out;

	for (int i = 0; i < str.length(); i = str.inc(i))
	{
		FXwchar wc = str.wc(i);

		if (wc == L'ß')
			out += "ss";
		else if (wc == L'Ä' || wc == L'ä')
			out += "ae";
		else if (wc == L'Ö' || wc == L'ö')
			out += "oe";
		else if (wc == L'Ü' || wc == L'ü')
			out += "ue";
		else
			out += std::tolower(wc);
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

// menu command with signals
// -------------------------
FXDEFMAP(FXMenuCommandEx) CommandMessageMap[]={
	FXMAPFUNC(SEL_COMMAND, FXMenuCommandEx::ID_SELFTARGET, FXMenuCommandEx::onInvokeCommand),
	FXMAPFUNC(SEL_UPDATE, FXMenuCommandEx::ID_SELFTARGET, FXMenuCommandEx::onInvokeUpdate)
};

// Object implementation
FXIMPLEMENT(FXMenuCommandEx,FXMenuCommand,CommandMessageMap,ARRAYNUMBER(CommandMessageMap))

FXMenuCommandEx::FXMenuCommandEx() : FXMenuCommand()
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
}

FXMenuCommandEx::FXMenuCommandEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,update_signal_t::slot_function_type upd,FXIcon* ic /*=NULL*/,FXuint opts /*=0*/) : FXMenuCommand(p, text, ic, NULL,0, opts)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
	onCommand(slot);
	onUpdate(upd);
}

FXMenuCommandEx::FXMenuCommandEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,FXIcon* ic /*=NULL*/,FXuint opts /*=0*/) : FXMenuCommand(p, text, ic, NULL,0, opts)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
	onCommand(slot);
}

long FXMenuCommandEx::onInvokeCommand(FXObject*,FXSelector,void*)
{
	if (isEnabled())
		m_onCommand();
	return 1;
}

long FXMenuCommandEx::onInvokeUpdate(FXObject*,FXSelector,void*)
{
	if (m_onUpdate.empty() || m_onUpdate())
	{
		if (!isEnabled())
			enable();
	}
	else
	{
		if (isEnabled())
			disable();
	}
	return 1;
}

// toolbar button with signals
// ---------------------------
FXDEFMAP(FXButtonEx) ButtonMessageMap[]={
	FXMAPFUNC(SEL_COMMAND, FXButtonEx::ID_SELFTARGET, FXButtonEx::onInvokeCommand),
	FXMAPFUNC(SEL_UPDATE, FXButtonEx::ID_SELFTARGET, FXButtonEx::onInvokeUpdate),
	FXMAPFUNC(SEL_COMMAND, FXWindow::ID_ACCEL, FXButtonEx::onInvokeCommand),
};

// Object implementation
FXIMPLEMENT(FXButtonEx,FXButton,ButtonMessageMap,ARRAYNUMBER(ButtonMessageMap))

FXButtonEx::FXButtonEx() : FXButton(NULL, "", NULL, NULL,0, /*BUTTON_NORMAL|BUTTON_TOOLBAR|*/FRAME_RAISED|BUTTON_AUTOGRAY)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
}

FXButtonEx::FXButtonEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,update_signal_t::slot_function_type upd,FXIcon* ic /*=NULL*/,FXuint opts /*=0*/) : FXButton(p, text, ic, NULL,0, opts|/*BUTTON_NORMAL|BUTTON_TOOLBAR|*/FRAME_RAISED|BUTTON_AUTOGRAY)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
	onCommand(slot);
	onUpdate(upd);
}

FXButtonEx::FXButtonEx(FXComposite* p,const FXString& text,command_signal_t::slot_function_type slot,FXIcon* ic /*=NULL*/,FXuint opts /*=0*/) : FXButton(p, text, ic, NULL,0, opts|/*BUTTON_NORMAL|BUTTON_TOOLBAR|*/FRAME_RAISED|BUTTON_AUTOGRAY)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
	onCommand(slot);
}

long FXButtonEx::onInvokeCommand(FXObject*,FXSelector,void*)
{
	if (isEnabled())
		m_onCommand();
	return 1;
}

long FXButtonEx::onInvokeUpdate(FXObject*,FXSelector,void*)
{
	if (m_onUpdate.empty() || m_onUpdate())
	{
		if (!isEnabled())
			enable();
	}
	else
	{
		if (isEnabled())
			disable();
	}
	return 1;
}

// check button with signals
// -------------------------
FXDEFMAP(FXCheckButtonEx) CheckButtonMessageMap[]={
	FXMAPFUNC(SEL_COMMAND, FXCheckButtonEx::ID_SELFTARGET, FXCheckButtonEx::onInvokeCommand),
	FXMAPFUNC(SEL_UPDATE, FXCheckButtonEx::ID_SELFTARGET, FXCheckButtonEx::onInvokeUpdate),
	FXMAPFUNC(SEL_COMMAND, FXWindow::ID_ACCEL, FXCheckButtonEx::onAccelCmd),
};

// Object implementation
FXIMPLEMENT(FXCheckButtonEx,FXCheckButton,CheckButtonMessageMap,ARRAYNUMBER(CheckButtonMessageMap))

FXCheckButtonEx::FXCheckButtonEx() : FXCheckButton(NULL, "", NULL,0, CHECKBUTTON_NORMAL|CHECKBUTTON_AUTOGRAY)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
}

FXCheckButtonEx::FXCheckButtonEx(FXComposite* p,const FXString& text,flag_signal_t::slot_function_type slot,update_signal_t::slot_function_type upd,FXuint opts /*=0*/) : FXCheckButton(p, text, NULL,0, opts|CHECKBUTTON_NORMAL|CHECKBUTTON_AUTOGRAY)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
	onCommand(slot);
	onUpdate(upd);
}

FXCheckButtonEx::FXCheckButtonEx(FXComposite* p,const FXString& text,flag_signal_t::slot_function_type slot,FXuint opts /*=0*/) : FXCheckButton(p, text, NULL,0, opts|CHECKBUTTON_NORMAL|CHECKBUTTON_AUTOGRAY)
{
	setTarget(this);
	setSelector(ID_SELFTARGET);
	onCommand(slot);
}

long FXCheckButtonEx::onAccelCmd(FXObject*,FXSelector,void*)
{
	if (isEnabled())
	{
		setCheck(!getCheck(), 1);
		//m_onCommand(getCheck() ? true : false);
	}
	return 1;
}

long FXCheckButtonEx::onInvokeCommand(FXObject*,FXSelector,void*)
{
	if (isEnabled())
		m_onCommand(getCheck() ? true : false);
	return 1;
}

long FXCheckButtonEx::onInvokeUpdate(FXObject*,FXSelector,void*)
{
	if (m_onUpdate.empty())
	{
		if (!isEnabled())
			enable();
	}
	else
	{
		int res = m_onUpdate();
		if (res == -1)
		{
			if (isEnabled())
				disable();
		}
		else
		{
			if (!isEnabled())
				enable();
			setCheck(res);
		}
	}
	return 1;
}
