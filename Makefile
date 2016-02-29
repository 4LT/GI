CC=cc
CFLAGS=-std=c99 -Wall -pedantic -ggdb
LDFLAGS= -framework SDL2

all: raytrace Makefile

main.o: main.c types.h scene.h shapes.h
	$(CC) $(CFLAGS) -c main.c

canvas.o: canvas.c canvas.h
	$(CC) $(CFLAGS) -c canvas.c

vecmatops.o: vecmatops.c vecmatops.h types.h
	$(CC) $(CFLAGS) -c vecmatops.c

util/linkedlist.o: util/linkedlist.c util/linkedlist.h
	$(CC) $(CFLAGS) -o util/linkedlist.o -c util/linkedlist.c

raytrace: main.o canvas.o vecmatops.o util/linkedlist.o
	$(CC) $(CFLAGS) -o raytrace.out main.o canvas.o vecmatops.o util/linkedlist.o $(LDFLAGS)

