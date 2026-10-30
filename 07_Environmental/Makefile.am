bin_PROGRAMS = rhasher
rhasher_SOURCES = rhasher.c
AM_CFLAGS = -std=c99 -Wall -Wextra -Wpedantic
rhasher_LDADD = $(RHASH_LIBS) $(READLINE_LIBS)
TESTS = tests/check.sh
EXTRA_DIST = LICENSE rhasher.spec tests/check.sh

.PHONY: maintainer-clean-local
maintainer-clean-local:
	rm -rf autom4te.cache
	rm -f configure configure~ config.h.in config.h.in~ Makefile.in aclocal.m4
	rm -f compile depcomp install-sh missing test-driver
