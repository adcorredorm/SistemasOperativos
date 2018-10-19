#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
  pid_t pid;
  int pipefd[2], ok;
  char buffer[32];

  ok = pipe(pipefd);
  if(ok == -1){
    printf("pipe error");
    exit(-1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork error");
    exit(-1);
  }

  if(pid == 0){
      close(pipefd[0]); //Child can't read

      ok = write(pipefd[1], "hola", 4);
      if(ok == -1){
        printf("write error");
        exit(-1);
      }
      close(pipefd[1]);

  }else{
    close(pipefd[1]); //Parent can't write

    ok = read(pipefd[0], buffer, 4);  //read is a blocking function
    if(ok == -1){
      printf("read error");
      exit(-1);
    }
    buffer[ok] = '\0'; //Indicates the end of buffer
    close(pipefd[0]);

    printf("%s\n", buffer);

  }

}
