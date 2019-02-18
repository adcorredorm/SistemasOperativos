#-!-Makefile-!-

all: client server

structures.o: src/structures.c lib/structures.h
	gcc -c src/structures.c -o structures.o

hash.o: src/hash.c lib/hash.h
		gcc -c src/hash.c -o hash.o

client: client.o structures.o
	gcc structures.o p3-dogClient.o -o p3-dogClient

client.o: src/p3-dogClient.c lib/structures.h
	gcc -c src/p3-dogClient.c -o p3-dogClient.o

server: server.o structures.o hash.o
	gcc structures.o hash.o p3-dogServer.o -o p3-dogServer -lpthread
	mkdir -p historias


server.o: src/p3-dogServer.c lib/structures.h lib/hash.h
	gcc -c src/p3-dogServer.c -o p3-dogServer.o

generator: generator.c
	gcc generator.c -o generator
	./generator #Esta linea es para generar los registros aleatorios

clean:
	rm -f *.o *.sh *.dat *.log p3-dogClient p3-dogServer generator
	rm -rf historias
