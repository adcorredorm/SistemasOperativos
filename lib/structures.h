#ifndef STRUCTURESH_INCLUDED
#define STRUCTURESH_INCLUDED

#define HASH_TABLE_SIZE 1009
int NUM_RECORDS; //Cantidad de Registros presentes en la tabla hash

typedef struct
{
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo;
	int record_number;
}dogType;

typedef struct
{
	dogType *data;
	char key[32];
}data_item;

typedef struct
{
	data_item *structures_list;
	int size;
	int max_size;
}hash_table_node;

unsigned long hash_code(unsigned char *key);//Implementado

short add_data(hash_table_node *table, dogType *record);//Implementado
dogType del_data(char *key,int num_reg);
dogType *get_data(hash_table_node *t_hash, char *key, int num_reg);
hash_table_node *new_HT();//Implementado
void imprimirMascota(void *p);


#endif
