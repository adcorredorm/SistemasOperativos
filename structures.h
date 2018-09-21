#ifndef STRUCTURESH_INCLUDED
#define STRUCTURESH_INCLUDED

#define HASH_TABLE_SIZE 1000
int NUM_RECORDS = 0; //Cantidad de Registros presentes en la tabla hash

typedef struct
{
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	double peso;
	char sexo;
	int *record_number;
}dogType;


typedef struct
{
		dogType *data;
		char *key;
}data_item;



typedef struct
{
		data_item *structures_list;
		int max_size = 10000;
		int size = 0;
}hash_table;

dogType *make_dogType(char *nombre,char *tipo, int *edad, char *raza, int *estatura, double *peso, char *sexo, int *record_number);
hash_table *new_HT();
int add_record(dogType *record);
dogType *search_record(char *key);
int del_record(char *key);

#endif