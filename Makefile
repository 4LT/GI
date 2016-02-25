CC=cc
CFLAGS=-std=c99 -Wall -pedantic -ggdb
LDFLAGS=-lSDL2 -lm

all: raytrace

main.o: main.c types.h scene.h shapes.h
	$(CC) $(CFLAGS) -c main.c

renderer.o: renderer.c renderer.h
	$(CC) $(CFLAGS) -c renderer.c

vecmatops.o: vecmatops.c vecmatops.h types.h
	$(CC) $(CFLAGS) -c vecmatops.c

util/linkedlist.o: util/linkedlist.c util/linkedlist.h
	$(CC) $(CFLAGS) util/linkedlist.c

raytrace: main.o renderer.o vecmatops.o util/linkedlist.o
	$(CC) $(CFLAGS) -o raytrace main.o renderer.o vecmatops.o util/linkedlist.o $(LDFLAGS)

