#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/structures.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

dogType* crear_registro();
void eliminar_registro();
void mostrar_editor(int dato, FILE *data);
void ver_registro();
void buscar_registro();
void insertar_registro(dogType *mascota);
void mostrar_menu();
void get_info(const char* prompt, const char* format, void *ptr);
void menu();

int socket_cliente;

void enviar(void *pointer, size_t size){
  int ok = send(socket_cliente, pointer, size, 0);
  if(ok == -1){
    perror("send error");
    exit(-1);
  }
}

void recibir(void *pointer, size_t size){
  int ok = recv(socket_cliente, pointer, size, 0);
  if(ok == -1){
    perror("recv error");
    exit(-1);
  }
}

void buscar_registro(){
    system("clear");

    int i;
    char nombre[32];
    get_info("Ingrese el nombre que desea buscar: ", " %s", nombre);

    recibir(&i, sizeof(int));
    if(i == 0){
        printf("No existe  el registro solicitado\n");
        return;
    }

    printf("\nLos registros que coinciden con el nombre %s son:\n");
    do{
      printf("->%i", i);
      recibir(&i, sizeof(int));
    }while(i > 0);//TODO: hacer que cuando acaben los registros que coinciden regrese -1
}

void eliminar_registro(){
    system("clear");

    int registros, dato, ok;
    recibir(&registros, sizeof(int));
    printf("El numero de registros es: %d\n", registros);

    get_info("Ingrese el registro que desea eliminar\n", " %i", &dato);
    while(dato > registros || dato < 0){
        //Verifica que si exista el registro
        get_info("Registro Incorrecto\nPor favor ingrese el registro que desea eliminar\n"," %i", &dato);
    }
    enviar(&dato, sizeof(int));

    recibir(&ok, sizeof(int)); //TODO: enviar confirmacion de que fue eliminado
}

void ver_registro(){
  system("clear");

  char *buffer;
  int registros, dato, i;

  recibir(&registros, sizeof(int));
  printf("El numero de registros es: %d\n", registros);

  get_info("Ingrese el registro que desea buscar:\n", " %i", &dato);
  while(dato > registros || dato < 0){
      //Verifica que si exista el registro
      get_info("Registro Incorrecto\nPor favor ingrese el registro que desea buscar: \n"," %i", &dato);
  }
  enviar(&dato, sizeof(int));

  size_t size;
  recibir(&size, sizeof(int));
  buffer = malloc(size);
  if(buffer == NULL){
    perror("");
    exit(-1);
  }

  FILE *temp = fopen("temp.txt", "w+");
  i = 0;
  recibir(&buffer, size);
  while(i < size){
    i += fwrite(buffer + i, size - i, 1, temp);
  }
  fclose(temp);

  system("gedit temp.txt");

  temp = fopen("temp.txt", "a");
  size = ftell(temp);

  enviar(&size, sizeof(size_t));

  rewind(temp);
  buffer = realloc(buffer, size);
  i = 0;
  while(i < size){
    i += fread(buffer + i, size - i, 1, temp);
  }
  enviar(&buffer, size);

  free(buffer);
  fclose(temp);
  remove("temp.txt");
}

void insertar_registro(dogType *mascota){
    enviar(mascota, sizeof(dogType));

    int posicion;
    recibir(&posicion, sizeof(int));
    printf("El registro fue insertado en la posicion %i", posicion);
    free(mascota);
}

void mostrar_menu(){
    system("clear");
    printf("Menu:\n");
    printf("1. Ingresar Registro\n");
    printf("2. Ver Registro\n");
    printf("3. Borrar Registro\n");
    printf("4. Buscar Registro\n");
    printf("5. Salir\n");
}

void get_info(const char* prompt, const char* format, void *ptr){
    printf("%s", prompt);
    scanf(format, ptr);
}

void menu(){
    int opcion;
    last_id = 1;
    reiniciar_hash();
    do {
        mostrar_menu();
        get_info("Ingrese la opcion: "," %i", &opcion);
        enviar(&opcion, sizeof(int));

        switch (opcion) {
            case 1:
            insertar_registro(crear_registro());
            reiniciar_hash();
            break;
            case 2:
            ver_registro();
            break;
            case 3:
            eliminar_registro();
            reiniciar_hash();
            break;
            case 4:
            buscar_registro();
            break;
            case 5:
            system("clear");
            printf("Fin de la Aplicación.\n");
            break;
            default:
            printf("Opción invalida\n");
            break;
        }
        if(opcion!=5)
        {
            printf("Presione enter para volver al menu\n");
            while(getchar() != '\n');
        }
        getchar();
    } while(opcion != 5);
}

void crear_socket(char *addr){

  socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in client;

  if(socket_cliente == -1){
    perror("socket creation failed");
    exit(-1);
  }

  bzero(client.sin_zero, 8);

  client.sin_family = AF_INET;
  client.sin_port = htons(PORT);
  client.sin_addr.s_addr = inet_addr(addr);

  printf("Intentando conectar con %s en el puerto %i \n", addr, PORT);

  int ok = connect(socket_cliente, (struct sockaddr*)&client, sizeof(struct sockaddr_in));

  if(ok == -1){
    perror("Conexion Rechazada\n");
    exit(-1);
  }else printf("Conexion Exitosa\n");
}

int main(int argc, char *argv[]){

  crear_socket(argv[1]);
  menu();
  return 0;
}
