#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

#define HILOS 8
#define SIMULTANEOS 1

sem_t *sem;

void *fuction(){
  printf("Inicio\n");
  sem_wait(sem);
  sleep(1);
  printf("Fin\n");
  sem_post(sem);
}

int main(){

  sem = sem_open("name2", O_CREAT, 0777, SIMULTANEOS);

  pthread_t thread[HILOS];
  int ok;
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

  sem_close(sem);
  sem_unlink("name2");
  return 0;
}
