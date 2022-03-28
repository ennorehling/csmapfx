#ifndef CSMAP_VERSION_H_
#define CSMAP_VERSION_H_

#define MACROTOSTR(STR) MACROTOSTRINGx(STR)
#define MACROTOSTRINGx(STR) #STR

	// global application title string
#define CSMAP_APP_TITLE      "Coast'n'Sea Mapper"

	// current application version
#define CSMAP_APP_VERSION    1, 2, 5

    // application version string
#define CSMAP_APP_VERSIONSTR "v1.2.5"

	// application title with full version string
#define CSMAP_APP_TITLE_VERSION CSMAP_APP_TITLE " " CSMAP_APP_VERSIONSTR

	// used as legal copyright (version.rc) and command line welcome text (main.cpp)
#define CSMAP_APP_COPYRIGHT  "Copyright (c) 2004-2010 Thomas J. Gritzan\nCopyright (c) 2021-2022 Enno Rehling"
	// used as company name (version.rc) and in the About Box (csmap.cpp)
#define CSMAP_APP_URL        "https://github.com/ennorehling/csmapfx"

#endif //CSMAP_VERSION_H_

