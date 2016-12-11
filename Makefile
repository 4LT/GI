CC=cc
BASEFLAGS=-std=c99 -Wall -Werror=implicit-function-declaration \
		  -Wno-unused-function -pedantic $(MORE_ARGS)
RADFLAGS=-I.

ifeq ($(shell uname), Darwin)
	LDFLAGS= -framework SDL2
else
	LDFLAGS= -lSDL2 -lm
endif

ifdef OPMZ
	CFLAGS=$(BASEFLAGS) -O2 -march=native
else
	CFLAGS=$(BASEFLAGS) -ggdb
endif

COMMON_OBJECTS= scene.o shapes.o canvas.o material.o materials.o\
				color.o tone_mapping.o util/linked_list.o util/collections.o\
				util/reszarr.o kd.o aabb.o
RAD_OBJECTS= rad/hemicube.o rad/patch.o rad/patchmap.o rad/read_patches.o

PROGRAMS= rad/rad ply2tri rad/draw_patches rayt rad/box2p
#PROGRAMS= ply2tri rayt 

.PHONY: all clean

all: $(PROGRAMS)

main.o: main.c types.h scene.h shapes.h material.h materials.h light.h color.h
	$(CC) $(CFLAGS) -c main.c

kd.o: kd.c kd.h
	$(CC) $(CFLAGS) -c kd.c

rad/rad.o: rad/rad.c types.h scene.h shapes.h material.h materials.h light.h\
	color.h
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/rad.o -c rad/rad.c

rad/draw_patches.o: rad/draw_patches.c rad/read_patches.h tone_mapping.h\
	canvas.h
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/draw_patches.o -c rad/draw_patches.c

ply2tri.o: ply2tri.c types.h scene.h shapes.h material.h materials.h light.h\
	color.h rply-1.1.4/rply.h
	$(CC) $(CFLAGS) -c ply2tri.c

rply-1.1.4/rply.o: rply-1.1.4/rply.c rply-1.1.4/rply.h rply-1.1.4/rplyfile.h
	$(CC) $(CFLAGS) -Wno-uninitialized -o rply-1.1.4/rply.o -c rply-1.1.4/rply.c

scene.o: scene.c scene.h
	$(CC) $(CFLAGS) -c scene.c

shapes.o: shapes.c shapes.h util/ops.h
	$(CC) $(CFLAGS) -c shapes.c

aabb.o: aabb.c aabb.h
	$(CC) $(CFLAGS) -c aabb.c

canvas.o: canvas.c canvas.h
	$(CC) $(CFLAGS) -c canvas.c

material.o: material.c material.h
	$(CC) $(CFLAGS) -c material.c

materials.o: materials.c materials.h
	$(CC) $(CFLAGS) -c materials.c

color.o: color.c color.h
	$(CC) $(CFLAGS) -c color.c

tone_mapping.o: tone_mapping.c tone_mapping.h
	$(CC) $(CFLAGS) -c tone_mapping.c

rad/read_patches.o: rad/read_patches.c rad/read_patches.h
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/read_patches.o -c rad/read_patches.c

rad/hemicube.o: rad/hemicube.c rad/hemicube.h
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/hemicube.o -c rad/hemicube.c

rad/patch.o: rad/patch.c rad/patch.h
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/patch.o -c rad/patch.c

rad/patchmap.o: rad/patchmap.c rad/patchmap.h
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/patchmap.o -c rad/patchmap.c

util/linked_list.o: util/linked_list.c util/linked_list.h
	$(CC) $(CFLAGS) -o util/linked_list.o -c util/linked_list.c

util/collections.o: util/collections.c util/collections.h
	$(CC) $(CFLAGS) -o util/collections.o -c util/collections.c

util/reszarr.o: util/reszarr.c util/reszarr.h
	$(CC) $(CFLAGS) -o util/reszarr.o -c util/reszarr.c

rad/box2p.o: rad/box2p.c
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/box2p.o -c rad/box2p.c

rayt: main.o $(COMMON_OBJECTS) Makefile
	$(CC) $(CFLAGS) -o rayt main.o $(COMMON_OBJECTS) $(LDFLAGS)

rad/rad: rad/rad.o $(RAD_OBJECTS) $(COMMON_OBJECTS) Makefile
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/rad rad/rad.o $(RAD_OBJECTS)\
		$(COMMON_OBJECTS) $(LDFLAGS)

ply2tri: ply2tri.o rply-1.1.4/rply.o $(COMMON_OBJECTS)  Makefile
	$(CC) $(CFLAGS) -o ply2tri ply2tri.o rply-1.1.4/rply.o $(COMMON_OBJECTS)\
		$(LDFLAGS)

rad/draw_patches: rad/draw_patches.o $(RAD_OBJECTS) $(COMMON_OBJECTS) Makefile
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/draw_patches rad/draw_patches.o \
		$(RAD_OBJECTS) $(COMMON_OBJECTS) $(LDFLAGS)

rad/box2p: rad/box2p.o $(COMMON_OBJECS) Makefile
	$(CC) $(CFLAGS) $(RADFLAGS) -o rad/box2p rad/box2p.o $(COMMON_OBJECTS)\
		$(LDFLAGS)

clean:
	rm -f *.o 
	rm -f util/*.o
	rm -f rply-1.1.4/*.o
	rm -f rad/*.o
	rm -f $(PROGRAMS)
