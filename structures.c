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
			new_ht[i].structures_list = (data_item*) calloc(100,sizeof(data_item));
			new_ht[i].size = 0;
			new_ht[i].max_size = 100;
		}
	}
	return new_ht;
}

hash_table_node *open_hash_table()
{
	FILE *f;
	int ok;

	f = fopen(HASH_TABLE_PATH,"r");
	if(f == NULL)
		return new_HT();

	hash_table_node *t_hash = (hash_table_node*) calloc(HASH_TABLE_SIZE,sizeof(hash_table_node));
	if(t_hash == NULL) {perror("no se pudo crear la tabla hash en open_hash_table\n");exit(-1);}

	for (int i = 0; i < HASH_TABLE_SIZE; i++) {

		ok = fread(&t_hash[i].size,sizeof(int),1,f);
		if(ok != 1) {printf("no se pudo leer el tamaño de la entrada en open_hash_table\n");exit(-1);}
		//printf("entrada %i size :%i ",i,t_hash[i].size );
		ok = fread(&t_hash[i].max_size,sizeof(int),1,f);
		if(ok != 1) {printf("no se pudo leer el tamaño máximo de la entrada en open_hash_table\n");exit(-1);}
		//printf("max_size %i ", t_hash[i].max_size);


		t_hash[i].structures_list = (data_item*) calloc(t_hash[i].max_size,sizeof(data_item));
		ok = fread(t_hash[i].structures_list, sizeof(data_item),t_hash[i].size,f);
		if(ok != t_hash[i].size) {printf("no se pudo leer la lista de la entrada en open_hash_table\n");exit(-1);}
		//printf("read %i\n",ok);
	}

	fclose(f);

	return t_hash;
}

void close_hash_table(hash_table_node* t_hash)
{
	if(t_hash == NULL) return;
	FILE *f;
	int ok;

	f = fopen(HASH_TABLE_PATH,"w+");
	if(f == NULL)
	{
		printf("El archivo %s no se pudo abrir en close_hash_table\n", HASH_TABLE_PATH);
		exit(-1);
	}
	for (int i = 0; i < HASH_TABLE_SIZE; i++)
	{
		ok = fwrite(&t_hash[i].size,sizeof(int),1,f);
		if(ok!=1){printf("El tamaño de la entrada no se pudo escribir\n"); exit(-1);}
		ok = fwrite(&t_hash[i].max_size,sizeof(int),1,f);
		if(ok!=1){printf("El tamaño maximo de la entrada no se pudo escribir\n"); exit(-1);}
		ok = fwrite(t_hash[i].structures_list,sizeof(data_item),t_hash[i].size,f);
		if(ok!=t_hash[i].size){printf("La lista de la entrada no se pudo escribir\n"); exit(-1);}
	}
	fclose(f);
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

short add_data(hash_table_node *t_hash, dogType *record)
{

	if(t_hash == NULL || record == NULL)
	{
		return 0;//retorne False
	}
	FILE *f;
	int index,ok;
	hash_table_node *entrada;
	data_item *data_ptr;

	index = (int) (hash_code((unsigned char*)record->nombre)%HASH_TABLE_SIZE);//calculo del la posición de la entrada en la hash table
	entrada = &t_hash[index];

	//printf("%i %zi\n", entrada->size, sizeof(entrada->size));
	//Si la lista de estructuras de la entrada está llena, expandirla al doble del máximo tamaño
	if(entrada->size == entrada->max_size)
	{
		printf("Ampliada la entrada %i a %i\n", index,entrada->size*2);
		entrada->max_size *= 2;
		entrada->structures_list = (data_item*) realloc(entrada->structures_list,sizeof(data_item)*entrada->max_size);
		if (entrada->structures_list == NULL)
		{
			perror("Error al ampliar la lista de una entrada en add_data_item");
			exit(-1);
		}
	}


	entrada->size++;
	//Se asignan los datos y la clave
	for (int i = 0; i < entrada->size; i++) {

		data_ptr = &entrada->structures_list[i];
		if(data_ptr->key == NULL)break;
	}

	f = fopen(DATA_PATH,"r");
	if(f == NULL) f = fopen(DATA_PATH,"w+");
	if(f == NULL) {printf("Error en add_data al abrir el archivo %s\n",DATA_PATH);exit(-1);}
	f = freopen(NULL,"r+",f);
	if(f == NULL) {printf("Error en add_data al abrir el archivo %s\n",DATA_PATH);exit(-1);}

	if(data_ptr->data_pos == 0)
	{
		ok = fseek(f,0,SEEK_END);
		if(ok == -1){printf("error en add_data->fseek\n");exit(-1);}
		long pos = ftell(f);
		record->num_reg = pos/sizeof(dogType)+1;
		ok = fwrite(record, sizeof(dogType),1,f);
		if(ok == 0){printf("error en add_data->fwrite\n");exit(-1);}
		data_ptr->data_pos = pos;
	}else{
		ok = fseek(f,data_ptr->data_pos-1,SEEK_SET);
		if(ok == -1){printf("error en add_data->fseek\n");exit(-1);}
		record->num_reg = data_ptr->data_pos/sizeof(dogType)+1;
		ok = fwrite(record, sizeof(dogType),1,f);
		if(ok == 0){printf("error en add_data->fwrite\n");exit(-1);}
	}
	int l = strlen(record->nombre);
	for(int i = 0; i< l;i++) data_ptr->key[i]=record->nombre[i];
	fclose(f);
	NUM_RECORDS++;
	//printf("%i\n", NUM_RECORDS);
	return 1;//Retorne True, todo salio bien.
}

dogType *get_data(hash_table_node *t_hash, char key[], int num_reg)
{
	if(t_hash == NULL || key == NULL || num_reg == 0) return NULL;

	int index,ok;long pos;
	hash_table_node *entrada;
	data_item *data_ptr;
	dogType *record;
	FILE *f;

 	index = (int) (hash_code((unsigned char*) key)%HASH_TABLE_SIZE);//claculo del hash
	entrada = &t_hash[index];// apuntamos a la entrada donde está la lista que contiene al dogType
	record = (dogType*)calloc(1,sizeof(dogType));

	//hacemos una busqueda lineal
	ok = 0;
	for (int i = 0; i < entrada->size; ++i)
	{
		//printf("entrada: %i  size: %i  max_size: %i\n", i, t_hash[i].size, t_hash[i].max_size);
		data_ptr = &entrada->structures_list[i];

		if(strcmp(data_ptr->key,key) == 0 && data_ptr->data_pos == (num_reg-1)*sizeof(dogType))
		{
			//printf("entrada %i key %s data_pos %li\n",index,data_ptr->key,data_ptr->data_pos);
			ok = 1;
			break;
		}
	}

	if(!ok) return NULL;

	f = fopen(DATA_PATH,"r");
	if(f == NULL) {printf("Error en get_data al abrir el archivo %s\n",DATA_PATH);exit(-1);}

	fseek(f,data_ptr->data_pos,SEEK_SET);
	fread(record,sizeof(dogType),1,f);
	fclose(f);

	return record;
}

dogType *del_data(hash_table_node *t_hash,char *key,int num_reg)
{
	if(t_hash == NULL || key == NULL || num_reg == 0) return NULL;

	int index,ok,l;long pos;
	hash_table_node *entrada;
	data_item *data_ptr;
	dogType *record;
	FILE *f;

 	index = (int) (hash_code((unsigned char*) key)%HASH_TABLE_SIZE);//claculo del hash
	entrada = &t_hash[index];// apuntamos a la entrada donde está la lista que contiene al dogType
	record = (dogType*)calloc(1,sizeof(dogType));

	//hacemos una busqueda lineal
	ok = 0;
	for (int i = 0; i < entrada->size; ++i)
	{
		data_ptr = &entrada->structures_list[i];
		if(strcmp(data_ptr->key,key) == 0 && data_ptr->data_pos == num_reg)
		{
			ok = 1;
			break;
		}
	}

	if(!ok) return NULL;

	f = fopen(DATA_PATH,"r");
	if(f == NULL) {printf("Error en del_data al abrir el archivo %s\n",DATA_PATH);exit(-1);}

	fseek(f,data_ptr->data_pos,SEEK_SET);
	fread(record,sizeof(dogType),1,f);
	fclose(f);
	l = strlen(data_ptr->key);
	for(int i=0;i < l; ++i) data_ptr->key[i] = '\0';
	NUM_RECORDS--;
	return record;
}

void imprimirMascota(void *p)
{
    dogType *pointer;
    pointer = p;

    printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %1.2f kg\nSexo: %c\nNúmero de Registro: %li\n",
        pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo,pointer->num_reg);
}
