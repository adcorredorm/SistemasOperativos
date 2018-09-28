#include<stdlib.h>
#include <stdio.h>
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
/*
void names_generator(int cant)
{
	FILE * file;
	file = (FILE *)fopen("names.txt","w+");
	int ok;
	char *name;
	if (file == NULL) {
		perror("names_generator error");
		exit(-1);
	}
	while(cant--)
	{
		name = generarRandomString(31);
		name = *name +"\n";
		ok = fwrite(name,sizeof(char),1,file);
		if (!ok) {
			perror("names_generator -> fwrite error");
			exit(-1);
		}
	}
	fclose(file);
	free(name);
}
*/
