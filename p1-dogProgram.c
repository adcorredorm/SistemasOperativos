#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "lib/structures.h"
#include "lib/generators.h"

void imprimirMascota(void *p){
    dogType *pointer;
    pointer = p;

    printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
        pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}



void generarEstructuras(int cantidad){
    FILE *file;
    int ok, i;


    dogType *randmascota, mascotas[cantidad];
    srand48(time(NULL)); //Semilla

    for(i = 0; i < cantidad; i++){
        randmascota = make_dogType(generarRandomString(32),
            generarRandomString(32),
            (int) rand_num(15),
            generarRandomString(16),
            (int)rand_num(180),
            (float)rand_num(180),
            rand_num(1)>0.5?'M':'F',
            i);
        if(randmascota == NULL){
          printf("malloc error en generarEstructuras");
          exit(-1);
        }

        mascotas[i] = *randmascota;

        imprimirMascota(randmascota);
    }
    free(randmascota);

    file = fopen("dataDog.dat", "w+");
    if(file == NULL){
        printf("fopen error");
        exit(-1);
    }

    ok = fwrite(randmascota, sizeof(dogType), cantidad, file);
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

void mostrar_menu()
{
	printf("Menu:\n");
	printf("1. Ingresar Registro\n");
	printf("2. Ver Registro\n");
	printf("3. Borrar Registro\n");
	printf("4. Buscar Registro\n");
	printf("5. Salir\n");
}

void menu()
{
	int option;
	mostrar_menu();
	printf("Ingrese la opcion: ");
	scanf(" %i", &option);
	while(option != 5)
	{
		switch(option)
		{
			case 1:
          printf("Aqui se Ingresa un Registro\n");
          generarEstructuras(1.0e+2);
          break;
          case 2:
          printf("Aqui se Ve un Registro\n");
          break;
          case 3:
          printf("Aqui se Borra un Registro\n");
          break;
          case 4:
          printf("Aqui se Busca un Registro\n");
          break;
          default:
          break;
      }
      while(getchar()!='\n');
      getchar();
      mostrar_menu();
      printf("Ingrese la opcion: ");
      scanf(" %i", &option);
  }
  printf("Adios :v\n");

}

int main(){
  menu();
  while(getchar()!='\n');
  getchar();
  return 0;
}
