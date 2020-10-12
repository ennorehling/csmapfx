#ifndef CSMAP_VERSION_H_
#define CSMAP_VERSION_H_

#define MACROTOSTR(STR) MACROTOSTRINGx(STR)
#define MACROTOSTRINGx(STR) #STR

	// subversion revision data
#define CSMAP_SVN_REV        134
#define CSMAP_SVN_REVSTR     "134"
#define CSMAP_SVN_REV_DATE   "2020/10/08 10:54:00"

	// global application title string
#define CSMAP_APP_TITLE      "Coast'n'Sea Mapper"

	// current application version and subversion revision
#define CSMAP_APP_VERSION    0, 6, 1, CSMAP_SVN_REV
#define CSMAP_APP_REVISION   "rev " CSMAP_SVN_REVSTR

	// application version string
#define CSMAP_APP_VERSIONSTR "v0.6 " CSMAP_APP_REVISION

	// application title with full version string
#define CSMAP_APP_TITLE_VERSION CSMAP_APP_TITLE " " CSMAP_APP_VERSIONSTR

	// used as legal copyright (version.rc) and command line welcome text (main.cpp)
#define CSMAP_APP_COPYRIGHT  "Copyright (c) 2004-2020 Thomas J. Gritzan"
	// used as company name (version.rc) and in the About Box (csmap.cpp)
#define CSMAP_APP_URL        "http://eressea.draig.de/csmapfx"
#define CSMAP_APP_EMAIL      "Phygon@gmx.de"

#endif //CSMAP_VERSION_H_

