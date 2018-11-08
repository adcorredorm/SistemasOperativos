#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "lib/structures.h"

int size = 32, cantNombres = 1670, cantRazas = 5, cantTipos = 6;

void generarEstructuras(int cantidad, char nombres[][32], char razas[][16], char tipos[][32]){
    FILE *file;
    int ok, i, size;

    if(cantidad > 50000) size = 50000;
    else size = cantidad;

    dogType *randmascota, mascotas[size];
    randmascota = malloc(sizeof(dogType));
    if(randmascota == NULL){
        printf("malloc error");
        exit(-1);
    }

    file = fopen("dataDogs.dat", "wb+");
    if(file == NULL){
        printf("fopen error");
        exit(-1);
    }

    srand48(time(NULL)); //Semilla

    for(i = 0; i < cantidad; i++){
        strcpy(randmascota->nombre, nombres[(int)(drand48()*cantNombres)]);
      randmascota->id = i + 1;
      strcpy(randmascota->tipo, tipos[(int)(drand48()*cantTipos)]);
      randmascota->edad = (int)(drand48()*15);
      strcpy(randmascota->raza, razas[(int)(drand48()*cantRazas)]);
      randmascota->estatura = (int)(drand48()*180);
      randmascota->peso = drand48()*80;
      randmascota->sexo = (drand48() > 0.5)?'H':'M';

      mascotas[i%50000] = *randmascota;

      if(i%(size+1) == size || i == cantidad - 1){
        if(i == cantidad - 1) size = (i % size) + 1;
        ok = fwrite(mascotas, sizeof(dogType), size, file);
        if(ok < size){
          printf("fwrite error in %i", i);
          exit(-1);
        }
      }
    }
    free(randmascota);

    ok = fclose(file);
    if(ok == EOF){
        printf("fclose error");
        exit(-1);
    }

}

void extraerDatos(char archivo[], char datos[][size]){
    FILE *file;

    file = fopen(archivo, "r");
    if(file == NULL){
        printf("Error al abrir el archivo %s", archivo);
        exit(-1);
    }

    int i;
    for(i = 0; !feof(file); i++) fscanf(file, " %s", datos[i]);

    i = fclose(file);
    if(i == EOF){
        printf("Error al cerrar el archivo %s", archivo);
        exit(-1);
    }
}

int main(){

    char nombres[cantNombres][32], razas[cantRazas][16], tipos[cantTipos][32];

    extraerDatos("nombres.txt", nombres);
    extraerDatos("tipos.txt", tipos);
    size = 16;
    extraerDatos("razas.txt", razas);

    generarEstructuras(1.0e+7, nombres, razas, tipos);

    return 0;
}
