AC_PREREQ([2.69])
AC_INIT([guess-number], [1.0], [alex@example.org])
AC_CONFIG_SRCDIR([src/guess-number.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AM_GNU_GETTEXT_VERSION([0.21])
AM_GNU_GETTEXT([external])
AC_PATH_PROG([PYTHON], [python3])
AS_IF([test -z "$PYTHON"], [AC_MSG_ERROR([python3 is required for tests])])
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_OUTPUT
