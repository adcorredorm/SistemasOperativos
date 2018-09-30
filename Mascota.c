#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

typedef struct{
  char nombre[32];
  char tipo[32];
  int edad;
  char raza[16];
  int estatura;
  double peso;
  char sexo; // M/F
} dogType;

void imprimirMascota(void *p){
  dogType *pointer;
  pointer = p;

  printf("Nombre: %s\nTipo: %s\nEdad: %i años\nRaza: %s\nEstatura: %i cm\nPeso: %f kg\nSexo: %c\n\n",
      pointer->nombre, pointer->tipo, pointer->edad, pointer->raza, pointer->estatura, pointer->peso, pointer->sexo);
}

int longitud(char *str){
  //Retorna la longitud de una cadena char*
	int j = 0;
	while(str[j] != '\0') j++;
	return j;
}

void lower_case(char *str){
  //Convierte una cadena a letras minusculas
	for(int i = 0; i < longitud(str); i++){
    str[i] = tolower(str[i]);
  }
}

unsigned long hash_value(char *str){
  //Retorna el valor hash de una cadena
	lower_case(str);
	unsigned long hash = 0;
	for(int j = 0 ; j < longitud(str); j++){
		hash = hash*33 + str[j]%33 + j*33 - (long)str[j]*13;
	}
	return hash%1000000;
}

void reiniciar_hash(int hashlist[]){
  //reinicia el registro de la tabla para incluir registros
  for(int j = 0; j < 1000000; j++) hashlist[j] = 0;

  FILE *file;
	int i = 0, ok;
  dogType *mascota;
  mascota = malloc(sizeof(dogType));
  if(mascota == NULL){
    printf("malloc error");
    exit(-1);
  }
	file = fopen("dataDogs.dat", "rb");
  if(file == NULL){
    printf("Error al abrir dataDogs.dat");
    exit(-1);
  }
	while(fread(mascota, sizeof(dogType), 1, file) == 1){
    //Almacena en la tabla la posicion del primer registro al que corresponde un valor de hash determinado
	  if(hashlist[hash_value(mascota->nombre)] == 0) hashlist[hash_value(mascota->nombre)] = i;
    i ++;
	}
	fclose(file);
}

void insertar_registro(dogType *mascota){

  FILE *file, *temp;
  file = fopen("dataDogs.dat", "rb+");
  if(file == NULL){
    printf("Error al abrir dataDogs.dat");
    exit(-1);
  }
  temp = fopen("dataDogs.temp", "wb+");
  if(file == NULL){
    printf("Error al crear dataDogs.temp");
    exit(-1);
  }

  int ok;
  dogType *newPet;
  newPet = malloc(sizeof(dogType));
  if(mascota == NULL){
    printf("Error al insertar el nuevo registro");
    exit(-1);
  }


  while(fread(newPet, sizeof(dogType), 1, file) > 0){
    if(hash_value(mascota->nombre) <= hash_value(newPet->nombre)){
      //Se ordenan los registros segun su valor hash en forma FIFO
      //Se copia en un archivo temporal la primera parte de los registros (antes del que va a ser insertado)
      ok = fwrite(newPet, sizeof(dogType), 1, temp);
      if(ok == 0){
        printf("Error al escribir en dataDogs.temp");
        remove("dataDogs.temp");
        exit(-1);
      }
    }else{
      ok = fwrite(mascota, sizeof(dogType), 1, temp);
      if(ok == 0){
        printf("Error al escribir en dataDogs.temp");
        remove("dataDogs.temp");
        exit(-1);
      }
      break;
    }
  }

  //Se copian los datos restantes
  while(fread(newPet, sizeof(dogType), 1, file) > 0){
    ok = fwrite(newPet, sizeof(dogType), 1, temp);
    if(ok == 0){
      printf("Error al escribir en dataDogs.temp");
      remove("dataDogs.temp");
      exit(-1);
    }
  }

	fclose(file);
	fclose(temp);
  free(newPet);

  //Se elimina el archivo original
	remove("dataDogs.dat");
  //Se renombra el temporal
  rename("dataDogs.temp","dataDogs.dat");
}

dogType* crear_registro(){
  dogType *mascota;
  mascota = malloc(sizeof(dogType));
  if(mascota == NULL){
    printf("Error al crear el registro");
    exit(-1);
  }

  system("clear");
  printf("Bienvenido el menú para registrar una nueva mascota.\nPorfavor ingrese los datos solicitados a continuación:\n\n");
	printf("Ingrese el nombre de la mascota\n");
	scanf(" %s", mascota->nombre);
	printf("Ingrese el tipo de la mascota\n");
	scanf(" %s", mascota->tipo);
	printf("Ingrese la edad de la mascota\n");
	scanf(" %d", &mascota->edad);
	printf("Ingrese la raza de la mascota\n");
	scanf(" %s", mascota->raza);
	printf("Ingrese la estatura de la mascota\n");
	scanf(" %d", &mascota->estatura);
	printf("Ingrese el peso de la mascota\n");
	scanf(" %f", &mascota->peso);
  printf("Ingrese el género de la mascota\n");
  scanf(" %c", &mascota->sexo);
  while(mascota->sexo != 'M' && mascota->sexo != 'F'){
    printf("El género de la mascota debe ser M para Masculino o F para Femenino\nIngrese nuevamente el género de la mascota");
    scanf(" %c", &mascota->sexo);
  }

  return mascota;
}



void mostrar_menu(){
  system("clear");
	printf("Menu:\n");
	printf("1. Ingresar Registro\n");
	printf("2. Ver Registro\n");
	printf("3. Borrar Registro\n");
	printf("4. Buscar Registro\n");
	printf("5. Salir\n");
}

int main(){

  int opcion, hashlist[1000000];

	reiniciar_hash(hashlist);

  do{
    mostrar_menu();

    printf("Ingrese la opcion: \n");
  	scanf(" %i", &opcion);
    switch(opcion){
			case 1:
				//printf("Aqui se Ingresa un Registro\n");
        dogType *nueva_mascota = crear_registro();
        insertar_registro(nueva_mascota);
        reiniciar_hash(hashlist);

			break;

			case 2:
				printf("Aqui se Ve un Registro\n");

			break;

			case 3:
				printf("Aqui se Borra un Registro\n");

			break;

			case 4:
				printf("Aqui se Busca un Registro\n");

			break;

      case 5:
  			printf("Fin de la aplicacion\n");
  		break;

			default:
        printf("Opcion invalida\n");

			break;
    }
    if(opcion != 5){
        printf("Presione enter para volver al menu\n");
        while(getchar() != '\n');
    }
    getchar();

  }while(opcion != 5);

  return 0;
}
