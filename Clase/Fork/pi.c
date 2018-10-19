#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define REPEAT 1.0e+9

double pi_calc(double divisor){
  double pi = 0.0, i;
  int sign = 1;

  for(i = 0.0; i < REPEAT; i++){
    pi += sign * 4.0/divisor;
    divisor += 2.0;
    sign *= -1;
  }
  return pi;
}

int main(){

  FILE *file;
  int ok;
  pid_t pid;
  double pi;

  remove("pi.dat");

  pid = fork();

  if(pid < 0){
    printf("fork error");
    exit(-1);
  }

  if(pid == 0){

    pi = pi_calc(1.0);

    printf("Hijo : %1.20f\n", pi);

    file = fopen("pi.dat", "w+");
    if(file == NULL){
      printf("fopen error (creation)");
      exit(-1);
    }

    ok = fwrite(&pi, sizeof(double), 1, file);
    if(ok == 0){
      printf("fwrite error");
      exit(-1);
    }

  }else{

      double pi2;

      pi2 = pi_calc(2.0*REPEAT + 1);

      do{
          file = fopen("pi.dat", "r");
       }while (file == NULL);

       ok = fread(&pi, sizeof(double), 1, file);
       if(ok < 1){
         printf("fread error");
         exit(-1);
       }

       printf("Padre: %1.20f\n", pi+pi2);
  }

  ok = fclose(file);
  if(ok == EOF){
    printf("fclose error");
    exit(-1);
  }

  return 0;
}
