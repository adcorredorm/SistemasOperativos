#ifndef GENERATORSH
#define GENERATORSH

#include "structures.h"

#define NAMES_PATH "nombresMascotas.txt"
#define REG_PATH "registered.dat"

char *generarRandomString(int size);
short name_generator(void *ptr);
void generarEstructuras(hash_table_node *t_hash, long cantidad);

#endif
