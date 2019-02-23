#ifndef BLOCKER_H

#define BLOCKER_H
#define SEMAPHORE 0
#define PIPE 1
#define MUTEX 2
#define DATA_SOURCE "data"
#define LOG_SOURCE "log"
#define HIST_SOURCE "hist"
#define DATA_SEM_NAME "data_sem"
#define LOG_SEM_NAME "log_sem"
#define HIST_SEM_NAME "hist_sem"
#define UNLOCK_PIPE_D "lock_d"
#define UNLOCK_PIPE_L "lock_l"
#define UNLOCK_PIPE_H "lock_h"

int BLOCK_OPTION;
pthread_mutex_t data_mutex, log_mutex, hist_mutex;
int data_pipefd[2], log_pipefd[2], hist_pipefd[2];

void init_blocker();
void lock(char* source);
void unlock(char* source);
void close_blocker();

#endif
