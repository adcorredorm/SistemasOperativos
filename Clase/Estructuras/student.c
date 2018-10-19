#include <stdio.h>
#include <stdlib.h>

struct student{
  char name[32];
  int age;
  char genre;
  float height;
};

/*
 * To print in console *stream must be "stdout"
*/
void printStudent(void *p, FILE *stream){
  struct student *pointer;
  pointer = p;

  fprintf(stream, "Name: %s\nAge: %i\nGenre: %c\nHeight: %f",
      pointer->name, pointer->age, pointer->genre, pointer->height);
}

void loadStudent(void *p){
  struct student *pointer;
  pointer = p;
  printf("Name: ");
  scanf(" %s", pointer->name);
  printf("Age: ");
  scanf(" %i", &pointer->age);
  printf("Genre (M/F): ");
  scanf(" %c", &pointer->genre);
  printf("Height: ");
  scanf(" %f", &pointer->height);

  //printStudent(pointer, stdout);
}

int main(){
  FILE *data;
  int r;

  struct student *studentp;
  studentp = malloc(sizeof(struct student));
  if(studentp == NULL){
    printf("malloc error");
    exit(-1);
  }

  data = fopen("data.dat", "r");
  if(data == NULL){
    printf("fopen error");
    exit(-1);
  }

  r = fread(studentp, sizeof(struct student), 1, data);
  if(r == 0){
    printf("fread error");
    exit(-1);
  }

  r = fclose(data);
  if(r == EOF){
    printf("fclose error");
    exit(-1);
  }

  printStudent(studentp, stdout);

  free(studentp);

  return 0;
}

int main2(){
  FILE *data;
  int r;

  struct student *studentp;
  studentp = malloc(sizeof(struct student));
  if(studentp == NULL){
    printf("malloc error");
    exit(-1);
  }
  loadStudent(studentp);
  data = fopen("data.dat", "w+");
  if(data == NULL){
    printf("fopen error");
    exit(-1);
  }


  r = fwrite(studentp, sizeof(struct student), 1, data);
  if(r == 0){
    printf("fwrite error");
    exit(-1);
  }

  r = fclose(data);
  if(r == EOF){
    printf("fclose error");
    exit(-1);
  }

  free(studentp);

  return 0;
}
