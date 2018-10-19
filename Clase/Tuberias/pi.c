#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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


  int ok, pipefd[2];
  pid_t pid;
  double pi;

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

    close(pipefd[0]);

    pi = pi_calc(1.0);

    printf("Hijo : %1.20f\n", pi);

    ok = write(pipefd[1], &pi, sizeof(double));
    if(ok == -1){
      printf("write error");
      exit(-1);
    }
    close(pipefd[1]);


  }else{

      double pi2;

      pi = pi_calc(2.0*REPEAT + 1);

      close(pipefd[1]);

      ok = read(pipefd[0], &pi2, sizeof(double));
      if(ok == -1){
        printf("read error");
        exit(-1);
      }

       printf("Padre: %1.20f\n", pi + pi2);
  }


  return 0;
}
