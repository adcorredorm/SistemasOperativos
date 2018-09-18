#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef struct{
  char nombre[32];
  char tipo[32];
  int edad;
  char raza[16];
  int estatura;
  double peso;
  char sexo; // M/F
} Mascota;

void imprimirMascota(void *p){
  Mascota *pointer;
  pointer = p;

  printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
      pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}

void generarRandomString(char string[], int size){

  int j, aux = (int)(drand48()*size-3)+3;

  for(j = 0; j < aux; j++){
    string[j] = 'a' + (int)(drand48()*26);
  }

  while(j < size){
    string[j] = '\0';
    j++;
  }

}

void generarEstructuras(int cantidad){
  FILE *file;
  int ok, i;

  Mascota *randmascota, mascotas[cantidad];
  randmascota = malloc(sizeof(Mascota));
  if(randmascota == NULL){
    printf("malloc error");
    exit(-1);
  }

  srand48(time(NULL)); //Semilla

  for(i = 0; i < cantidad; i++){
    generarRandomString(randmascota->nombre, 32);
    generarRandomString(randmascota->tipo, 32);
    randmascota->edad = (int)(drand48()*15); //El 15 es porque YOLO xD
    generarRandomString(randmascota->raza, 16);
    randmascota->estatura = (int)(drand48()*180); //Lo mismo el 180
    randmascota->peso = drand48()*80;
    if(drand48() > 0.5) randmascota->sexo = 'M';
    else randmascota->sexo = 'F';

    mascotas[i] = *randmascota;

    imprimirMascota(randmascota);
  }
  free(randmascota);

  file = fopen("BaseMascotas.dat", "w+");
  if(file == NULL){
    printf("fopen error");
    exit(-1);
  }

  ok = fwrite(randmascota, sizeof(Mascota), cantidad, file);
  if(ok == 0){
    //TODO: No funciona para 100.000+ datos (se puede escribir dentro del for, pero es muy lento)
    printf("fwrite error in %i", i);
    exit(-1);
  }

  ok = fclose(file);
  if(ok == EOF){
    printf("fclose error");
    exit(-1);
  }

}

int main(){
  generarEstructuras(1.0e+2);
  return 0;
}
