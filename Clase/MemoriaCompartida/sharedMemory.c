#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char const *argv[]) {
  key_t key = 123; //Key is defined by user
  int shmId;
  float *ap;
  int r;

  shmId = shmget(key, sizeof(float), IPC_CREAT|0666);

  if (shmId == -1) {
    perror("Error in shmId");
    exit(-1);
  }

  ap = (float*) shmat(shmId, 0, 0);

  if (*ap == -1) {
    perror("Error in shmat");
    exit(-2);
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("Error in fork()");
    exit(-3);
  } else if(pid == 0){
    *ap = 3.14159265;
  }else{
    while(*ap == 0.0);
    printf("%f\n", *ap);
  }

  r = shmdt(ap);

  if (r == -1) {
    perror("Error in shmdt");
    exit(-4);
  }


  return 0;
}
