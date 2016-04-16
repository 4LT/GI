CC=cc
CFLAGS=-std=c99 -Wall -pedantic -ggdb
ifeq ($(shell uname), Darwin)
	LDFLAGS= -framework SDL2
else
	LDFLAGS= -lSDL2 -lm
endif

all: rayt rad

main.o: main.c types.h scene.h shapes.h material.h materials.h light.h color.h
	$(CC) $(CFLAGS) -c main.c

rad.o: rad.c types.h scene.h shapes.h material.h materials.h light.h color.h
	$(CC) $(CFLAGS) -c rad.c

scene.o: scene.c scene.h
	$(CC) $(CFLAGS) -c scene.c

shapes.o: shapes.c shapes.h
	$(CC) $(CFLAGS) -c shapes.c

canvas.o: canvas.c canvas.h
	$(CC) $(CFLAGS) -c canvas.c

material.o: material.c material.h
	$(CC) $(CFLAGS) -c material.c

materials.o: materials.c materials.h
	$(CC) $(CFLAGS) -c materials.c

vecmatops.o: vecmatops.c vecmatops.h
	$(CC) $(CFLAGS) -c vecmatops.c

color.o: color.c color.h
	$(CC) $(CFLAGS) -c color.c

util/linkedlist.o: util/linkedlist.c util/linkedlist.h
	$(CC) $(CFLAGS) -o util/linkedlist.o -c util/linkedlist.c

rayt: main.o scene.o shapes.o canvas.o material.o materials.o vecmatops.o color.o util/linkedlist.o Makefile
	$(CC) $(CFLAGS) -o rayt main.o scene.o shapes.o canvas.o material.o materials.o vecmatops.o color.o util/linkedlist.o $(LDFLAGS)

rad: rad.o scene.o shapes.o canvas.o material.o materials.o vecmatops.o color.o util/linkedlist.o Makefile
	$(CC) $(CFLAGS) -o rad rad.o scene.o shapes.o canvas.o material.o materials.o vecmatops.o color.o util/linkedlist.o $(LDFLAGS)
