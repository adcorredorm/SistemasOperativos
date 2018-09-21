#include <stdio.h>

#define SIZE 1000

typedef struct dogType
{
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	double peso;
	char sexo;
}dogType;


typedef struct H_T_item
{
		dogType data;
		char key[32];
}data_item;