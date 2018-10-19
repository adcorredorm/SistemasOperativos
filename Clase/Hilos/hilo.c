#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define HILOS 8

void *funcion(int *i){
  printf("Hola, soy el hilo #%i\n", *i);
  return 0;
}

int main(){
  pthread_t thread[HILOS];
  int i_value[HILOS];
  int ok;
  for(int i = 0; i < HILOS; i++){
    i_value[i] = i;
    ok = pthread_create(&thread[i], NULL, (void *)funcion, (void*)&i_value[i]);
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


  return 0;
}
