#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lib/structures.h"
#include "../lib/hash.h"

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

  return mascota;
}

void imprimirMascota(void *p){
  dogType *pointer;
  pointer = p;

  printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
  pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}
