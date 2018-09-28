# -*- Makefile -*-

p1-dogProgram: p1-dogProgram.o generators.o structures.o
	gcc p1-dogProgram.o generators.o structures.o -o p1-dogProgram.sh

p1-dogProgram.o: p1-dogProgram.c lib/generators.h lib/structures.h
	gcc -c p1-dogProgram.c -o p1-dogProgram.o

generators.o: generators.c lib/generators.h
	gcc -c generators.c -o generators.o

structures.o: structures.c lib/structures.h
	gcc -c structures.c -o structures.o

clean:
	rm -f *.o mascota
