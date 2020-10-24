#ifndef CSMAP_VERSION_H_
#define CSMAP_VERSION_H_

#define MACROTOSTR(STR) MACROTOSTRINGx(STR)
#define MACROTOSTRINGx(STR) #STR

	// subversion revision data
#define CSMAP_SVN_REV        136
#define CSMAP_SVN_REVSTR     "136"

	// global application title string
#define CSMAP_APP_TITLE      "Coast'n'Sea Mapper"

	// current application version and subversion revision
#define CSMAP_APP_VERSION    0, 6, 3, CSMAP_SVN_REV
#define CSMAP_APP_REVISION   "rev " CSMAP_SVN_REVSTR

	// application version string
#define CSMAP_APP_VERSIONSTR "v0.6 " CSMAP_APP_REVISION

	// application title with full version string
#define CSMAP_APP_TITLE_VERSION CSMAP_APP_TITLE " " CSMAP_APP_VERSIONSTR

	// used as legal copyright (version.rc) and command line welcome text (main.cpp)
#define CSMAP_APP_COPYRIGHT  "Copyright (c) 2004-2010 Thomas J. Gritzan\nCopyright (c) 2020 Enno Rehling"
	// used as company name (version.rc) and in the About Box (csmap.cpp)
#define CSMAP_APP_URL        "https://github.com/ennorehling/csmapfx"

#endif //CSMAP_VERSION_H_

