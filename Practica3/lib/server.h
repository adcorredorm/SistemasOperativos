#ifndef SERVER_H
#define SERVER_H


#define BACKLOG 32
#define LOG_PATH "serverDogs.log"
#define NO_ASIGNADO -2

int NUM_CLIENTES;
int socket_servidor;
pthread_t thread[BACKLOG];
int clientesfd[BACKLOG];

#endif
