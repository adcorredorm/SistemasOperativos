#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/structures.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

dogType* crear_registro();
void eliminar_registro();
void mostrar_editor(int dato, FILE *data);
void ver_registro();
void buscar_registro();
void insertar_registro(dogType *mascota);
void mostrar_menu();
void get_info(const char* prompt, const char* format, void *ptr);
void menu();

void buscar_registro()
{
    system("clear");
    int aux = 0, i, ok;
    dogType *pet;

    char nombre[32];
    get_info("Ingrese el nombre que desea buscar: ", " %s",nombre);

    //Se compruba que exista almenos un registro con ese nombre
    if(!buscar(nombre)){
        printf("No existe  el registro solicitado\n");
    }
}

void eliminar_registro()
{
    system("clear");
    int dato;
    dogType* to_del;

    printf("El numero de registros es: %d\n", NUM_DATA);
    get_info("Ingrese el registro que desea eliminar\n", " %i", &dato);

    while(dato > NUM_DATA || dato < 0){
        //Verifica que si exista el registro
        get_info("Registro Incorrecto\nPor favor ingrese el registro que desea eliminar\n", " %i", &dato);
    }

    if ((to_del = eliminar(dato)) == NULL) {
        printf("%s\n", "eliminar-error en eliminar_registro");
        exit(-1);
    }

    printf("La mascota que será eliminada es :\"%s\"\n", to_del->nombre);

    //Se elimina la historia clinica relacionada a la mascota
    char ruta[32];
    sprintf(ruta, "historias/%i.txt", to_del->id);
    remove(ruta);

    free(to_del);

    //Se abre y se trunca el archivo para borrar el ultimo dato (el cual ya se salvo)
    int fd = open(DATA_PATH, O_WRONLY);
    if(fd == -1){
      perror("Open error\n");
      exit(-1);
    }
    ftruncate(fd, (NUM_DATA - 1) * sizeof(dogType));
    close(fd);
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

void ver_registro()
{
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

void insertar_registro(dogType *new_pet)
{

    FILE *file;
    int ok, idx;
    char aux[32];
    dogType *last_pet;

    file = fopen(DATA_PATH, "wb+");
    if(file == NULL){
        printf("Error al abrir %s\n",DATA_PATH);
        exit(-1);
    }

    strcpy(aux,new_pet->nombre);
    idx = hash_value(aux);
    new_pet->prev = hash_list[idx];
    hash_list[idx] = (int) (ftell(file)/sizeof(new_pet)) + 1;

    ok = fseek(file, (new_pet->prev -1)*sizeof(new_pet),SEEK_SET);
    if (ok == -1) {
        printf("%s\n", "feseek1-error en insertar_registro");
        exit(-1);
    }

    if((last_pet = (dogType*) malloc(sizeof(new_pet))) == NULL){
        printf("%s\n", "malloc-error en insertar_registro");
        exit(-1);
    }

    if (!fread(last_pet, sizeof(last_pet),1,file)) {
        printf("%s\n", "fread-error en insertar_registro");
        exit(-1);
    }

    last_pet->next = hash_list[idx];

    ok = fseek(file, -sizeof(new_pet),SEEK_CUR);
    if (ok == -1) {
        printf("%s\n", "feseek2-error en insertar_registro");
        exit(-1);
    }

    ok = fwrite(last_pet, sizeof(dogType), 1, file);
    if(ok == 0){
        printf("fwrite1-error en insertar_registro \n");
        exit(-1);
    }

    ok = fseek(file, 0, SEEK_END);
    if (ok == -1) {
        printf("%s\n", "feseek3-error en insertar_registro");
        exit(-1);
    }

    ok = fwrite(new_pet, sizeof(dogType), 1, file);
    if(ok == 0){
        printf("fwrite2-error en insertar_registro \n");
        exit(-1);
    }

    fclose(file);
    free(new_pet);
    free(last_pet);
}

void mostrar_menu()
{
    system("clear");
    printf("Menu:\n");
    printf("1. Ingresar Registro\n");
    printf("2. Ver Registro\n");
    printf("3. Borrar Registro\n");
    printf("4. Buscar Registro\n");
    printf("5. Salir\n");
}

void get_info(const char* prompt, const char* format, void *ptr)
{
    printf("%s", prompt);
    scanf(format, ptr);
}

void menu()
{
    int opcion;
    last_id = 1;
    //iniciar_hash();
    do {
        mostrar_menu();
        get_info("Ingrese la opcion: "," %i", &opcion);
        switch (opcion) {
            case 1:
            insertar_registro(crear_registro());
            break;
            case 2:
            ver_registro();
            break;
            case 3:
            eliminar_registro();
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

int main()
{
    menu();
    //printf("%zi\n",sizeof(dogType));
    return 0;
}
