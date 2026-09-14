CC = cc
CFLAGS = -Wall -Wextra -Werror
LDLIBS = -lncurses

all: Show

Show: Show.c
	$(CC) $(CFLAGS) Show.c -o Show $(LDLIBS)

clean:
	rm -f *~ *.o Show

.PHONY: all clean
