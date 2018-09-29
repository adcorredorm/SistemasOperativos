#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/structures.h"


hash_table_node *new_HT()
{
	hash_table_node *new_ht;
	new_ht = (hash_table_node*) malloc(sizeof(hash_table_node)*HASH_TABLE_SIZE);
	if(new_ht != NULL)
	{
		for (int i = 0; i < HASH_TABLE_SIZE; ++i)
		{
			new_ht[i].structures_list = (data_item*) malloc(sizeof(data_item)*100);
			new_ht[i].size = 0;
			new_ht[i].max_size = 100;
		}
	}
	return new_ht;
}

short add_data(hash_table_node *t_hash, dogType *record)
{

	if(t_hash == NULL || record == NULL)
	{
		return 0;//retorne False
	}

	int index;
	hash_table_node *entrada;
	data_item *new_data;

	index = (int) (hash_code((unsigned char*)record->nombre)%HASH_TABLE_SIZE);//calculo del la posición de la entrada en la hash table
	entrada = &t_hash[index];

	//Si la lista de estructuras de la entrada está llena, expandirla al doble del máximo tamaño
	if(entrada->size == entrada->max_size)
	{
		entrada->max_size *= 2;
		entrada->structures_list = (data_item*) realloc(entrada->structures_list,sizeof(data_item)*entrada->max_size);
		if (entrada->structures_list == NULL)
		{
			perror("Error al ampliar la lista de una entrada en add_data_item");
			exit(-1);
		}
	}
	//Aumenta el tamaño de la lista de la entrada y el número de datos totales en la tabla una unidad


	//Se asignan los datos y la clave
	new_data = &entrada->structures_list[entrada->size];
	new_data -> data = record;

	int l = strlen(record->nombre);
	for(int i = 0; i< l;i++) new_data->key[i]=record->nombre[i];
	entrada->size++;
	NUM_RECORDS++;
	return 1;//Retorne True, todo salio bien.
}

// algoritmo djb2 hecho por dan bernstein, funcion original llamada "hash"
unsigned long hash_code(unsigned char *str)
{
	for(int i = 0;i<5;++i)
    {
        if (str[i]>='A' && str[i]<='Z') {
            str[i]+= 32;
        }
    }
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

dogType *get_data(hash_table_node *t_hash, char key[], int num_reg)
{
	if(t_hash == NULL || key == NULL || num_reg == 0) return NULL;

	int index;
	hash_table_node *entrada;
	dogType *record;

 	index = (int) (hash_code((unsigned char*) key)%HASH_TABLE_SIZE);//claculo del hash
	entrada = &t_hash[index];// apuntamos a la entrada donde está la lista que contiene al dogType

	//hacemos una busqueda lineal
	for (int i = 0; i < entrada->size; ++i)
	{
		record = entrada->structures_list[i].data;
		if(strcmp(record->nombre,key) == 0 && record->record_number == num_reg)
		{
			return record;
		}
	}
	return NULL;
}

dogType del_data(char *key)
{

}

void imprimirMascota(void *p)
{
    dogType *pointer;
    pointer = p;

    printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
        pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}
