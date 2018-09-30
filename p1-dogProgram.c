#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib/structures.h"
#include "lib/generators.h"

void generarEstructuras(hash_table_node *t_hash, long cantidad)
{
    long i;
    int l;

    dogType *randmascota;
    srand48(time(NULL)); //Semilla

    //Genera 'cantidad' estructuras
    for(i = 0; i < cantidad; i++)
    {
        randmascota = (dogType*) calloc(1,sizeof(dogType));

        if(randmascota == NULL){
          printf("malloc error en generarEstructuras");
          exit(-1);
        }

        //asigna valores aleatorios
        name_generator(randmascota->nombre);
		strcpy(randmascota->tipo,generarRandomString(32));
        randmascota->edad = (int)(drand48()*15);
        strcpy(randmascota->raza,generarRandomString(16));
        randmascota->estatura = (int)(drand48()*160);
        randmascota->peso = (float)(drand48()*20+40);
        randmascota->sexo = (drand48()>0.5)?'M':'F';


        if(add_data(t_hash,randmascota))//añade el registro a la tabla hash
        {
            printf("Registro Guardado\n");
        }else{
            printf("Ocurrio Un Problema al Guardar el Registro\n");
        }

        //imprimirMascota(randmascota);
    }
    free(randmascota);
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

void _pause()
{
  while(getchar()!='\n');
  getchar();
}

void get_info(const char *prompt, const char *format, void *ptr)
{
    printf("%s",prompt);
    scanf(format, ptr);
    printf("\n");
}

void ingresar_registro(hash_table_node* t_hash)
{

    dogType *new_record;

    new_record = (dogType*) calloc(1,sizeof(dogType));
    if(new_record == NULL)
    {
        perror("error al crear la nueva mascota en ingresar_registro");
        exit(-1);
    }

    get_info("Ingrese el Nombre: "," %s",new_record->nombre);
    while(strlen(new_record->nombre)>32) get_info("Muy Largo!! Ingrese el Nombre de nuevo: "," %s",new_record->nombre);
    get_info("Ingrese el Tipo: "," %s",new_record->tipo);
    while(strlen(new_record->tipo)>32) get_info("Muy Largo!! Ingrese el Tipo de nuevo: "," %s",new_record-> tipo);
    get_info("Ingrese la Edad: "," %i",&new_record->edad);
    get_info("Ingrese la Raza: "," %s",new_record -> raza);
    while(strlen(new_record->raza)>32) get_info("Muy Largo!! Ingrese la Raza de nuevo: "," %s",new_record->raza);
    get_info("Ingrese la Estatura: "," %i",&new_record->estatura);
    get_info("Ingrese el Peso: "," %f", &new_record->peso);
    get_info("Ingrese el Sexo (H o M): "," %c",&new_record->sexo);
    while(new_record->sexo != 'M' && new_record->sexo !='H') get_info("Entrada Invalida! Ingrese el sexo de nuevo: "," %c",&new_record->sexo);


   if(add_data(t_hash,new_record))
   {
    printf("Registro Guardado\n");
	imprimirMascota(new_record);
   }else{
        printf("Ocurrio Un Problema al Guardar el Registro\n");
   }
}

void buscar_registro(hash_table_node* t_hash)
{
	char name[32];
	int num_reg;
	get_info("Nombre de la mascota: "," %s", name);

	get_info("\nNúmero del Registro: "," %i",&num_reg);

	imprimirMascota(get_data(t_hash,name,num_reg));
}

void menu()
{
	int option;
    hash_table_node *t_hash;
	printf("%p\n", t_hash);
    t_hash = open_hash_table();
	printf("%p\n", t_hash);
    if(t_hash == NULL)
    {
        perror("No se pudo crear la tabla hash en menu");
        exit(-1);
    }
	generarEstructuras(t_hash,10e+7);
    mostrar_menu();
    get_info("Ingrese la opcion: ", " %i", &option);

    while(option != 5)
    {
        switch(option)
        {
            case 1:
                printf("Aqui se Ingresa un Registro\n");

                ingresar_registro(t_hash);
                break;
            case 2:
            printf("Aqui se Ve un Registro\n");
                break;
            case 3:
            printf("Aqui se Borra un Registro\n");

                break;
            case 4:
                printf("Aqui se Busca un Registro\n");
                buscar_registro(t_hash);
                break;
            default:
                break;
        }
        _pause();
        mostrar_menu();
        get_info("Ingrese la opcion: ", " %i", &option);
    }
	close_hash_table(t_hash);
    printf("Adios :'v\n");
    free(t_hash);
}

int main()
{
  menu();
  _pause();
  return 0;
}
