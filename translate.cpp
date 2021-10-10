#define _CRT_SECURE_NO_WARNINGS

#include "translate.h"
#include "fxhelper.h"
#ifdef HAVE_INTL
#include "whereami.c"
#endif

#include <fx.h>
#include <FXStat.h>
#include <FXString.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

void init_intl(void) {
#ifdef HAVE_INTL
    const char *reldir = "locale";

    if (FXStat::exists(reldir)) {
        bindtextdomain("csmapfx", reldir);
    }
    else {
        size_t length;
        int dirname_length;
        char *path = NULL;

        length = (size_t)wai_getExecutablePath(NULL, 0, &dirname_length);
        if (length > 0) {
            /* TODO: this is a worst-case allocation */
            path = (char *) malloc(length + strlen(reldir) + 1);
            if (path) {
                char *pos;
                wai_getExecutablePath(path, (int)length, &dirname_length);
                path[length] = 0;
                pos = strrchr(path, PATH_SEP);
                if (pos) {
                    strcpy(pos + 1, reldir);
                    if (!FXStat::isDirectory(path)) {
                        free(path);
                        path = NULL;
                    }
                } else {
                    free(path);
                    path = NULL;
                }
            }
        }
        if (path) {
            bindtextdomain("csmapfx", path);
            free(path);
        }
        else {
            bindtextdomain("csmapfx", "/usr/share/locale");
        }
    }
    textdomain("csmapfx");
#endif
}

