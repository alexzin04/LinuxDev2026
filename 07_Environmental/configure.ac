AC_INIT([rhasher], [1.0], [alex@example.org])
AC_CONFIG_SRCDIR([rhasher.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AC_CHECK_HEADERS([rhash.h], [], [AC_MSG_ERROR([librhash development headers are required])])
AC_CHECK_LIB([rhash], [rhash_library_init], [RHASH_LIBS=-lrhash],
             [AC_MSG_ERROR([librhash is required])])
AC_SUBST([RHASH_LIBS])
AC_CHECK_FUNCS([getline], [], [AC_MSG_ERROR([getline is required])])
AC_ARG_WITH([readline],
  [AS_HELP_STRING([--without-readline], [use getline only (default: auto-detect readline)])],
  [], [with_readline=auto])
AS_CASE([$with_readline], [yes|no|auto], [],
        [AC_MSG_ERROR([--with-readline accepts yes, no or auto])])
have_readline=no
AS_IF([test "x$with_readline" != xno], [
  AC_CHECK_HEADERS([readline/readline.h], [
    AC_CHECK_LIB([readline], [readline], [
      have_readline=yes
      READLINE_LIBS=-lreadline
      AC_DEFINE([USE_READLINE], [1], [Use readline for interactive input])
    ])
  ])
  AS_IF([test "x$with_readline" = xyes && test "x$have_readline" = xno],
        [AC_MSG_ERROR([readline requested but development library was not found])])
])
AC_SUBST([READLINE_LIBS])
AC_MSG_NOTICE([interactive readline support: $have_readline])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
