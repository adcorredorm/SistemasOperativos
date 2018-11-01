#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#define PORT 3535
#define LEN 32

int main(int argc, char *argv[]) {

  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in client;
  char buffer[LEN];

  if(clientfd == -1){
    perror("socket creation failed");
    exit(-1);
  }

  bzero(client.sin_zero, 8);

  client.sin_family = AF_INET;
  client.sin_port = htons(PORT);
  client.sin_addr.s_addr = inet_addr(argv[1]);

  printf("Trying to connect with %s on port %i \n", argv[1], PORT);

  int ok = connect(clientfd, (struct sockaddr*)&client, sizeof(struct sockaddr_in));

  if(ok == -1){
    perror("Error, connection refused");
    exit(-1);
  }else printf("Connection success\n");

  ok = recv(clientfd, buffer, LEN, 0);
  if(ok == -1){
    perror("recv error");
    exit(-1);
  }

  printf("%s\n", buffer);

  ok = send(clientfd, "hello server\n", 32, 0);
  if(ok == -1){
    perror("send error");
    exit(-1);
  }

  close(clientfd);
  return 0;
}
