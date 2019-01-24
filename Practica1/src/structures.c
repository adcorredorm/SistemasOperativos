#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lib/structures.h"

#define SZ_DOGTYPE sizeof(dogType)
#define SZ_INT sizeof(int)

void iniciar_hash() {
    //reinicia el registro de la tabla para incluir registros
    for(int j = 0; j < HASH_TABLE_SIZE; j++) hash_list[j] = 0;

    FILE *file;
    int i = 1, ok,idx;
    dogType *new_pet;
    char aux_nom[32];

    file = fopen(DATA_PATH, "rb+");
    if(file == NULL){
        printf("Error al abrir dataDogs.dat");
        exit(-1);
    }

    new_pet = (dogType*) malloc(sizeof(dogType));

    if(new_pet == NULL){
        printf("malloc error");
        exit(-1);
    }

    while(fread(new_pet, sizeof(dogType), 1, file) != 0){
        //Almacena en la tabla la posicion del ultimo registro al que corresponde un valor de hash determinado
        strcpy(aux_nom,new_pet->nombre);
        idx = hash_value(aux_nom);
        if(hash_list[idx] != 0) {

            new_pet->prev = hash_list[idx];

            /* Apuntamos en el archivo donde está el campo de la posicion siguiente del últomo registro */
            if (fseek(file,(new_pet->prev-1)*SZ_DOGTYPE+SZ_INT, SEEK_SET) == -1) {
                printf("%s\n", "fseek1-error en iniciar_hash");
                exit(-1);
            }

            /* Enlazamos el último registro con el nuevo, escribiendo en el campo siguiente del último registro
            la posicion del nuevo registro */
            if (!fwrite(&i,SZ_INT,1,file)) {
                printf("%s\n", "fwrite1-error en iniciar_hash");
                exit(-1);
            }

            //printf("%s\n", "Hola");

            /* Apuntamos al campo "prev" de "new_pet" */
            if (fseek(file,(i-1)*SZ_DOGTYPE+2*SZ_INT, SEEK_SET) == -1) {
                printf("%s\n", "fseek2-error en iniciar_hash");
                exit(-1);
            }

            /* Enlazamos el nuevo registro con el último, escribiendo en el campo "prev" del nuevo registro
            la posicion del último registro */
            if (!fwrite(&new_pet->prev,SZ_INT,1,file)) {
                printf("%s\n", "fwrite2-error en iniciar_hash");
                exit(-1);
            }

            /* Apuntamos al siguiente registro */
            if (fseek(file,i*SZ_DOGTYPE, SEEK_SET) == -1) {
                printf("%s\n", "fseek1-error en iniciar_hash");
                exit(-1);
            }
        }
        hash_list[idx] = i;
        if(new_pet->id > last_id) last_id = new_pet->id;
        i++;
    }
    NUM_DATA = i - 1;
    free(new_pet);
    fclose(file);
}

unsigned long hash_value(char *str){
    //Retorna el valor hash de una cadena
    lower_case(str);
    unsigned long hash = 0;
    for(int j = 0 , len = strlen(str); j < len; j++){
        hash = hash*33 + str[j]%33 + j*33 - (long)str[j]*13;
    }
    return hash%HASH_TABLE_SIZE;
}

void lower_case(char *str){
    //Convierte una cadena a letras minusculas
    for(int i = 0,len = strlen(str); i < len; i++){
        str[i] = tolower(str[i]);
    }
}

dogType* crear_registro(){
    dogType *mascota;
    mascota = (dogType*) malloc(sizeof(dogType));
    if(mascota == NULL){
        printf("Error al crear el registro");
        exit(-1);
    }
    system("clear");

    mascota->id = last_id + 1;

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

    mascota->next = mascota->prev = 0;
    NUM_DATA++;
    return mascota;
}

int buscar(char nombre[]) {
    int idx,ok,*pos;
    dogType* pet;
    FILE* file;
    char aux1[32], aux2[32];

    idx = (int) hash_value(strcpy(aux1,nombre));
    if(hash_list[idx] == 0)
        return 0;

    file = fopen(DATA_PATH,"r");
    if(file == NULL){
        printf("Error al abrir el archivo al buscar");
        exit(-1);
    }

    pet = (dogType*) malloc(sizeof(dogType));
    if (pet == NULL) {
        printf("pet-malloc error al buscar\n");
        exit(-1);
    }

    /* Se asigma la posición del último registro que coincide con
    el nombre a pos */
    pos = &hash_list[idx];

    ok = fseek(file,(*pos-1)*sizeof(pet),SEEK_SET);
    if (ok == -1) {
        printf("fseek error en buscar\n");
        exit(-1);
    }

    /* Leemos todos los registros con el mismo nombre, desde el
    último hasta el primero */
    while ((ok = fread(pet,sizeof(pet),1,file))>0) {
        strcpy(aux2, pet->nombre);
        if (aux1 == aux2) {
            printf("%i\n", *pos);
        }
        if (pet->prev == 0) break;//Si no hay mas con el mismo nombre, interrumpo

        /* Asignamos la posicion de la coincidencia anterior */
        pos = &pet->prev;

        // Apuntamos en el archivo al anterior registro
        ok = fseek(file,(*pos-1)*sizeof(pet),SEEK_SET);
        if (ok == -1) {
            printf("while-fseek error en buscar\n");
            exit(-1);
        }
    }
    free(pet);
    fclose(file);
    return 1;
}

dogType* eliminar(int num_registro){
    FILE *file;
    int idx;
    dogType* prev_next_last, *deleted;
    char aux[32];

    if ((file = fopen(DATA_PATH,"rb+")) == NULL) {
        printf("%s\n", "fopen-error en eliminar");
        return NULL;
    }

    if ((prev_next_last = (dogType*) malloc(3*sizeof(dogType))) == NULL) {
        printf("%s\n", "malloc1-error en eliminar");
        return NULL;
    }

    if ((deleted = (dogType*) malloc(sizeof(dogType))) == NULL) {
        printf("%s\n", "malloc2-error en eliminar");
        return NULL;
    }

    /* Leemos el registro a eliminar (pet[0]) */
    if (!read_pet(num_registro, deleted, file)) {
        printf("%s\n", "read_pet1-error en eliminar");
        return NULL;
    }

    /* Leemos el ultimo registro (pet[2]) */
    if (!read_pet(NUM_DATA, &prev_next_last[2], file)) {
        printf("%s\n", "error al leer el ultimo registro en eliminar");
        return NULL;
    }

    if (deleted->prev != 0) {
        if (deleted->next != 0) {
            /* Leemos la siguiente coinicidencia al registro a eliminar (pet[1]) */
            if (!read_pet(deleted->next, &prev_next_last[1], file)) {
                printf("%s\n", "read_pet1-error en eliminar");
                return NULL;
            }

            /* Leemos la anterior coinicidencia al registro a eliminar (pet[0]) */
            if (!read_pet(deleted->prev, &prev_next_last[0], file)) {
                printf("%s\n", "read_pet1-error en eliminar");
                return NULL;
            }

            prev_next_last[0].next = deleted->next;
            prev_next_last[1].prev = deleted->prev;

            if (!write_pet(deleted->prev, &prev_next_last[0], file)) {
                printf("%s\n", "error al escribir el ultimo registro en eliminar");
                return NULL;
            }

            if (!write_pet(deleted->next, &prev_next_last[1], file)) {
                printf("%s\n", "error al escribir el ultimo registro en eliminar");
                return NULL;
            }
        }else{
            /* Leemos la anterior coinicidencia al registro a eliminar (pet[0]) */
            if (!read_pet(deleted->prev, &prev_next_last[0], file)) {
                printf("%s\n", "read_pet1-error en eliminar");
                return NULL;
            }

            idx = (int) (hash_value(strcpy(aux, deleted->nombre)));
            prev_next_last[0].next = 0;
            hash_list[idx] = deleted->prev;//actualizamos la tabla hash

            if (!write_pet(deleted->prev, &prev_next_last[0], file)) {
                printf("%s\n", "error al escribir el ultimo registro en eliminar");
                return NULL;
            }
        }
    }else{
        if (deleted->next != 0) {
            /* Leemos la anterior coinicidencia al registro a eliminar (pet[0]) */
            if (!read_pet(deleted->next, &prev_next_last[1], file)) {
                printf("%s\n", "read_pet1-error en eliminar");
                return NULL;
            }

            prev_next_last[1].prev = 0;

            if (!write_pet(deleted->next, &prev_next_last[1], file)) {
                printf("%s\n", "error al escribir el ultimo registro en eliminar");
                return NULL;
            }
        }
    }

    idx = (int) (hash_value(strcpy(aux, prev_next_last[2].nombre)));
    hash_list[idx] = num_registro;//actualizamos la nueva ubicación del, antes, último registro

    if (!write_pet(num_registro, &prev_next_last[2], file)) {
        printf("%s\n", "error al escribir el ultimo registro en eliminar");
        return NULL;
    }

    free(prev_next_last);
    fclose(file);

    return deleted;
}

int read_pet(int pos, dogType* pet, FILE* file){
    if (pos > 0) {
        if (fseek(file, (pos-1)*sizeof(dogType), SEEK_SET) == -1) {
            printf("%s\n", "fseek-for-error en read_pet");
            return 0;
        }

        if (fread(pet, sizeof(dogType), 1, file) == 0) {
            printf("%s\n", "fread-error en read_pet");
            return 0;
        }
    }
    return 1;
}
int write_pet(int pos, dogType* pet, FILE* file){
    if (pos > 0) {
        int ok;
        if (fseek(file, (pos-1)*sizeof(dogType), SEEK_SET) == -1) {
            printf("%s\n", "fseek-for-error en read_pet");
            return 0;
        }

        if ((ok = fwrite(pet, sizeof(dogType), 1, file)) == 0) {
            printf("%s\n", "fwrite-error en write_pet");
            //printf("pos %i  ok %i\n", pos, ok);
            return 0;
        }
    }
    return 1;
}


void imprimirMascota(void *p){
    dogType *pointer;
    pointer = p;

    printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
    pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}
