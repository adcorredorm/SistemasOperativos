#-!-Makefile-!-

all: p1-dogProgram generator

p1-dogProgram: structures.o p1-dogProgram.o
	gcc structures.o p1-dogProgram.o -o p1-dogProgram.sh
	mkdir -p historias

structures.o: src/structures.c lib/structures.h
	gcc -c src/structures.c -o structures.o

p1-dogProgram.o: src/p1-dogProgram.c lib/structures.h
	gcc -c src/p1-dogProgram.c -o p1-dogProgram.o

generator: generator.c
	gcc generator.c -o generator.sh

clean:
	rm -f *.o *.sh *.dat historias/*
