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

int BLOCK_OPTION;

void init_blocker();
void lock(char* source);
void unlock(char* source);
void close_blocker();

#endif
