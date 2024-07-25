#ifndef LRTL_XML_H_GUARD
# define LRTL_XML_H_GUARD 1
/* BEGIN_C_DECLS should be used at the beginning of your declarations,
so that C++ compilers don't mangle their names.  Use END_C_DECLS at
the end of C declarations. */
# undef BEGIN_C_DECLS
# undef END_C_DECLS
# ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS }
# else
#  define BEGIN_C_DECLS /* empty */
#  define END_C_DECLS /* empty */
# endif

/* PARAMS is a macro used to wrap function prototypes, so that
     compilers that don't understand ANSI C prototypes still work,
       and ANSI C compilers can issue warnings about type mismatches. */
# undef PARAMS
# if defined (__STDC__) || defined (_AIX) \
	       || (defined (__mips) && defined (_SYSTYPE_SVR4)) \
       || defined(WIN32) || defined(__cplusplus)
#  define PARAMS(protos) protos
# else
#  define PARAMS(protos) ()
# endif

#include "mkinfo.h"

BEGIN_C_DECLS

char **getv_xpath PARAMS((xmlDocPtr doc, xmlChar *xpath));
xmlDocPtr xopen PARAMS((char *file));
void xclose PARAMS((xmlDocPtr doc));
int modxml PARAMS((struct changes ch));
char **get_xpath PARAMS((char *xml, xmlChar *xpath, int *cnt));

enum errors {
     EXML_PARSE = -1,
     EXML_XPATHCTX = -2,
     EXML_XPATHEV = -3
};

END_C_DECLS

#endif /* LRTL_XML_H_GUARD */
