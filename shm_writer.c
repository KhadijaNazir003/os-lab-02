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

// Declaration from shm_reader.c
void filter(const char *shm_name, const char *sem_name);

int main() {
    const char *shm_name = "/shm_start";
    const char *sem_name = "/sem_start";

    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shm_block));

    shm_block *shm = mmap(NULL, sizeof(shm_block),
                          PROT_WRITE, MAP_SHARED, fd, 0);

    shm->count = 0;
    for (int i = 2; i <= 1000; i++) {
        shm->data[shm->count++] = i;
    }

    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 0);

    if (fork() == 0) {
        filter(shm_name, sem_name);
    } else {
        sem_post(sem);
        wait(NULL);
    }

    munmap(shm, sizeof(shm_block));
    close(fd);
    sem_close(sem);

    shm_unlink(shm_name);
    sem_unlink(sem_name);

    return 0;
}
