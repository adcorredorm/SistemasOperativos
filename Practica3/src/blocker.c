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

sem_t *data_sem, *log_sem, *hist_sem;
char *pipe_buffer;

void init_pipe(int pipefd[]) {
    int ok;
    if (ok = pipe(pipefd)) {
        perror("Error at init_pipe => pipe");
        exit(-1);
    }
    if((ok = write(pipefd[1], UNLOCK_PIPE_D, sizeof(UNLOCK_PIPE_D))) == -1){
        perror("Error at init_pipe => write");
        exit(-1);
    }
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
    int ok;
    ok = pthread_mutex_init(mutex, NULL);
    if (ok == -1) {
        perror("Error at init_mutex");
        exit(-1);
    }
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
    printf("%s\n", "Cerrando Mutex");
    pthread_mutex_destroy(mutex);
}

void lock_sem(const char* source) {
    int ok;
    if (!strcmp(source, DATA_SOURCE)) {
        ok = sem_wait(data_sem);
        if (ok == -1) {
            perror("Error at lock_sem, source: DATA_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, LOG_SOURCE)) {
        ok = sem_wait(log_sem);
        if (ok == -1) {
            perror("Error at lock_sem, source: LOG_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, HIST_SOURCE)) {
        ok = sem_wait(hist_sem);
        if (ok == -1) {
            perror("Error at lock_sem, source: HIST_SOURCE");
            exit(-1);
        }
    }
}

void lock_pipe(char* source) {
    int ok;
    if (!strcmp(source, DATA_SOURCE)) {
        ok = read(data_pipefd[0], pipe_buffer, sizeof(UNLOCK_PIPE_D));
        if (ok == -1) {
            perror("Error at lock_pipe, source: DATA_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, LOG_SOURCE)) {
        ok = read(log_pipefd[0], pipe_buffer, sizeof(UNLOCK_PIPE_L));
        if (ok == -1) {
            perror("Error at lock_pipe, source: LOG_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, HIST_SOURCE)) {
        ok = read(hist_pipefd[0], pipe_buffer, sizeof(UNLOCK_PIPE_H));
        if (ok == -1) {
            perror("Error at lock_pipe, source: HIST_SOURCE");
            exit(-1);
        }
    }
}

void lock_mutex(char* source) {
    if (!strcmp(source, DATA_SOURCE)) pthread_mutex_lock(&data_mutex);
    else if (!strcmp(source, LOG_SOURCE)) pthread_mutex_lock(&log_mutex);
    else if (!strcmp(source, HIST_SOURCE)) pthread_mutex_lock(&hist_mutex);
}

void unlock_sem(char* source) {
    int ok;
    if (!strcmp(source, DATA_SOURCE)) {
        ok = sem_post(data_sem);
        if (ok == -1) {
            perror("Error at unlock_sem, source: DATA_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, LOG_SOURCE)) {
        ok = sem_post(log_sem);
        if (ok == -1) {
            perror("Error at unlock_sem, source: LOG_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, HIST_SOURCE)) {
        ok = sem_post(hist_sem);
        if (ok == -1) {
            perror("Error at unlock_sem, source: HIST_SOURCE");
            exit(-1);
        }
    }
}

void unlock_pipe(char* source) {
    int ok;
    if (!strcmp(source, DATA_SOURCE)) {
        ok = write(data_pipefd[1], UNLOCK_PIPE_D, sizeof(UNLOCK_PIPE_D));
        if (ok == -1) {
            perror("Error at unlock_pipe, source: DATA_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, LOG_SOURCE)) {
        ok = write(log_pipefd[1], UNLOCK_PIPE_L, sizeof(UNLOCK_PIPE_L));
        if (ok == -1) {
            perror("Error at unlock_pipe, source: LOG_SOURCE");
            exit(-1);
        }
    }
    else if (!strcmp(source, HIST_SOURCE)) {
        ok = write(hist_pipefd[1], UNLOCK_PIPE_H, sizeof(UNLOCK_PIPE_H));
        if (ok == -1) {
            perror("Error at unlock_pipe, source: HIST_SOURCE");
            exit(-1);
        }
    }
}

void unlock_mutex(char* source) {
    if (!strcmp(source, DATA_SOURCE)) pthread_mutex_unlock(&data_mutex);
    else if (!strcmp(source, LOG_SOURCE)) pthread_mutex_unlock(&log_mutex);
    else if (!strcmp(source, HIST_SOURCE)) pthread_mutex_unlock(&hist_mutex);
}

void init_blocker() {
    switch (BLOCK_OPTION) {
      case SEMAPHORE:
        data_sem = init_sem(DATA_SEM_NAME);
        log_sem = init_sem(LOG_SEM_NAME);
        hist_sem = init_sem(HIST_SEM_NAME);
      break;
      case PIPE:
        pipe_buffer = malloc(sizeof(UNLOCK_PIPE_D));
        if (pipe_buffer == NULL) {
            perror("Error at init_blocker => pipe");
            exit(-1);
        }
        init_pipe(data_pipefd);
        init_pipe(log_pipefd);
        init_pipe(hist_pipefd);
      break;

      case MUTEX:
        printf("%s\n", "Init Mutex");
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
    printf("bloqueado %s\n", source);
    sleep(3);
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
    printf("desbloqueado %s\n", source);
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
        free(pipe_buffer);
      break;

      case MUTEX:
        close_mutex(&data_mutex);
        close_mutex(&log_mutex);
        close_mutex(&hist_mutex);
      break;

    }
}
