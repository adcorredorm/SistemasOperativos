#include<stdlib.h>
#include<stdio.h>
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

short name_generator(char *ptr)
{
	FILE *f;
	int num_line, l,ok;

	num_line = (int) (drand48()*1669);
	f= fopen(NAMES_PATH,"r");
	if(f == NULL)
	{
		printf("Error en name_generator al abrir el archivo\n");
		return 0;
	}
	l=0;

	while (l++<num_line) {
		fscanf(f," %s", ptr);
	}
	fclose(f);
	return 1;
}
