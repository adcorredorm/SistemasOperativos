#ifndef HASH_H
#define HASH_H

#define HASH_TABLE_SIZE 1000000
#define DATA_PATH "dataDogs.dat"

int hash_list[HASH_TABLE_SIZE], last_id;

void reiniciar_hash();
unsigned long hash_value(char *str);
void lower_case(char *str);

#endif
