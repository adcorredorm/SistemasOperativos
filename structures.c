#include <stdlib.h>
#include <stdio.h>
#include "lib/structures.h"

int NUM_RECORDS = 0;

dogType *make_dogType(char *nombre,char *tipo, int edad, char *raza, int estatura, float peso, char sexo, int record_number)
{
	dogType *new_record;
	new_record = (dogType*) malloc(sizeof(dogType));
	if(new_record != NULL)
	{
		new_record->nombre = nombre;
		new_record->tipo = tipo;
		new_record->edad = edad;
		new_record->raza = raza;
		new_record->estatura = estatura;
		new_record->peso = peso;
		new_record->sexo = sexo;
		new_record->record_number = record_number;
	}
	return new_record;
}

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

short add_data_item(hash_table_node *hash, char *key, dogType *record)
{
	if(key == NULL || hash == NULL || record == NULL)
	{
		return 0;//retorne Falso
	}
	int index = (int) (hash_code((unsigned char*)key)%HASH_TABLE_SIZE);//calculo del la posición de la entrada en la hash table
	hash_table_node *entrada = &hash[index];

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
	entrada->size += 1;
	NUM_RECORDS++;

	//Se asignan los datos y la clave
	data_item *new_data = &entrada->structures_list[entrada->size];
	new_data -> data = record;
	new_data -> key = key;

	return 1;
}

// algoritmo djb2 hecho por dan bernstein, funcion original llamada "hash"
unsigned long hash_code(unsigned char *str) 
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

