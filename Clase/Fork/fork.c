#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  pid_t pid;

  printf("(%i)Parent process\n", getpid());

  pid = fork();

  if(pid < 0){
    perror("fork error");
  }

  if(pid == 0) printf("(%i)Nooooo!(%i)\n", getpid(), getppid());
  else printf("(%i)I'm your father\n", getpid());

  return 0;
}
