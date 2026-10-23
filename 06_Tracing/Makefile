CC ?= cc
CFLAGS ?= -O2 -g
CFLAGS += -Wall -Wextra -Werror -std=c11
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib
.PHONY: all test clean install
all: move libprotect.so safemove
move: move.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<
libprotect.so: protect.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -shared $(LDFLAGS) -o $@ $< -ldl
safemove: safemove.in Makefile
	sed -e 's|@LIBDIR@|$(LIBDIR)|g' -e 's|@BINDIR@|$(BINDIR)|g' $< > $@
	chmod +x $@
test: all
	python3 tests.py
install: all
	install -Dm755 move $(DESTDIR)$(BINDIR)/move
	install -Dm755 safemove $(DESTDIR)$(BINDIR)/safemove
	install -Dm755 libprotect.so $(DESTDIR)$(LIBDIR)/libprotect.so
clean:
	rm -f move libprotect.so safemove
	rm -rf __pycache__
