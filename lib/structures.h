#ifndef STRUCTURESH_INCLUDED
#define STRUCTURESH_INCLUDED

#define HASH_TABLE_SIZE 1009
int NUM_RECORDS; //Cantidad de Registros presentes en la tabla hash

typedef struct
{
	char *nombre;
	char *tipo;
	int edad;
	char *raza;
	int estatura;
	float peso;
	char sexo;
	int record_number;
}dogType;


typedef struct
{
	dogType *data;
	char *key;
}data_item;


typedef struct
{
	data_item *structures_list;
	int size;
	int max_size;
}hash_table_node;

unsigned long hash_code(unsigned char *key);

dogType *make_dogType(char *nombre,char *tipo, int edad, char *raza, int estatura, float peso, char sexo, int record_number);
hash_table_node *new_HT();
short add_data_item(hash_table_node *hash,char *key, dogType *record);
data_item *search_record(char *key);
short del_record(char *key);

#endif