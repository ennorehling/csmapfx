#ifdef HAVE_GETTEXT
extern "C" {
#include <libintl.h>
#include <locale.h>
}
#else
#define gettext(X) (X)
#define ngettext(S, P, N) (((N)==1)?S:P)
#endif

#define _(str) gettext(str)
#define t(str) (str)

