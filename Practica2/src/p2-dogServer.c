#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/structures.h"
#include "../lib/hash.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BACKLOG 32
int socket_servidor;

void enviar(int socket_cliente, void *pointer, size_t size){
  int ok = send(socket_cliente, pointer, size, 0);
  if(ok == -1){
    perror("send error");
    exit(-1);
  }
}

void recibir(int socket_cliente, void *pointer, size_t size){
  int ok = recv(socket_cliente, pointer, size, 0);
  if(ok == -1){
    perror("recv error");
    exit(-1);
  }
}

void buscar_registro(int socket_cliente){
  system("clear");

  FILE *file;
  int aux = 0, i, ok;
  dogType *pet;

  char nombre[32],aux_n[32];
  recibir(socket_cliente, nombre, 32);
  //Se compruba que exista almenos un registro con ese nombre

  i = hash_list[hash_value(nombre)];
  if(i == 0){
      enviar(socket_cliente, &i, sizeof(int));
      return;
  }

  file = fopen(DATA_PATH,"rb");
  if(file == NULL){
      printf("no se pudo abrir el archivo %s en buscar_registro\n",DATA_PATH);
      exit(-1);
  }
  fseek(file, (i - 1) * sizeof(dogType), SEEK_SET);
  pet = (dogType*) malloc(sizeof(dogType));
  if(pet == NULL){
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
  while(fread(pet, sizeof(dogType), 1, file) > 0){
      //Busca los registros sucesivos que coincidan con el nombre
      strcpy(aux_n, pet->nombre);
      lower_case(aux_n);
      if(strcmp(nombre, aux_n) == 0){
          enviar(socket_cliente, &i, sizeof(int));
      }
      i++;
  }

  i = -1;
  enviar(socket_cliente, &i, sizeof(int));
  free(pet);
  fclose(file);
}

void eliminar_registro(int socket_cliente){
  system("clear");

  FILE *file, *temp;
  int dato, registros, eleccion;
  file = fopen(DATA_PATH, "rb+");
  if(file == NULL){
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
  if(fd == -1){
    perror("Open error ");
  }
  ftruncate(fd, (registros - 1) * sizeof(dogType));
  close(fd);

  enviar(socket_cliente, eliminada->nombre, 32);
  free(ultimo);
  free(eliminada);
}

void enviar_historia(int socket_cliente, int dato, FILE *data){
  dogType *mascota = malloc(sizeof(dogType));
  char ruta[32];

  //Acomoda el puntero justo al inicio de el registro deseado y lo guarda en una estructura
  fseek(data, (dato - 1) * sizeof(dogType), SEEK_SET);
  fread(mascota, sizeof(dogType), 1, data);
  fclose(data);

  //Comprueba la existencia de la historia clinica de la mascota
  sprintf(ruta, "historias/%i.txt", mascota->id);
  FILE *file = fopen(ruta, "r");
  if(file == NULL){
    //En caso de que no exista el archivo, lo crea y lo inicia con los datos de la mascota
    file = fopen(ruta, "w+");
    if(file == NULL){
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
  fopen(ruta, "a");

  int size = fseek(file, 0, SEEK_SET);
  enviar(socket_cliente, &size, sizeof(int));

  char *buffer = malloc(size);
  rewind(file);
  int i = 0;
  printf("empieza a leer %i\n", size);
  while(i < size){
    i += fread(buffer + i, size - i, 1, file);
  }
  printf("termina de leer %i\n", size);

  enviar(socket_cliente, buffer, size);
  fclose(file);

  recibir(socket_cliente, &size, sizeof(size_t));
  buffer = realloc(buffer, size);
  recibir(socket_cliente, buffer, size);
  i = 0;
  file = fopen(ruta, "w+");
  while(i < size){
    i += fwrite(buffer + i, size - i, 1, file);
  }

  fclose(file);
  free(buffer);
}

void ver_registro(int socket_cliente){
  FILE *data;
  int dato, cant_registos;
  char *buffer;
  data = fopen(DATA_PATH,"rb+");
  if(data == NULL){
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
}

void insertar_registro(int socket_cliente){
  dogType mascota;
  recibir(socket_cliente, &mascota, sizeof(dogType));

  FILE *file;
  int ok, id;
  file = fopen(DATA_PATH, "ab+");
  if(file == NULL){
      printf("Error al abrir %s\n",DATA_PATH);
      exit(-1);
  }
  id = last_id + 1;
  mascota.id = id;

  ok = fwrite(&mascota, sizeof(dogType), 1, file);
  if(ok == 0){
      printf("Error al escribir en %s\n",DATA_PATH);
      exit(-1);
  }

  enviar(socket_cliente, &id, sizeof(int));

  fclose(file);
}

void crear_socket(){

  struct sockaddr_in server;
  socket_servidor = socket(AF_INET, SOCK_STREAM, 0);

  if(socket_servidor == -1){
    perror("socket creation failed");
    exit(-1);
  }

  bzero(server.sin_zero, 8);

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;

  printf("Iniciando el servidor en el puerto %i \n", PORT);

  int ok = bind(socket_servidor, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
  if(ok == -1){
    perror("Error al iniciar el servidor\n");
    exit(-1);
  }else printf("Inicio Exitoso\n");

  ok = listen(socket_servidor, BACKLOG);
  if(ok == -1){
    perror("listen error");
    exit(-1);
  }
}

void *atencion_cliente(int *socket_cliente){

  int opcion, csize;
  struct sockaddr_in client;
  getpeername(*socket_cliente, (struct sockaddr *)&client, &csize);

  do{
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
        printf("Conexion finalizada con %s\n", inet_ntoa(client.sin_addr));
      break;
    }
  }while(opcion != 5);
}

int main(){
  crear_socket();

  last_id = 1;
  reiniciar_hash();
  pthread_t thread[BACKLOG];
  int clientes[BACKLOG], csize, i = 0;

  struct sockaddr_in client;

  printf("Presione Ctrl + C para terminar\n");
  do{
    clientes[i] = accept(socket_servidor, (struct sockaddr *) &client, (socklen_t *)&csize);
    if(clientes[i] == -1){
      perror("Error de conexion");
      exit(-1);
    }
    pthread_create(&thread[i], NULL, (void *)atencion_cliente, (void *)&clientes[i]);

    i++;
  }while(i < BACKLOG);

  return 0;
}
