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

void recibir(void *ponter, size_t size){
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

void mostrar_editor(int dato, FILE *data)
{
    dogType *mascota = malloc(sizeof(dogType));
    char ruta[32], comando[38] = "gedit ";

    //Acomoda el puntero justo al inicio de el registro deseado y lo guarda en una estructura
    fseek(data, (dato - 1) * sizeof(dogType), SEEK_SET);
    fread(mascota, sizeof(dogType), 1, data);
    fclose(data);

    //Comprueba la existencia d la historia clinica de la mascota
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

    //Abre el archivo usando gedit
    strcat(comando, ruta);
    system(comando);
}

void ver_registro(){

    system("clear");
    FILE *data;
    int dato, cant_registos;
    data = fopen(DATA_PATH,"rb+");
    if(data == NULL){
        printf("Error al abrir %s\n",DATA_PATH);
        exit(-1);
    }

    //Envía el puntero al final del archivo
    fseek(data, 0, SEEK_END);
    //Calcula la cantidad de elementos del archivo
    cant_registos = ftell(data)/sizeof(dogType);
    printf("El numero de registros es: %d\n", cant_registos);

    get_info("Ingrese el registro que desea buscar: "," %i", &dato);

    while(dato > cant_registos || dato < 0){
        //Verifica que si exista el registro
        printf("Registro Incorrecto\n");
        get_info("Por favor ingrese el registro que desea buscar: "," %i", &dato);
    }

    mostrar_editor(dato, data);
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

void crear_socket(char *addr, char *port){

  socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in client;

  if(clientfd == -1){
    perror("socket creation failed");
    exit(-1);
  }

  bzero(client.sin_zero, 8);

  client.sin_family = AF_INET;
  client.sin_port = htons(port);
  client.sin_addr.s_addr = inet_addr(addr);

  printf("Intentando conectar con %s en el puerto %s \n", addr, port);

  int ok = connect(clientfd, (struct sockaddr*)&client, sizeof(struct sockaddr_in));

  if(ok == -1){
    perror("Conexion Rechazada\n");
    exit(-1);
  }else printf("Conexion Exitosa\n");

  return clientfd;
}

int main(int argc, char *argv[]){

  crear_socket(argv[1], argv[2]);
  menu();
  return 0;
}
