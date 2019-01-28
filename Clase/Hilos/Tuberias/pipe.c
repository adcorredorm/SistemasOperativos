#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#define HILOS 8
#define SIMULTANEOS 1

int pipefd[2];
char buffer[1] = "|";

void *fuction(){
  printf("Inicio\n");
  read(pipefd[0], buffer, sizeof(char));
  sleep(1);
  printf("Fin\n");
  write(pipefd[1], buffer, sizeof(char));
}

int main(){

  int ok = pipe(pipefd);
  if(ok == -1){
    printf("pipe error");
    exit(-1);
  }
  write(pipefd[1], "12345678", sizeof(char)*SIMULTANEOS);
  //En realidad no importa lo que se escriba, solo la cantidad de elementos disponibles para lectura

  pthread_t thread[HILOS];
  for(int i = 0; i < HILOS; i++){
    ok = pthread_create(&thread[i], NULL, (void *)fuction, NULL);
    if(ok < 0){
      perror("");
      exit(-1);
    }
  }

  for(int i = 0; i < HILOS; i++){
    ok = pthread_join(thread[i], NULL);
    if(ok < 0){
      perror("");
      exit(-1);
    }
  }

  close(pipefd[0]);
  close(pipefd[1]);
  return 0;
}
