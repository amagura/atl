#ifndef LRTL_MKINFO_H_GUARD
# define LRTL_MKINFO_H_GUARD 1
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

BEGIN_C_DECLS

# ifndef STARTDIR
#  define STARTDIR "/bcs/lgnt/clientapp/agmtest/scripts/40k"
# endif
# define PNAME "mkinfo"

# ifndef bool
#  include <stdbool.h>
# endif

# ifndef BTL_TMPDIR
#  define BTL_TMPDIR STARTDIR "/tmp"
# endif

# ifndef BTL_BUILD
#  define BTL_BUILD BTL_TMPDIR "/BTL_BUILD"
# endif

# if !defined(BTL_TMPLATE) && !defined(LR_DUMMY)
#  define BTL_TMPLATE BTL_BUILD ".XXXXXX"
# elif defined(LR_DUMMY)
#  define BTL_TMPLATE "./out.XXXXXX"
# else
#  define BTL_TMPLATE BTL_BUILD "/out.XXXXXX"
# endif

# ifndef BTL_TMPSIZE
#  define BTL_TMPSIZE (sizeof BTL_TMPLATE)
# endif

struct btl_build_data {
     unsigned char *xpath;
     char *v[2];
     bool used[2];
};

struct changes {
     char *value;
     unsigned char *xpath;
     int chid;
     char *xml;
     bool dry; /* dry run */
     bool deploy;
};

END_C_DECLS

#endif /* LRTL_MKINFO_H_GUARD */
/* vim: ts=5:sts=5:sw=5:set expandtab: */
