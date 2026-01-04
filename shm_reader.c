#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 1024

typedef struct {
    int count;
    int data[MAX];
} shm_block;

void filter(const char *shm_name, const char *sem_name) {
    int fd = shm_open(shm_name, O_RDONLY, 0666);
    shm_block *shm = mmap(NULL, sizeof(shm_block),
                          PROT_READ, MAP_SHARED, fd, 0);

    sem_t *sem = sem_open(sem_name, 0);
    sem_wait(sem);

    if (shm->count == 0) {
        munmap(shm, sizeof(shm_block));
        close(fd);
        sem_close(sem);
        exit(0);
    }

    int prime = shm->data[0];
    printf("%d ", prime);
    fflush(stdout);

    shm_unlink(shm_name);
    sem_unlink(sem_name);

    char next_shm[32], next_sem[32];
    snprintf(next_shm, sizeof(next_shm), "/shm_%d", prime);
    snprintf(next_sem, sizeof(next_sem), "/sem_%d", prime);

    int fd2 = shm_open(next_shm, O_CREAT | O_RDWR, 0666);
    ftruncate(fd2, sizeof(shm_block));
    shm_block *next = mmap(NULL, sizeof(shm_block),
                           PROT_WRITE, MAP_SHARED, fd2, 0);

    next->count = 0;

    for (int i = 1; i < shm->count; i++) {
        if (shm->data[i] % prime != 0) {
            next->data[next->count++] = shm->data[i];
        }
    }

    sem_t *next_sem = sem_open(next_sem, O_CREAT, 0666, 0);

    if (fork() == 0) {
        filter(next_shm, next_sem);
    } else {
        sem_post(next_sem);
        wait(NULL);
    }

    munmap(shm, sizeof(shm_block));
    munmap(next, sizeof(shm_block));
    close(fd);
    close(fd2);
    sem_close(sem);
    sem_close(next_sem);
    exit(0);
}
