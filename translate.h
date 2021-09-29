#ifdef HAVE_INTL
extern "C" {
#include <libintl.h>
}
#else
#define gettext(X) (X)
#define ngettext(S, P, N) (((N)==1)?S:P)
#endif

#define _(str) gettext(str)

#ifdef WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif
void init_intl(void);
