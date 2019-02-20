#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "../lib/structures.h"
#include "../lib/hash.h"
#include "../lib/blocker.h"
#include "../lib/server.h"

void escribir_log(int socket_cliente, int tipo_operacion, char *solicitud){
        char registro[10], timef[100];
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        strftime(timef, 100, "%Y%m%d%H%M%S", tm);

        switch (tipo_operacion) {
          case 1: strcpy(registro, "insercion\0"); break;
          case 2: strcpy(registro, "lectura\0"  ); break;
          case 3: strcpy(registro, "borrado\0"  ); break;
          case 4: strcpy(registro, "busqueda\0" ); break;
        }

        struct sockaddr_in client;
        int csize;
        getpeername(socket_cliente, (struct sockaddr *)&client, &csize);

        lock(LOG_SOURCE);
        FILE *file = fopen(LOG_PATH, "a+");
        fprintf(file, "%s %s %s %s\n", timef, inet_ntoa(client.sin_addr), registro, solicitud);
        fclose(file);
        unlock(LOG_SOURCE);
}

void enviar(int socket_cliente, void *pointer, size_t size){
        int ok = send(socket_cliente, pointer, size, 0);
        if(ok < 1) {
                perror("send error");
                exit(-1);
        }
}

void recibir(int socket_cliente, void *pointer, size_t size){
        int ok = recv(socket_cliente, pointer, size, 0);
        if(ok < 1) {
                perror("recv error");
                exit(-1);
        }
}

void buscar_registro(int socket_cliente){
        FILE *file;
        int aux = 0, i, ok;
        dogType *pet;

        char nombre[32],aux_n[32];
        recibir(socket_cliente, nombre, 32);
        //Se compruba que exista almenos un registro con ese nombre

        i = hash_list[hash_value(nombre)];
        if(i == 0) {
                enviar(socket_cliente, &i, sizeof(int));
                return;
        }

        lock(DATA_SOURCE);

        file = fopen(DATA_PATH,"rb");
        if(file == NULL) {
                printf("no se pudo abrir el archivo %s en buscar_registro\n",DATA_PATH);
                exit(-1);
        }
        fseek(file, (i - 1) * sizeof(dogType), SEEK_SET);
        pet = (dogType*) malloc(sizeof(dogType));
        if(pet == NULL) {
                printf("malloc error");
                exit(-1);
        }
        ok = fread(pet, sizeof(dogType), 1, file);
        if (ok == 0) {
                printf("error al leer en buscar_registro\n" );
                exit(-1);
        }

        enviar(socket_cliente, &i, sizeof(int));
        i++;
        while(fread(pet, sizeof(dogType), 1, file) > 0) {
                //Busca los registros sucesivos que coincidan con el nombre
                strcpy(aux_n, pet->nombre);
                lower_case(aux_n);
                if(strcmp(nombre, aux_n) == 0) {
                        enviar(socket_cliente, &i, sizeof(int));
                }
                i++;
        }

        i = -1;
        enviar(socket_cliente, &i, sizeof(int));
        free(pet);
        fclose(file);

        unlock(DATA_SOURCE);

        escribir_log(socket_cliente, 4, nombre);
}

void eliminar_registro(int socket_cliente){
        FILE *file, *temp;
        int dato, registros, eleccion;

        lock(DATA_SOURCE);

        file = fopen(DATA_PATH, "rb+");
        if(file == NULL) {
                printf("Error al abrir %s", DATA_PATH);
                exit(-1);
        }

        //Envía el puntero al final del archivo
        fseek(file,0,SEEK_END);
        //Calcula la cantidad de elementos del archivo
        registros = ftell(file)/sizeof(dogType);
        enviar(socket_cliente, &registros, sizeof(int));

        recibir(socket_cliente, &dato, sizeof(int));

        dogType *ultimo = malloc(sizeof(dogType)), *eliminada = malloc(sizeof(dogType));

        //Se guarda el ultimo dato del archivo
        fseek(file, -sizeof(dogType), SEEK_END);
        fread(ultimo, sizeof(dogType), 1, file);

        //Posicionamos el puntero justo antes del dato a eliminar
        fseek(file, (dato - 1) * sizeof(dogType), SEEK_SET);


        fread(eliminada, sizeof(dogType), 1, file);

        //Se elimina la historia clinica relacionada a la mascota
        char ruta[32];
        sprintf(ruta, "historias/%i.txt", eliminada->id);
        remove(ruta);

        //Se sobre-escribe el dato a eliminar
        fseek(file, -sizeof(dogType), SEEK_CUR);
        fwrite(ultimo, sizeof(dogType), 1, file);

        fclose(file);

        //Se abre y se trunca el archivo para borrar el ultimo dato (el cual ya se salvo)

        int fd = open(DATA_PATH, O_WRONLY);
        if(fd == -1) {
                perror("Open error ");
        }
        ftruncate(fd, (registros - 1) * sizeof(dogType));
        close(fd);

        unlock(DATA_SOURCE);

        enviar(socket_cliente, eliminada->nombre, 32);
        free(ultimo);
        free(eliminada);

        char lg[10];
        sprintf(lg, "%d", dato);
        escribir_log(socket_cliente, 3, lg);
}

void enviar_historia(int socket_cliente, int dato, FILE *data){
        dogType *mascota = malloc(sizeof(dogType));
        char ruta[32];

        //Acomoda el puntero justo al inicio de el registro deseado y lo guarda en una estructura
        fseek(data, (dato - 1) * sizeof(dogType), SEEK_SET);
        fread(mascota, sizeof(dogType), 1, data);
        fclose(data);

        unlock(DATA_SOURCE);

        //Comprueba la existencia de la historia clinica de la mascota
        sprintf(ruta, "historias/%i.txt", mascota->id);
        FILE *file = fopen(ruta, "r");
        if(file == NULL) {
                //En caso de que no exista el archivo, lo crea y lo inicia con los datos de la mascota
                file = fopen(ruta, "w+");
                if(file == NULL) {
                        printf("Error al crear %s\n", ruta);
                        exit(-1);
                }

                fprintf(file, "Nombre: %s\n", mascota->nombre);
                fprintf(file, "Tipo:   %s\n", mascota->tipo);
                fprintf(file, "Edad:   %i\n", mascota->edad);
                fprintf(file, "Raza:   %s\n", mascota->raza);
                fprintf(file, "Tamaño: %i\n", mascota->estatura);
                fprintf(file, "Peso:   %f\n", mascota->peso);
                fprintf(file, "Genero: %c\n", mascota->sexo);
        }
        free(mascota);
        fclose(file);

        lock(HIST_SOURCE);

        file = fopen(ruta, "r+");
        fseek(file, 0L, SEEK_END);
        size_t size = ftell(file);
        enviar(socket_cliente, &size, sizeof(size_t));

        rewind(file);

        char *buffer = malloc(size);
        ssize_t i;
        while((i = getline(&buffer, &size, file)) > 0) buffer += i;
        buffer -= size;

        enviar(socket_cliente, buffer, size);
        fclose(file);

        unlock(HIST_SOURCE);

        recibir(socket_cliente, &size, sizeof(size_t));
        buffer = realloc(buffer, size);

        recibir(socket_cliente, buffer, size);

        lock(HIST_SOURCE);

        file = fopen(ruta, "w+");
        fprintf(file, "%s\n", buffer);

        free(buffer);
        fclose(file);

        unlock(HIST_SOURCE);
}

void ver_registro(int socket_cliente){
        FILE *data;
        int dato, cant_registos;
        char *buffer;

        lock(DATA_SOURCE);

        data = fopen(DATA_PATH,"rb+");
        if(data == NULL) {
                printf("Error al abrir %s\n",DATA_PATH);
                exit(-1);
        }

        //Envía el puntero al final del archivo
        fseek(data, 0, SEEK_END);
        //Calcula la cantidad de elementos del archivo
        cant_registos = ftell(data)/sizeof(dogType);
        enviar(socket_cliente, &cant_registos, sizeof(int));

        recibir(socket_cliente, &dato, sizeof(int));

        enviar_historia(socket_cliente, dato, data);

        char lg[10];
        sprintf(lg, "%d", dato);
        escribir_log(socket_cliente, 2, lg);
}

void insertar_registro(int socket_cliente){
        dogType mascota;
        recibir(socket_cliente, &mascota, sizeof(dogType));

        FILE *file;
        int ok, id;

        lock(DATA_SOURCE);

        file = fopen(DATA_PATH, "ab+");
        if(file == NULL) {
                printf("Error al abrir %s\n",DATA_PATH);
                exit(-1);
        }
        id = last_id + 1;
        mascota.id = id;

        ok = fwrite(&mascota, sizeof(dogType), 1, file);
        if(ok == 0) {
                printf("Error al escribir en %s\n",DATA_PATH);
                exit(-1);
        }

        enviar(socket_cliente, &id, sizeof(int));

        fclose(file);

        unlock(DATA_SOURCE);

        escribir_log(socket_cliente, 1, "");
}

void crear_socket(){

        struct sockaddr_in server;
        socket_servidor = socket(AF_INET, SOCK_STREAM, 0);

        if(socket_servidor == -1) {
                perror("socket creation failed");
                exit(-1);
        }

        bzero(server.sin_zero, 8);

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = INADDR_ANY;

        int ok = bind(socket_servidor, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
        if(ok == -1) {
                perror("Error al iniciar el servidor\n");
                exit(-1);
        }

        ok = listen(socket_servidor, BACKLOG);
        if(ok == -1) {
                perror("listen error");
                exit(-1);
        }
}

void *atencion_cliente(int *socket_cliente){

        int opcion, csize;
        struct sockaddr_in client;
        getpeername(*socket_cliente, (struct sockaddr *)&client, &csize);

        do {
                recibir(*socket_cliente, &opcion, sizeof(int));
                switch (opcion) {
                case 1:
                        insertar_registro(*socket_cliente);
                        reiniciar_hash();
                        break;

                case 2:
                        ver_registro(*socket_cliente);
                        break;

                case 3:
                        eliminar_registro(*socket_cliente);
                        reiniciar_hash();
                        break;

                case 4:
                        buscar_registro(*socket_cliente);
                        break;

                case 5:
                        //printf("Conexion finalizada con %s\n", inet_ntoa(client.sin_addr));
                        close(*socket_cliente);
                        *socket_cliente = NO_ASIGNADO;
                        NUM_CLIENTES--;
                        break;
                }
        } while(opcion != 5);
}

int main(int argc, char *argv[]){
    BLOCK_OPTION = atoi(argv[1]);

    init_blocker();

    crear_socket();

    last_id = 1;
    reiniciar_hash();
    pthread_t thread[BACKLOG];
    int clientesfd[BACKLOG], csize, i, ok;
    for (i = 0; i < BACKLOG; i++) clientesfd[i] = NO_ASIGNADO;
    struct sockaddr_in client;

    do {
        for (size_t j = 0; j < BACKLOG; j++) {
            if (clientesfd[j] == NO_ASIGNADO) {
                i = j;
                break;
            }
        }

            clientesfd[i] = accept(socket_servidor, (struct sockaddr *) &client, (socklen_t *)&csize);

            if(clientesfd[i] == -1) {
                    perror("Error de conexion");
                    exit(-1);
            }
            pthread_create(&thread[i], NULL, (void *)atencion_cliente, (void *)&clientesfd[i]);
            NUM_CLIENTES++;
    } while(NUM_CLIENTES < BACKLOG);

    for (i = 0; i < BACKLOG; i++) {
        ok = pthread_join(thread[i],NULL);
        if (ok != 0){
            printf("Error al hacer join al hilo %i\n",i );
            exit(-1);
        }
    }
    close(socket_servidor);

    close_blocker();
    return 0;
}
