#ifndef STRUCTURESH_INCLUDED
#define STRUCTURESH_INCLUDED

#define HASH_TABLE_SIZE 1009
#define DATA_PATH "dataDog.dat"
#define HASH_TABLE_PATH "hash_table.bin"

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
	long num_reg;
}dogType;

typedef struct
{
	long data_pos;
	char key[32];
}data_item;

typedef struct
{
	data_item *structures_list;
	int size;
	int max_size;
}hash_table_node;

unsigned long hash_code(unsigned char *key);

short add_data(hash_table_node *table, dogType *record);
dogType *del_data(hash_table_node *t_hash, char *key,int num_reg);
dogType *get_data(hash_table_node *t_hash, char *key, int num_reg);
hash_table_node *open_hash_table();
void close_hash_table(hash_table_node* t_hash);
void imprimirMascota(void *p);


#endif
