#include<stdlib.h>
#include"lib/generators.h"

char *generarRandomString(int size){
	char *string = (char*) malloc(sizeof(char)*size);
	int j, aux = (int)(drand48()*size-3)+3;

	for(j = 0; j < aux; j++){
		string[j] = 'a' + (int)(drand48()*26);
	}

	while(j < size){
		string[j] = '\0';
		j++;
	}
	return string;
}

double rand_num(double amp)
{
	return drand48()*amp;
}