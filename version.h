#ifndef CSMAP_VERSION_H_
#define CSMAP_VERSION_H_

#define MACROTOSTR(STR) MACROTOSTRINGx(STR)
#define MACROTOSTRINGx(STR) #STR

	// subversion revision data
#define CSMAP_SVN_REV        133
#define CSMAP_SVN_REVSTR     "133"
#define CSMAP_SVN_REV_DATE   "2020/03/29 18:56:00"

/*
	- C:\Programme\TortoiseSVN\bin\SubWCRev.exe
	Highest committed revision number:  132
	Date of highest committed revision: 2010/05/30 22:26:48
	Update revision range:              132
	Repository URL of the working copy: svn+ssh://phygon@draig.de/svn/csmapfx/trunk

	True if local modifications found: true
	True if mixed update revisions found: false
*/

	// global application title string
#define CSMAP_APP_TITLE      "Coast'n'Sea Mapper"

	// current application version and subversion revision
#define CSMAP_APP_VERSION    0, 5, 1, CSMAP_SVN_REV
#define CSMAP_APP_REVISION   "rev " CSMAP_SVN_REVSTR

	// application version string
#define CSMAP_APP_VERSIONSTR "v0.5 " CSMAP_APP_REVISION

	// application title with full version string
#define CSMAP_APP_TITLE_VERSION CSMAP_APP_TITLE " " CSMAP_APP_VERSIONSTR

	// used as legal copyright (version.rc) and command line welcome text (main.cpp)
#define CSMAP_APP_COPYRIGHT  "Copyright (c) 2004-2010 Thomas J. Gritzan"
	// used as company name (version.rc) and in the About Box (csmap.cpp)
#define CSMAP_APP_URL        "http://eressea.draig.de/csmapfx"
#define CSMAP_APP_EMAIL      "Phygon@gmx.de"

#endif //CSMAP_VERSION_H_

