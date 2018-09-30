#include<stdlib.h>
#include<stdio.h>
#include"lib/generators.h"

char *generarRandomString(int size){
	char *string = (char*) malloc(sizeof(char)*size);
	int j, aux = (int)(drand48()*size-3)+3;

	for(j = 0; j < aux; j++){
		string[j] = 'a' + (int)(drand48()*26);
	}

	while(j < size){
		string[j] = '\0';
		j++;
	}
	return string;
}

short name_generator(void *ptr)
{
	FILE *f;
	int num_line, l,ok;

	num_line = (int) (drand48()*1669);
	f= fopen(NAMES_PATH,"r");
	if(f == NULL)
	{
		printf("Error en name_generator al abrir el archivo\n");
		exit(-1);
	}

	l=0;
	while (l++<num_line) {
		fscanf(f," %s", (char*)ptr);
	}
	fclose(f);
	return 1;
}

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
            //printf("Registro Guardado\n");
        }else{
            printf("Ocurrio Un Problema al Guardar el Registro\n");
        }

        //imprimirMascota(randmascota);
    }
    free(randmascota);
}
