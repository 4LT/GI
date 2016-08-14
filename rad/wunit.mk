CC=cc
CFLAGS=-std=c99 -Wall -pedantic -ggdb -I..
TESTS=pmap.wunit
OBJECTS=pmap.wunit.o

ifeq ($(shell uname), Darwin)
	LDFLAGS=
else
	LDFLAGS=
endif

.PHONY: all clean

all: $(TESTS)

pmap.wunit.o: pmap.wunit.c patchmap.c patchmap.h
	$(CC) $(CFLAGS) -c pmap.wunit.c

pmap.wunit: pmap.wunit.o
	$(CC) $(CFLAGS) -o pmap.wunit pmap.wunit.o ../util/linked_list.o\
		../util/collections.o $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(TESTS)
