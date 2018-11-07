#-!-Makefile-!-

all: client server

structures.o: src/structures.c lib/structures.h
	gcc -c src/structures.c -o structures.o

hash.o: src/hash.c lib/hash.h
		gcc -c src/hash.c -o hash.o

client: client.o structures.o
	gcc structures.o p2-dogClient.o -o client

client.o: src/p2-dogClient.c lib/structures.h
	gcc -c src/p2-dogClient.c -o p2-dogClient.o

server: server.o structures.o hash.o generator
	gcc structures.o hash.o p2-dogServer.o -o server -lpthread
	mkdir -p historias
	./generator #Esta linea es para generar los registros aleatorios

server.o: src/p2-dogServer.c lib/structures.h lib/hash.h
	gcc -c src/p2-dogServer.c -o p2-dogServer.o

generator: generator.c
	gcc generator.c -o generator

clean:
	rm -f *.o *.sh *.dat *.log client server generator
	rm -r historias
