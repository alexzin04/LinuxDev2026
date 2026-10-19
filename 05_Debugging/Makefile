CC = cc
CFLAGS = -Wall -Wextra -std=c11
GDB = gdb

all: range

range: range.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -O0 -g $(LDFLAGS) -o $@ $< $(LDLIBS)

test: range
	$(GDB) --batch --quiet -nx -x multiples.gdb ./range > multiples.log 2>&1
	grep '^@@@' multiples.log > multiples.out
	diff -u multiples.expected multiples.out
	$(GDB) --batch --quiet -nx -x positions.gdb ./range > positions.log 2>&1
	grep '^@@@' positions.log > positions.out
	diff -u positions.expected positions.out

clean:
	rm -f range *.o multiples.log positions.log multiples.out positions.out
	rm -rf range.dSYM

.PHONY: all test clean
