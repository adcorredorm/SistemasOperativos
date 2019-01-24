#ifndef STRUCTURES_H
#define STRUCTURES_H

#define HASH_TABLE_SIZE 1000000
#define DATA_PATH "dataDogs.dat"
#define TEMP_PATH "dataDogs.temp"

int hash_list[HASH_TABLE_SIZE], last_id, NUM_DATA;

typedef struct{
    int id,next,prev;
    int edad;    
    int estatura;
    float peso;
    char nombre[32];
    char tipo[32];
    char raza[16];
    char sexo; // H/M
} dogType;

void iniciar_hash();
unsigned long hash_value(char *str);
void lower_case(char *str);
dogType* crear_registro();
void imprimirMascota(void *p);
int read_pet(int pos, dogType* pet, FILE* file);
int write_pet(int pos, dogType* pet, FILE* file);
dogType* eliminar(int num_registro);
int buscar(char nombre[]);

#endif
