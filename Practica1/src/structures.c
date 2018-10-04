#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lib/structures.h"

void reiniciar_hash()
{
    //reinicia el registro de la tabla para incluir registros
    for(int j = 0; j < HASH_TABLE_SIZE; j++) hash_list[j] = 0;

    FILE *file;
    int i = 1, ok;
    dogType *mascota;
    mascota = malloc(sizeof(dogType));
    if(mascota == NULL){
        printf("malloc error");
        exit(-1);
    }
    file = fopen("dataDogs.dat", "rb");
    if(file == NULL){
        printf("Error al abrir dataDogs.dat");
        exit(-1);
    }
    while(fread(mascota, sizeof(dogType), 1, file) != 0){
        //Almacena en la tabla la posicion del primer registro al que corresponde un valor de hash determinado
        if(hash_list[hash_value(mascota->nombre)] == 0) hash_list[hash_value(mascota->nombre)] = i;
        i ++;
    }
    free(mascota);
    fclose(file);
}

unsigned long hash_value(char *str)
{
    //Retorna el valor hash de una cadena
    lower_case(str);
    unsigned long hash = 0;
    for(int j = 0 , len = strlen(str); j < len; j++){
        hash = hash*33 + str[j]%33 + j*33 - (long)str[j]*13;
    }
    return hash%HASH_TABLE_SIZE;
}

void lower_case(char *str)
{
    //Convierte una cadena a letras minusculas
    for(int i = 0,len = strlen(str); i < len; i++){
        str[i] = tolower(str[i]);
    }
}

dogType* crear_registro()
{
    dogType *mascota;
    mascota = (dogType*) malloc(sizeof(dogType));
    if(mascota == NULL){
        printf("Error al crear el registro");
        exit(-1);
    }
    system("clear");

    FILE *file = fopen(DATA_PATH, "rb");
    //Validar
    fseek(file, -sizeof(dogType), SEEK_END);
    fread(mascota, sizeof(dogType), 1, file);
    fclose(file);
    mascota->id = mascota->id+1;

    printf("Bienvenido el menú para registrar una nueva mascota.\nPorfavor ingrese los datos solicitados a continuación:\n\n");
    printf("Ingrese el nombre de la mascota\n");
    scanf(" %s", mascota->nombre);
    printf("Ingrese el tipo de la mascota\n");
    scanf(" %s", mascota->tipo);
    printf("Ingrese la edad de la mascota\n");
    scanf(" %d", &mascota->edad);
    printf("Ingrese la raza de la mascota\n");
    scanf(" %s", mascota->raza);
    printf("Ingrese la estatura de la mascota\n");
    scanf(" %d", &mascota->estatura);
    printf("Ingrese el peso de la mascota\n");
    scanf(" %f", &mascota->peso);
    printf("Ingrese el género de la mascota\n");
    scanf(" %c", &mascota->sexo);
    while(mascota->sexo != 'H' && mascota->sexo != 'M'){
        printf("El género de la mascota debe ser H para Masculino o M para Femenino\nIngrese nuevamente el género de la mascota");
        scanf(" %c", &mascota->sexo);
    }

    return mascota;
}

void imprimirMascota(void *p)
{
    dogType *pointer;
    pointer = p;

    printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
    pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}
