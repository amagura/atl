#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([User Tools], [0.0.1], [sickhadas@gmail.com])
#AC_CONFIG_SRCDIR([here.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-m4])
AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_MKDIR_P
AC_CHECK_PROGS([CYTHON], [cython], AC_MSG_ERROR([Cython is a make dependency for User Tools]))
#AC_CHECK_LIB([xml2], [xmlXPathEvalExpression])

PKG_PROG_PKG_CONFIG
#[export PKG_CONFIG_PATH="/apps/python_3.5/lib/pkgconfig:$PKG_CONFIG_PATH"]
PKG_CHECK_MODULES([XML2], [libxml-2.0],, AC_MSG_ERROR([libxml2 is a make dependency for User Tools]))
PKG_CHECK_MODULES([PYTHON], [python3], [python3],,)

# Checks for libraries.
AC_CHECK_LIB([commoner], [trim])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdint.h stdlib.h string.h unistd.h commoner.h bsd/string.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero mempcpy memset realpath regcomp strcspn strdup strerror strncasecmp])
AC_SEARCH_LIBS([strlcpy],
               [c bsd],
               [AC_DEFINE([HAVE_STRLCPY], [1], [safer version of strcpy])])

AC_CONFIG_FILES([Makefile
                 aux/src/Makefile])
###############################
# Options
###############################
AC_ARG_ENABLE([inspect],
              AS_HELP_STRING([  --enable-inspect disable compiler optimizations to preserve values])
              [case "${enableval}" in
               yes) inspect=true ;;
               no) inspect=false ;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-inspect]) ;;
              esac],[inspect=false])

AC_ARG_ENABLE([static],
              AS_HELP_STRING([  --enable-static turn on static compilation (default: disabled)])
              [case "${enableval}" in
               yes) static=true ;;
               no) static=false ;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-static]) ;;
              esac],[static=false])

AC_ARG_ENABLE([debug],
              [  --enable-debug		turn on debugging (default: disabled)],
              [case "${enableval}" in
               yes) debug=true ;;
               no) debug=false ;;
               *) AC_MSG_WARN([bad value '${enableval}' for --enable-debug]) ;;
             esac],[debug=no])

test "x$debug" = "xno" && no_debug=false
test "x$debug" = "xfalse" && no_debug=true

# Automake directives
dnl `foreign' prevents autotools from installing a copy of the GPL
dnl `-Wall' gets rid of unnecessary warnings
AM_INIT_AUTOMAKE([foreign -Wall])

AM_CONDITIONAL([INSPECT], [test "x$inspect" = "xtrue"])
AM_CONDITIONAL([NATIVE], [test "x$native" = "xtrue"])
AM_CONDITIONAL([GENERIC], [test "x$generic" = "xtrue"])
AM_CONDITIONAL([STATIC], [test "x$static" = "xtrue"])
AM_CONDITIONAL([DEBUG], [test "x$debug" = "xtrue"])
AM_CONDITIONAL([NO_DEBUG], [test "x$debug" != "xtrue"])
AM_CONDITIONAL([TEST], [test "x$testdir" = "xtrue"])
#AC_CONFIG_SUBDIRS([src])
AC_OUTPUT
