#ifndef STRUCTURES_H
#define STRUCTURES_H

#define HASH_TABLE_SIZE 1000000
#define DATA_PATH "dataDogs.dat"
#define TEMP_PATH "dataDogs.temp"

#define PORT 80

int hash_list[HASH_TABLE_SIZE], last_id;

typedef struct{
    int id;
    char nombre[32];
    char tipo[32];
    int edad;
    char raza[16];
    int estatura;
    float peso;
    char sexo; // H/M
} dogType;

void reiniciar_hash();
unsigned long hash_value(char *str);
void lower_case(char *str);
dogType* crear_registro();
void imprimirMascota(void *p);

#endif
