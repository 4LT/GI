CC=cc
CFLAGS=-std=c99 -Wall -pedantic -ggdb
ifeq ($(shell uname), Darwin)
	LDFLAGS= -framework SDL2
else
	LDFLAGS= -lSDL2 -lm
endif

COMMON_OBJECTS= scene.o shapes.o canvas.o material.o materials.o vecmatops.o\
				color.o tone_mapping.o util/linkedlist.o

all: rayt rad ply2tri

main.o: main.c types.h scene.h shapes.h material.h materials.h light.h color.h
	$(CC) $(CFLAGS) -c main.c

rad.o: rad.c types.h scene.h shapes.h material.h materials.h light.h color.h
	$(CC) $(CFLAGS) -c rad.c

ply2tri.o: ply2tri.c types.h scene.h shapes.h material.h materials.h light.h color.h rply-1.1.4/rply.h
	$(CC) $(CFLAGS) -c ply2tri.c

rply-1.1.4/rply.o: rply-1.1.4/rply.c rply-1.1.4/rply.h rply-1.1.4/rplyfile.h
	$(CC) $(CFLAGS) -o rply-1.1.4/rply.o -c rply-1.1.4/rply.c

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

tone_mapping.o: tone_mapping.c tone_mapping.h
	$(CC) $(CFLAGS) -c tone_mapping.c

util/linkedlist.o: util/linkedlist.c util/linkedlist.h
	$(CC) $(CFLAGS) -o util/linkedlist.o -c util/linkedlist.c

rayt: main.o $(COMMON_OBJECTS) Makefile
	$(CC) $(CFLAGS) -o rayt main.o $(COMMON_OBJECTS) $(LDFLAGS)

rad: rad.o $(COMMON_OBJECTS) Makefile
	$(CC) $(CFLAGS) -o rad rad.o $(COMMON_OBJECTS) $(LDFLAGS)

ply2tri: ply2tri.o rply-1.1.4/rply.o $(COMMON_OBJECTS)  Makefile
	$(CC) $(CFLAGS) -o ply2tri ply2tri.o rply-1.1.4/rply.o $(COMMON_OBJECTS)\
		$(LDFLAGS)
