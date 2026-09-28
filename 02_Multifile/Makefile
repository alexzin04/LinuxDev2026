CC = cc
CFLAGS = -Wall -fPIC
LDFLAGS_SO = -Wl,-rpath,.
GENERATES = prog prog-a prog-so liboutput_static.a liboutput.so README test-*.out
TRASH = *.o *~

all: prog prog-a prog-so liboutput_static.a liboutput.so

liboutput_static.a: fun.o const.o
	ar rcs $@ $^

liboutput.so: fun.o const.o
	$(CC) -shared -o $@ $^

prog: prog.o fun.o const.o
	$(CC) -o $@ $^

prog-a: prog.o liboutput_static.a
	$(CC) -o $@ prog.o liboutput_static.a

prog-so: prog.o liboutput.so
	$(CC) -o $@ prog.o -L. -loutput $(LDFLAGS_SO)

README: prog
	./$< 2> $@

fun.o prog.o: outlib.h

test: all
	./prog > test-prog-noargs.out 2>&1
	./prog-a > test-prog-a-noargs.out 2>&1
	./prog-so > test-prog-so-noargs.out 2>&1
	cmp test-prog-noargs.out test-prog-a-noargs.out
	cmp test-prog-noargs.out test-prog-so-noargs.out
	./prog one > test-prog-one.out 2>&1
	./prog-a one > test-prog-a-one.out 2>&1
	./prog-so one > test-prog-so-one.out 2>&1
	cmp test-prog-one.out test-prog-a-one.out
	cmp test-prog-one.out test-prog-so-one.out
	./prog one two three > test-prog-three.out 2>&1
	./prog-a one two three > test-prog-a-three.out 2>&1
	./prog-so one two three > test-prog-so-three.out 2>&1
	cmp test-prog-three.out test-prog-a-three.out
	cmp test-prog-three.out test-prog-so-three.out

clean:
	rm -f $(TRASH) $(GENERATES)

distclean: clean

.PHONY: all test clean distclean
