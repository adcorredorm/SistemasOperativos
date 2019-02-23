/* Librerias para los bloqueadores de las zonas críticas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <pthread.h>
#include "../lib/blocker.h"

char content[1] = "|";

void init_pipe(int pipefd[]) {
    pipe(pipefd);
    write(pipefd[1], content, sizeof(char));
}

sem_t* init_sem(const char* name) {
    sem_t *sem;
    sem = sem_open(name, O_CREAT, 0777, 1);
    if (sem == NULL) {
        perror("Error en init_sem");
        exit(-1);
    }
    return sem;
}

void init_mutex(pthread_mutex_t* mutex) {
    pthread_mutex_init(mutex, NULL);
}

void close_sem(sem_t* sem, char* name) {
    sem_close(sem);
    sem_unlink(name);
}

void close_pipe(int pipefd[]) {
    close(pipefd[0]);
    close(pipefd[1]);
}

void close_mutex(pthread_mutex_t* mutex) {
    pthread_mutex_destroy(mutex);
}

void lock_sem(const char* source) {
    if (!strcmp(source, DATA_SOURCE)) sem_wait(data_sem);
    else if (!strcmp(source, LOG_SOURCE)) sem_wait(log_sem);
    else if (!strcmp(source, HIST_SOURCE)) sem_wait(hist_sem);
}

void lock_pipe(char* source) {
    if (source == DATA_SOURCE) read(data_pipefd[0], content, sizeof(char));
    else if (source == LOG_SOURCE) read(log_pipefd[0], content, sizeof(char));
    else if (source == HIST_SOURCE) read(hist_pipefd[0], content, sizeof(char));
}

void lock_mutex(char* source) {
    if (source == DATA_SOURCE) pthread_mutex_lock(&data_mutex);
    else if (source == LOG_SOURCE) pthread_mutex_lock(&log_mutex);
    else if (source == HIST_SOURCE) pthread_mutex_lock(&hist_mutex);
}

void unlock_sem(char* source) {
    if (!strcmp(source, DATA_SOURCE)) sem_post(data_sem);
    else if (!strcmp(source, LOG_SOURCE)) sem_post(log_sem);
    else if (!strcmp(source, HIST_SOURCE)) sem_post(hist_sem);
}

void unlock_pipe(char* source) {
    if (source == DATA_SOURCE) write(data_pipefd[1], content, sizeof(char));
    else if (source == LOG_SOURCE) write(log_pipefd[1], content, sizeof(char));
    else if (source == HIST_SOURCE) write(hist_pipefd[1], content, sizeof(char));
}

void unlock_mutex(char* source) {
    if (source == DATA_SOURCE) pthread_mutex_unlock(&data_mutex);
    else if (source == LOG_SOURCE) pthread_mutex_unlock(&log_mutex);
    else if (source == HIST_SOURCE) pthread_mutex_unlock(&log_mutex);
}

void init_blocker() {
    switch (BLOCK_OPTION) {
      case SEMAPHORE:
        data_sem = init_sem(DATA_SEM_NAME);
        log_sem = init_sem(LOG_SEM_NAME);
        hist_sem = init_sem(HIST_SEM_NAME);
      break;
      case PIPE:
        init_pipe(data_pipefd);
        init_pipe(log_pipefd);
        init_pipe(hist_pipefd);
      break;

      case MUTEX:
        init_mutex(&data_mutex);
        init_mutex(&log_mutex);
        init_mutex(&hist_mutex);
      break;

    }
}

void lock(char* source) {
    switch (BLOCK_OPTION) {
      case SEMAPHORE:
        lock_sem(source);
      break;

      case PIPE:
        lock_pipe(source);
      break;

      case MUTEX:
        lock_mutex(source);
      break;
    }
    printf("bloqueado\n");
    sleep(5);
}

void unlock(char* source) {
    switch (BLOCK_OPTION) {
      case SEMAPHORE:
        unlock_sem(source);
      break;

      case PIPE:
        unlock_pipe(source);
      break;

      case MUTEX:
        unlock_mutex(source);
      break;
    }
    printf("desbloquear\n");
}

void close_blocker() {
    switch (BLOCK_OPTION) {
      case SEMAPHORE:
        close_sem(data_sem, DATA_SEM_NAME);
        close_sem(log_sem, LOG_SEM_NAME);
        close_sem(hist_sem, HIST_SEM_NAME);
      break;

      case PIPE:
        close_pipe(data_pipefd);
        close_pipe(log_pipefd);
        close_pipe(hist_pipefd);
      break;

      case MUTEX:
        close_mutex(&data_mutex);
        close_mutex(&log_mutex);
        close_mutex(&hist_mutex);
      break;

    }
}
