#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lib/structures.h"
#include "../lib/hash.h"

void reiniciar_hash(){
        //reinicia el registro de la tabla para incluir registros
        for(int j = 0; j < HASH_TABLE_SIZE; j++) hash_list[j] = 0;

        FILE *file;
        int i = 1, ok;
        dogType *mascota;
        mascota = malloc(sizeof(dogType));
        if(mascota == NULL) {
                printf("malloc error");
                exit(-1);
        }
        file = fopen("dataDogs.dat", "rb");
        if(file == NULL) {
                printf("Error al abrir dataDogs.dat");
                exit(-1);
        }
        while(fread(mascota, sizeof(dogType), 1, file) != 0) {
                //Almacena en la tabla la posicion del primer registro al que corresponde un valor de hash determinado
                if(hash_list[hash_value(mascota->nombre)] == 0) hash_list[hash_value(mascota->nombre)] = i;
                if(mascota->id > last_id) last_id= mascota->id;
                i++;
        }
        free(mascota);
        fclose(file);
}

unsigned long hash_value(char *str){
        //Retorna el valor hash de una cadena
        lower_case(str);
        unsigned long hash = 0;
        for(int j = 0, len = strlen(str); j < len; j++) {
                hash = hash*33 + str[j]%33 + j*33 - (long)str[j]*13;
        }
        return hash%HASH_TABLE_SIZE;
}

void lower_case(char *str){
        //Convierte una cadena a letras minusculas
        for(int i = 0,len = strlen(str); i < len; i++) {
                str[i] = tolower(str[i]);
        }
}
