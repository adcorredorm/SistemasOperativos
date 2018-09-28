#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "lib/structures.h"
#include "lib/generators.h"


/*void generarEstructuras(int cantidad){
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

}*/

void mostrar_menu()
{
	printf("Menu:\n");
	printf("1. Ingresar Registro\n");
	printf("2. Ver Registro\n");
	printf("3. Borrar Registro\n");
	printf("4. Buscar Registro\n");
	printf("5. Salir\n");
}

void ingresar_mascota(hash_table_node *tabla){
  dogType *new_record, *perro;
	new_record = (dogType*) malloc(sizeof(dogType));
	if(new_record == NULL){
    printf("No se pudo crear el registro\n");
    return ;
	}
  printf("Ingrese Nombre\n");
  scanf(" %s", new_record->nombre);
  printf("Ingrese Tipo\n");
  scanf(" %s", new_record->tipo);
  printf("Ingrese Edad\n");
  scanf(" %i", &new_record->edad);
  printf("Ingrese Raza\n");
  scanf(" %s", new_record->raza);
  printf("Ingrese Estatura\n");
  scanf(" %i", &new_record->estatura);
  printf("Ingrese Peso\n");
  scanf(" %f", &new_record->peso);
  printf("Ingrese Sexo\n");
  scanf(" %c", &new_record->sexo);
  printf("Ingrese Record\n");
  scanf(" %i", &new_record->record_number);

  short ok = add_data_item(tabla, new_record);

  if(ok == 0){
    printf("No se pudo guardar el registro\n");
    return ;
  }else{
    printf("Registro creado con exito\n");
    perro = search_data_item(tabla, new_record->nombre, new_record->record_number);
    imprimirMascota(new_record);
    //imprimirMascota(perro);
  }
	//return new_record;
  free(perro);
}

void menu()
{
  hash_table_node *tabla; //Esto hay que leerlo en el archivo
  tabla = new_HT();

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
          ingresar_mascota(tabla);
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
  //names_generator(1000);
  menu();
  while(getchar()!='\n');
  getchar();
  return 0;
}
