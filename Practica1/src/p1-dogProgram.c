#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/structures.h"

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
    FILE *file;
    int aux = 0, i, ok;
    dogType *pet;

    char nombre[32],aux_n[32];
    get_info("Ingrese el nombre que desea buscar: ", " %s",nombre);
    //Se compruba que exista almenos un registro con ese nombre

    i = hash_list[hash_value(nombre)];
    if(i == 0){
        printf("No existe  el registro solicitado\n");
        return;
    }

    file = fopen(DATA_PATH,"rb");
    if(file == NULL)
    {
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
    printf("\nLos registros que coinciden con el nombre %s son:\n-> %i\n", nombre, i++);
    while(fread(pet, sizeof(dogType), 1, file) > 0){
        //Busca los registros sucesivos que coincidan con el nombre
        strcpy(aux_n,pet->nombre);
        lower_case(aux_n);
        if(strcmp(nombre, aux_n) == 0){
            printf("-> %i\n", i);
        }
        i++;
    }
    free(pet);
    fclose(file);
}

void eliminar_registro()
{
    system("clear");
    FILE *file, *temp;
    int dato, numero_de_registros, eleccion;
    file = fopen(DATA_PATH,"rb");
    if(file == NULL){
        printf("Error al abrir %s",DATA_PATH);
        exit(-1);
    }

    //Envía el puntero al final del archivo
    fseek(file,0,SEEK_END);
    //Calcula la cantidad de elementos del archivo
    numero_de_registros = ftell(file)/sizeof(dogType);
    printf("El numero de registros es: %d\n", numero_de_registros);

    printf("Ingrese el registro que desea eliminar\n");
    scanf(" %i", &dato);
    while(dato > numero_de_registros || dato < 0){
        //Verifica que si exista el registro
        printf("Registro Incorrecto\n");
        printf("Por favor ingrese el registro que desea eliminar\n");
        scanf("%i", &dato);
    }

    //Regresa el puntero file al inicio del archivo
    rewind(file);
    temp = fopen(TEMP_PATH, "wb+");
    if(file == NULL){
        printf("Error al crear %s\n",TEMP_PATH);
        exit(-1);
    }
    int i = 1;
    dogType *newPet = malloc(sizeof(dogType));
    if(newPet == NULL){
        printf("malloc error");
        exit(-1);
    }
    while( fread(newPet, sizeof(dogType), 1, file) != 0 && dato != i){
        //Se copia en un archivo temporal la primera parte de los registros (antes del que va a ser eliminado)
        fwrite(newPet, sizeof(dogType), 1, temp);
        i ++;
    }

    printf("La mascota que será eliminada es :\"%s\"\n", newPet->nombre);

    //Se copian los datos restantes
    while(fread(newPet, sizeof(dogType), 1, file) != 0)
        fwrite(newPet, sizeof(dogType), 1, temp);

    free(newPet);
    fclose(file);
    fclose(temp);

    //Se elimina el archivo original
    remove(DATA_PATH);
    //Se renombra el temporal
    rename(TEMP_PATH, DATA_PATH);
}


void mostrar_editor(int dato, FILE *data)
{
    dogType *newPet = malloc(sizeof(dogType));
    int eleccion;
    //Acomoda el puntero justo al inicio de el registro deseado y lo guarda en una estructura
    fseek(data, (dato - 1) * sizeof(dogType), SEEK_SET);
    fread(newPet, sizeof(dogType), 1, data);

    //Se prepara el archivo temporal para modificar el registro
    FILE *file = fopen(TEMP_PATH,"w+");
    if(file == NULL){
        printf("Error al abrir %s\n",TEMP_PATH);
        exit(-1);
    }
    fprintf(file, "Nombre: %s\n", newPet->nombre);
    fprintf(file, "Tipo:   %s\n", newPet->tipo);
    fprintf(file, "Edad:   %i\n", newPet->edad);
    fprintf(file, "Raza:   %s\n", newPet->raza);
    fprintf(file, "Tamaño: %i\n", newPet->estatura);
    fprintf(file, "Peso:   %f\n", newPet->peso);
    fprintf(file, "Genero: %c\n", newPet->sexo);
    fclose(file);

    system("gedit dataDogs.temp");
    printf("linea %i\n",156);
    printf("¿Desea guardar los cambios realizados en el archivo?\n");
    get_info("1 si, 0 no : "," %i", &eleccion);

    if(eleccion == 1){
        //string para almacenar basura
        char trh[32];

        //Se recupera la informacion del archivo temporal
        file = fopen(TEMP_PATH,"r");
        if(file == NULL){
            printf("Error al abrir %s\n",TEMP_PATH);
            exit(-1);
        }
        fscanf(file, "%s %s", trh, newPet->nombre);
        fscanf(file, "%s %s", trh, newPet->tipo);
        fscanf(file, "%s %i", trh, &newPet->edad);
        fscanf(file, "%s %s", trh, newPet->raza);
        fscanf(file, "%s %i", trh, &newPet->estatura);
        fscanf(file, "%s %f", trh, &newPet->peso);
        fscanf(file, "%s %c", trh, &newPet->sexo);
        fclose(file);

        //Se reinserta la informacion recuperada en el registro de datos
        fseek(data, (dato - 1) * sizeof(dogType), SEEK_SET);
        fwrite(newPet, sizeof(dogType), 1, data);
        reiniciar_hash();
    }
    fclose(data);
    free(newPet);
    remove(TEMP_PATH);
}

void ver_registro()
{

    system("clear");
    FILE *data;
    int dato, numero_de_registros;
    data = fopen(DATA_PATH,"rb+");
    if(data == NULL){
        printf("Error al abrir %s\n",DATA_PATH);
        exit(-1);
    }

    //Envía el puntero al final del archivo
    fseek(data, 0, SEEK_END);
    //Calcula la cantidad de elementos del archivo
    numero_de_registros = ftell(data)/sizeof(dogType);
    printf("El numero de registros es: %d\n", numero_de_registros);

    get_info("Ingrese el registro que desea buscar: "," %i",&dato);

    while(dato > numero_de_registros || dato < 0){
        //Verifica que si exista el registro
        printf("Registro Incorrecto\n");
        get_info("Por favor ingrese el registro que desea buscar: "," %i", &dato);
    }

    mostrar_editor(dato, data);
}

void insertar_registro(dogType *mascota)
{

    FILE *file;
    int ok;
    file = fopen(DATA_PATH, "ab+");
    if(file == NULL){
        printf("Error al abrir %s\n",DATA_PATH);
        exit(-1);
    }

    ok = fwrite(mascota, sizeof(dogType), 1, file);
    if(ok == 0){
        printf("Error al escribir en %s\n",DATA_PATH);
        exit(-1);
    }

    fclose(file);
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
    reiniciar_hash();
    do {
        mostrar_menu();
        get_info("Ingrese la opcion: "," %i", &opcion);
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

int main()
{
    menu();
    return 0;
}
