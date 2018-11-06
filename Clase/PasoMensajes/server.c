#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<strings.h>
#include <arpa/inet.h>

#define PORT 3536
#define BACKLOG 8
#define LEN 13
#define BUFFLEN 32

int main(void) {

  int serverfd = socket(AF_INET, SOCK_STREAM, 0)
    ,clientfd;
  socklen_t csize;
  char buffer[BUFFLEN];
  struct sockaddr_in server, client;

  if(serverfd == -1){
    perror("socket creation failed");
    exit(-1);
  }

  bzero(server.sin_zero, 8);

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;

  int ok = bind(serverfd, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
  if(ok == -1){
    perror("Failed to bind the socket to a local address");
    exit(-1);
  }

  ok = listen(serverfd, BACKLOG);
  if(ok == -1){
    perror("listen error");
    exit(-1);
  }

  csize = 0;
  //do-while, fork (for multiple connections)
  clientfd = accept(serverfd, (struct sockaddr *) &client, (socklen_t *)&csize);
  if(clientfd == -1){
    perror("accept error");
    exit(-1);
  }

  ok = send(clientfd, "hello client\0", LEN, 0);
  if(ok == -1){
    perror("send error");
    exit(-1);
  }

  ok = recv(clientfd, buffer, LEN, 0);
  if(ok == -1){
    perror("recv error");
    exit(-1);
  }

  printf("%s\n", buffer);

  close(clientfd);
  close(serverfd);
  return 0;
}
