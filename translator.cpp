#include <iostream>
#include <string>
#include <map>
#include <fx.h>
#include <FXTranslator.h>

#include "translator.h"
#include "fxhelper.h"

// GermanTranslation implementation
// --------------------------------

GermanTranslation::GermanTranslation(FXApp* app) : FXTranslator(app)
{
	m_trans[std::make_pair("FXStatusLine", "Ready.")] = L"Bereit.";

	m_trans[std::make_pair("FXInputDialog", "&OK")] = L"&OK";
	m_trans[std::make_pair("FXInputDialog", "&Cancel")] = L"A&bbrechen";

	m_trans[std::make_pair("FXProgressDialog", "&Cancel")] = L"A&bbrechen";

	m_trans[std::make_pair("FXMessageBox", "&OK")] = L"&OK";
	m_trans[std::make_pair("FXMessageBox", "&Cancel")] = L"A&bbrechen";
	m_trans[std::make_pair("FXMessageBox", "&Yes")] = L"&Ja";
	m_trans[std::make_pair("FXMessageBox", "&No")] = L"&Nein";
	m_trans[std::make_pair("FXMessageBox", "&Save")] = L"&Speichern";
	m_trans[std::make_pair("FXMessageBox", "&Don't Save")] = L"&Nicht speichern";

	m_trans[std::make_pair("FXFileList", "Name")] = L"Name";
	m_trans[std::make_pair("FXFileList", "Type")] = L"Typ";
	m_trans[std::make_pair("FXFileList", "Size")] = L"Gr\u00f6\u00dfe";
	m_trans[std::make_pair("FXFileList", "Modified Date")] = L"\u00cfnderungsdatum";
	m_trans[std::make_pair("FXFileList", "User")] = L"Benutzer";
	m_trans[std::make_pair("FXFileList", "Group")] = L"Gruppe";
	m_trans[std::make_pair("FXFileList", "Attributes")] = L"Attribute";

	m_trans[std::make_pair("FXFileSelector", "&File Name:")] = L"&Dateiname";
	m_trans[std::make_pair("FXFileSelector", "&OK")] = L"&OK";
	m_trans[std::make_pair("FXFileSelector", "File F&ilter:")] = L"Dateif&ilter:";
	m_trans[std::make_pair("FXFileSelector", "Read Only")] = L"Schreibgesch\u00fctzt";
	m_trans[std::make_pair("FXFileSelector", "&Cancel")] = L"A&bbrechen";
	m_trans[std::make_pair("FXFileSelector", "Directory:")] = L"Verzeichnis:";
	
	m_trans[std::make_pair("FXFileSelector", "&Set bookmark\t\tBookmark current directory.")] = L"Lesezeichen &setzen\t\tEin Lesezeichen f\u00fcr das aktuelle Verzeichnis setzen.";
	m_trans[std::make_pair("FXFileSelector", "&Clear bookmarks\t\tClear bookmarks.")] = L"Lesezeichen &l\u00f6schen\t\tAlle Lesezeichen l\u00f6schen.";
	m_trans[std::make_pair("FXFileSelector", "\tGo up one directory\tMove up to higher directory.")] = L"\tEin Verzeichnis h\u00f6her\tIn das h\u00f6here Verzeichnis wechseln.";
	m_trans[std::make_pair("FXFileSelector", "\tGo to home directory\tBack to home directory.")] = L"\tIns Benutzerverzeichnis\tIns Benutzerverzeichnis wechseln.";
	m_trans[std::make_pair("FXFileSelector", "\tGo to work directory\tBack to working directory.")] = L"\tIns Arbeitsverzeichnis\tIns Arbeitsverzeichnis wechseln.";
	m_trans[std::make_pair("FXFileSelector", "\tBookmarks\tVisit bookmarked directories.")] = L"\tLesezeichen\tListe der Lesezeichen.";
	m_trans[std::make_pair("FXFileSelector", "\tCreate new directory\tCreate new directory.")] = L"\tVerzeichnis erstellen\tErstellt ein neues Verzeichnis.";
	m_trans[std::make_pair("FXFileSelector", "\tShow list\tDisplay directory with small icons.")] = L"\tListe Zeigen\tZeige Verzeichnisse mit kleinen Icons.";
	m_trans[std::make_pair("FXFileSelector", "\tShow icons\tDisplay directory with big icons.")] = L"\tIcons zeigen\tZeige Verzeichnisse mit gro\u00dfen Icons.";
	m_trans[std::make_pair("FXFileSelector", "\tShow details\tDisplay detailed directory listing.")] = L"\tDetails zeigen\tZeige Verzeichnisse mit detailierten Informationen.";
	m_trans[std::make_pair("FXFileSelector", "\tHide Hidden Files\tHide hidden files and directories.")] = L"\tVerstecke versteckte Dateien\tZeigt keine Dateien, die als versteckt markiert sind.";
	m_trans[std::make_pair("FXFileSelector", "\tShow hidden files\tShow hidden files and directories.")] = L"\tZeige versteckte Dateien\tZeigt Dateien, die als versteckt markiert sind.";

	m_trans[std::make_pair("FXFileSelector", "Create new directory with name: ")] = L"Erstelle neues Verzeichnis mit dem Namen: ";
	m_trans[std::make_pair("FXFileSelector", "Create New Directory")] = L"Neues Verzeichnis erstellen";
	m_trans[std::make_pair("FXFileSelector", "Up one level")] = L"Ein Level h\u00f6her";
	m_trans[std::make_pair("FXFileSelector", "Home directory")] = L"Benutzerverzeichnis";
	m_trans[std::make_pair("FXFileSelector", "Work directory")] = L"Arbeitsverzeichnis";
	m_trans[std::make_pair("FXFileSelector", "Select all")] = L"Alle ausw\u00e4hlen";
	m_trans[std::make_pair("FXFileSelector", "Sort by")] = L"Sortieren nach";
	m_trans[std::make_pair("FXFileSelector", "Name")] = L"Name";
	m_trans[std::make_pair("FXFileSelector", "Type")] = L"Typ";
	m_trans[std::make_pair("FXFileSelector", "Size")] = L"Gr\u00f6\u00dfe";
	m_trans[std::make_pair("FXFileSelector", "Time")] = L"Zeit";
	m_trans[std::make_pair("FXFileSelector", "User")] = L"Benutzer";
	m_trans[std::make_pair("FXFileSelector", "Group")] = L"Gruppe";
	m_trans[std::make_pair("FXFileSelector", "Reverse")] = L"R\u00fcckw\u00e4rts";
	m_trans[std::make_pair("FXFileSelector", "Ignore case")] = L"Gro\u00df-/Kleinschreibung ignorieren";
	m_trans[std::make_pair("FXFileSelector", "View")] = L"Ansicht";
	m_trans[std::make_pair("FXFileSelector", "Small icons")] = L"Kleine Symbole";
	m_trans[std::make_pair("FXFileSelector", "Big icons")] = L"Gro\u00dfe Symbole";
	m_trans[std::make_pair("FXFileSelector", "Details")] = L"Details";
	m_trans[std::make_pair("FXFileSelector", "Rows")] = L"Zeilen";
	m_trans[std::make_pair("FXFileSelector", "Columns")] = L"Spalten";
	m_trans[std::make_pair("FXFileSelector", "Hidden files")] = L"Versteckte Dateien";
	m_trans[std::make_pair("FXFileSelector", "Preview images")] = L"Bildvorschau";
	m_trans[std::make_pair("FXFileSelector", "Normal images")] = L"Normale Bilder";
	m_trans[std::make_pair("FXFileSelector", "Medium images")] = L"Mittlere Bilder";
	m_trans[std::make_pair("FXFileSelector", "Giant images")] = L"Gro\u00dfe Bilder";
	m_trans[std::make_pair("FXFileSelector", "Bookmarks")] = L"Lesezeichen";
	m_trans[std::make_pair("FXFileSelector", "Set bookmark")] = L"Lesezeichen setzen";
	m_trans[std::make_pair("FXFileSelector", "Clear bookmarks")] = L"Lesezeichen l\u00f6schen";
	m_trans[std::make_pair("FXFileSelector", "New directory...")] = L"Neues Verzeichnis...";
	m_trans[std::make_pair("FXFileSelector", "Copy...")] = L"Kopieren...";
	m_trans[std::make_pair("FXFileSelector", "Move...")] = L"Verschieben...";
	m_trans[std::make_pair("FXFileSelector", "Link...")] = L"Verlinken...";
	m_trans[std::make_pair("FXFileSelector", "Delete...")] = L"L\u00f6schen...";
	m_trans[std::make_pair("FXFileSelector", "Are you sure you want to delete the file:\n\n%s")] = L"Sind Sie sicher, dass Sie die Datei l\u00f6schen m\u00f6chten:\n\n%s";
	m_trans[std::make_pair("FXFileSelector", "Deleting files")] = L"Dateien l\u00f6schen";
	m_trans[std::make_pair("FXFileSelector", "Copy file from location:\n\n%s\n\nto location: ")] = L"Kopiere Datei von:\n\n%s\n\nnach: ";
	m_trans[std::make_pair("FXFileSelector", "Copy File")] = L"Datei kopieren";
	m_trans[std::make_pair("FXFileSelector", "Move file from location:\n\n%s\n\to location: ")] = L"Verschiebe Datei von:\n\n%s\n\nnach: ";
	m_trans[std::make_pair("FXFileSelector", "Move File")] = L"Datei verschieben";
	m_trans[std::make_pair("FXFileSelector", "Link file from location:\n\n%s\n\nto location: ")] = L"Verlinke Datei von:\n\n%s\n\nnach: ";
	m_trans[std::make_pair("FXFileSelector", "Link File")] = L"Datei verlinken";

	for (translation_t::iterator itor = m_trans.begin(); itor != m_trans.end(); itor++)
		itor->second = iso2utf(itor->second);
}

/*virtual*/ const FXchar* GermanTranslation::tr(const FXchar* context,const FXchar* message,const FXchar* hint /*=NULL*/) const
{
	translation_t::const_iterator elem = m_trans.find(std::make_pair(context, message));
	if (elem != m_trans.end())
		return elem->second.text();
		
#ifdef DEBUG
	showError((FXString)L"\u00dcbersetzung fehlt: (" + context + ") " + message);
#endif

	return message;
}
