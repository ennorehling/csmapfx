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
	m_trans[std::make_pair("FXStatusLine", "Ready.")] = "Bereit.";

	m_trans[std::make_pair("FXInputDialog", "&OK")] = "&OK";
	m_trans[std::make_pair("FXInputDialog", "&Cancel")] = "A&bbrechen";

	m_trans[std::make_pair("FXProgressDialog", "&Cancel")] = "A&bbrechen";

	m_trans[std::make_pair("FXMessageBox", "&OK")] = "&OK";
	m_trans[std::make_pair("FXMessageBox", "&Cancel")] = "A&bbrechen";
	m_trans[std::make_pair("FXMessageBox", "&Yes")] = "&Ja";
	m_trans[std::make_pair("FXMessageBox", "&No")] = "&Nein";
	m_trans[std::make_pair("FXMessageBox", "&Save")] = "&Speichern";
	m_trans[std::make_pair("FXMessageBox", "&Don't Save")] = "&Nicht speichern";

	m_trans[std::make_pair("FXFileList", "Name")] = "Name";
	m_trans[std::make_pair("FXFileList", "Type")] = "Typ";
	m_trans[std::make_pair("FXFileList", "Size")] = "Größe";
	m_trans[std::make_pair("FXFileList", "Modified Date")] = "Änderungsdatum";
	m_trans[std::make_pair("FXFileList", "User")] = "Benutzer";
	m_trans[std::make_pair("FXFileList", "Group")] = "Gruppe";
	m_trans[std::make_pair("FXFileList", "Attributes")] = "Attribute";

	m_trans[std::make_pair("FXFileSelector", "&File Name:")] = "&Dateiname";
	m_trans[std::make_pair("FXFileSelector", "&OK")] = "&OK";
	m_trans[std::make_pair("FXFileSelector", "File F&ilter:")] = "Dateif&ilter:";
	m_trans[std::make_pair("FXFileSelector", "Read Only")] = "Schreibgeschützt";
	m_trans[std::make_pair("FXFileSelector", "&Cancel")] = "A&bbrechen";
	m_trans[std::make_pair("FXFileSelector", "Directory:")] = "Verzeichnis:";
	
	m_trans[std::make_pair("FXFileSelector", "&Set bookmark\t\tBookmark current directory.")] = "Lesezeichen &setzen\t\tEin Lesezeichen für das aktuelle Verzeichnis setzen.";
	m_trans[std::make_pair("FXFileSelector", "&Clear bookmarks\t\tClear bookmarks.")] = "Lesezeichen &löschen\t\tAlle Lesezeichen löschen.";
	m_trans[std::make_pair("FXFileSelector", "\tGo up one directory\tMove up to higher directory.")] = "\tEin Verzeichnis höher\tIn das höhere Verzeichnis wechseln.";
	m_trans[std::make_pair("FXFileSelector", "\tGo to home directory\tBack to home directory.")] = "\tIns Benutzerverzeichnis\tIns Benutzerverzeichnis wechseln.";
	m_trans[std::make_pair("FXFileSelector", "\tGo to work directory\tBack to working directory.")] = "\tIns Arbeitsverzeichnis\tIns Arbeitsverzeichnis wechseln.";
	m_trans[std::make_pair("FXFileSelector", "\tBookmarks\tVisit bookmarked directories.")] = "\tLesezeichen\tListe der Lesezeichen.";
	m_trans[std::make_pair("FXFileSelector", "\tCreate new directory\tCreate new directory.")] = "\tVerzeichnis erstellen\tErstellt ein neues Verzeichnis.";
	m_trans[std::make_pair("FXFileSelector", "\tShow list\tDisplay directory with small icons.")] = "\tListe Zeigen\tZeige Verzeichnisse mit kleinen Icons.";
	m_trans[std::make_pair("FXFileSelector", "\tShow icons\tDisplay directory with big icons.")] = "\tIcons zeigen\tZeige Verzeichnisse mit großen Icons.";
	m_trans[std::make_pair("FXFileSelector", "\tShow details\tDisplay detailed directory listing.")] = "\tDetails zeigen\tZeige Verzeichnisse mit detailierten Informationen.";
	m_trans[std::make_pair("FXFileSelector", "\tHide Hidden Files\tHide hidden files and directories.")] = "\tVerstecke versteckte Dateien\tZeigt keine Dateien, die als versteckt markiert sind.";
	m_trans[std::make_pair("FXFileSelector", "\tShow hidden files\tShow hidden files and directories.")] = "\tZeige versteckte Dateien\tZeigt Dateien, die als versteckt markiert sind.";

	m_trans[std::make_pair("FXFileSelector", "Create new directory with name: ")] = "Erstelle neues Verzeichnis mit dem Namen: ";
	m_trans[std::make_pair("FXFileSelector", "Create New Directory")] = "Neues Verzeichnis erstellen";
	m_trans[std::make_pair("FXFileSelector", "Up one level")] = "Ein Level höher";
	m_trans[std::make_pair("FXFileSelector", "Home directory")] = "Benutzerverzeichnis";
	m_trans[std::make_pair("FXFileSelector", "Work directory")] = "Arbeitsverzeichnis";
	m_trans[std::make_pair("FXFileSelector", "Select all")] = "Alle auswählen";
	m_trans[std::make_pair("FXFileSelector", "Sort by")] = "Sortieren nach";
	m_trans[std::make_pair("FXFileSelector", "Name")] = "Name";
	m_trans[std::make_pair("FXFileSelector", "Type")] = "Typ";
	m_trans[std::make_pair("FXFileSelector", "Size")] = "Größe";
	m_trans[std::make_pair("FXFileSelector", "Time")] = "Zeit";
	m_trans[std::make_pair("FXFileSelector", "User")] = "Benutzer";
	m_trans[std::make_pair("FXFileSelector", "Group")] = "Gruppe";
	m_trans[std::make_pair("FXFileSelector", "Reverse")] = "Rückwärts";
	m_trans[std::make_pair("FXFileSelector", "Ignore case")] = "Groß-/Kleinschreibung ignorieren";
	m_trans[std::make_pair("FXFileSelector", "View")] = "Ansicht";
	m_trans[std::make_pair("FXFileSelector", "Small icons")] = "Kleine Symbole";
	m_trans[std::make_pair("FXFileSelector", "Big icons")] = "Große Symbole";
	m_trans[std::make_pair("FXFileSelector", "Details")] = "Details";
	m_trans[std::make_pair("FXFileSelector", "Rows")] = "Zeilen";
	m_trans[std::make_pair("FXFileSelector", "Columns")] = "Spalten";
	m_trans[std::make_pair("FXFileSelector", "Hidden files")] = "Versteckte Dateien";
	m_trans[std::make_pair("FXFileSelector", "Preview images")] = "Bildvorschau";
	m_trans[std::make_pair("FXFileSelector", "Normal images")] = "Normale Bilder";
	m_trans[std::make_pair("FXFileSelector", "Medium images")] = "Mittlere Bilder";
	m_trans[std::make_pair("FXFileSelector", "Giant images")] = "Große Bilder";
	m_trans[std::make_pair("FXFileSelector", "Bookmarks")] = "Lesezeichen";
	m_trans[std::make_pair("FXFileSelector", "Set bookmark")] = "Lesezeichen setzen";
	m_trans[std::make_pair("FXFileSelector", "Clear bookmarks")] = "Lesezeichen löschen";
	m_trans[std::make_pair("FXFileSelector", "New directory...")] = "Neues Verzeichnis...";
	m_trans[std::make_pair("FXFileSelector", "Copy...")] = "Kopieren...";
	m_trans[std::make_pair("FXFileSelector", "Move...")] = "Verschieben...";
	m_trans[std::make_pair("FXFileSelector", "Link...")] = "Verlinken...";
	m_trans[std::make_pair("FXFileSelector", "Delete...")] = "Löschen...";
	m_trans[std::make_pair("FXFileSelector", "Are you sure you want to delete the file:\n\n%s")] = "Sind Sie sicher, dass Sie die Datei löschen möchten:\n\n%s";
	m_trans[std::make_pair("FXFileSelector", "Deleting files")] = "Dateien löschen";
	m_trans[std::make_pair("FXFileSelector", "Copy file from location:\n\n%s\n\nto location: ")] = "Kopiere Datei von:\n\n%s\n\nnach: ";
	m_trans[std::make_pair("FXFileSelector", "Copy File")] = "Datei kopieren";
	m_trans[std::make_pair("FXFileSelector", "Move file from location:\n\n%s\n\to location: ")] = "Verschiebe Datei von:\n\n%s\n\nnach: ";
	m_trans[std::make_pair("FXFileSelector", "Move File")] = "Datei verschieben";
	m_trans[std::make_pair("FXFileSelector", "Link file from location:\n\n%s\n\nto location: ")] = "Verlinke Datei von:\n\n%s\n\nnach: ";
	m_trans[std::make_pair("FXFileSelector", "Link File")] = "Datei verlinken";

	for (translation_t::iterator itor = m_trans.begin(); itor != m_trans.end(); itor++)
		itor->second = iso2utf(itor->second);
}

/*virtual*/ const FXchar* GermanTranslation::tr(const FXchar* context,const FXchar* message,const FXchar* hint /*=NULL*/) const
{
	translation_t::const_iterator elem = m_trans.find(std::make_pair(context, message));
	if (elem != m_trans.end())
		return elem->second.text();
		
#ifdef DEBUG
	showError((FXString)L"Übersetzung fehlt: (" + context + ") " + message);
#endif

	return message;
}
