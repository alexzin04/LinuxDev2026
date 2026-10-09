CC = cc
CFLAGS = -Wall -Wextra -Werror
LDLIBS = -lncurses
prefix = /usr/local
bindir = $(prefix)/bin
INSTALL = install

all: Show

Show: Show.c
	$(CC) $(CFLAGS) Show.c -o Show $(LDLIBS)

install: Show
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 Show $(DESTDIR)$(bindir)/Show

clean:
	rm -f *~ *.o Show

.PHONY: all install clean
