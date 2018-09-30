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
	int i = 1, ok;
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
	while(fread(mascota, sizeof(dogType), 1, file) != 0){
    //Almacena en la tabla la posicion del primer registro al que corresponde un valor de hash determinado
	  if(hashlist[hash_value(mascota->nombre)] == 0) hashlist[hash_value(mascota->nombre)] = i;
    i ++;
	}
	fclose(file);

  //for(int i = 0; i < 1000000; i++) if(hashlist[i] != 0) printf("%i ", hashlist[i]);
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

  int flag = 0;

  while(fread(newPet, sizeof(dogType), 1, file) > 0){

    if(strcmp(mascota->nombre, newPet->nombre) > 0){
      //Inserta el nuevo registro al final de los que tengan el mismo nombre
      ok = fwrite(mascota, sizeof(dogType), 1, temp);
      if(ok == 0){
        printf("Error al escribir en dataDogs.temp");
        remove("dataDogs.temp");
        exit(-1);
      }
      flag = 1;
    }

    //Se copia en un archivo temporal la primera parte de los registros (antes del que va a ser insertado)
    ok = fwrite(newPet, sizeof(dogType), 1, temp);
    if(ok == 0){
      printf("Error al escribir en dataDogs.temp");
      remove("dataDogs.temp");
      exit(-1);
    }

    if(flag == 1) break;

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

void ver_registro(){

  system("clear");
	FILE *file;
	int dato, numero_de_registros, eleccion;
	file = fopen("dataDogs.dat","rb");
  if(file == NULL){
    printf("Error al abrir dataDogs.dat");
    exit(-1);
  }

  //Envía el puntero al final del archivo
  fseek(file,0,SEEK_END);
  //Calcula la cantidad de elementos del archivo
	numero_de_registros = ftell(file)/sizeof(dogType);
	printf("El numero de registros es: %d\n", numero_de_registros);

  printf("Ingrese el registro que desea buscar\n");
	scanf(" %i", &dato);
	while(dato > numero_de_registros || dato < 0){
    //Verifica que si exista el registro
		printf("Registro Incorrecto\n");
		printf("por favor ingrese el registro que desea buscar\n");
		scanf("%i", &dato);
	}


	dogType *newPet = malloc(sizeof(dogType));
  //Acomoda el puntero justo al inicio de el registro deseado y lo guarda en una estructura
	fseek(file, (dato - 1) * sizeof(dogType),SEEK_SET);
  fread(newPet, sizeof(dogType), 1, file);
	fclose(file);

  //Se prepara el archivo temporal para modificar el registro
  file = fopen("temp.txt","w+");
  if(file == NULL){
    printf("Error al abrir temp.txt");
    exit(-1);
  }
  fprintf(file, "Nombre: %s\n", newPet->nombre);
	fprintf(file, "Tipo:   %s\n", newPet->tipo);
	fprintf(file, "Edad:   %i\n", newPet->edad);
	fprintf(file, "Raza:   %s\n", newPet->raza);
	fprintf(file, "Tamaño: %i\n", newPet->estatura);
	fprintf(file, "Peso:   %f\n", newPet->peso);
	fprintf(file, "Genero: %c\n", newPet->sexo);
	fclose(file);
	system("gedit temp.txt");

  printf("¿Desea guardar los cambios realizados en el archivo?\n");
	printf("1 si, 0 no\n");
	scanf(" %i", &eleccion);
	if(eleccion == 1){
    //string para almacenar basura
    char trh[32];

    //Se recupera la informacion del archivo temporal
		file = fopen("temp.txt","r");
    if(file == NULL){
      printf("Error al abrir temp.txt");
      exit(-1);
    }
		fscanf(file, "%s %s", trh, newPet->nombre);
		fscanf(file, "%s %s", trh, newPet->tipo);
		fscanf(file, "%s %i", trh, &newPet->edad);
		fscanf(file, "%s %s", trh, newPet->raza);
		fscanf(file, "%s %i", trh, &newPet->estatura);
		fscanf(file, "%s %f", trh, &newPet->peso);
		fscanf(file, "%s %c", trh, &newPet->sexo);
		fclose(file);

    //Se reinserta la informacion recuperada en el registro de datos
    file = fopen("dataDogs.dat", "rb+");
    if(file == NULL){
      printf("Error al abrir temp.txt");
      exit(-1);
    }
    fseek(file, (dato - 1) * sizeof(dogType),SEEK_SET);
    fwrite(newPet, sizeof(dogType), 1, file);
    fclose(file);
	}
  free(newPet);
	remove("temp.txt");
}

void eliminar_registro(){

  system("clear");
  FILE *file, *temp;
	int dato, numero_de_registros, eleccion;
	file = fopen("dataDogs.dat","rb");
  if(file == NULL){
    printf("Error al abrir dataDogs.dat");
    exit(-1);
  }

  //Envía el puntero al final del archivo
  fseek(file,0,SEEK_END);
  //Calcula la cantidad de elementos del archivo
	numero_de_registros = ftell(file)/sizeof(dogType);
	printf("El numero de registros es: %d\n", numero_de_registros);

  printf("Ingrese el registro que desea eliminar\n");
	scanf(" %i", &dato);
	while(dato > numero_de_registros || dato < 0){
    //Verifica que si exista el registro
		printf("Registro Incorrecto\n");
		printf("Por favor ingrese el registro que desea eliminar\n");
		scanf("%i", &dato);
	}

  //Regresa el puntero file al inicio del archivo
  rewind(file);
  temp = fopen("dataDogs.temp", "wb+");
  if(file == NULL){
    printf("Error al crear dataDogs.temp");
    exit(-1);
  }
	int i = 1;
  dogType *newPet = malloc(sizeof(dogType));
  if(newPet == NULL){
    printf("malloc error");
    exit(-1);
  }
	while( fread(newPet, sizeof(dogType), 1, file) != 0 && dato != i){
    //Se copia en un archivo temporal la primera parte de los registros (antes del que va a ser eliminado)
		fwrite(newPet, sizeof(dogType), 1, temp);
		i ++;
	}

  printf("La mascota que será eliminada es %s:\n", newPet->nombre);

  //Se copian los datos restantes
  while(fread(newPet, sizeof(dogType), 1, file) != 0)
    fwrite(newPet, sizeof(dogType), 1, temp);

  free(newPet);
  fclose(file);
	fclose(temp);

  //Se elimina el archivo original
	remove("dataDogs.dat");
  //Se renombra el temporal
  rename("dataDogs.temp", "dataDogs.dat");
}

void buscar_registro(int hashlist[]){
	FILE *file;
	int aux = 0, i;

	char nombre[32];
	printf("Ingrese el nombre que desea buscar\n");
	scanf(" %s", nombre);

  //Se compruba que exista almenos un registro con ese nombre
	i = hashlist[hash_value(nombre)];
	if(i == 0){
		printf("No existe  el registro solicitado\n");
		return;
	}

  file = fopen("dataDogs.dat","rb");
	fseek(file, (i - 1) * sizeof(dogType), SEEK_SET);
	dogType *pet = malloc(sizeof(dogType));
  if(pet == NULL){
      printf("malloc error");
      exit(-1);
  }
	fread(pet, sizeof(dogType), 1, file);
	printf("Los registros que coinciden con el nombre %s son:\n%i\n", nombre, i++);
	/*while(aux == 0 && fread(pet, sizeof(dogType), 1, file) > 0){
    //Busca los registros sucesivos que coincidan con el nombre //TODO: este se puede usar si los generados estan en orden
		if(strcmp(nombre, pet->nombre) == 0){
      printf("%i\n", ++i);
    }
		else aux = 1;
	}*/
  while(fread(pet, sizeof(dogType), 1, file) > 0){
    //Busca los registros sucesivos que coincidan con el nombre
    lower_case(pet->nombre);
		if(strcmp(nombre, pet->nombre) == 0){
      printf("%i\n", i);
    }
		i++;
	}
  free(pet);
	fclose(file);
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

void imprimir_datos(){
  //Funcion solo de test
  FILE *file;
  dogType *pet = malloc(sizeof(dogType));
  file = fopen("dataDogs.dat", "rb");
  while(fread(pet, sizeof(dogType), 1, file) > 0){
    imprimirMascota(pet);
  }
  free(pet);
  fclose(file);
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

        insertar_registro(crear_registro());
        reiniciar_hash(hashlist);

			break;

			case 2:

        ver_registro();
        reiniciar_hash(hashlist);

			break;

			case 3:

        eliminar_registro();
        reiniciar_hash(hashlist);

			break;

			case 4:

        printf("Aqui se Busca un Registro\n");
        buscar_registro(hashlist);

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
